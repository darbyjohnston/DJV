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
            Load(fileInfo, context)
        {
            DJV_DEBUG("FFmpegLoad::FFmpegLoad");

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

            // Find the first video and audio stream.
            for (unsigned int i = 0; i < _avFormatContext->nb_streams; ++i)
            {
                if (-1 == _avVideoStream && _avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
                {
                    _avVideoStream = i;
                }
                if (-1 == _avAudioStream && _avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
                {
                    _avAudioStream = i;
                }
            }
            DJV_DEBUG_PRINT("video stream = " << _avVideoStream);
            DJV_DEBUG_PRINT("audio stream = " << _avAudioStream);
            if (-1 == _avVideoStream && -1 == _avAudioStream)
            {
                throw Core::Error(
                    FFmpeg::staticName,
                    qApp->translate("djv::AV::FFmpegLoad", "Cannot find any streams"));
            }

            // Find the codec for the video stream.
            auto avVideoStream = _avFormatContext->streams[_avVideoStream];
            auto avVideoCodecParameters = avVideoStream->codecpar;
            auto avVideoCodec = avcodec_find_decoder(avVideoCodecParameters->codec_id);
            if (!avVideoCodec)
            {
                throw Core::Error(
                    FFmpeg::staticName,
                    qApp->translate("djv::AV::FFmpegLoad", "Cannot find video codec"));
            }
            _avCodecParameters[_avVideoStream] = avcodec_parameters_alloc();
            r = avcodec_parameters_copy(_avCodecParameters[_avVideoStream], avVideoCodecParameters);
            if (r < 0)
            {
                throw Core::Error(
                    FFmpeg::staticName,
                    FFmpeg::toString(r));
            }
            _avCodecContext[_avVideoStream] = avcodec_alloc_context3(avVideoCodec);
            r = avcodec_parameters_to_context(_avCodecContext[_avVideoStream], _avCodecParameters[_avVideoStream]);
            if (r < 0)
            {
                throw Core::Error(
                    FFmpeg::staticName,
                    FFmpeg::toString(r));
            }
            r = avcodec_open2(_avCodecContext[_avVideoStream], avVideoCodec, 0);
            if (r < 0)
            {
                throw Core::Error(
                    FFmpeg::staticName,
                    FFmpeg::toString(r));
            }

            // Find the codec for the audio stream.
            auto avAudioStream = _avFormatContext->streams[_avAudioStream];
            auto avAudioCodecParameters = avAudioStream->codecpar;
            Audio::TYPE audioType = FFmpeg::fromFFmpeg(static_cast<AVSampleFormat>(avAudioCodecParameters->format));
            if (Audio::TYPE_NONE == audioType)
            {
                throw Core::Error(
                    FFmpeg::staticName,
                    qApp->translate("djv::AV::FFmpegLoad", "Unsupported audio format: %1").arg(FFmpeg::toString(avAudioCodecParameters->format)));
            }
            auto avAudioCodec = avcodec_find_decoder(avAudioCodecParameters->codec_id);
            if (!avAudioCodec)
            {
                throw Core::Error(
                    FFmpeg::staticName,
                    qApp->translate("djv::AV::FFmpegLoad", "Cannot find audio codec"));
            }
            _avCodecParameters[_avAudioStream] = avcodec_parameters_alloc();
            r = avcodec_parameters_copy(_avCodecParameters[_avAudioStream], avAudioCodecParameters);
            if (r < 0)
            {
                throw Core::Error(
                    FFmpeg::staticName,
                    FFmpeg::toString(r));
            }
            _avCodecContext[_avAudioStream] = avcodec_alloc_context3(avAudioCodec);
            r = avcodec_parameters_to_context(_avCodecContext[_avAudioStream], _avCodecParameters[_avAudioStream]);
            if (r < 0)
            {
                throw Core::Error(
                    FFmpeg::staticName,
                    FFmpeg::toString(r));
            }
            r = avcodec_open2(_avCodecContext[_avAudioStream], avAudioCodec, 0);
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
                _avCodecParameters[_avVideoStream]->width,
                _avCodecParameters[_avVideoStream]->height,
                static_cast<AVPixelFormat>(_avCodecParameters[_avVideoStream]->format),
                _avCodecParameters[_avVideoStream]->width,
                _avCodecParameters[_avVideoStream]->height,
                AV_PIX_FMT_RGBA,
                SWS_BILINEAR,
                0,
                0,
                0);

            // Get file information.
            _ioInfo.layers[0].fileName = _fileInfo;
            _ioInfo.layers[0].size = glm::ivec2(_avCodecParameters[_avVideoStream]->width, _avCodecParameters[_avVideoStream]->height);
            _ioInfo.layers[0].pixel = Pixel::RGBA_U8;
            _ioInfo.layers[0].mirror.y = true;
            _ioInfo.audio.channels = _avCodecParameters[_avAudioStream]->channels;
            _ioInfo.audio.type = audioType;
            _ioInfo.audio.sampleRate = _avCodecParameters[_avAudioStream]->sample_rate;            
            int64_t duration = 0;
            if (avVideoStream->duration != AV_NOPTS_VALUE)
            {
                duration = av_rescale_q(
                    avVideoStream->duration,
                    avVideoStream->time_base,
                    FFmpeg::timeBaseQ());
            }
            else if (_avFormatContext->duration != AV_NOPTS_VALUE)
            {
                duration = _avFormatContext->duration;
            }
            const Core::Speed speed(avVideoStream->r_frame_rate.num, avVideoStream->r_frame_rate.den);
            //DJV_DEBUG_PRINT("duration = " << static_cast<qint64>(duration));
            //DJV_DEBUG_PRINT("speed = " << speed);
            int64_t nbFrames = 0;
            if (avVideoStream->nb_frames != 0)
            {
                nbFrames = avVideoStream->nb_frames;
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
            _ioInfo.sequence = Core::Sequence(0, nbFrames - 1, 0, speed);
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
            for (auto i : _avCodecContext)
            {
                avcodec_free_context(&i.second);
            }
            for (auto i : _avCodecParameters)
            {
                avcodec_parameters_free(&i.second);
            }
            _avVideoStream = -1;
            if (_avFormatContext)
            {
                avformat_close_input(&_avFormatContext);
                _avFormatContext = nullptr;
            }
        }

        void FFmpegLoad::read(Image & image, const ImageIOInfo & frame)
        {
            //DJV_DEBUG("FFmpegLoad::read");
            //DJV_DEBUG_PRINT("frame = " << frame);

            image.colorProfile = ColorProfile();
            image.tags = Tags();
            PixelData * data = frame.proxy ? &_tmp : &image;
            data->set(_ioInfo.layers[0]);
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
                    (f * _ioInfo.sequence.speed.duration()) /
                    static_cast<float>(_ioInfo.sequence.speed.scale()) *
                    AV_TIME_BASE;
                //DJV_DEBUG_PRINT("seek = " << static_cast<qint64>(seek));
                int r = av_seek_frame(
                    _avFormatContext,
                    _avVideoStream,
                    av_rescale_q(seek, FFmpeg::timeBaseQ(), avStream->time_base),
                    AVSEEK_FLAG_BACKWARD);
                //DJV_DEBUG_PRINT("r = " << FFmpeg::toString(r));
                avcodec_flush_buffers(_avCodecContext[_avVideoStream]);
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
                _avCodecParameters[_avVideoStream]->height,
                _avFrameRgb->data,
                _avFrameRgb->linesize);

            if (frame.proxy)
            {
                PixelDataInfo info = _ioInfo.layers[0];
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
                        r = avcodec_send_packet(_avCodecContext[_avVideoStream], &packet());
                        if (r < 0)
                        {
                            break;
                        }
                    }
                    r = avcodec_receive_frame(_avCodecContext[_avVideoStream], _avFrame);
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
