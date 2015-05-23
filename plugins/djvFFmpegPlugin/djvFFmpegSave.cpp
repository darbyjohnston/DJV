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
    DJV_DEBUG("djvFFmpegSave::open");
    DJV_DEBUG_PRINT("fileInfo = " << fileInfo);
    DJV_DEBUG_PRINT("info = " << info);
    
    // Open the file.
    
    if (avio_open2(
        &_avIoContext,
        fileInfo.fileName().toLatin1().data(),
        AVIO_FLAG_WRITE,
        0,
        0) < 0)
    {
        throw djvError(
            djvFFmpegPlugin::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_OPEN].arg(fileInfo));
    }
    
    if (avformat_alloc_output_context2(
        &_avFormatContext,
        0,
        _options.codec.toLatin1().data(),
        0) < 0)
    {
        throw djvError(
            djvFFmpegPlugin::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_WRITE].arg(fileInfo));
    }
    
    _avFormatContext->pb = _avIoContext;
    
    _avStream = avformat_new_stream(_avFormatContext, 0);
    
    AVCodecContext * codec = _avStream->codec;
    codec->codec_id      = CODEC_ID_H264;
    codec->codec_type    = AVMEDIA_TYPE_VIDEO;
    codec->width         = info.size.x;
    codec->height        = info.size.y;
    codec->pix_fmt       = PIX_FMT_YUV420P;
    codec->time_base.den = info.sequence.speed.scale();
    codec->time_base.num = info.sequence.speed.duration();

    if (avformat_write_header(_avFormatContext, 0) < 0)
    {
        throw djvError(
            djvFFmpegPlugin::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_WRITE].arg(fileInfo));
    }

    _info = djvPixelDataInfo();
    _info.fileName = fileInfo;
    _info.size     = info.size;
    _info.pixel    = djvPixel::RGBA_U8;

    _image.set(_info);
    
    // Initialize the buffers.
    
    _avFrame = av_frame_alloc();

    _avFrameBuf = (uint8_t *)av_malloc(
        avpicture_get_size(codec->pix_fmt, codec->width, codec->height));

    avpicture_fill(
        (AVPicture *)_avFrame,
        _avFrameBuf,
        codec->pix_fmt,
        codec->width,
        codec->height);

    _avFrameRgb = av_frame_alloc();
    
    // Initialize the software scaler.

    DJV_DEBUG_PRINT("width = " << codec->width);
    DJV_DEBUG_PRINT("height = " << codec->height);
    DJV_DEBUG_PRINT("pix_fmt = " << codec->pix_fmt);
    
    _swsContext = sws_getContext(
        codec->width,
        codec->height,
        PIX_FMT_RGBA,
        codec->width,
        codec->height,
        codec->pix_fmt,
        SWS_BILINEAR,
        0,
        0,
        0);
}

void djvFFmpegSave::write(const djvImage & in, const djvImageIoFrameInfo & frame)
    throw (djvError)
{
    DJV_DEBUG("djvFFmpegSave::write");
    DJV_DEBUG_PRINT("in = " << in);
    DJV_DEBUG_PRINT("frame = " << frame);

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
    
    /*sws_scale(
        _swsContext,
        (uint8_t const * const *)_avFrameRgb->data,
        _avFrameRgb->linesize,
        0,
        p->h(),
        _avFrame->data,
        _avFrame->linesize);*/
    
    AVCodecContext * codec = _avStream->codec;

    djvFFmpegPacket packet;
    //packet().stream_index = _avStream->index;
    
    int got_packet_ptr = 0;
    
    if (avcodec_encode_video2(
        codec,
        &packet(),
        _avFrame,
        &got_packet_ptr) < 0)
    {
        throw djvError(
            djvFFmpegPlugin::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_WRITE].arg(_info.fileName));
    }

    // Write the image.
    
    if (av_interleaved_write_frame(_avFormatContext, &packet()) < 0)
    {
        throw djvError(
            djvFFmpegPlugin::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_WRITE].arg(_info.fileName));
    }
}

void djvFFmpegSave::close() throw (djvError)
{
    DJV_DEBUG("djvFFmpegSave::close");
    
    djvError error;
    
    if (_avFormatContext)
    {
        DJV_DEBUG_PRINT("write trailer");
    
        if (av_write_trailer(_avFormatContext) < 0)
        {
            error.add(
                djvFFmpegPlugin::staticName,
                djvImageIo::errorLabels()[djvImageIo::ERROR_WRITE].arg(_info.fileName));
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

    if (_avFormatContext)
    {
        avformat_free_context(_avFormatContext);
        
        _avFormatContext = 0;
    }
    
    if (_avIoContext)
    {
        avio_close(_avIoContext);
        
        _avIoContext = 0;
    }
    
    if (error.count())
    {
        throw error;
    }
}
