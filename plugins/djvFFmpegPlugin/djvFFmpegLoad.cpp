//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

//! \file djvFFmpegLoad.cpp

#include <djvFFmpegLoad.h>

#include <djvDebug.h>
#include <djvFileIoUtil.h>
#include <djvImage.h>
#include <djvPixelDataUtil.h>

#include <QApplication>

//------------------------------------------------------------------------------
// djvFFmpegLoad
//------------------------------------------------------------------------------

djvFFmpegLoad::djvFFmpegLoad() :
    _formatContext  (0),
    _videoStream    (0),
    _codecContext   (0),
    _avFrame        (0),
    _avFrameRgb     (0),
    _avFrameBuffer  (0),
    _swsContext     (0),
    _startFrame     (0),
    _frame          (0)
{}

djvFFmpegLoad::~djvFFmpegLoad()
{
    close();
}

void djvFFmpegLoad::open(const djvFileInfo & in, djvImageIoInfo & info)
    throw (djvError)
{
    DJV_DEBUG("djvFFmpegLoad::open");
    DJV_DEBUG_PRINT("in = " << in);

    close();

    // Open the file.
    
    if (avformat_open_input(
        &_formatContext,
        in.fileName().toLatin1().data(),
        0,
        0) != 0)
    {
        throw djvError(
            djvFFmpegPlugin::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_OPEN].arg(in));
    }
    
    if (avformat_find_stream_info(_formatContext, 0) < 0)
    {
        throw djvError(
            djvFFmpegPlugin::staticName,
            qApp->translate("djvFFmpegLoad", "Cannot find stream information: %1").arg(in));
    }
    
    av_dump_format(_formatContext, 0, in.fileName().toLatin1().data(), 0);
    
    // Find the first video stream.
    
    _videoStream = -1;
    
    for (int i = 0; i < _formatContext->nb_streams; ++i)
    {
        if (_formatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            _videoStream = i;
            
            break;
        }
    }
    
    if (-1 == _videoStream)
    {
        throw djvError(
            djvFFmpegPlugin::staticName,
            qApp->translate("djvFFmpegLoad", "Cannot find video stream: %1").arg(in));
    }

    AVStream * stream = _formatContext->streams[_videoStream];
    
    AVCodecContext * codecContext = stream->codec;
    
    // Find the codec for the video stream.
    
    AVCodec * codec = avcodec_find_decoder(codecContext->codec_id);
    
    if (! codec)
    {
        throw djvError(
            djvFFmpegPlugin::staticName,
            qApp->translate("djvFFmpegLoad", "Cannot find codec: %1").arg(in));
    }
    
    _codecContext = avcodec_alloc_context3(codec);
    
    if (avcodec_copy_context(_codecContext, codecContext) != 0)
    {
        throw djvError(
            djvFFmpegPlugin::staticName,
            qApp->translate("djvFFmpegLoad", "Cannot copy context: %1").arg(in));
    }
    
    if (avcodec_open2(_codecContext, codec, 0) < 0)
    {
        throw djvError(
            djvFFmpegPlugin::staticName,
            qApp->translate("djvFFmpegLoad", "Cannot open codec: %1").arg(in));
    }
    
    // Get file information.
    
    _info.fileName = in;
    
    _info.size = djvVector2i(_codecContext->width, _codecContext->height);
    
    _info.pixel = djvPixel::RGBA_U8;
    
    const int64_t duration = _formatContext->duration + 5000;
    const int secs  = duration / AV_TIME_BASE;
    const int usecs = duration % AV_TIME_BASE;

    DJV_DEBUG_PRINT("secs = " << secs);
    DJV_DEBUG_PRINT("usecs = " << usecs);

    /*DJV_DEBUG_PRINT("avg_frame_rate = " <<
        stream->avg_frame_rate.num << " " <<
        stream->avg_frame_rate.den);
    DJV_DEBUG_PRINT("tbr = " <<
        stream->r_frame_rate.num << " " <<
        stream->r_frame_rate.den);
    DJV_DEBUG_PRINT("tbn = " <<
        stream->time_base.num << " " <<
        stream->time_base.den);
    DJV_DEBUG_PRINT("tbc = " <<
        codecContext->time_base.num << " " <<
        codecContext->time_base.den);*/
    
    const djvSpeed speed(stream->r_frame_rate.num, stream->r_frame_rate.den);
    
    _info.sequence = djvSequence(
        0,
        secs * djvSpeed::speedToFloat(speed),
        0,
        speed);

    info = _info;
    
    // Initialize the buffers.
    
    _avFrame = av_frame_alloc();
    
    _avFrameRgb = av_frame_alloc();
    
    quint64 bufferSize = avpicture_get_size(
        PIX_FMT_RGBA,
        _codecContext->width,
        _codecContext->height);
    
    //DJV_DEBUG_PRINT("bufferSize = " << bufferSize);
    
    _avFrameBuffer = (uint8_t *)av_malloc(bufferSize * sizeof(uint8_t));
    
    avpicture_fill(
        (AVPicture *)_avFrameRgb,
        _avFrameBuffer,
        PIX_FMT_RGBA,
        _codecContext->width,
        _codecContext->height);
    
    // Initialize the software scaler.
    
    _swsContext = sws_getContext(
        _codecContext->width,
        _codecContext->height,
        _codecContext->pix_fmt,
        _codecContext->width,
        _codecContext->height,
        PIX_FMT_RGBA,
        SWS_BILINEAR,
        0,
        0,
        0);
}

void djvFFmpegLoad::read(djvImage & image, const djvImageIoFrameInfo & frame)
    throw (djvError)
{
    DJV_DEBUG("djvFFmpegLoad::read");
    DJV_DEBUG_PRINT("frame = " << frame);

    image.colorProfile = djvColorProfile();
    image.tags = djvImageTags();
    
    djvPixelData * data = frame.proxy ? &_tmp : &image;
    data->set(_info);

    int f = frame.frame;

    if (-1 == f)
    {
        f = 0;
    }
    else
    {
        f -= _startFrame;
    }
    
    DJV_DEBUG_PRINT("frame = " << f);
    
    if (f != _frame + 1)
    {
        int64_t pos   = f;
        int     flags = AVSEEK_FLAG_ANY;
        
        if (_videoStream >= 0)
        {
            AVStream * stream = _formatContext->streams[_videoStream];
    
            pos = av_rescale_q(pos, AV_TIME_BASE_Q, stream->time_base);
        }
    
        DJV_DEBUG_PRINT("pos = " << pos);
    
        av_seek_frame(_formatContext, _videoStream, pos, flags);
    }
    
    _frame = f;
    
    AVPacket packet;
    int      frameFinished = 0;
    
    while (av_read_frame(_formatContext, &packet) >= 0)
    {
        if (_videoStream == packet.stream_index)
        {
            avcodec_decode_video2(_codecContext, _avFrame, &frameFinished, &packet);
            
            if (frameFinished)
                break;
        }
    }
    
    const int w = _codecContext->width;
    const int h = _codecContext->height;
    
    sws_scale(
        _swsContext,
        (uint8_t const * const *)_avFrame->data,
        _avFrame->linesize,
        0,
        h,
        _avFrameRgb->data,
        _avFrameRgb->linesize);
    
    for (int y = 0; y < h; ++y)
    {
        const uint32_t * inP = reinterpret_cast<uint32_t *>(
            _avFrameRgb->data[0] +
            (h - 1 - y) * _avFrameRgb->linesize[0]);
        
        djvPixel::U8_T * outP = data->data(0, y);
        
        for (int x = 0; x < w; ++x)
        {
            outP[0] = ((*inP) & 0x000000ff);
            outP[1] = ((*inP) & 0x0000ff00) >> 8;
            outP[2] = ((*inP) & 0x00ff0000) >> 16;
            outP[3] = ((*inP) & 0xff000000) >> 24;
            
            inP  += 1;
            outP += 4;
        }
    }

    if (frame.proxy)
    {
        djvPixelDataInfo info = _info;
        info.size = djvPixelDataUtil::proxyScale(info.size, frame.proxy);
        info.proxy = frame.proxy;
        image.set(info);

        djvPixelDataUtil::proxyScale(_tmp, image, frame.proxy);
    }
    
    av_free_packet(&packet);
}

void djvFFmpegLoad::close() throw (djvError)
{
    //DJV_DEBUG("djvFFmpegLoad::close");
    
    if (_swsContext)
    {
        sws_freeContext(_swsContext);
        
        _swsContext = 0;
    }

    if (_avFrameBuffer)
    {
        av_free(_avFrameBuffer);
        
        _avFrameBuffer = 0;
    }

    if (_avFrameRgb)
    {
        av_free(_avFrameRgb);
        
        _avFrameRgb = 0;
    }

    if (_avFrame)
    {
        av_free(_avFrame);
        
        _avFrame = 0;
    }
    
    if (_formatContext)
    {
        avformat_close_input(&_formatContext);
        
        _formatContext = 0;
    }
}
