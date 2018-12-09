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
#include <djvCore/Timer.h>
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
                        DJV_PRIVATE_PTR();
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
                                &p.avFormatContext,
                                fileName.c_str(),
                                nullptr,
                                nullptr);
                            if (r < 0)
                            {
                                throw Core::Error(
                                    "djv::AV::IO::FFmpeg::Read",
                                    FFmpeg::getErrorString(r));
                            }
                            r = avformat_find_stream_info(p.avFormatContext, 0);
                            if (r < 0)
                            {
                                throw Core::Error(
                                    "djv::AV::IO::FFmpeg::Read",
                                    FFmpeg::getErrorString(r));
                            }
                            av_dump_format(p.avFormatContext, 0, fileName.c_str(), 0);

                            // Find the first video and audio stream.
                            for (unsigned int i = 0; i < p.avFormatContext->nb_streams; ++i)
                            {
                                if (-1 == p.avVideoStream && p.avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
                                {
                                    p.avVideoStream = i;
                                }
                                if (-1 == p.avAudioStream && p.avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
                                {
                                    p.avAudioStream = i;
                                }
                            }
                            if (-1 == p.avVideoStream && -1 == p.avAudioStream)
                            {
                                throw Core::Error(
                                    "djv::AV::IO::FFmpeg::Read",
                                    DJV_TEXT("Cannot find any streams"));
                            }

                            Info info;
                            info.fileName = fileName;

                            if (p.avVideoStream != -1)
                            {
                                // Find the codec for the video stream.
                                auto avVideoStream = p.avFormatContext->streams[p.avVideoStream];
                                auto avVideoCodecParameters = avVideoStream->codecpar;
                                auto avVideoCodec = avcodec_find_decoder(avVideoCodecParameters->codec_id);
                                if (!avVideoCodec)
                                {
                                    throw Core::Error(
                                        "djv::AV::IO::FFmpeg::Read",
                                        DJV_TEXT("Cannot find video codec"));
                                }
                                p.avCodecParameters[p.avVideoStream] = avcodec_parameters_alloc();
                                r = avcodec_parameters_copy(p.avCodecParameters[p.avVideoStream], avVideoCodecParameters);
                                if (r < 0)
                                {
                                    throw Core::Error(
                                        "djv::AV::IO::FFmpeg::Read",
                                        FFmpeg::getErrorString(r));
                                }
                                p.avCodecContext[p.avVideoStream] = avcodec_alloc_context3(avVideoCodec);
                                r = avcodec_parameters_to_context(p.avCodecContext[p.avVideoStream], p.avCodecParameters[p.avVideoStream]);
                                if (r < 0)
                                {
                                    throw Core::Error(
                                        "djv::AV::IO::FFmpeg::Read",
                                        FFmpeg::getErrorString(r));
                                }
                                r = avcodec_open2(p.avCodecContext[p.avVideoStream], avVideoCodec, 0);
                                if (r < 0)
                                {
                                    throw Core::Error(
                                        "djv::AV::IO::FFmpeg::Read",
                                        FFmpeg::getErrorString(r));
                                }

                                // Initialize the buffers.
                                p.avFrame = av_frame_alloc();
                                p.avFrameRgb = av_frame_alloc();

                                // Initialize the software scaler.
                                p.swsContext = sws_getContext(
                                    p.avCodecParameters[p.avVideoStream]->width,
                                    p.avCodecParameters[p.avVideoStream]->height,
                                    static_cast<AVPixelFormat>(p.avCodecParameters[p.avVideoStream]->format),
                                    p.avCodecParameters[p.avVideoStream]->width,
                                    p.avCodecParameters[p.avVideoStream]->height,
                                    AV_PIX_FMT_RGBA,
                                    SWS_BILINEAR,
                                    0,
                                    0,
                                    0);

                                // Get file information.
                                const auto pixelDataInfo = Pixel::Info(
                                    glm::ivec2(p.avCodecParameters[p.avVideoStream]->width, p.avCodecParameters[p.avVideoStream]->height),
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
                                else if (p.avFormatContext->duration != AV_NOPTS_VALUE)
                                {
                                    duration = p.avFormatContext->duration;
                                }
                                const Speed speed(avVideoStream->r_frame_rate.num, avVideoStream->r_frame_rate.den);
                                p.videoInfo = VideoInfo(pixelDataInfo, speed, duration);
                                info.setVideo(p.videoInfo);
                                if (auto context = _context.lock())
                                {
                                    std::stringstream ss;
                                    ss << fileName << ", image size: " << pixelDataInfo.size << std::endl;
                                    ss << fileName << ", pixel type: " << pixelDataInfo.type << std::endl;
                                    ss << fileName << ", duration: " << duration << std::endl;
                                    ss << fileName << ", speed: " << speed << std::endl;
                                    context->log("djv::AV::IO::FFmpeg::Read", ss.str());
                                }
                            }

                            if (p.avAudioStream != -1)
                            {
                                // Find the codec for the audio stream.
                                auto avAudioStream = p.avFormatContext->streams[p.avAudioStream];
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
                                p.avCodecParameters[p.avAudioStream] = avcodec_parameters_alloc();
                                r = avcodec_parameters_copy(p.avCodecParameters[p.avAudioStream], avAudioCodecParameters);
                                if (r < 0)
                                {
                                    throw Core::Error(
                                        "djv::AV::IO::FFmpeg::Read",
                                        FFmpeg::getErrorString(r));
                                }
                                p.avCodecContext[p.avAudioStream] = avcodec_alloc_context3(avAudioCodec);
                                r = avcodec_parameters_to_context(p.avCodecContext[p.avAudioStream], p.avCodecParameters[p.avAudioStream]);
                                if (r < 0)
                                {
                                    throw Core::Error(
                                        "djv::AV::IO::FFmpeg::Read",
                                        FFmpeg::getErrorString(r));
                                }
                                r = avcodec_open2(p.avCodecContext[p.avAudioStream], avAudioCodec, 0);
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
                                else if (p.avFormatContext->duration != AV_NOPTS_VALUE)
                                {
                                    duration = p.avFormatContext->duration;
                                }
                                p.audioInfo = AudioInfo(
                                    Audio::DataInfo(
                                        p.avCodecParameters[p.avAudioStream]->channels,
                                        audioType,
                                        p.avCodecParameters[p.avAudioStream]->sample_rate),
                                    duration);
                                info.setAudio(p.audioInfo);
                            }

                            p.infoPromise.set_value(info);

                            while (_queue && p.running)
                            {
                                bool read = false;
                                Timestamp seek = -1;
                                {
                                    std::unique_lock<std::mutex> lock(_queue->getMutex());
                                    if (p.queueCV.wait_for(
                                        lock,
                                        Core::Timer::getMilliseconds(Core::Timer::Value::Fast),
                                        [this]
                                    {
                                        DJV_PRIVATE_PTR();
                                        const bool video = p.avVideoStream != -1 && (_queue->getVideoCount() < _queue->getVideoMax());
                                        const bool audio = p.avAudioStream != -1 && (_queue->getAudioCount() < _queue->getAudioMax());
                                        return video || audio || p.seek != -1;
                                    }))
                                    {
                                        read = true;
                                        if (p.seek != -1)
                                        {
                                            seek = p.seek;
                                            p.seek = -1;
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
                                            p.avFormatContext,
                                            -1,
                                            seek,
                                            AVSEEK_FLAG_BACKWARD) < 0)
                                        {
                                            throw std::exception();
                                        }
                                        if (p.avVideoStream != -1)
                                        {
                                            avcodec_flush_buffers(p.avCodecContext[p.avVideoStream]);
                                        }
                                        if (p.avAudioStream != -1)
                                        {
                                            avcodec_flush_buffers(p.avCodecContext[p.avAudioStream]);
                                        }
                                        Timestamp pts = 0;
                                        while (pts < seek - 1)
                                        {
                                            if (av_read_frame(p.avFormatContext, &packet) < 0)
                                            {
                                                if (p.avVideoStream != -1)
                                                {
                                                    _decodeVideo(nullptr);
                                                    avcodec_flush_buffers(p.avCodecContext[p.avVideoStream]);
                                                }
                                                if (p.avAudioStream != -1)
                                                {
                                                    _decodeAudio(nullptr);
                                                    avcodec_flush_buffers(p.avCodecContext[p.avAudioStream]);
                                                }
                                                throw std::exception();
                                            }
                                            if (p.avVideoStream == packet.stream_index)
                                            {
                                                Timestamp r = _decodeVideo(&packet, true);
                                                if (r < 0)
                                                {
                                                    throw std::exception();
                                                }
                                                pts = r;
                                            }
                                            else if (p.avAudioStream == packet.stream_index)
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
                                        if (av_read_frame(p.avFormatContext, &packet) < 0)
                                        {
                                            if (p.avVideoStream != -1)
                                            {
                                                _decodeVideo(nullptr);
                                                avcodec_flush_buffers(p.avCodecContext[p.avVideoStream]);
                                            }
                                            if (p.avAudioStream != -1)
                                            {
                                                _decodeAudio(nullptr);
                                                avcodec_flush_buffers(p.avCodecContext[p.avAudioStream]);
                                            }
                                            throw std::exception();
                                        }
                                        if (p.avVideoStream == packet.stream_index)
                                        {
                                            if (_decodeVideo(&packet) < 0)
                                            {
                                                throw std::exception();
                                            }
                                        }
                                        else if (p.avAudioStream == packet.stream_index)
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
                                        p.running = false;
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
                    if (_p->thread.joinable())
                    {
                        _p->thread.join();
                    }
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
                    DJV_PRIVATE_PTR();
                    std::lock_guard<std::mutex> lock(_queue->getMutex());
                    p.seek = value;
                    p.queueCV.notify_one();
                }

                Timestamp Read::_decodeVideo(AVPacket * packet, bool seek)
                {
                    DJV_PRIVATE_PTR();
                    int r = avcodec_send_packet(p.avCodecContext[p.avVideoStream], packet);
                    if (r < 0)
                    {
                        return r;
                    }
                    Timestamp pts = 0;
                    while (r >= 0)
                    {
                        r = avcodec_receive_frame(p.avCodecContext[p.avVideoStream], p.avFrame);
                        if (AVERROR(EAGAIN) == r)
                        {
                            return pts;
                        }
                        else if (r < 0)
                        {
                            return r;
                        }

                        pts = av_rescale_q(
                            p.avFrame->pts,
                            p.avFormatContext->streams[p.avVideoStream]->time_base,
                            FFmpeg::getTimeBaseQ());

                        if (auto context = _context.lock())
                        {
                            //std::stringstream ss;
                            //ss << _fileName << ", decoded frame: " << pts;
                            //context->log("djv::AV::IO::FFmpeg::Read", ss.str());
                        }

                        if (!seek)
                        {
                            auto image = Image::create(p.videoInfo.info);
                            av_image_fill_arrays(
                                p.avFrameRgb->data,
                                p.avFrameRgb->linesize,
                                image->getData(),
                                AV_PIX_FMT_RGBA,
                                image->getWidth(),
                                image->getHeight(),
                                1);
                            sws_scale(
                                p.swsContext,
                                (uint8_t const * const *)p.avFrame->data,
                                p.avFrame->linesize,
                                0,
                                p.avCodecParameters[p.avVideoStream]->height,
                                p.avFrameRgb->data,
                                p.avFrameRgb->linesize);
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
                    DJV_PRIVATE_PTR();
                    int r = avcodec_send_packet(p.avCodecContext[p.avAudioStream], packet);
                    if (r < 0)
                    {
                        return r;
                    }
                    Timestamp pts = 0;
                    while (r >= 0)
                    {
                        r = avcodec_receive_frame(p.avCodecContext[p.avAudioStream], p.avFrame);
                        if (r == AVERROR(EAGAIN))
                        {
                            return pts;
                        }
                        else if (r < 0)
                        {
                            return r;
                        }

                        pts = av_rescale_q(
                            p.avFrame->pts,
                            p.avFormatContext->streams[p.avVideoStream]->time_base,
                            FFmpeg::getTimeBaseQ());

                        if (!seek)
                        {
                            const auto & info = p.audioInfo.info;
                            auto audioData = Audio::Data::create(info, p.avFrame->nb_samples * info.channelCount);
                            switch (p.avCodecParameters[p.avAudioStream]->format)
                            {
                            case AV_SAMPLE_FMT_U8:
                            case AV_SAMPLE_FMT_S16:
                            case AV_SAMPLE_FMT_S32:
                            case AV_SAMPLE_FMT_FLT:
                                memcpy(audioData->getData(), p.avFrame->data[0], audioData->getByteCount());
                                break;
                            case AV_SAMPLE_FMT_U8P:
                            case AV_SAMPLE_FMT_S16P:
                            case AV_SAMPLE_FMT_S32P:
                            case AV_SAMPLE_FMT_FLTP:
                            {
                                const size_t channelCount = info.channelCount;
                                const float ** c = new const float * [channelCount];
                                for (size_t i = 0; i < channelCount; ++i)
                                {
                                    c[i] = reinterpret_cast<float *>(p.avFrame->data[i]);
                                }
                                Audio::Data::planarInterleave(
                                    c,
                                    reinterpret_cast<float *>(audioData->getData()),
                                    audioData->getSampleCount(),
                                    channelCount);
                                break;
                            }
                            default: break;
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

