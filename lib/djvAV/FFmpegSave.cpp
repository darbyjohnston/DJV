//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
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

#include <djvAV/FFmpegSave.h>

#include <djvAV/OpenGLImage.h>

#include <djvCore/CoreContext.h>

#include <QCoreApplication>

#include <stdio.h>

namespace djv
{
    namespace AV
    {
        FFmpegSave::FFmpegSave(const Core::FileInfo & fileInfo, const IOInfo & ioInfo, const FFmpeg::Options & options, const QPointer<Core::CoreContext> & context) :
            Save(fileInfo, ioInfo, context),
            _options(options)
        {
            //DJV_DEBUG("FFmpegSave::FFmpegSave");
            //DJV_DEBUG_PRINT("file info = " << fileInfo);
            //DJV_DEBUG_PRINT("io info = " << ioInfo);

            _frame = 0;

            // Open the file.    
            Pixel::PIXEL pixel = static_cast<Pixel::PIXEL>(0);
            bool bgr = false;
            QString avFormatName;
            AVCodecID avCodecId = static_cast<AVCodecID>(0);
            AVPixelFormat avPixel = static_cast<AVPixelFormat>(0);
            float avQScale = -1.f;
            _avFrameRgbPixel = static_cast<AVPixelFormat>(0);

            FFmpeg::Dictionary dictionary;
            QString value;
            switch (_options.format)
            {
                /*case FFmpeg::H264:
                    pixel            = Pixel::RGBA_U8;
                    avFormatName     = "mov";
                    avCodecId        = AV_CODEC_ID_H264;
                    switch (_options.quality)
                    {
                        case FFmpeg::LOW:    value = "fast";   break;
                        case FFmpeg::MEDIUM: value = "medium"; break;
                        case FFmpeg::HIGH:   value = "slow";   break;
                        default: break;
                    }
                    av_dict_set(
                        dictionary(),
                        "preset",
                        value.toUtf8().data(),
                        0);
                    break;*/
            case FFmpeg::MPEG4:
                pixel = Pixel::RGBA_U8;
                bgr = ioInfo.layers[0].bgr;
                avFormatName = "mp4";
                avCodecId = AV_CODEC_ID_MPEG4;
                avPixel = AV_PIX_FMT_YUV420P;
                _avFrameRgbPixel = bgr ? AV_PIX_FMT_BGRA : AV_PIX_FMT_RGBA;
                switch (_options.quality)
                {
                case FFmpeg::LOW:    avQScale = 9.f; break;
                case FFmpeg::MEDIUM: avQScale = 3.f; break;
                case FFmpeg::HIGH:   avQScale = 1.f; break;
                default: break;
                }
                break;
            case FFmpeg::PRO_RES:
                pixel = Pixel::RGB_U16;
                bgr = ioInfo.layers[0].bgr;
                avFormatName = "mov";
                avCodecId = AV_CODEC_ID_PRORES;
                avPixel = AV_PIX_FMT_YUV422P10;
                _avFrameRgbPixel = bgr ? AV_PIX_FMT_BGR48 : AV_PIX_FMT_RGB48;
                switch (_options.quality)
                {
                case FFmpeg::LOW:    value = "1"; break;
                case FFmpeg::MEDIUM: value = "2"; break;
                case FFmpeg::HIGH:   value = "3"; break;
                default: break;
                }
                av_dict_set(
                    dictionary(),
                    "profile",
                    value.toUtf8().data(),
                    0);
                break;
            case FFmpeg::MJPEG:
                pixel = Pixel::RGBA_U8;
                bgr = ioInfo.layers[0].bgr;
                avFormatName = "mov";
                avCodecId = AV_CODEC_ID_MJPEG;
                avPixel = AV_PIX_FMT_YUVJ422P;
                _avFrameRgbPixel = bgr ? AV_PIX_FMT_BGRA : AV_PIX_FMT_RGBA;
                switch (_options.quality)
                {
                case FFmpeg::LOW:    avQScale = 9.f; break;
                case FFmpeg::MEDIUM: avQScale = 3.f; break;
                case FFmpeg::HIGH:   avQScale = 1.f; break;
                default: break;
                }
                break;
            default: break;
            }
            //DJV_DEBUG_PRINT("pixel = " << pixel);
            //DJV_DEBUG_PRINT("av format name = " << avFormatName);
            //DJV_DEBUG_PRINT("av codec id = " << avCodecId);
            //DJV_DEBUG_PRINT("av pixel = " << avPixel);
            //DJV_DEBUG_PRINT("av rgb pixel = " << _avFrameRgbPixel);
            //DJV_DEBUG_PRINT("av qscale = " << avQScale);

            AVOutputFormat * avFormat = av_guess_format(
                avFormatName.toUtf8().data(),
                0, //fileInfo.fileName().toUtf8().data(),
                0);
            if (!avFormat)
            {
                throw Core::Error(
                    FFmpeg::staticName,
                    qApp->translate("djv::AV::FFmpegSave", "Cannot find format: %1").
                    arg(FFmpeg::formatLabels()[_options.format]));
            }
            //DJV_DEBUG_PRINT("av format extensions = " << avFormat->extensions);

            _avFormatContext = avformat_alloc_context();
            _avFormatContext->oformat = avFormat;

            AVCodec * avCodec = avcodec_find_encoder(avCodecId);
            if (!avCodec)
            {
                throw Core::Error(
                    FFmpeg::staticName,
                    qApp->translate("djv::AV::FFmpegSave", "Cannot find encoder: %1").
                    arg(FFmpeg::formatLabels()[_options.format]));
            }

            AVCodecContext * avCodecContext = avcodec_alloc_context3(avCodec);
            avcodec_get_context_defaults3(avCodecContext, avCodec);
            //DJV_DEBUG_PRINT("default bit rate = " << avCodecContext->bit_rate);
            //DJV_DEBUG_PRINT("default gop = " << avCodecContext->gop_size);

            avCodecContext->pix_fmt = avPixel;
            avCodecContext->width = ioInfo.layers[0].size.x;
            avCodecContext->height = ioInfo.layers[0].size.y;
            avCodecContext->time_base.den = ioInfo.sequence.speed.scale();
            avCodecContext->time_base.num = ioInfo.sequence.speed.duration();

            if (avFormat->flags & AVFMT_GLOBALHEADER)
                avCodecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

            if (avQScale >= 0.f)
            {
                avCodecContext->flags |= AV_CODEC_FLAG_QSCALE;
                avCodecContext->global_quality = FF_QP2LAMBDA * avQScale;
            }

            int r = avcodec_open2(avCodecContext, avCodec, dictionary());
            if (r < 0)
            {
                throw Core::Error(
                    FFmpeg::staticName,
                    FFmpeg::toString(r));
            }

            _avStream = avformat_new_stream(_avFormatContext, avCodecContext->codec);
            if (!_avStream)
            {
                throw Core::Error(
                    FFmpeg::staticName,
                    qApp->translate("djv::AV::FFmpegSave", "Cannot create stream"));
            }

            _avStream->codec = avCodecContext;
            _avStream->time_base.den = ioInfo.sequence.speed.scale();
            _avStream->time_base.num = ioInfo.sequence.speed.duration();

            r = avio_open2(
                &_avIoContext,
                fileInfo.fileName().toUtf8().data(),
                AVIO_FLAG_READ_WRITE,
                0,
                0);
            if (r < 0)
            {
                throw Core::Error(
                    FFmpeg::staticName,
                    FFmpeg::toString(r));
            }

            _avFormatContext->pb = _avIoContext;
            r = avformat_write_header(_avFormatContext, 0);
            if (r < 0)
            {
                throw Core::Error(
                    FFmpeg::staticName,
                    FFmpeg::toString(r));
            }

            _info = PixelDataInfo();
            _info.fileName = fileInfo;
            _info.size = ioInfo.layers[0].size;
            _info.pixel = pixel;
            _info.bgr = ioInfo.layers[0].bgr;

            // Initialize the buffers.
            _image.set(_info);

            _avFrame = av_frame_alloc();
            _avFrame->width = ioInfo.layers[0].size.x;
            _avFrame->height = ioInfo.layers[0].size.y;
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
                ioInfo.layers[0].size.x,
                ioInfo.layers[0].size.y,
                _avFrameRgbPixel,
                avCodecContext->width,
                avCodecContext->height,
                avCodecContext->pix_fmt,
                SWS_BILINEAR,
                0,
                0,
                0);
            if (!_swsContext)
            {
                throw Core::Error(
                    FFmpeg::staticName,
                    qApp->translate("djv::AV::FFmpegSave", "Cannot create software scaler"));
            }
        }

        FFmpegSave::~FFmpegSave()
        {}

        void FFmpegSave::write(const Image & in, const ImageIOInfo & frame)
        {
            //DJV_DEBUG("FFmpegSave::write");
            //DJV_DEBUG_PRINT("in = " << in);
            //DJV_DEBUG_PRINT("frame = " << frame);

            // Convert the image if necessary.
            const PixelData * p = &in;
            if (in.info() != _info)
            {
                //DJV_DEBUG_PRINT("convert = " << _image);
                _image.zero();
                OpenGLImage().copy(in, _image);
                p = &_image;
            }

            // Encode the image.
            avpicture_fill(
                (AVPicture *)_avFrameRgb,
                p->data(),
                _avFrameRgbPixel,
                p->w(),
                p->h());

            quint64 scanlineByteCount = p->scanlineByteCount();
            quint64 dataByteCount = p->dataByteCount();
            const uint8_t * const data[] =
            {
                p->data() + dataByteCount - scanlineByteCount,
                p->data() + dataByteCount - scanlineByteCount,
                p->data() + dataByteCount - scanlineByteCount,
                p->data() + dataByteCount - scanlineByteCount
            };
            const int lineSize[] =
            {
                -static_cast<int>(scanlineByteCount),
                -static_cast<int>(scanlineByteCount),
                -static_cast<int>(scanlineByteCount),
                -static_cast<int>(scanlineByteCount)
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
            FFmpeg::Packet packet;
            packet().data = nullptr;
            packet().size = 0;
            _avFrame->pts = _frame++;
            _avFrame->quality = avCodecContext->global_quality;

            int finished = 0;
            int r = avcodec_encode_video2(
                avCodecContext,
                &packet(),
                _avFrame,
                &finished);
            if (r < 0)
            {
                throw Core::Error(
                    FFmpeg::staticName,
                    FFmpeg::toString(r));
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
                throw Core::Error(
                    FFmpeg::staticName,
                    FFmpeg::toString(r));
            }
        }

        void FFmpegSave::close()
        {
            //DJV_DEBUG("FFmpegSave::close");
            Core::Error error;
            int r = 0;
            if (_avFormatContext)
            {
                r = av_interleaved_write_frame(_avFormatContext, 0);
                if (r < 0)
                {
                    throw Core::Error(
                        FFmpeg::staticName,
                        FFmpeg::toString(r));
                }
                //DJV_DEBUG_PRINT("frames = " << static_cast<qint64>(_avStream->nb_frames));
                //DJV_DEBUG_PRINT("write trailer");
                r = av_write_trailer(_avFormatContext);
                if (r < 0)
                {
                    throw Core::Error(
                        FFmpeg::staticName,
                        FFmpeg::toString(r));
                }
            }
            if (_swsContext)
            {
                sws_freeContext(_swsContext);
                _swsContext = nullptr;
            }
            if (_avFrameRgb)
            {
                av_frame_free(&_avFrameRgb);
                _avFrameRgb = nullptr;
            }
            if (_avFrameBuf)
            {
                av_free(_avFrameBuf);
                _avFrameBuf = nullptr;
            }
            if (_avFrame)
            {
                av_frame_free(&_avFrame);
                _avFrame = nullptr;
            }
            if (_avIoContext)
            {
                avio_close(_avIoContext);
                _avIoContext = nullptr;
            }
            if (_avFormatContext)
            {
                avformat_free_context(_avFormatContext);
                _avFormatContext = nullptr;
            }
            if (error.count())
            {
                throw error;
            }
        }

    } // namespace AV
} // namespace djv
