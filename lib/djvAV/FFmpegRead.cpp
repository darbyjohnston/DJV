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

#include <djvAV/FFmpeg.h>

extern "C"
{
#include <libavutil/imgutils.h>

} // extern "C"

#include <sstream>

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace FFmpeg
            {
                namespace
                {
                    const size_t videoFrameQueueMax = 100;
                    const size_t audioFrameQueueMax = 100;
                    size_t videoFrameQueue = videoFrameQueueMax;
                    size_t audioFrameQueue = audioFrameQueueMax;
                    const size_t timeout = 10;

                } // namespace

                void Read::_init(
                    const std::string & fileName,
                    const std::shared_ptr<ReadQueue> & queue,
                    const std::shared_ptr<Core::Context> & context)
                {
                    IRead::_init(fileName, queue, context);

                    _running = true;
                    _thread = std::thread(
                        [this, fileName]
                    {
                        try
                        {
                            // Open the file.
                            int r = avformat_open_input(
                                &_avFormatContext,
                                fileName.c_str(),
                                nullptr,
                                nullptr);
                            if (r < 0)
                            {
                                throw Core::Error(
                                    "djv::AV::FFmpeg::Read",
                                    FFmpeg::getErrorString(r));
                            }
                            r = avformat_find_stream_info(_avFormatContext, 0);
                            if (r < 0)
                            {
                                throw Core::Error(
                                    "djv::AV::FFmpeg::Read",
                                    FFmpeg::getErrorString(r));
                            }
                            av_dump_format(_avFormatContext, 0, fileName.c_str(), 0);

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
                                    "djv::AV::FFmpeg::Read",
                                    DJV_TEXT("Cannot find any streams"));
                            }

                            Info info;
                            info.setFileName(fileName);

                            if (_avVideoStream != -1)
                            {
                                // Find the codec for the video stream.
                                auto avVideoStream = _avFormatContext->streams[_avVideoStream];
                                auto avVideoCodecParameters = avVideoStream->codecpar;
                                auto avVideoCodec = avcodec_find_decoder(avVideoCodecParameters->codec_id);
                                if (!avVideoCodec)
                                {
                                    throw Core::Error(
                                        "djv::AV::FFmpeg::Read",
                                        DJV_TEXT("Cannot find video codec"));
                                }
                                _avCodecParameters[_avVideoStream] = avcodec_parameters_alloc();
                                r = avcodec_parameters_copy(_avCodecParameters[_avVideoStream], avVideoCodecParameters);
                                if (r < 0)
                                {
                                    throw Core::Error(
                                        "djv::AV::FFmpeg::Read",
                                        FFmpeg::getErrorString(r));
                                }
                                _avCodecContext[_avVideoStream] = avcodec_alloc_context3(avVideoCodec);
                                r = avcodec_parameters_to_context(_avCodecContext[_avVideoStream], _avCodecParameters[_avVideoStream]);
                                if (r < 0)
                                {
                                    throw Core::Error(
                                        "djv::AV::FFmpeg::Read",
                                        FFmpeg::getErrorString(r));
                                }
                                r = avcodec_open2(_avCodecContext[_avVideoStream], avVideoCodec, 0);
                                if (r < 0)
                                {
                                    throw Core::Error(
                                        "djv::AV::FFmpeg::Read",
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
                                const auto pixelDataInfo = Pixel::Info(
                                    glm::ivec2(_avCodecParameters[_avVideoStream]->width, _avCodecParameters[_avVideoStream]->height),
                                    Pixel::Type::RGBA_U8,
                                    Pixel::Layout(Pixel::Mirror(false, true)));
                                Duration duration = 0;
                                if (avVideoStream->duration != AV_NOPTS_VALUE)
                                {
                                    duration = av_rescale_q(
                                        avVideoStream->duration,
                                        avVideoStream->time_base,
                                        FFmpeg::getTimeBaseQ());
                                }
                                else if (_avFormatContext->duration != AV_NOPTS_VALUE)
                                {
                                    duration = _avFormatContext->duration;
                                }
                                const Speed speed(avVideoStream->r_frame_rate.num, avVideoStream->r_frame_rate.den);
                                _videoInfo = VideoInfo(pixelDataInfo, speed, duration);
                                info.setVideo(_videoInfo);
                            }

                            if (_avAudioStream != -1)
                            {
                                // Find the codec for the audio stream.
                                auto avAudioStream = _avFormatContext->streams[_avAudioStream];
                                auto avAudioCodecParameters = avAudioStream->codecpar;
                                Audio::Type audioType = FFmpeg::toAudioType(static_cast<AVSampleFormat>(avAudioCodecParameters->format));
                                if (Audio::Type::None == audioType)
                                {
                                    std::stringstream s;
                                    s << DJV_TEXT("Unsupported audio format: ") << FFmpeg::toString(static_cast<AVSampleFormat>(avAudioCodecParameters->format));
                                    throw Core::Error("djv::AV::FFmpeg::Read", s.str());
                                }
                                auto avAudioCodec = avcodec_find_decoder(avAudioCodecParameters->codec_id);
                                if (!avAudioCodec)
                                {
                                    throw Core::Error(
                                        "djv::AV::FFmpeg::Read",
                                        DJV_TEXT("Cannot find audio codec"));
                                }
                                _avCodecParameters[_avAudioStream] = avcodec_parameters_alloc();
                                r = avcodec_parameters_copy(_avCodecParameters[_avAudioStream], avAudioCodecParameters);
                                if (r < 0)
                                {
                                    throw Core::Error(
                                        "djv::AV::FFmpeg::Read",
                                        FFmpeg::getErrorString(r));
                                }
                                _avCodecContext[_avAudioStream] = avcodec_alloc_context3(avAudioCodec);
                                r = avcodec_parameters_to_context(_avCodecContext[_avAudioStream], _avCodecParameters[_avAudioStream]);
                                if (r < 0)
                                {
                                    throw Core::Error(
                                        "djv::AV::FFmpeg::Read",
                                        FFmpeg::getErrorString(r));
                                }
                                r = avcodec_open2(_avCodecContext[_avAudioStream], avAudioCodec, 0);
                                if (r < 0)
                                {
                                    throw Core::Error(
                                        "djv::AV::FFmpeg::Read",
                                        FFmpeg::getErrorString(r));
                                }

                                // Get file information.
                                Duration duration = 0;
                                if (avAudioStream->duration != AV_NOPTS_VALUE)
                                {
                                    duration = av_rescale_q(
                                        avAudioStream->duration,
                                        avAudioStream->time_base,
                                        FFmpeg::getTimeBaseQ());
                                }
                                else if (_avFormatContext->duration != AV_NOPTS_VALUE)
                                {
                                    duration = _avFormatContext->duration;
                                }
                                _audioInfo = AudioInfo(
                                    Audio::DataInfo(
                                        _avCodecParameters[_avAudioStream]->channels,
                                        audioType,
                                        _avCodecParameters[_avAudioStream]->sample_rate),
                                    duration);
                                info.setAudio(_audioInfo);
                            }

                            _infoPromise.set_value(info);

                            while (_queue && _running)
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
                                                (_avVideoStream != -1 && _queue->getVideoFrameCount() < videoFrameQueue) ||
                                                (_avAudioStream != -1 && _queue->getAudioFrameCount() < audioFrameQueue) ||
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
                                            -1,
                                            seek,
                                            AVSEEK_FLAG_BACKWARD) < 0)
                                        {
                                            throw std::exception();
                                        }
                                        if (_avVideoStream != -1)
                                        {
                                            avcodec_flush_buffers(_avCodecContext[_avVideoStream]);
                                        }
                                        if (_avAudioStream != -1)
                                        {
                                            avcodec_flush_buffers(_avCodecContext[_avAudioStream]);
                                        }
                                        Timestamp pts = 0;
                                        while (pts < seek - 1)
                                        {
                                            if (av_read_frame(_avFormatContext, &packet) < 0)
                                            {
                                                if (_avVideoStream != -1)
                                                {
                                                    _decodeVideo(nullptr);
                                                    avcodec_flush_buffers(_avCodecContext[_avVideoStream]);
                                                }
                                                if (_avAudioStream != -1)
                                                {
                                                    _decodeAudio(nullptr);
                                                    avcodec_flush_buffers(_avCodecContext[_avAudioStream]);
                                                }
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
                                            if (_avVideoStream != -1)
                                            {
                                                _decodeVideo(nullptr);
                                                avcodec_flush_buffers(_avCodecContext[_avVideoStream]);
                                            }
                                            if (_avAudioStream != -1)
                                            {
                                                _decodeAudio(nullptr);
                                                avcodec_flush_buffers(_avCodecContext[_avAudioStream]);
                                            }
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

                Read::Read()
                {}

                Read::~Read()
                {
                    _running = false;
                    _thread.join();
                }

                std::shared_ptr<Read> Read::create(const std::string & fileName, const std::shared_ptr<ReadQueue> & queue, const std::shared_ptr<Core::Context> & context)
                {
                    auto out = std::shared_ptr<Read>(new Read);
                    out->_init(fileName, queue, context);
                    return out;
                }

                std::future<Info> Read::getInfo()
                {
                    return _infoPromise.get_future();
                }

                void Read::seek(Timestamp value)
                {
                    std::lock_guard<std::mutex> lock(_queue->getMutex());
                    _seek = value;
                }

                Timestamp Read::_decodeVideo(AVPacket * packet, bool seek)
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
                            FFmpeg::getTimeBaseQ());

                        if (!seek)
                        {
                            auto image = Image::create(_videoInfo.getInfo());
                            av_image_fill_arrays(
                                _avFrameRgb->data,
                                _avFrameRgb->linesize,
                                image->getData(),
                                AV_PIX_FMT_RGBA,
                                image->getWidth(),
                                image->getHeight(),
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
                                _queue->addVideoFrame(pts, image);
                            }
                        }
                    }
                    return pts;
                }

                Timestamp Read::_decodeAudio(AVPacket * packet, bool seek)
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
                            FFmpeg::getTimeBaseQ());

                        if (!seek)
                        {
                            const auto & info = _audioInfo.getInfo();
                            auto audioData = Audio::Data::create(info, _avFrame->nb_samples * info.getChannels());
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
                                audioData = Audio::Data::planarInterleave(audioData);
                                break;
                            }
                            }
                            {
                                std::lock_guard<std::mutex> lock(_queue->getMutex());
                                _queue->addAudioFrame(pts, audioData);
                            }
                        }
                    }
                    return pts;
                }

            } // namespace FFmpeg
        } // namespace IO
    } // namespace AV
} // namespace djv

