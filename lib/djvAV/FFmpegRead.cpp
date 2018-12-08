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

#include <djvCore/Context.h>
#include <djvCore/Vector.h>

extern "C"
{
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>

} // extern "C"

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
                    const size_t timeout = 10;

                } // namespace

                struct Read::Private
                {
                    VideoInfo videoInfo;
                    AudioInfo audioInfo;
                    std::promise<Info> infoPromise;
                    std::condition_variable queueCV;
                    Timestamp seek = -1;
                    std::thread thread;
                    std::atomic<bool> running;

                    AVFormatContext * avFormatContext = nullptr;
                    int avVideoStream = -1;
                    int avAudioStream = -1;
                    std::map<int, AVCodecParameters *> avCodecParameters;
                    std::map<int, AVCodecContext *> avCodecContext;
                    AVFrame * avFrame = nullptr;
                    AVFrame * avFrameRgb = nullptr;
                    SwsContext * swsContext = nullptr;
                };

                void Read::_init(
                    const std::string & fileName,
                    const std::shared_ptr<Queue> & queue,
                    const std::shared_ptr<Core::Context> & context)
                {
                    IRead::_init(fileName, queue, context);

                    _p->running = true;
                    _p->thread = std::thread(
                        [this, fileName]
                    {
                        try
                        {
                            // Open the file.
                            if (auto context = _context.lock())
                            {
                                std::stringstream ss;
                                ss << "Reading file: " << fileName << std::endl;
                                context->log("djv::AV::IO::FFmpeg::Read", ss.str());
                            }
                            int r = avformat_open_input(
                                &_p->avFormatContext,
                                fileName.c_str(),
                                nullptr,
                                nullptr);
                            if (r < 0)
                            {
                                throw Core::Error(
                                    "djv::AV::IO::FFmpeg::Read",
                                    FFmpeg::getErrorString(r));
                            }
                            r = avformat_find_stream_info(_p->avFormatContext, 0);
                            if (r < 0)
                            {
                                throw Core::Error(
                                    "djv::AV::IO::FFmpeg::Read",
                                    FFmpeg::getErrorString(r));
                            }
                            av_dump_format(_p->avFormatContext, 0, fileName.c_str(), 0);

                            // Find the first video and audio stream.
                            for (unsigned int i = 0; i < _p->avFormatContext->nb_streams; ++i)
                            {
                                if (-1 == _p->avVideoStream && _p->avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
                                {
                                    _p->avVideoStream = i;
                                }
                                if (-1 == _p->avAudioStream && _p->avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
                                {
                                    _p->avAudioStream = i;
                                }
                            }
                            if (-1 == _p->avVideoStream && -1 == _p->avAudioStream)
                            {
                                throw Core::Error(
                                    "djv::AV::IO::FFmpeg::Read",
                                    DJV_TEXT("Cannot find any streams"));
                            }

                            Info info;
                            info.setFileName(fileName);

                            if (_p->avVideoStream != -1)
                            {
                                // Find the codec for the video stream.
                                auto avVideoStream = _p->avFormatContext->streams[_p->avVideoStream];
                                auto avVideoCodecParameters = avVideoStream->codecpar;
                                auto avVideoCodec = avcodec_find_decoder(avVideoCodecParameters->codec_id);
                                if (!avVideoCodec)
                                {
                                    throw Core::Error(
                                        "djv::AV::IO::FFmpeg::Read",
                                        DJV_TEXT("Cannot find video codec"));
                                }
                                _p->avCodecParameters[_p->avVideoStream] = avcodec_parameters_alloc();
                                r = avcodec_parameters_copy(_p->avCodecParameters[_p->avVideoStream], avVideoCodecParameters);
                                if (r < 0)
                                {
                                    throw Core::Error(
                                        "djv::AV::IO::FFmpeg::Read",
                                        FFmpeg::getErrorString(r));
                                }
                                _p->avCodecContext[_p->avVideoStream] = avcodec_alloc_context3(avVideoCodec);
                                r = avcodec_parameters_to_context(_p->avCodecContext[_p->avVideoStream], _p->avCodecParameters[_p->avVideoStream]);
                                if (r < 0)
                                {
                                    throw Core::Error(
                                        "djv::AV::IO::FFmpeg::Read",
                                        FFmpeg::getErrorString(r));
                                }
                                r = avcodec_open2(_p->avCodecContext[_p->avVideoStream], avVideoCodec, 0);
                                if (r < 0)
                                {
                                    throw Core::Error(
                                        "djv::AV::IO::FFmpeg::Read",
                                        FFmpeg::getErrorString(r));
                                }

                                // Initialize the buffers.
                                _p->avFrame = av_frame_alloc();
                                _p->avFrameRgb = av_frame_alloc();

                                // Initialize the software scaler.
                                _p->swsContext = sws_getContext(
                                    _p->avCodecParameters[_p->avVideoStream]->width,
                                    _p->avCodecParameters[_p->avVideoStream]->height,
                                    static_cast<AVPixelFormat>(_p->avCodecParameters[_p->avVideoStream]->format),
                                    _p->avCodecParameters[_p->avVideoStream]->width,
                                    _p->avCodecParameters[_p->avVideoStream]->height,
                                    AV_PIX_FMT_RGBA,
                                    SWS_BILINEAR,
                                    0,
                                    0,
                                    0);

                                // Get file information.
                                const auto pixelDataInfo = Pixel::Info(
                                    glm::ivec2(_p->avCodecParameters[_p->avVideoStream]->width, _p->avCodecParameters[_p->avVideoStream]->height),
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
                                else if (_p->avFormatContext->duration != AV_NOPTS_VALUE)
                                {
                                    duration = _p->avFormatContext->duration;
                                }
                                const Speed speed(avVideoStream->r_frame_rate.num, avVideoStream->r_frame_rate.den);
                                _p->videoInfo = VideoInfo(pixelDataInfo, speed, duration);
                                info.setVideo(_p->videoInfo);
                                if (auto context = _context.lock())
                                {
                                    std::stringstream ss;
                                    ss << fileName << ", image size: " << pixelDataInfo.getSize() << std::endl;
                                    ss << fileName << ", pixel type: " << pixelDataInfo.getType() << std::endl;
                                    ss << fileName << ", duration: " << duration << std::endl;
                                    ss << fileName << ", speed: " << speed << std::endl;
                                    context->log("djv::AV::IO::FFmpeg::Read", ss.str());
                                }
                            }

                            if (_p->avAudioStream != -1)
                            {
                                // Find the codec for the audio stream.
                                auto avAudioStream = _p->avFormatContext->streams[_p->avAudioStream];
                                auto avAudioCodecParameters = avAudioStream->codecpar;
                                Audio::Type audioType = FFmpeg::toAudioType(static_cast<AVSampleFormat>(avAudioCodecParameters->format));
                                if (Audio::Type::None == audioType)
                                {
                                    std::stringstream s;
                                    s << DJV_TEXT("Unsupported audio format: ") << FFmpeg::toString(static_cast<AVSampleFormat>(avAudioCodecParameters->format));
                                    throw Core::Error("djv::AV::IO::FFmpeg::Read", s.str());
                                }
                                auto avAudioCodec = avcodec_find_decoder(avAudioCodecParameters->codec_id);
                                if (!avAudioCodec)
                                {
                                    throw Core::Error(
                                        "djv::AV::IO::FFmpeg::Read",
                                        DJV_TEXT("Cannot find audio codec"));
                                }
                                _p->avCodecParameters[_p->avAudioStream] = avcodec_parameters_alloc();
                                r = avcodec_parameters_copy(_p->avCodecParameters[_p->avAudioStream], avAudioCodecParameters);
                                if (r < 0)
                                {
                                    throw Core::Error(
                                        "djv::AV::IO::FFmpeg::Read",
                                        FFmpeg::getErrorString(r));
                                }
                                _p->avCodecContext[_p->avAudioStream] = avcodec_alloc_context3(avAudioCodec);
                                r = avcodec_parameters_to_context(_p->avCodecContext[_p->avAudioStream], _p->avCodecParameters[_p->avAudioStream]);
                                if (r < 0)
                                {
                                    throw Core::Error(
                                        "djv::AV::IO::FFmpeg::Read",
                                        FFmpeg::getErrorString(r));
                                }
                                r = avcodec_open2(_p->avCodecContext[_p->avAudioStream], avAudioCodec, 0);
                                if (r < 0)
                                {
                                    throw Core::Error(
                                        "djv::AV::IO::FFmpeg::Read",
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
                                else if (_p->avFormatContext->duration != AV_NOPTS_VALUE)
                                {
                                    duration = _p->avFormatContext->duration;
                                }
                                _p->audioInfo = AudioInfo(
                                    Audio::DataInfo(
                                        _p->avCodecParameters[_p->avAudioStream]->channels,
                                        audioType,
                                        _p->avCodecParameters[_p->avAudioStream]->sample_rate),
                                    duration);
                                info.setAudio(_p->audioInfo);
                            }

                            _p->infoPromise.set_value(info);

                            while (_queue && _p->running)
                            {
                                bool read = false;
                                Timestamp seek = -1;
                                {
                                    std::unique_lock<std::mutex> lock(_queue->getMutex());
                                    if (_p->queueCV.wait_for(
                                        lock,
                                        std::chrono::milliseconds(timeout),
                                        [this]
                                    {
                                        const bool video = _p->avVideoStream != -1 && (_queue->getVideoCount() < _queue->getVideoMax());
                                        const bool audio = _p->avAudioStream != -1 && (_queue->getAudioCount() < _queue->getAudioMax());
                                        return video || audio || _p->seek != -1;
                                    }))
                                    {
                                        read = true;
                                        if (_p->seek != -1)
                                        {
                                            seek = _p->seek;
                                            _p->seek = -1;
                                            _queue->setFinished(false);
                                            _queue->clear();
                                        }
                                    }
                                }
                                AVPacket packet;
                                try
                                {
                                    if (seek != -1)
                                    {
                                        if (auto context = _context.lock())
                                        {
                                            //std::stringstream ss;
                                            //ss << fileName << ", seek: " << seek;
                                            //context->log("djv::AV::IO::FFmpeg::Read", ss.str());
                                        }
                                        if (av_seek_frame(
                                            _p->avFormatContext,
                                            -1,
                                            seek,
                                            AVSEEK_FLAG_BACKWARD) < 0)
                                        {
                                            throw std::exception();
                                        }
                                        if (_p->avVideoStream != -1)
                                        {
                                            avcodec_flush_buffers(_p->avCodecContext[_p->avVideoStream]);
                                        }
                                        if (_p->avAudioStream != -1)
                                        {
                                            avcodec_flush_buffers(_p->avCodecContext[_p->avAudioStream]);
                                        }
                                        Timestamp pts = 0;
                                        while (pts < seek - 1)
                                        {
                                            if (av_read_frame(_p->avFormatContext, &packet) < 0)
                                            {
                                                if (_p->avVideoStream != -1)
                                                {
                                                    _decodeVideo(nullptr);
                                                    avcodec_flush_buffers(_p->avCodecContext[_p->avVideoStream]);
                                                }
                                                if (_p->avAudioStream != -1)
                                                {
                                                    _decodeAudio(nullptr);
                                                    avcodec_flush_buffers(_p->avCodecContext[_p->avAudioStream]);
                                                }
                                                throw std::exception();
                                            }
                                            if (_p->avVideoStream == packet.stream_index)
                                            {
                                                Timestamp r = _decodeVideo(&packet, true);
                                                if (r < 0)
                                                {
                                                    throw std::exception();
                                                }
                                                pts = r;
                                            }
                                            else if (_p->avAudioStream == packet.stream_index)
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
                                        if (av_read_frame(_p->avFormatContext, &packet) < 0)
                                        {
                                            if (_p->avVideoStream != -1)
                                            {
                                                _decodeVideo(nullptr);
                                                avcodec_flush_buffers(_p->avCodecContext[_p->avVideoStream]);
                                            }
                                            if (_p->avAudioStream != -1)
                                            {
                                                _decodeAudio(nullptr);
                                                avcodec_flush_buffers(_p->avCodecContext[_p->avAudioStream]);
                                            }
                                            throw std::exception();
                                        }
                                        if (_p->avVideoStream == packet.stream_index)
                                        {
                                            if (_decodeVideo(&packet) < 0)
                                            {
                                                throw std::exception();
                                            }
                                        }
                                        else if (_p->avAudioStream == packet.stream_index)
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
                                    if (auto context = _context.lock())
                                    {
                                        std::stringstream ss;
                                        ss << fileName << ", finished";
                                        context->log("djv::AV::IO::FFmpeg::Read", ss.str());
                                    }
                                    av_packet_unref(&packet);
                                    std::lock_guard<std::mutex> lock(_queue->getMutex());
                                    _queue->setFinished(true);
                                    if (_queue->hasCloseOnFinish())
                                    {
                                        _p->running = false;
                                    }
                                }
                            }
                        }
                        catch (const Core::Error & error)
                        {
                            if (auto context = _context.lock())
                            {
                                context->log("djvAV::IO::FFmpeg::Read", error.what(), Core::LogLevel::Error);
                            }
                        }
                    });
                }

                Read::Read() :
                    _p(new Private)
                {}

                Read::~Read()
                {
                    _p->thread.join();
                }

                std::shared_ptr<Read> Read::create(const std::string & fileName, const std::shared_ptr<Queue> & queue, const std::shared_ptr<Core::Context> & context)
                {
                    auto out = std::shared_ptr<Read>(new Read);
                    out->_init(fileName, queue, context);
                    return out;
                }

                std::future<Info> Read::getInfo()
                {
                    return _p->infoPromise.get_future();
                }

                void Read::seek(Timestamp value)
                {
                    std::lock_guard<std::mutex> lock(_queue->getMutex());
                    _p->seek = value;
                    _p->queueCV.notify_one();
                }

                Timestamp Read::_decodeVideo(AVPacket * packet, bool seek)
                {
                    int r = avcodec_send_packet(_p->avCodecContext[_p->avVideoStream], packet);
                    if (r < 0)
                    {
                        return r;
                    }
                    Timestamp pts = 0;
                    while (r >= 0)
                    {
                        r = avcodec_receive_frame(_p->avCodecContext[_p->avVideoStream], _p->avFrame);
                        if (AVERROR(EAGAIN) == r)
                        {
                            return pts;
                        }
                        else if (r < 0)
                        {
                            return r;
                        }

                        pts = av_rescale_q(
                            _p->avFrame->pts,
                            _p->avFormatContext->streams[_p->avVideoStream]->time_base,
                            FFmpeg::getTimeBaseQ());

                        if (auto context = _context.lock())
                        {
                            //std::stringstream ss;
                            //ss << _fileName << ", decoded frame: " << pts;
                            //context->log("djv::AV::IO::FFmpeg::Read", ss.str());
                        }

                        if (!seek)
                        {
                            auto image = Image::create(_p->videoInfo.getInfo());
                            av_image_fill_arrays(
                                _p->avFrameRgb->data,
                                _p->avFrameRgb->linesize,
                                image->getData(),
                                AV_PIX_FMT_RGBA,
                                image->getWidth(),
                                image->getHeight(),
                                1);
                            sws_scale(
                                _p->swsContext,
                                (uint8_t const * const *)_p->avFrame->data,
                                _p->avFrame->linesize,
                                0,
                                _p->avCodecParameters[_p->avVideoStream]->height,
                                _p->avFrameRgb->data,
                                _p->avFrameRgb->linesize);
                            {
                                std::lock_guard<std::mutex> lock(_queue->getMutex());
                                _queue->addVideo(pts, image);
                            }
                        }
                    }
                    return pts;
                }

                Timestamp Read::_decodeAudio(AVPacket * packet, bool seek)
                {
                    int r = avcodec_send_packet(_p->avCodecContext[_p->avAudioStream], packet);
                    if (r < 0)
                    {
                        return r;
                    }
                    Timestamp pts = 0;
                    while (r >= 0)
                    {
                        r = avcodec_receive_frame(_p->avCodecContext[_p->avAudioStream], _p->avFrame);
                        if (r == AVERROR(EAGAIN))
                        {
                            return pts;
                        }
                        else if (r < 0)
                        {
                            return r;
                        }

                        pts = av_rescale_q(
                            _p->avFrame->pts,
                            _p->avFormatContext->streams[_p->avVideoStream]->time_base,
                            FFmpeg::getTimeBaseQ());

                        if (!seek)
                        {
                            const auto & info = _p->audioInfo.getInfo();
                            auto audioData = Audio::Data::create(info, _p->avFrame->nb_samples * info.getChannels());
                            switch (_p->avCodecParameters[_p->avAudioStream]->format)
                            {
                            case AV_SAMPLE_FMT_U8:
                            case AV_SAMPLE_FMT_S16:
                            case AV_SAMPLE_FMT_S32:
                            case AV_SAMPLE_FMT_FLT:
                                memcpy(audioData->getData(), _p->avFrame->data[0], audioData->getByteCount());
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
                                    memcpy(p, _p->avFrame->data[0], byteCount);
                                }
                                audioData = Audio::Data::planarInterleave(audioData);
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

            } // namespace FFmpeg
        } // namespace IO
    } // namespace AV
} // namespace djv

