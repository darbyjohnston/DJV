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

#include <djvOpenGlImage.h>

#include <QCoreApplication>

#include <stdio.h>

//------------------------------------------------------------------------------
// djvFFmpegSave
//------------------------------------------------------------------------------

djvFFmpegSave::djvFFmpegSave(const djvFFmpeg::Options & options, djvImageContext * context) :
    djvImageSave(context),
    _options        (options),
    _frame          (0),
    _avIoContext    (0),
    _avFormatContext(0),
    _avStream       (0),
    _avFrame        (0),
    _avFrameBuf     (0),
    _avFrameRgb     (0),
    _avFrameRgbPixel(static_cast<AVPixelFormat>(0)),
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
    
    close();
    
    _frame = 0;
        
    // Open the file.
    
    djvPixel::PIXEL pixel         = static_cast<djvPixel::PIXEL>(0);
    bool            bgr           = false;
    
    QString         avFormatName;
    AVCodecID       avCodecId     = static_cast<AVCodecID>(0);
    AVPixelFormat   avPixel       = static_cast<AVPixelFormat>(0);
    double          avQScale      = -1.0;
    
    _avFrameRgbPixel = static_cast<AVPixelFormat>(0);

    djvFFmpeg::Dictionary dictionary;
    QString               value;

    switch (_options.format)
    {
        /*case djvFFmpeg::H264:

            pixel            = djvPixel::RGBA_U8;
        
            avFormatName     = "mov";
            avCodecId        = AV_CODEC_ID_H264;
            
            switch (_options.quality)
            {
                case djvFFmpeg::LOW:    value = "fast";   break;
                case djvFFmpeg::MEDIUM: value = "medium"; break;
                case djvFFmpeg::HIGH:   value = "slow";   break;

                default: break;
            }

            av_dict_set(
                dictionary(),
                "preset",
                value.toLatin1().data(),
                0);

            break;*/
        
        case djvFFmpeg::MPEG4:

            pixel            = djvPixel::RGBA_U8;
            bgr              = info.bgr;

            avFormatName     = "mp4";
            avCodecId        = AV_CODEC_ID_MPEG4;
            avPixel          = AV_PIX_FMT_YUV420P;
            _avFrameRgbPixel = bgr ? AV_PIX_FMT_BGRA : AV_PIX_FMT_RGBA;

            switch (_options.quality)
            {
                case djvFFmpeg::LOW:    avQScale = 9.0; break;
                case djvFFmpeg::MEDIUM: avQScale = 3.0; break;
                case djvFFmpeg::HIGH:   avQScale = 1.0; break;

                default: break;
            }

            break;
        
        case djvFFmpeg::PRO_RES:

            pixel            = djvPixel::RGB_U16;
            bgr              = info.bgr;

            avFormatName     = "mov";
            avCodecId        = AV_CODEC_ID_PRORES;
            avPixel          = AV_PIX_FMT_YUV422P10;
            _avFrameRgbPixel = bgr ? AV_PIX_FMT_RGB48 : AV_PIX_FMT_BGR48;
         
            switch (_options.quality)
            {
                case djvFFmpeg::LOW:    value = "1"; break;
                case djvFFmpeg::MEDIUM: value = "2"; break;
                case djvFFmpeg::HIGH:   value = "3"; break;

                default: break;
            }

            av_dict_set(
                dictionary(),
                "profile",
                value.toLatin1().data(),
                0);

            break;
        
        case djvFFmpeg::MJPEG:

            pixel            = djvPixel::RGBA_U8;
            bgr              = info.bgr;

            avFormatName     = "mov";
            avCodecId        = AV_CODEC_ID_MJPEG;
            avPixel          = AV_PIX_FMT_YUVJ422P;
            _avFrameRgbPixel = bgr ? AV_PIX_FMT_BGRA : AV_PIX_FMT_RGBA;

            switch (_options.quality)
            {
                case djvFFmpeg::LOW:    avQScale = 9.0; break;
                case djvFFmpeg::MEDIUM: avQScale = 3.0; break;
                case djvFFmpeg::HIGH:   avQScale = 1.0; break;

                default: break;
            }

            break;
        
        default: break;
    }
    
    //DJV_DEBUG_PRINT("pixel = " << pixel);

    //DJV_DEBUGBUG_PRINT("av format name = " << avFormatName);
    //DJV_DEBUGBUG_PRINT("av codec id = " << avCodecId);
    //DJV_DEBUGBUG_PRINT("av pixel = " << avPixel);
    //DJV_DEBUGBUG_PRINT("av rgb pixel = " << _avFrameRgbPixel);
    //DJV_DEBUGBUG_PRINT("av qscale = " << avQScale);
    
    AVOutputFormat * avFormat = av_guess_format(
        avFormatName.toLatin1().data(),
        0, //fileInfo.fileName().toLatin1().data(),
        0);
    
    if (! avFormat)
    {
        throw djvError(
            djvFFmpeg::staticName,
            qApp->translate("djvFFmpegSave", "Cannot find format: %1").
                arg(djvFFmpeg::formatLabels()[_options.format]));
    }
    
    //DJV_DEBUGBUG_PRINT("av format extensions = " << avFormat->extensions);
    
    _avFormatContext = avformat_alloc_context();
    _avFormatContext->oformat = avFormat;

    AVCodec * avCodec = avcodec_find_encoder(avCodecId);

    if (! avCodec)
    {
        throw djvError(
            djvFFmpeg::staticName,
            qApp->translate("djvFFmpegSave", "Cannot find encoder: %1").
                arg(djvFFmpeg::formatLabels()[_options.format]));
    }

    AVCodecContext * avCodecContext = avcodec_alloc_context3(avCodec);
    
    avcodec_get_context_defaults3(avCodecContext, avCodec);
    
    //DJV_DEBUGBUG_PRINT("default bit rate = " << avCodecContext->bit_rate);
    //DJV_DEBUGBUG_PRINT("default gop = " << avCodecContext->gop_size);
    
    avCodecContext->pix_fmt       = avPixel;
    avCodecContext->width         = info.size.x;
    avCodecContext->height        = info.size.y;
    avCodecContext->time_base.den = info.sequence.speed.scale();
    avCodecContext->time_base.num = info.sequence.speed.duration();
    
    if (avFormat->flags & AVFMT_GLOBALHEADER)
        avCodecContext->flags |= CODEC_FLAG_GLOBAL_HEADER;

    if (avQScale >= 0.0)
    {
        avCodecContext->flags |= CODEC_FLAG_QSCALE;
        avCodecContext->global_quality = FF_QP2LAMBDA * avQScale;
    }
    
    int r = avcodec_open2(avCodecContext, avCodec, dictionary());
    
    if (r < 0)
    {
        throw djvError(
            djvFFmpeg::staticName,
            djvFFmpeg::toString(r));
    }

    _avStream = avformat_new_stream(_avFormatContext, avCodecContext->codec);

    if (! _avStream)
    {
        throw djvError(
            djvFFmpeg::staticName,
            qApp->translate("djvFFmpegSave", "Cannot create stream"));
    }
    
    _avStream->codec         = avCodecContext;
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
        throw djvError(
            djvFFmpeg::staticName,
            djvFFmpeg::toString(r));
    }
    
    _avFormatContext->pb = _avIoContext;

    r = avformat_write_header(_avFormatContext, 0);
    
    if (r < 0)
    {
        throw djvError(
            djvFFmpeg::staticName,
            djvFFmpeg::toString(r));
    }
    
    _info          = djvPixelDataInfo();
    _info.fileName = fileInfo;
    _info.size     = info.size;
    _info.pixel    = pixel;
    _info.bgr      = info.bgr;

    // Initialize the buffers.
    
    _image.set(_info);
    
    _avFrame         = av_frame_alloc();
    _avFrame->width  = info.size.x;
    _avFrame->height = info.size.y;
    _avFrame->format = avCodecContext->pix_fmt;

    _avFrameBuf = (uint8_t *)av_malloc(
        avpicture_get_size(
            avCodecContext->pix_fmt,
            avCodecContext->width,
            avCodecContext->height));

    avpicture_fill(
        (AVPicture *)_avFrame,
        _avFrameBuf,
        avCodecContext->pix_fmt,
        avCodecContext->width,
        avCodecContext->height);

    _avFrameRgb = av_frame_alloc();
    
    // Initialize the software scaler.

    _swsContext = sws_getContext(
        info.size.x,
        info.size.y,
        _avFrameRgbPixel,
        avCodecContext->width,
        avCodecContext->height,
        avCodecContext->pix_fmt,
        SWS_BILINEAR,
        0,
        0,
        0);

    if (! _swsContext)
    {
        throw djvError(
            djvFFmpeg::staticName,
            qApp->translate("djvFFmpegSave", "Cannot create software scaler"));
    }
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
        _avFrameRgbPixel,
        p->w(),
        p->h());
    
    quint64 pixelByteCount = p->pixelByteCount();
    quint64 scanlineByteCount = p->scanlineByteCount();
    quint64 dataByteCount = p->dataByteCount();
    
    const uint8_t * const data [] =
    {
        p->data() + dataByteCount - scanlineByteCount,
        p->data() + dataByteCount - scanlineByteCount,
        p->data() + dataByteCount - scanlineByteCount,
        p->data() + dataByteCount - scanlineByteCount
    };
    
    const int lineSize [] =
    {
        -scanlineByteCount,
        -scanlineByteCount,
        -scanlineByteCount,
        -scanlineByteCount
    };
    
    sws_scale(
        _swsContext,
        //(uint8_t const * const *)_avFrameRgb->data,
        //_avFrameRgb->linesize,
        data,
        lineSize,
        0,
        p->h(),
        _avFrame->data,
        _avFrame->linesize);
    
    AVCodecContext * avCodecContext = _avStream->codec;

    djvFFmpeg::Packet packet;
    packet().data = 0;
    packet().size = 0;
    
    _avFrame->pts     = _frame++;
    _avFrame->quality = avCodecContext->global_quality;
    
    int finished = 0;
    
    int r = avcodec_encode_video2(
        avCodecContext,
        &packet(),
        _avFrame,
        &finished);
    
    if (r < 0)
    {
        throw djvError(
            djvFFmpeg::staticName,
            djvFFmpeg::toString(r));
    }

    //DJV_DEBUG_PRINT("finished = " << finished);
    //DJV_DEBUG_PRINT("size = " << packet().size);
    //DJV_DEBUG_PRINT("pts = " << static_cast<qint64>(packet().pts));
    //DJV_DEBUG_PRINT("dts = " << static_cast<qint64>(packet().dts));
    //DJV_DEBUG_PRINT("duration = " << static_cast<qint64>(packet().duration));

    // Write the image.
    
    packet().pts = av_rescale_q(
        packet().pts,
        avCodecContext->time_base,
        _avStream->time_base);
    packet().dts = av_rescale_q(
        packet().dts,
        avCodecContext->time_base,
        _avStream->time_base);
    packet().duration = av_rescale_q(
        packet().duration,
        avCodecContext->time_base,
        _avStream->time_base);
    packet().stream_index = _avStream->index;

    r = av_interleaved_write_frame(_avFormatContext, &packet());
    
    if (r < 0)
    {
        throw djvError(
            djvFFmpeg::staticName,
            djvFFmpeg::toString(r));
    }
}

void djvFFmpegSave::close() throw (djvError)
{
    //DJV_DEBUG("djvFFmpegSave::close");
    
    djvError error;
    
    int r = 0;
    
    if (_avFormatContext)
    {
        r = av_interleaved_write_frame(_avFormatContext, 0);
        
        if (r < 0)
        {
            throw djvError(
                djvFFmpeg::staticName,
                djvFFmpeg::toString(r));
        }
    
        //DJV_DEBUG_PRINT("frames = " <<
        //    static_cast<qint64>(_avStream->nb_frames));
        //DJV_DEBUG_PRINT("write trailer");
        
        r = av_write_trailer(_avFormatContext);
        
        if (r < 0)
        {
            throw djvError(
                djvFFmpeg::staticName,
                djvFFmpeg::toString(r));
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

