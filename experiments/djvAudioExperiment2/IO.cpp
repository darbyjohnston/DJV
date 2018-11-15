//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
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

#include <IO.h>

#include <djvCore/Error.h>

#include <QCoreApplication>

extern "C"
{
#include <libavutil/imgutils.h>

} // extern "C"

using namespace djv;

namespace djv
{
    namespace AudioExperiment2
    {
        namespace
        {
            const size_t videoFrames = 100;
            const size_t audioFrames = 100;
            const size_t timeout = 10;

        } // namespace

        IO::IO(const Core::FileInfo & fileInfo, const QPointer<Context> & context, QObject * parent) :
            QObject(parent),
            _context(context),
            _queue(new Util::AVQueue)
        {
            _running = true;
            _thread = std::thread(
                [this, fileInfo]
            {
                DJV_DEBUG("IO::IO");
                DJV_DEBUG_PRINT("file info = " << fileInfo);
                try
                {
                    // Open the file.
                    int r = avformat_open_input(
                        &_avFormatContext,
                        fileInfo.fileName().toUtf8().data(),
                        nullptr,
                        nullptr);
                    if (r < 0)
                    {
                        throw Core::Error(
                            "IO",
                            AV::FFmpeg::toString(r));
                    }
                    r = avformat_find_stream_info(_avFormatContext, 0);
                    if (r < 0)
                    {
                        throw Core::Error(
                            "IO",
                            AV::FFmpeg::toString(r));
                    }
                    av_dump_format(_avFormatContext, 0, fileInfo.fileName().toUtf8().data(), 0);

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
                            "IO",
                            qApp->translate("djv::AV::FFmpegLoad", "Cannot find any streams"));
                    }

                    // Find the codec for the video stream.
                    auto avVideoStream = _avFormatContext->streams[_avVideoStream];
                    auto avVideoCodecParameters = avVideoStream->codecpar;
                    auto avVideoCodec = avcodec_find_decoder(avVideoCodecParameters->codec_id);
                    if (!avVideoCodec)
                    {
                        throw Core::Error(
                            "IO",
                            qApp->translate("djv::AV::FFmpegLoad", "Cannot find video codec"));
                    }
                    _avCodecParameters[_avVideoStream] = avcodec_parameters_alloc();
                    r = avcodec_parameters_copy(_avCodecParameters[_avVideoStream], avVideoCodecParameters);
                    if (r < 0)
                    {
                        throw Core::Error(
                            "IO",
                            AV::FFmpeg::toString(r));
                    }
                    _avCodecContext[_avVideoStream] = avcodec_alloc_context3(avVideoCodec);
                    r = avcodec_parameters_to_context(_avCodecContext[_avVideoStream], _avCodecParameters[_avVideoStream]);
                    if (r < 0)
                    {
                        throw Core::Error(
                            "IO",
                            AV::FFmpeg::toString(r));
                    }
                    r = avcodec_open2(_avCodecContext[_avVideoStream], avVideoCodec, 0);
                    if (r < 0)
                    {
                        throw Core::Error(
                            "IO",
                            AV::FFmpeg::toString(r));
                    }

                    // Find the codec for the audio stream.
                    auto avAudioStream = _avFormatContext->streams[_avAudioStream];
                    auto avAudioCodecParameters = avAudioStream->codecpar;
                    AV::Audio::TYPE audioType = AV::FFmpeg::fromFFmpeg(static_cast<AVSampleFormat>(avAudioCodecParameters->format));
                    if (AV::Audio::TYPE_NONE == audioType)
                    {
                        throw Core::Error(
                            "IO",
                            qApp->translate("djv::AV::FFmpegLoad", "Unsupported audio format: %1").arg(AV::FFmpeg::toString(avAudioCodecParameters->format)));
                    }
                    auto avAudioCodec = avcodec_find_decoder(avAudioCodecParameters->codec_id);
                    if (!avAudioCodec)
                    {
                        throw Core::Error(
                            "IO",
                            qApp->translate("djv::AV::FFmpegLoad", "Cannot find audio codec"));
                    }
                    _avCodecParameters[_avAudioStream] = avcodec_parameters_alloc();
                    r = avcodec_parameters_copy(_avCodecParameters[_avAudioStream], avAudioCodecParameters);
                    if (r < 0)
                    {
                        throw Core::Error(
                            "IO",
                            AV::FFmpeg::toString(r));
                    }
                    _avCodecContext[_avAudioStream] = avcodec_alloc_context3(avAudioCodec);
                    r = avcodec_parameters_to_context(_avCodecContext[_avAudioStream], _avCodecParameters[_avAudioStream]);
                    if (r < 0)
                    {
                        throw Core::Error(
                            "IO",
                            AV::FFmpeg::toString(r));
                    }
                    r = avcodec_open2(_avCodecContext[_avAudioStream], avAudioCodec, 0);
                    if (r < 0)
                    {
                        throw Core::Error(
                            "IO",
                            AV::FFmpeg::toString(r));
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
                    _ioInfo.fileInfo = fileInfo;

                    _ioInfo.video.info.size = glm::ivec2(_avCodecParameters[_avVideoStream]->width, _avCodecParameters[_avVideoStream]->height);
                    _ioInfo.video.info.pixel = AV::Pixel::RGBA_U8;
                    _ioInfo.video.info.mirror.y = true;
                    _ioInfo.video.duration = 0;
                    if (avVideoStream->duration != AV_NOPTS_VALUE)
                    {
                        _ioInfo.video.duration = av_rescale_q(
                            avVideoStream->duration,
                            avVideoStream->time_base,
                            AV::FFmpeg::timeBaseQ());
                    }
                    else if (_avFormatContext->duration != AV_NOPTS_VALUE)
                    {
                        _ioInfo.video.duration = _avFormatContext->duration;
                    }
                    const Core::Speed speed(avVideoStream->r_frame_rate.num, avVideoStream->r_frame_rate.den);
                    _ioInfo.video.speed = speed;

                    _ioInfo.audio.channels = _avCodecParameters[_avAudioStream]->channels;
                    _ioInfo.audio.type = audioType;
                    _ioInfo.audio.sampleRate = _avCodecParameters[_avAudioStream]->sample_rate;
                    _ioInfo.audio.duration = 0;
                    if (avAudioStream->duration != AV_NOPTS_VALUE)
                    {
                        _ioInfo.audio.duration = av_rescale_q(
                            avAudioStream->duration,
                            avAudioStream->time_base,
                            AV::FFmpeg::timeBaseQ());
                    }
                    else if (_avFormatContext->duration != AV_NOPTS_VALUE)
                    {
                        _ioInfo.audio.duration = _avFormatContext->duration;
                    }
                    _ioInfoPromise.set_value(_ioInfo);

                    while (_running)
                    {
                        bool read = false;
                        int64_t seek = 0;
                        {
                            std::unique_lock<std::mutex> lock(_queue->mutex);
                            if (_queueCV.wait_for(
                                lock,
                                std::chrono::milliseconds(timeout),
                                [this]
                            {
                                return _queue->video.size() < videoFrames || _queue->audio.size() < audioFrames || _seek;
                            }))
                            {
                                read = _queue->video.size() < videoFrames || _queue->audio.size() < audioFrames;
                                if (_seek)
                                {
                                    seek = _seek;
                                    _seek = 0;
                                    while (_queue->video.size())
                                    {
                                        _queue->video.pop();
                                    }
                                    while (_queue->audio.size())
                                    {
                                        _queue->audio.pop();
                                    }
                                }
                            }
                        }
                        if (seek)
                        {
                            av_seek_frame(
                                _avFormatContext,
                                _avVideoStream,
                                av_rescale_q(seek, AV::FFmpeg::timeBaseQ(), _avFormatContext->streams[_avVideoStream]->time_base),
                                AVSEEK_FLAG_BACKWARD);
                            avcodec_flush_buffers(_avCodecContext[_avVideoStream]);
                            AVPacket packet;
                            int64_t pts = 0;
                            while (av_read_frame(_avFormatContext, &packet) >= 0 && pts < seek)
                            {
                                if (_avVideoStream == packet.stream_index)
                                {
                                    pts = _decodeVideo(&packet, true);
                                }
                                else if (_avAudioStream == packet.stream_index)
                                {
                                    _decodeAudio(&packet, true);
                                }
                                av_packet_unref(&packet);
                            }
                        }
                        if (read)
                        {
                            AVPacket packet;
                            if (av_read_frame(_avFormatContext, &packet) >= 0)
                            {
                                if (_avVideoStream == packet.stream_index)
                                {
                                    _decodeVideo(&packet);
                                }
                                else if (_avAudioStream == packet.stream_index)
                                {
                                    _decodeAudio(&packet);
                                }
                                av_packet_unref(&packet);
                            }
                            else
                            {
                                _decodeVideo(nullptr);
                                _decodeAudio(nullptr);
                            }
                        }
                    }
                }
                catch (const Core::Error & error)
                {
                    _error = error;
                    _running = false;
                }
            });
            if (_error.count())
            {
                throw _error;
            }
        }

        IO::~IO()
        {
            _running = false;
            _thread.join();
        }

        std::future<IOInfo> IO::ioInfo()
        {
            return _ioInfoPromise.get_future();
        }

        std::shared_ptr<Util::AVQueue> IO::queue() const
        {
            return _queue;
        }

        void IO::seek(int64_t value)
        {
            std::lock_guard<std::mutex>(_queue->mutex);
            _seek = value;
        }

        int64_t IO::_decodeVideo(AVPacket * packet, bool seek)
        {
            int64_t pts = 0;
            int r = avcodec_send_packet(_avCodecContext[_avVideoStream], packet);
            if (r < 0)
            {
                //! \todo Error
                return 0;
            }
            //DJV_DEBUG("_decodeVideo");
            while (r >= 0)
            {
                r = avcodec_receive_frame(_avCodecContext[_avVideoStream], _avFrame);
                if (r == AVERROR(EAGAIN) || r == AVERROR_EOF)
                {
                    //char buf[Core::StringUtil::cStringLength];
                    //DJV_DEBUG_PRINT("r = " << av_make_error_string(buf, Core::StringUtil::cStringLength, r));
                    return 0;
                }

                pts = av_rescale_q(
                    _avFrame->pts,
                    _avFormatContext->streams[_avVideoStream]->time_base,
                    AV::FFmpeg::timeBaseQ());
                //DJV_DEBUG_PRINT("pts = " << pts);

                if (!seek)
                {
                    auto pixelData = std::make_shared<AV::PixelData>(_ioInfo.video.info);
                    av_image_fill_arrays(
                        _avFrameRgb->data,
                        _avFrameRgb->linesize,
                        pixelData->data(),
                        AV_PIX_FMT_RGBA,
                        pixelData->w(),
                        pixelData->h(),
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
                        std::lock_guard<std::mutex> lock(_queue->mutex);
                        _queue->video.push(std::make_pair(pts, pixelData));
                    }
                }
            }
            return pts;
        }

        int64_t IO::_decodeAudio(AVPacket * packet, bool seek)
        {
            int64_t pts = 0;
            int r = avcodec_send_packet(_avCodecContext[_avAudioStream], packet);
            if (r < 0)
            {
                //! \todo Error
                return 0;
            }
            //DJV_DEBUG("_decodeAudio");
            while (r >= 0)
            {
                r = avcodec_receive_frame(_avCodecContext[_avAudioStream], _avFrame);
                if (r == AVERROR(EAGAIN) || r == AVERROR_EOF)
                    return 0;

                pts = av_rescale_q(
                    _avFrame->pts,
                    _avFormatContext->streams[_avVideoStream]->time_base,
                    AV::FFmpeg::timeBaseQ());
                //DJV_DEBUG_PRINT("pts = " << pts);

                if (!seek)
                {
                    AV::AudioInfo info;
                    info.channels = _ioInfo.audio.channels;
                    info.type = _ioInfo.audio.type;
                    info.sampleRate = _ioInfo.audio.sampleRate;
                    info.sampleCount = _avFrame->nb_samples * _ioInfo.audio.channels;
                    //DJV_DEBUG_PRINT("sample count = " << info.sampleCount);
                    auto audioData = std::make_shared<AV::AudioData>(info);
                    switch (_avCodecParameters[_avAudioStream]->format)
                    {
                    case AV_SAMPLE_FMT_U8:
                    case AV_SAMPLE_FMT_S16:
                    case AV_SAMPLE_FMT_S32:
                    case AV_SAMPLE_FMT_FLT:
                        memcpy(audioData->data(), _avFrame->data[0], audioData->byteCount());
                        break;
                    case AV_SAMPLE_FMT_U8P:
                    case AV_SAMPLE_FMT_S16P:
                    case AV_SAMPLE_FMT_S32P:
                    case AV_SAMPLE_FMT_FLTP:
                    {
                        auto p = audioData->data();
                        const size_t byteCount = audioData->byteCount() / info.channels;
                        for (size_t i = 0; i < info.channels; ++i, p += byteCount)
                        {
                            memcpy(p, _avFrame->data[0], byteCount);
                        }
                        *audioData = AV::AudioData::planarInterleave(*audioData);
                        break;
                    }
                    }
                    {
                        std::lock_guard<std::mutex> lock(_queue->mutex);
                        _queue->audio.push(std::make_pair(pts, audioData));
                    }
                }
            }
            return pts;
        }

    } // namespace AudioExperiment2
} // namespace djv
