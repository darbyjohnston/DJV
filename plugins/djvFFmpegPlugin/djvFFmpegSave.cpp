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

//! \file djvFFmpegSave.cpp

#include <djvFFmpegSave.h>

#include <djvFFmpegUtil.h>

#include <djvOpenGlImage.h>

#include <stdio.h>

//------------------------------------------------------------------------------
// djvFFmpegSave
//------------------------------------------------------------------------------

djvFFmpegSave::djvFFmpegSave(const djvFFmpegPlugin::Options & options) :
    _options(options),
    _avIoContext    (0),
    _avFormatContext(0),
    _avStream       (0),
    _avFrame        (0),
    _avFrameBuf     (0),
    _avFrameRgb     (0),
    _swsContext     (0)
{}

djvFFmpegSave::~djvFFmpegSave()
{}

void djvFFmpegSave::open(const djvFileInfo & fileInfo, const djvImageIoInfo & info)
    throw (djvError)
{
    //DJV_DEBUG("djvFFmpegSave::open");
    //DJV_DEBUG_PRINT("fileInfo = " << fileInfo);
    //DJV_DEBUG_PRINT("info = " << info);
    
    // Open the file.
    
    AVOutputFormat * format = av_guess_format(
        _options.codec.toLatin1().data(),
        fileInfo.fileName().toLatin1().data(),
        0);
    
    AVCodec * codec = avcodec_find_encoder(CODEC_ID_MPEG4);
    
    AVCodecContext * codecContext = avcodec_alloc_context3(codec);

    avcodec_get_context_defaults3(codecContext, codec);
    codecContext->pix_fmt       = PIX_FMT_YUV420P;
    codecContext->width         = info.size.x;
    codecContext->height        = info.size.y;
    codecContext->time_base.den = info.sequence.speed.scale();
    codecContext->time_base.num = info.sequence.speed.duration();
    
    int r = avcodec_open2(codecContext, codec, 0);
    
    if (r < 0)
    {
        djvError error;
        error.add(
            djvFFmpegPlugin::staticName,
            djvFFmpegUtil::toString(r));
        error.add(
            djvFFmpegPlugin::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_WRITE].arg(fileInfo));
        throw error;
    }
    
    _avFormatContext = avformat_alloc_context();
    _avFormatContext->oformat = format;

    _avStream = avformat_new_stream(_avFormatContext, codecContext->codec);
    _avStream->codec         = codecContext;
    _avStream->time_base.den = info.sequence.speed.scale();
    _avStream->time_base.num = info.sequence.speed.duration();
    
    r = avio_open2(
        &_avIoContext,
        fileInfo.fileName().toLatin1().data(),
        AVIO_FLAG_READ_WRITE,
        0,
        0);
    
    if (r < 0)
    {
        djvError error;
        error.add(
            djvFFmpegPlugin::staticName,
            djvFFmpegUtil::toString(r));
        error.add(
            djvFFmpegPlugin::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_OPEN].arg(fileInfo));
        throw error;
    }
    
    _avFormatContext->pb = _avIoContext;

    r = avformat_write_header(_avFormatContext, 0);
    
    if (r < 0)
    {
        djvError error;
        error.add(
            djvFFmpegPlugin::staticName,
            djvFFmpegUtil::toString(r));
        error.add(
            djvFFmpegPlugin::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_WRITE].arg(fileInfo));
        throw error;
    }
    
    _info = djvPixelDataInfo();
    _info.fileName = fileInfo;
    _info.size     = info.size;
    _info.pixel    = djvPixel::RGBA_U8;
    _info.mirror.y = true;

    _image.set(_info);
    
    // Initialize the buffers.
    
    _avFrame = av_frame_alloc();
    _avFrame->width  = info.size.x;
    _avFrame->height = info.size.y;
    _avFrame->format = PIX_FMT_YUV420P;

    _avFrameBuf = (uint8_t *)av_malloc(
        avpicture_get_size(
            codecContext->pix_fmt,
            codecContext->width,
            codecContext->height));

    avpicture_fill(
        (AVPicture *)_avFrame,
        _avFrameBuf,
        codecContext->pix_fmt,
        codecContext->width,
        codecContext->height);

    _avFrameRgb = av_frame_alloc();
    
    // Initialize the software scaler.

    _swsContext = sws_getContext(
        codecContext->width,
        codecContext->height,
        PIX_FMT_RGBA,
        codecContext->width,
        codecContext->height,
        codecContext->pix_fmt,
        SWS_BILINEAR,
        0,
        0,
        0);
}

void djvFFmpegSave::write(const djvImage & in, const djvImageIoFrameInfo & frame)
    throw (djvError)
{
    //DJV_DEBUG("djvFFmpegSave::write");
    //DJV_DEBUG_PRINT("in = " << in);
    //DJV_DEBUG_PRINT("frame = " << frame);

    // Convert the image if necessary.

    const djvPixelData * p = &in;

    if (in.info() != _info)
    {
        //DJV_DEBUG_PRINT("convert = " << _image);

        _image.zero();

        djvOpenGlImage::copy(in, _image);

        p = &_image;
    }
    
    // Encode the image.

    avpicture_fill(
        (AVPicture *)_avFrameRgb,
        p->data(),
        PIX_FMT_RGBA,
        p->w(),
        p->h());
    
    sws_scale(
        _swsContext,
        (uint8_t const * const *)_avFrameRgb->data,
        _avFrameRgb->linesize,
        0,
        p->h(),
        _avFrame->data,
        _avFrame->linesize);
    
    AVCodecContext * codec = _avStream->codec;

    djvFFmpegUtil::Packet packet;
    packet().data = 0;
    packet().size = 0;
    
    _avFrame->pts = frame.frame;// * AV_TIME_BASE;
    
    int finished = 0;
    
    int r = avcodec_encode_video2(
        codec,
        &packet(),
        _avFrame,
        &finished);
    
    if (r < 0)
    {
        djvError error;
        error.add(
            djvFFmpegPlugin::staticName,
            djvFFmpegUtil::toString(r));
        error.add(
            djvFFmpegPlugin::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_WRITE].
                arg(_info.fileName));
        throw error;
    }

    //DJV_DEBUG_PRINT("finished = " << finished);
    //DJV_DEBUG_PRINT("size = " << packet().size);
    //DJV_DEBUG_PRINT("pts = " << static_cast<qint64>(packet().pts));
    //DJV_DEBUG_PRINT("dts = " << static_cast<qint64>(packet().dts));
    //DJV_DEBUG_PRINT("duration = " << static_cast<qint64>(packet().duration));

    // Write the image.

    /*packet().pts = av_rescale_q(
        packet().pts,
        codec->time_base,
        _avStream->time_base);
    packet().dts = av_rescale_q(
        packet().dts,
        codec->time_base,
        _avStream->time_base);
    packet().duration = av_rescale_q(
        packet().duration,
        codec->time_base,
        _avStream->time_base);*/
    packet().stream_index = _avStream->index;

    r = av_interleaved_write_frame(_avFormatContext, &packet());
    
    if (r < 0)
    {
        djvError error;
        error.add(
            djvFFmpegPlugin::staticName,
            djvFFmpegUtil::toString(r));
        error.add(
            djvFFmpegPlugin::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_WRITE].
                arg(_info.fileName));
        throw error;
    }    
}

void djvFFmpegSave::close() throw (djvError)
{
    //DJV_DEBUG("djvFFmpegSave::close");
    
    djvError error;
    
    if (_avFormatContext)
    {
        if (av_interleaved_write_frame(_avFormatContext, 0) < 0)
        {
            error.add(
                djvFFmpegPlugin::staticName,
                djvImageIo::errorLabels()[djvImageIo::ERROR_WRITE].
                    arg(_info.fileName));
        }
    
        //DJV_DEBUG_PRINT("frames = " <<
        //    static_cast<qint64>(_avStream->nb_frames));
        //DJV_DEBUG_PRINT("write trailer");
        
        if (av_write_trailer(_avFormatContext) < 0)
        {
            error.add(
                djvFFmpegPlugin::staticName,
                djvImageIo::errorLabels()[djvImageIo::ERROR_WRITE].
                    arg(_info.fileName));
        }
    }

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

    if (_avFrameBuf)
    {
        av_free(_avFrameBuf);

        _avFrameBuf = 0;
    }

    if (_avFrame)
    {
        av_frame_free(&_avFrame);
        
        _avFrame = 0;
    }

    if (_avIoContext)
    {
        avio_close(_avIoContext);
        
        _avIoContext = 0;
    }

    if (_avFormatContext)
    {
        avformat_free_context(_avFormatContext);
        
        _avFormatContext = 0;
    }
        
    if (error.count())
    {
        throw error;
    }
}

