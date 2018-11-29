//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <AV/IO.h>

#include <AV/Context.h>

#include <Core/Error.h>

#include <QCoreApplication>

extern "C"
{
#include <libavutil/imgutils.h>

} // extern "C"

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            VideoInfo::VideoInfo()
            {}

            VideoInfo::VideoInfo(const PixelDataInfo & info, const Core::Speed & speed, Duration duration) :
                _info(info),
                _speed(speed),
                _duration(duration)
            {}

            bool VideoInfo::operator == (const VideoInfo & other) const
            {
                return _info == other._info && _speed == other._speed && _duration == other._duration;
            }

            AudioInfo::AudioInfo()
            {}

            AudioInfo::AudioInfo(const AudioDataInfo & info, Duration duration) :
                _info(info),
                _duration(duration)
            {}

            bool AudioInfo::operator == (const AudioInfo & other) const
            {
                return _info == other._info && _duration == other._duration;
            }

            Info::Info()
            {}

            Info::Info(const QString & fileName, const VideoInfo & video, const AudioInfo & audio) :
                _fileName(fileName),
                _video(video),
                _audio(audio)
            {}

            Queue::Queue()
            {}

            std::mutex & Queue::getMutex()
            {
                return _mutex;
            }

            size_t Queue::getVideoSize() const
            {
                return _video.size();
            }

            size_t Queue::getAudioSize() const
            {
                return _audio.size();
            }

            void Queue::addVideo(Timestamp ts, const std::shared_ptr<PixelData> & data)
            {
                _video.push_back(std::make_pair(ts, data));
            }

            void Queue::addAudio(Timestamp ts, const std::shared_ptr<AudioData> & data)
            {
                _audio.push_back(std::make_pair(ts, data));
            }

            void Queue::clear()
            {
                _video.clear();
                _audio.clear();
            }

            namespace
            {
                const size_t videoFrameQueueMax = 100;
                const size_t audioFrameQueueMax = 100;
                size_t videoFrameQueue = videoFrameQueueMax;
                size_t audioFrameQueue = audioFrameQueueMax;
                const size_t timeout = 10;

            } // namespace

            Loader::Loader(const QString & fileName, const std::shared_ptr<Queue> & queue, const QPointer<Context> & context, QObject * parent) :
                QObject(parent),
                _context(context),
                _queue(queue)
            {
                _running = true;
                _thread = std::thread(
                    [this, fileName]
                {
                    try
                    {
                        // Open the file.
                        int r = avformat_open_input(
                            &_avFormatContext,
                            fileName.toUtf8().data(),
                            nullptr,
                            nullptr);
                        if (r < 0)
                        {
                            throw Core::Error(
                                "djv::AV::IO::Loader",
                                FFmpeg::getErrorString(r));
                        }
                        r = avformat_find_stream_info(_avFormatContext, 0);
                        if (r < 0)
                        {
                            throw Core::Error(
                                "djv::AV::IO::Loader",
                                FFmpeg::getErrorString(r));
                        }
                        av_dump_format(_avFormatContext, 0, fileName.toUtf8().data(), 0);

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
                        if (-1 == _avVideoStream && -1 == _avAudioStream)
                        {
                            throw Core::Error(
                                "djv::AV::IO::Loader",
                                qApp->translate("djv::AV::IO::Loader", "Cannot find any streams"));
                        }

                        // Find the codec for the video stream.
                        auto avVideoStream = _avFormatContext->streams[_avVideoStream];
                        auto avVideoCodecParameters = avVideoStream->codecpar;
                        auto avVideoCodec = avcodec_find_decoder(avVideoCodecParameters->codec_id);
                        if (!avVideoCodec)
                        {
                            throw Core::Error(
                                "djv::AV::IO::Loader",
                                qApp->translate("djv::AV::IO::Loader", "Cannot find video codec"));
                        }
                        _avCodecParameters[_avVideoStream] = avcodec_parameters_alloc();
                        r = avcodec_parameters_copy(_avCodecParameters[_avVideoStream], avVideoCodecParameters);
                        if (r < 0)
                        {
                            throw Core::Error(
                                "djv::AV::IO::Loader",
                                FFmpeg::getErrorString(r));
                        }
                        _avCodecContext[_avVideoStream] = avcodec_alloc_context3(avVideoCodec);
                        r = avcodec_parameters_to_context(_avCodecContext[_avVideoStream], _avCodecParameters[_avVideoStream]);
                        if (r < 0)
                        {
                            throw Core::Error(
                                "djv::AV::IO::Loader",
                                FFmpeg::getErrorString(r));
                        }
                        r = avcodec_open2(_avCodecContext[_avVideoStream], avVideoCodec, 0);
                        if (r < 0)
                        {
                            throw Core::Error(
                                "djv::AV::IO::Loader",
                                FFmpeg::getErrorString(r));
                        }

                        // Find the codec for the audio stream.
                        auto avAudioStream = _avFormatContext->streams[_avAudioStream];
                        auto avAudioCodecParameters = avAudioStream->codecpar;
                        Audio::Type audioType = FFmpeg::fromFFmpeg(static_cast<AVSampleFormat>(avAudioCodecParameters->format));
                        if (Audio::Type::None == audioType)
                        {
                            throw Core::Error(
                                "djv::AV::IO::Loader",
                                qApp->translate("djv::AV::IO::Loader", "Unsupported audio format: %1").arg(FFmpeg::toString(static_cast<AVSampleFormat>(avAudioCodecParameters->format))));
                        }
                        auto avAudioCodec = avcodec_find_decoder(avAudioCodecParameters->codec_id);
                        if (!avAudioCodec)
                        {
                            throw Core::Error(
                                "djv::AV::IO::Loader",
                                qApp->translate("djv::AV::IO::Loader", "Cannot find audio codec"));
                        }
                        _avCodecParameters[_avAudioStream] = avcodec_parameters_alloc();
                        r = avcodec_parameters_copy(_avCodecParameters[_avAudioStream], avAudioCodecParameters);
                        if (r < 0)
                        {
                            throw Core::Error(
                                "djv::AV::IO::Loader",
                                FFmpeg::getErrorString(r));
                        }
                        _avCodecContext[_avAudioStream] = avcodec_alloc_context3(avAudioCodec);
                        r = avcodec_parameters_to_context(_avCodecContext[_avAudioStream], _avCodecParameters[_avAudioStream]);
                        if (r < 0)
                        {
                            throw Core::Error(
                                "djv::AV::IO::Loader",
                                FFmpeg::getErrorString(r));
                        }
                        r = avcodec_open2(_avCodecContext[_avAudioStream], avAudioCodec, 0);
                        if (r < 0)
                        {
                            throw Core::Error(
                                "djv::AV::IO::Loader",
                                FFmpeg::getErrorString(r));
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

                        const auto pixelDataInfo = PixelDataInfo(
                            glm::ivec2(_avCodecParameters[_avVideoStream]->width, _avCodecParameters[_avVideoStream]->height),
                            Pixel(GL_RGBA, GL_UNSIGNED_BYTE),
                            PixelDataLayout(PixelDataMirror(false, true)));
                        Duration duration = 0;
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
                        const auto videoInfo = VideoInfo(pixelDataInfo, speed, duration);

                        duration = 0;
                        if (avAudioStream->duration != AV_NOPTS_VALUE)
                        {
                            duration = av_rescale_q(
                                avAudioStream->duration,
                                avAudioStream->time_base,
                                FFmpeg::timeBaseQ());
                        }
                        else if (_avFormatContext->duration != AV_NOPTS_VALUE)
                        {
                            duration = _avFormatContext->duration;
                        }
                        const auto audioInfo = AudioInfo(
                            AudioDataInfo(
                                _avCodecParameters[_avAudioStream]->channels,
                                audioType,
                                _avCodecParameters[_avAudioStream]->sample_rate),
                            duration);

                        _info = Info(fileName, videoInfo, audioInfo);
                        _infoPromise.set_value(_info);

                        while (_running)
                        {
                            AVPacket packet;
                            try
                            {
                                bool read = false;
                                Timestamp seek = -1;
                                {
                                    std::unique_lock<std::mutex> lock(_queue->getMutex());
                                    if (_queueCV.wait_for(
                                        lock,
                                        std::chrono::milliseconds(timeout),
                                        [this]
                                    {
                                        return
                                            _queue->getVideoSize() < videoFrameQueue ||
                                            _queue->getAudioSize() < audioFrameQueue ||
                                            _seek != -1;
                                    }))
                                    {
                                        read = true;
                                        if (_seek != -1)
                                        {
                                            seek = _seek;
                                            _seek = -1;
                                            videoFrameQueue = videoFrameQueueMax;
                                            audioFrameQueue = audioFrameQueueMax;
                                            _queue->clear();
                                        }
                                    }
                                }
                                if (seek != -1)
                                {
                                    if (av_seek_frame(
                                        _avFormatContext,
                                        _avVideoStream,
                                        av_rescale_q(seek, FFmpeg::timeBaseQ(), _avFormatContext->streams[_avVideoStream]->time_base),
                                        AVSEEK_FLAG_BACKWARD) < 0)
                                    {
                                        throw std::exception();
                                    }
                                    avcodec_flush_buffers(_avCodecContext[_avVideoStream]);
                                    Timestamp pts = 0;
                                    while (pts < seek - 1)
                                    {
                                        if (av_read_frame(_avFormatContext, &packet) < 0)
                                        {
                                            _decodeVideo(nullptr);
                                            _decodeAudio(nullptr);
                                            avcodec_flush_buffers(_avCodecContext[_avVideoStream]);
                                            avcodec_flush_buffers(_avCodecContext[_avAudioStream]);
                                            throw std::exception();
                                        }
                                        if (_avVideoStream == packet.stream_index)
                                        {
                                            Timestamp r = _decodeVideo(&packet, true);
                                            if (r < 0)
                                            {
                                                throw std::exception();
                                            }
                                            pts = r;
                                        }
                                        else if (_avAudioStream == packet.stream_index)
                                        {
                                            if (_decodeAudio(&packet, true) < 0)
                                            {
                                                throw std::exception();
                                            }
                                        }
                                        av_packet_unref(&packet);
                                    }
                                }
                                if (read)
                                {
                                    if (av_read_frame(_avFormatContext, &packet) < 0)
                                    {
                                        _decodeVideo(nullptr);
                                        _decodeAudio(nullptr);
                                        avcodec_flush_buffers(_avCodecContext[_avVideoStream]);
                                        avcodec_flush_buffers(_avCodecContext[_avAudioStream]);
                                        throw std::exception();
                                    }
                                    if (_avVideoStream == packet.stream_index)
                                    {
                                        if (_decodeVideo(&packet) < 0)
                                        {
                                            throw std::exception();
                                        }
                                    }
                                    else if (_avAudioStream == packet.stream_index)
                                    {
                                        if (_decodeAudio(&packet) < 0)
                                        {
                                            throw std::exception();
                                        }
                                    }
                                    av_packet_unref(&packet);
                                }
                            }
                            catch (const std::exception &)
                            {
                                av_packet_unref(&packet);
                                videoFrameQueue = 0;
                                audioFrameQueue = 0;
                            }
                        }
                    }
                    catch (const Core::Error & error)
                    {
                        _hasError = true;
                        _error = error;
                        _running = false;
                    }
                });
                if (_hasError)
                {
                    throw _error;
                }
            }

            Loader::~Loader()
            {
                _running = false;
                _thread.join();
            }

            std::future<Info> Loader::getInfo()
            {
                return _infoPromise.get_future();
            }

            void Loader::seek(Timestamp value)
            {
                std::lock_guard<std::mutex> lock(_queue->getMutex());
                _seek = value;
            }

            Timestamp Loader::_decodeVideo(AVPacket * packet, bool seek)
            {
                int r = avcodec_send_packet(_avCodecContext[_avVideoStream], packet);
                if (r < 0)
                {
                    return r;
                }
                Timestamp pts = 0;
                while (r >= 0)
                {
                    r = avcodec_receive_frame(_avCodecContext[_avVideoStream], _avFrame);
                    if (AVERROR(EAGAIN) == r)
                    {
                        return pts;
                    }
                    else if (r < 0)
                    {
                        return r;
                    }

                    pts = av_rescale_q(
                        _avFrame->pts,
                        _avFormatContext->streams[_avVideoStream]->time_base,
                        FFmpeg::timeBaseQ());

                    if (!seek)
                    {
                        auto pixelData = PixelData::create(_info.getVideo().getInfo());
                        av_image_fill_arrays(
                            _avFrameRgb->data,
                            _avFrameRgb->linesize,
                            pixelData->getData(),
                            AV_PIX_FMT_RGBA,
                            pixelData->getWidth(),
                            pixelData->getHeight(),
                            1);
                        sws_scale(
                            _swsContext,
                            (uint8_t const * const *)_avFrame->data,
                            _avFrame->linesize,
                            0,
                            _avCodecParameters[_avVideoStream]->height,
                            _avFrameRgb->data,
                            _avFrameRgb->linesize);
                        {
                            std::lock_guard<std::mutex> lock(_queue->getMutex());
                            _queue->addVideo(pts, pixelData);
                        }
                    }
                }
                return pts;
            }

            Timestamp Loader::_decodeAudio(AVPacket * packet, bool seek)
            {
                int r = avcodec_send_packet(_avCodecContext[_avAudioStream], packet);
                if (r < 0)
                {
                    return r;
                }
                Timestamp pts = 0;
                while (r >= 0)
                {
                    r = avcodec_receive_frame(_avCodecContext[_avAudioStream], _avFrame);
                    if (r == AVERROR(EAGAIN))
                    {
                        return pts;
                    }
                    else if (r < 0)
                    {
                        return r;
                    }

                    pts = av_rescale_q(
                        _avFrame->pts,
                        _avFormatContext->streams[_avVideoStream]->time_base,
                        FFmpeg::timeBaseQ());

                    if (!seek)
                    {
                        const auto & info = _info.getAudio().getInfo();
                        auto audioData = AudioData::create(info, _avFrame->nb_samples * info.getChannels());
                        switch (_avCodecParameters[_avAudioStream]->format)
                        {
                        case AV_SAMPLE_FMT_U8:
                        case AV_SAMPLE_FMT_S16:
                        case AV_SAMPLE_FMT_S32:
                        case AV_SAMPLE_FMT_FLT:
                            memcpy(audioData->getData(), _avFrame->data[0], audioData->getByteCount());
                            break;
                        case AV_SAMPLE_FMT_U8P:
                        case AV_SAMPLE_FMT_S16P:
                        case AV_SAMPLE_FMT_S32P:
                        case AV_SAMPLE_FMT_FLTP:
                        {
                            auto p = audioData->getData();
                            const size_t byteCount = audioData->getByteCount() / info.getChannels();
                            for (size_t i = 0; i < info.getChannels(); ++i, p += byteCount)
                            {
                                memcpy(p, _avFrame->data[0], byteCount);
                            }
                            audioData = AudioData::planarInterleave(audioData);
                            break;
                        }
                        }
                        {
                            std::lock_guard<std::mutex> lock(_queue->getMutex());
                            _queue->addAudio(pts, audioData);
                        }
                    }
                }
                return pts;
            }

            double Util::timestampToSeconds(Timestamp value)
            {
                return value / static_cast<double>(AV_TIME_BASE);
            }

            Timestamp Util::secondsToTimestamp(double value)
            {
                return static_cast<Timestamp>(value * static_cast<double>(AV_TIME_BASE));
            }

        } // namespace IO
    } // namespace AV
} // namespace djv

