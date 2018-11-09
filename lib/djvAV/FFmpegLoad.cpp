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

#include <djvAV/FFmpegLoad.h>

#include <djvAV/Image.h>
#include <djvAV/PixelDataUtil.h>

#include <djvCore/CoreContext.h>
#include <djvCore/Debug.h>
#include <djvCore/FileIOUtil.h>

#include <QCoreApplication>

extern "C"
{
#include <libavutil/imgutils.h>

} // extern "C"

namespace djv
{
    namespace AV
    {
        FFmpegLoad::FFmpegLoad(const Core::FileInfo & fileInfo, const QPointer<Core::CoreContext> & context) :
            ImageLoad(fileInfo, context)
        {
            // Open the file.
            int r = avformat_open_input(
                &_avFormatContext,
                _fileInfo.fileName().toUtf8().data(),
                0,
                0);
            if (r < 0)
            {
                throw Core::Error(
                    FFmpeg::staticName,
                    FFmpeg::toString(r));
            }
            r = avformat_find_stream_info(_avFormatContext, 0);
            if (r < 0)
            {
                throw Core::Error(
                    FFmpeg::staticName,
                    FFmpeg::toString(r));
            }
            av_dump_format(_avFormatContext, 0, _fileInfo.fileName().toUtf8().data(), 0);

            // Find the first video stream.
            for (unsigned int i = 0; i < _avFormatContext->nb_streams; ++i)
            {
                if (_avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
                {
                    _avVideoStream = i;

                    break;
                }
            }
            //DJV_DEBUG_PRINT("video stream = " << _avVideoStream);
            if (-1 == _avVideoStream)
            {
                throw Core::Error(
                    FFmpeg::staticName,
                    qApp->translate("djv::AV::FFmpegLoad", "Cannot find video stream"));
            }

            // Find the codec for the video stream.
            AVStream * avStream = _avFormatContext->streams[_avVideoStream];
            AVCodecParameters * avCodecParameters = avStream->codecpar;
            AVCodec * avCodec = avcodec_find_decoder(avCodecParameters->codec_id);
            if (!avCodec)
            {
                throw Core::Error(
                    FFmpeg::staticName,
                    qApp->translate("djv::AV::FFmpegLoad", "Cannot find codec"));
            }
            _avCodecParameters = avcodec_parameters_alloc();
            r = avcodec_parameters_copy(_avCodecParameters, avCodecParameters);
            if (r < 0)
            {
                throw Core::Error(
                    FFmpeg::staticName,
                    FFmpeg::toString(r));
            }
            _avCodecContext = avcodec_alloc_context3(avCodec);
            r = avcodec_parameters_to_context(_avCodecContext, _avCodecParameters);
            if (r < 0)
            {
                throw Core::Error(
                    FFmpeg::staticName,
                    FFmpeg::toString(r));
            }
            r = avcodec_open2(_avCodecContext, avCodec, 0);
            if (r < 0)
            {
                throw Core::Error(
                    FFmpeg::staticName,
                    FFmpeg::toString(r));
            }

            // Initialize the buffers.
            _avFrame = av_frame_alloc();
            _avFrameRgb = av_frame_alloc();

            // Initialize the software scaler.
            _swsContext = sws_getContext(
                _avCodecParameters->width,
                _avCodecParameters->height,
                static_cast<AVPixelFormat>(_avCodecParameters->format),
                _avCodecParameters->width,
                _avCodecParameters->height,
                AV_PIX_FMT_RGBA,
                SWS_BILINEAR,
                0,
                0,
                0);

            // Get file information.
            _imageIOInfo.fileName = _fileInfo;
            _imageIOInfo.size = glm::ivec2(_avCodecParameters->width, _avCodecParameters->height);
            _imageIOInfo.pixel = Pixel::RGBA_U8;
            _imageIOInfo.mirror.y = true;
            int64_t duration = 0;
            if (avStream->duration != AV_NOPTS_VALUE)
            {
                duration = av_rescale_q(
                    avStream->duration,
                    avStream->time_base,
                    FFmpeg::timeBaseQ());
            }
            else if (_avFormatContext->duration != AV_NOPTS_VALUE)
            {
                duration = _avFormatContext->duration;
            }
            const Core::Speed speed(avStream->r_frame_rate.num, avStream->r_frame_rate.den);
            //DJV_DEBUG_PRINT("duration = " << static_cast<qint64>(duration));
            //DJV_DEBUG_PRINT("speed = " << speed);
            int64_t nbFrames = 0;
            if (avStream->nb_frames != 0)
            {
                nbFrames = avStream->nb_frames;
            }
            else
            {
                nbFrames =
                    duration / static_cast<float>(AV_TIME_BASE) *
                    Core::Speed::speedToFloat(speed);
            }
            if (!nbFrames)
            {
                //DJV_DEBUG_PRINT("count frames");

                //! \todo As a last resort count the number of frames manually. If
                //! there are a lot of frames (like a two hour movie) this could take
                //! a really long time.
                int64_t pts = 0;
                while (readFrame(pts))
                {
                    ++nbFrames;
                }
                av_seek_frame(
                    _avFormatContext,
                    _avVideoStream,
                    0,
                    AVSEEK_FLAG_BACKWARD);
            }
            //DJV_DEBUG_PRINT("nbFrames = " << static_cast<qint64>(nbFrames));

            _imageIOInfo.sequence = Core::Sequence(0, nbFrames - 1, 0, speed);
        }

        FFmpegLoad::~FFmpegLoad()
        {
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
            if (_avFrame)
            {
                av_frame_free(&_avFrame);
                _avFrame = nullptr;
            }
            if (_avCodecContext)
            {
                avcodec_free_context(&_avCodecContext);
                _avCodecContext = nullptr;
            }
            if (_avCodecParameters)
            {
                avcodec_parameters_free(&_avCodecParameters);
                _avCodecParameters = nullptr;
            }
            _avVideoStream = -1;
            if (_avFormatContext)
            {
                avformat_close_input(&_avFormatContext);
                _avFormatContext = nullptr;
            }
        }

        void FFmpegLoad::read(Image & image, const ImageIOFrameInfo & frame)
        {
            //DJV_DEBUG("FFmpegLoad::read");
            //DJV_DEBUG_PRINT("frame = " << frame);

            image.colorProfile = ColorProfile();
            image.tags = ImageTags();
            PixelData * data = frame.proxy ? &_tmp : &image;
            data->set(_imageIOInfo);
            av_image_fill_arrays(
                _avFrameRgb->data,
                _avFrameRgb->linesize,
                data->data(),
                AV_PIX_FMT_RGBA,
                data->w(),
                data->h(),
                1);

            int f = frame.frame;
            if (-1 == f)
            {
                f = 0;
            }
            //DJV_DEBUG_PRINT("frame = " << f);

            AVStream * avStream = _avFormatContext->streams[_avVideoStream];
            int64_t pts = 0;
            if (f != _frame + 1)
            {
                const int64_t seek =
                    (f * _imageIOInfo.sequence.speed.duration()) /
                    static_cast<float>(_imageIOInfo.sequence.speed.scale()) *
                    AV_TIME_BASE;
                //DJV_DEBUG_PRINT("seek = " << static_cast<qint64>(seek));
                int r = av_seek_frame(
                    _avFormatContext,
                    _avVideoStream,
                    av_rescale_q(seek, FFmpeg::timeBaseQ(), avStream->time_base),
                    AVSEEK_FLAG_BACKWARD);
                //DJV_DEBUG_PRINT("r = " << FFmpeg::toString(r));
                avcodec_flush_buffers(_avCodecContext);
                while (readFrame(pts) && pts < seek)
                    ;
            }
            else
            {
                readFrame(pts);
            }
            _frame = f;

            sws_scale(
                _swsContext,
                (uint8_t const * const *)_avFrame->data,
                _avFrame->linesize,
                0,
                _avCodecParameters->height,
                _avFrameRgb->data,
                _avFrameRgb->linesize);

            if (frame.proxy)
            {
                PixelDataInfo info = _imageIOInfo;
                info.size = PixelDataUtil::proxyScale(info.size, frame.proxy);
                info.proxy = frame.proxy;
                image.set(info);
                PixelDataUtil::proxyScale(_tmp, image, frame.proxy);
            }
        }

        bool FFmpegLoad::readFrame(int64_t & pts)
        {
            //DJV_DEBUG("FFmpegLoad::readFrame");
            FFmpeg::Packet packet;
            int finished = 0;
            while (!finished)
            {
                int r = av_read_frame(_avFormatContext, &packet());
                //DJV_DEBUG_PRINT("packet");
                //DJV_DEBUG_PRINT("  size = " << static_cast<qint64>(packet().size));
                //DJV_DEBUG_PRINT("  pos = " << static_cast<qint64>(packet().pos));
                //DJV_DEBUG_PRINT("  pts = " << static_cast<qint64>(packet().pts));
                //DJV_DEBUG_PRINT("  dts = " << static_cast<qint64>(packet().dts));
                //DJV_DEBUG_PRINT("  duration = " << static_cast<qint64>(packet().duration));
                //DJV_DEBUG_PRINT("  keyframe = " << (packet().flags & AV_PKT_FLAG_KEY));
                //DJV_DEBUG_PRINT("  corrupt = " << (packet().flags & AV_PKT_FLAG_CORRUPT));
                //DJV_DEBUG_PRINT("  r = " << FFmpeg::toString(r));
                if (r < 0)
                {
                    packet().data = nullptr;
                    packet().size = 0;
                }
                if (_avVideoStream == packet().stream_index)
                {
                    if (&packet())
                    {
                        r = avcodec_send_packet(_avCodecContext, &packet());
                        if (r < 0)
                        {
                            break;
                        }
                    }
                    r = avcodec_receive_frame(_avCodecContext, _avFrame);
                    if (r < 0 && r != AVERROR(EAGAIN) && r != AVERROR_EOF)
                        return r;
                    if (r >= 0)
                        finished = 1;
                }
            }
            pts = _avFrame->pts;
            //DJV_DEBUG_PRINT("pts = " << static_cast<qint64>(pts));
            pts = av_rescale_q(
                pts,
                _avFormatContext->streams[_avVideoStream]->time_base,
                FFmpeg::timeBaseQ());
            //DJV_DEBUG_PRINT("pts = " << static_cast<qint64>(pts));
            return finished;
        }

    } // namespace AV
} // namespace djv
