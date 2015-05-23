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

#include <djvFFmpegUtil.h>

#include <djvDebug.h>
#include <djvFileIoUtil.h>
#include <djvImage.h>
#include <djvPixelDataUtil.h>

#include <QApplication>

//------------------------------------------------------------------------------
// djvFFmpegLoad
//------------------------------------------------------------------------------

djvFFmpegLoad::djvFFmpegLoad() :
    _avFormatContext( 0),
    _avVideoStream  (-1),
    _avCodecContext ( 0),
    _avFrame        ( 0),
    _avFrameRgb     ( 0),
    _swsContext     ( 0),
    _startFrame     ( 0),
    _frame          ( 0)
{}

djvFFmpegLoad::~djvFFmpegLoad()
{
    close();
}

void djvFFmpegLoad::open(const djvFileInfo & in, djvImageIoInfo & info)
    throw (djvError)
{
    //DJV_DEBUG("djvFFmpegLoad::open");
    //DJV_DEBUG_PRINT("in = " << in);

    close();

    // Open the file.
    
    if (avformat_open_input(
        &_avFormatContext,
        in.fileName().toLatin1().data(),
        0,
        0) < 0)
    {
        throw djvError(
            djvFFmpegPlugin::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_OPEN].arg(in));
    }
    
    if (avformat_find_stream_info(_avFormatContext, 0) < 0)
    {
        throw djvError(
            djvFFmpegPlugin::staticName,
            qApp->translate("djvFFmpegLoad", "Cannot find stream information: %1").arg(in));
    }
    
    //av_dump_format(_avFormatContext, 0, in.fileName().toLatin1().data(), 0);
    
    // Find the first video stream.
    
    for (unsigned int i = 0; i < _avFormatContext->nb_streams; ++i)
    {
        if (_avFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            _avVideoStream = i;
            
            break;
        }
    }
    
    //DJV_DEBUG_PRINT("video stream = " << _avVideoStream);
    
    if (-1 == _avVideoStream)
    {
        throw djvError(
            djvFFmpegPlugin::staticName,
            qApp->translate("djvFFmpegLoad", "Cannot find video stream: %1").arg(in));
    }

    // Find the codec for the video stream.
    
    AVStream * stream = _avFormatContext->streams[_avVideoStream];
    
    AVCodecContext * codecContext = stream->codec;
    
    AVCodec * codec = avcodec_find_decoder(codecContext->codec_id);
    
    if (! codec)
    {
        throw djvError(
            djvFFmpegPlugin::staticName,
            qApp->translate("djvFFmpegLoad", "Cannot find codec: %1").arg(in));
    }
    
    _avCodecContext = avcodec_alloc_context3(codec);
    
    if (avcodec_copy_context(_avCodecContext, codecContext) < 0)
    {
        throw djvError(
            djvFFmpegPlugin::staticName,
            qApp->translate("djvFFmpegLoad", "Cannot copy context: %1").arg(in));
    }
    
    if (avcodec_open2(_avCodecContext, codec, 0) < 0)
    {
        throw djvError(
            djvFFmpegPlugin::staticName,
            qApp->translate("djvFFmpegLoad", "Cannot open codec: %1").arg(in));
    }
    
    // Initialize the buffers.
    
    _avFrame = av_frame_alloc();
    
    _avFrameRgb = av_frame_alloc();
    
    // Initialize the software scaler.
    
    _swsContext = sws_getContext(
        _avCodecContext->width,
        _avCodecContext->height,
        _avCodecContext->pix_fmt,
        _avCodecContext->width,
        _avCodecContext->height,
        PIX_FMT_RGBA,
        SWS_BILINEAR,
        0,
        0,
        0);
    
    // Get file information.
    
    _info.fileName = in;
    _info.size     = djvVector2i(_avCodecContext->width, _avCodecContext->height);
    _info.pixel    = djvPixel::RGBA_U8;
    _info.mirror.y = true;
    
    int64_t duration = 0;
    
    if (stream->duration != AV_NOPTS_VALUE)
    {
        duration = av_rescale_q(
            stream->duration,
            stream->time_base,
            djvFFmpegPlugin::timeBaseQ());
    }
    else if (_avFormatContext->duration != AV_NOPTS_VALUE)
    {
        duration = _avFormatContext->duration;
    }
    
    const djvSpeed speed(stream->r_frame_rate.num, stream->r_frame_rate.den);
    
    //DJV_DEBUG_PRINT("duration = " << int(duration));
    //DJV_DEBUG_PRINT("speed = " << speed);

    int64_t nbFrames = 0;
    
    if (stream->nb_frames != 0)
    {
        nbFrames = stream->nb_frames;
    }
    else
    {
        nbFrames =
            duration / static_cast<double>(AV_TIME_BASE) *
            djvSpeed::speedToFloat(speed);
    }
    
    if (! nbFrames)
    {
        //! \todo As a last resort count the number of frames manually. If
        //! there are a lot of frames (like a two hour movie) this could take
        //! a really long time.
        
        int64_t pts = 0;
        
        while (readFrame(pts) >= 0)
        {
            ++nbFrames;
        }

        av_seek_frame(
            _avFormatContext,
            _avVideoStream,
            0,
            AVSEEK_FLAG_BACKWARD);
    }

    //DJV_DEBUG_PRINT("nbFrames = " << int(nbFrames));

    _info.sequence = djvSequence(0, nbFrames - 1, 0, speed);

    info = _info;
}

void djvFFmpegLoad::read(djvImage & image, const djvImageIoFrameInfo & frame)
    throw (djvError)
{
    //DJV_DEBUG("djvFFmpegLoad::read");
    //DJV_DEBUG_PRINT("frame = " << frame);
    
    int r = 0;

    image.colorProfile = djvColorProfile();
    image.tags = djvImageTags();
    
    djvPixelData * data = frame.proxy ? &_tmp : &image;
    data->set(_info);
    
    avpicture_fill(
        (AVPicture *)_avFrameRgb,
        data->data(),
        PIX_FMT_RGBA,
        data->w(),
        data->h());

    int f = frame.frame;

    if (-1 == f)
    {
        f = 0;
    }
    else
    {
        f -= _startFrame;
    }
    
    //DJV_DEBUG_PRINT("frame = " << f);
        
    AVStream * stream = _avFormatContext->streams[_avVideoStream];
    
    int64_t pts = 0;
        
    if (f != _frame + 1)
    {
        const int64_t pos =
            (f * _info.sequence.speed.duration()) /
            static_cast<double>(_info.sequence.speed.scale()) *
            AV_TIME_BASE;

        //DJV_DEBUG_PRINT("pos = " << pos);

        r = av_seek_frame(
            _avFormatContext,
            _avVideoStream,
            av_rescale_q(pos, djvFFmpegPlugin::timeBaseQ(), stream->time_base),
            AVSEEK_FLAG_BACKWARD);
        
        //DJV_DEBUG_PRINT("seek = " << r);

        avcodec_flush_buffers(_avCodecContext);
        
        r = readFrame(pts);
        
        //DJV_DEBUG_PRINT("r = " << r);
        //DJV_DEBUG_PRINT("pts = " << pts);
        
        //int64_t keyframe =
        //    pts / static_cast<double>(AV_TIME_BASE) *
        //    djvSpeed::speedToFloat(_info.sequence.speed);
        //
        //DJV_DEBUG_PRINT("keyframe = " << keyframe);

        while (r >= 0 && pts < pos)
        //for (int64_t i = keyframe; i < f; ++i)
        {
            r = readFrame(pts);

            //DJV_DEBUG_PRINT("r = " << r);
            //DJV_DEBUG_PRINT("pts = " << pts);
        }
    }
    else
    {
        r = readFrame(pts);
    }

    _frame = f;

    const int w = _avCodecContext->width;
    const int h = _avCodecContext->height;
    
    sws_scale(
        _swsContext,
        (uint8_t const * const *)_avFrame->data,
        _avFrame->linesize,
        0,
        h,
        _avFrameRgb->data,
        _avFrameRgb->linesize);

    if (frame.proxy)
    {
        djvPixelDataInfo info = _info;
        info.size = djvPixelDataUtil::proxyScale(info.size, frame.proxy);
        info.proxy = frame.proxy;
        image.set(info);

        djvPixelDataUtil::proxyScale(_tmp, image, frame.proxy);
    }
}

void djvFFmpegLoad::close() throw (djvError)
{
    //DJV_DEBUG("djvFFmpegLoad::close");
    
    if (_swsContext)
    {
        sws_freeContext(_swsContext);
        
        _swsContext = 0;
    }

    if (_avFrameRgb)
    {
        av_frame_free(&_avFrameRgb);
        
        _avFrameRgb = 0;
    }

    if (_avFrame)
    {
        av_frame_free(&_avFrame);
        
        _avFrame = 0;
    }
    
    if (_avCodecContext)
    {
        avcodec_free_context(&_avCodecContext);
        
        _avCodecContext = 0;
    }
    
    _avVideoStream = -1;
    
    if (_avFormatContext)
    {
        avformat_close_input(&_avFormatContext);
        
        _avFormatContext = 0;
    }
}

int djvFFmpegLoad::readFrame(int64_t & pts)
{
    //DJV_DEBUG("djvFFmpegLoad::readFrame");
    
    int r = -1;
    
    int finished = 0;
    
    djvFFmpegPacket packet;

    do
    {
        r = av_read_frame(_avFormatContext, &packet());
        
        //DJV_DEBUG_PRINT("packet");
        //DJV_DEBUG_PRINT("  size = " << int(_avPacket.size));
        //DJV_DEBUG_PRINT("  pos = " << int(_avPacket.pos));
        //DJV_DEBUG_PRINT("  keyframe = " << (_avPacket.flags & AV_PKT_FLAG_KEY));
        //DJV_DEBUG_PRINT("  corrupt = " << (_avPacket.flags & AV_PKT_FLAG_CORRUPT));
    
        if (_avVideoStream == packet().stream_index)
        {
            if (avcodec_decode_video2(
                _avCodecContext,
                _avFrame,
                &finished,
                &packet()) <= 0)
             {
                break;
             }
        }
    }
    while (! finished);

    pts = _avFrame->pkt_pts;
    
    //DJV_DEBUG_PRINT("pts = " << int(pts));
    
    pts = av_rescale_q(
        pts,
        _avFormatContext->streams[_avVideoStream]->time_base,
        djvFFmpegPlugin::timeBaseQ());
    
    //DJV_DEBUG_PRINT("pts = " << int(pts));
    //DJV_DEBUG_PRINT("finished = " << finished);
    //DJV_DEBUG_PRINT("r = " << r);
    
    return r;
}
