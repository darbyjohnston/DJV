//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvCore/LogSystem.h>
#include <djvCore/Timer.h>
#include <djvCore/Vector.h>

extern "C"
{
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>

} // extern "C"

using namespace djv::Core;

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
                    Time::Timestamp seek = -1;
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
                    const FileSystem::FileInfo& fileInfo,
                    const ReadOptions& options,
                    const std::shared_ptr<ResourceSystem>& resourceSystem,
                    const std::shared_ptr<LogSystem>& logSystem)
                {
                    IRead::_init(fileInfo, options, resourceSystem, logSystem);
                    DJV_PRIVATE_PTR();
                    p.running = true;
                    p.thread = std::thread(
                        [this]
                    {
                        DJV_PRIVATE_PTR();
                        try
                        {
                            // Open the file.
                            /*{
                                std::stringstream ss;
                                ss << "Reading file: " << _fileInfo;
                                _logSystem->log("djv::AV::IO::FFmpeg::Read", ss.str());
                            }*/
                            int r = avformat_open_input(
                                &p.avFormatContext,
                                _fileInfo.getFileName().c_str(),
                                nullptr,
                                nullptr);
                            if (r < 0)
                            {
                                std::stringstream ss;
                                ss << DJV_TEXT("The file") << " '" << _fileInfo << "' " <<
                                    DJV_TEXT("cannot be opened") << ". " << FFmpeg::getErrorString(r);
                                throw std::runtime_error(ss.str());
                            }
                            r = avformat_find_stream_info(p.avFormatContext, 0);
                            if (r < 0)
                            {
                                std::stringstream ss;
                                ss << DJV_TEXT("The file") << " '" << _fileInfo << "' " <<
                                    DJV_TEXT("cannot be opened") << ". " << FFmpeg::getErrorString(r);
                                throw std::runtime_error(ss.str());
                            }
                            av_dump_format(p.avFormatContext, 0, _fileInfo.getFileName().c_str(), 0);

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
                                std::stringstream ss;
                                ss << DJV_TEXT("The file") << " '" << _fileInfo << "' " <<
                                    DJV_TEXT("does not have any video or audio streams") << ".";
                                throw std::runtime_error(ss.str());
                            }

                            Info info;
                            info.fileName = _fileInfo;

                            if (p.avVideoStream != -1 || p.avAudioStream != -1)
                            {
                                // Initialize the buffers.
                                p.avFrame = av_frame_alloc();
                            }

                            if (p.avVideoStream != -1)
                            {
                                // Find the codec for the video stream.
                                auto avVideoStream = p.avFormatContext->streams[p.avVideoStream];
                                auto avVideoCodecParameters = avVideoStream->codecpar;
                                auto avVideoCodec = avcodec_find_decoder(avVideoCodecParameters->codec_id);
                                if (!avVideoCodec)
                                {
                                    std::stringstream ss;
                                    ss << DJV_TEXT("The file") << " '" << _fileInfo << "' " <<
                                        DJV_TEXT("does not match any video codecs") << ".";
                                    throw std::runtime_error(ss.str());
                                }
                                p.avCodecParameters[p.avVideoStream] = avcodec_parameters_alloc();
                                r = avcodec_parameters_copy(p.avCodecParameters[p.avVideoStream], avVideoCodecParameters);
                                if (r < 0)
                                {
                                    std::stringstream ss;
                                    ss << DJV_TEXT("The file") << " '" << _fileInfo << "' " <<
                                        DJV_TEXT("cannot be opened") << ". " << FFmpeg::getErrorString(r);
                                    throw std::runtime_error(ss.str());
                                }
                                p.avCodecContext[p.avVideoStream] = avcodec_alloc_context3(avVideoCodec);
                                r = avcodec_parameters_to_context(p.avCodecContext[p.avVideoStream], p.avCodecParameters[p.avVideoStream]);
                                if (r < 0)
                                {
                                    std::stringstream ss;
                                    ss << DJV_TEXT("The file") << " '" << _fileInfo << "' " <<
                                        DJV_TEXT("cannot be opened") << ". " << FFmpeg::getErrorString(r);
                                    throw std::runtime_error(ss.str());
                                }
                                r = avcodec_open2(p.avCodecContext[p.avVideoStream], avVideoCodec, 0);
                                if (r < 0)
                                {
                                    std::stringstream ss;
                                    ss << DJV_TEXT("The file") << " '" << _fileInfo << "' " <<
                                        DJV_TEXT("cannot be opened") << ". " << FFmpeg::getErrorString(r);
                                    throw std::runtime_error(ss.str());
                                }

                                // Initialize the buffers.
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

                                // Get information.
                                const auto pixelDataInfo = Image::Info(
                                    p.avCodecParameters[p.avVideoStream]->width,
                                    p.avCodecParameters[p.avVideoStream]->height,
                                    Image::Type::RGBA_U8);
                                Time::Timestamp duration = 0;
                                if (avVideoStream->duration != AV_NOPTS_VALUE)
                                {
                                    duration = av_rescale_q(
                                        avVideoStream->duration,
                                        avVideoStream->time_base,
                                        av_get_time_base_q());
                                }
                                else if (p.avFormatContext->duration != AV_NOPTS_VALUE)
                                {
                                    duration = p.avFormatContext->duration;
                                }
                                const Time::Speed speed(avVideoStream->r_frame_rate.num, avVideoStream->r_frame_rate.den);
                                p.videoInfo = VideoInfo(pixelDataInfo, speed, duration);
                                p.videoInfo.codec = std::string(avVideoCodec->long_name);
                                info.video.push_back(p.videoInfo);
                                /*{
                                    std::stringstream ss;
                                    ss << _fileInfo << ": image size " << pixelDataInfo.size << "\n";
                                    ss << _fileInfo << ": pixel type " << pixelDataInfo.type << "\n";
                                    ss << _fileInfo << ": duration " << duration << "\n";
                                    ss << _fileInfo << ": speed " << speed << "\n";
                                    _logSystem->log("djv::AV::IO::FFmpeg::Read", ss.str());
                                }*/
                            }

                            if (p.avAudioStream != -1)
                            {
                                // Find the codec for the audio stream.
                                auto avAudioStream = p.avFormatContext->streams[p.avAudioStream];
                                auto avAudioCodecParameters = avAudioStream->codecpar;
                                Audio::Type audioType = FFmpeg::toAudioType(static_cast<AVSampleFormat>(avAudioCodecParameters->format));
                                if (Audio::Type::None == audioType)
                                {
                                    std::stringstream ss;
                                    ss << DJV_TEXT("The audio format ") <<
                                        " '" << FFmpeg::toString(static_cast<AVSampleFormat>(avAudioCodecParameters->format)) << "' " <<
                                        DJV_TEXT("is not supported") << ".";
                                    throw std::runtime_error(ss.str());
                                }
                                auto avAudioCodec = avcodec_find_decoder(avAudioCodecParameters->codec_id);
                                if (!avAudioCodec)
                                {
                                    std::stringstream ss;
                                    ss << DJV_TEXT("The file") << " '" << _fileInfo << "' " <<
                                        DJV_TEXT("does not match any audio codecs") << ".";
                                    throw std::runtime_error(ss.str());
                                }
                                p.avCodecParameters[p.avAudioStream] = avcodec_parameters_alloc();
                                r = avcodec_parameters_copy(p.avCodecParameters[p.avAudioStream], avAudioCodecParameters);
                                if (r < 0)
                                {
                                    std::stringstream ss;
                                    ss << DJV_TEXT("The file") << " '" << _fileInfo << "' " <<
                                        DJV_TEXT("cannot be opened") << ". " << FFmpeg::getErrorString(r);
                                    throw std::runtime_error(ss.str());
                                }
                                p.avCodecContext[p.avAudioStream] = avcodec_alloc_context3(avAudioCodec);
                                r = avcodec_parameters_to_context(p.avCodecContext[p.avAudioStream], p.avCodecParameters[p.avAudioStream]);
                                if (r < 0)
                                {
                                    std::stringstream ss;
                                    ss << DJV_TEXT("The file") << " '" << _fileInfo << "' " <<
                                        DJV_TEXT("cannot be opened") << ". " << FFmpeg::getErrorString(r);
                                    throw std::runtime_error(ss.str());
                                }
                                r = avcodec_open2(p.avCodecContext[p.avAudioStream], avAudioCodec, 0);
                                if (r < 0)
                                {
                                    std::stringstream ss;
                                    ss << DJV_TEXT("The file") << " '" << _fileInfo << "' " <<
                                        DJV_TEXT("cannot be opened") << ". " << FFmpeg::getErrorString(r);
                                    throw std::runtime_error(ss.str());
                                }

                                // Get information.
                                Time::Timestamp duration = 0;
                                if (avAudioStream->duration != AV_NOPTS_VALUE)
                                {
                                    duration = av_rescale_q(
                                        avAudioStream->duration,
                                        avAudioStream->time_base,
                                        av_get_time_base_q());
                                }
                                else if (p.avFormatContext->duration != AV_NOPTS_VALUE)
                                {
                                    duration = p.avFormatContext->duration;
                                }
                                uint8_t channelCount = p.avCodecParameters[p.avAudioStream]->channels;
                                switch (channelCount)
                                {
                                case 1:
                                case 2:
                                case 6:
                                case 7:
                                case 8: break;
                                default: channelCount = 2; break;
                                }
                                switch (audioType)
                                {
                                case Audio::Type::S32: audioType = Audio::Type::F32; break;
                                default: break;
                                }
                                p.audioInfo = AudioInfo(
                                    Audio::DataInfo(
                                        channelCount,
                                        audioType,
                                        p.avCodecParameters[p.avAudioStream]->sample_rate),
                                    duration);
                                p.videoInfo.codec = std::string(avAudioCodec->long_name);
                                info.audio.push_back(p.audioInfo);
                            }

                            AVDictionaryEntry* tag = nullptr;
                            while ((tag = av_dict_get(p.avFormatContext->metadata, "", tag, AV_DICT_IGNORE_SUFFIX)))
                            {
                                info.tags.setTag(tag->key, tag->value);
                            }

                            p.infoPromise.set_value(info);

                            while (p.running)
                            {
                                bool read = false;
                                Time::Timestamp seek = -1;
                                {
                                    std::unique_lock<std::mutex> lock(_mutex);
                                    if (p.queueCV.wait_for(
                                        lock,
                                        Time::getMilliseconds(Time::TimerValue::Fast),
                                        [this]
                                    {
                                        DJV_PRIVATE_PTR();
                                        const bool video = p.avVideoStream != -1 && (_videoQueue.isFinished() ? false : (_videoQueue.getFrameCount() < _videoQueue.getMax()));
                                        const bool audio = p.avAudioStream != -1 && (_audioQueue.isFinished() ? false : (_audioQueue.getFrameCount() < _audioQueue.getMax()));
                                        return video || audio || p.seek != -1;
                                    }))
                                    {
                                        read = true;
                                        if (p.seek != -1)
                                        {
                                            seek = p.seek;
                                            p.seek = -1;
                                            _videoQueue.setFinished(false);
                                            _videoQueue.clearFrames();
                                            _audioQueue.setFinished(false);
                                            _audioQueue.clearFrames();
                                        }
                                    }
                                }
                                AVPacket packet;
                                try
                                {
                                    if (seek != -1)
                                    {
                                        /*{
                                            std::stringstream ss;
                                            ss << _fileInfo << ": seek " << seek;
                                            _logSystem->log("djv::AV::IO::FFmpeg::Read", ss.str());
                                        }*/
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
                                        Time::Timestamp pts = 0;
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
                                                Time::Timestamp t = 0;
                                                if (_seek(&packet, packet.stream_index, t) < 0)
                                                {
                                                    throw std::exception();
                                                }
                                                pts = t;
                                            }
                                            else if (p.avAudioStream == packet.stream_index)
                                            {
                                                Time::Timestamp t = 0;
                                                if (_seek(&packet, packet.stream_index, t) < 0)
                                                {
                                                    throw std::exception();
                                                }
                                                pts = t;
                                            }
                                            av_packet_unref(&packet);
                                        }
                                    }
                                    if (read)
                                    {
                                        int r = av_read_frame(p.avFormatContext, &packet);
                                        if (r < 0)
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
                                catch (const std::exception&)
                                {
                                    /*{
                                        std::stringstream ss;
                                        ss << _fileInfo << ": finished";
                                        _logSystem->log("djv::AV::IO::FFmpeg::Read", ss.str());
                                    }*/
                                    av_packet_unref(&packet);
                                    {
                                        std::lock_guard<std::mutex> lock(_mutex);
                                        _videoQueue.setFinished(true);
                                        _audioQueue.setFinished(true);
                                    }
                                }
                            }
                        }
                        catch (const std::exception & e)
                        {
                            p.infoPromise.set_value(Info());
                            _logSystem->log("djvAV::IO::FFmpeg::Read", e.what(), LogLevel::Error);
                        }
                        if (p.swsContext)
                        {
                            sws_freeContext(p.swsContext);
                        }
                        if (p.avFrameRgb)
                        {
                            av_frame_free(&p.avFrameRgb);
                        }
                        if (p.avFrame)
                        {
                            av_frame_free(&p.avFrame);
                        }
                        for (auto i : p.avCodecContext)
                        {
                            avcodec_close(i.second);
                            avcodec_free_context(&i.second);
                        }
                        for (auto i : p.avCodecParameters)
                        {
                            avcodec_parameters_free(&i.second);
                        }
                        if (p.avFormatContext)
                        {
                            avformat_close_input(&p.avFormatContext);
                        }
                    });
                }

                Read::Read() :
                    _p(new Private)
                {}

                Read::~Read()
                {
                    DJV_PRIVATE_PTR();
                    p.running = false;
                    if (p.thread.joinable())
                    {
						//! \todo How do we safely detach the thread here so we don't block?
                        p.thread.join();
                    }
                }

                std::shared_ptr<Read> Read::create(
                    const FileSystem::FileInfo& fileInfo,
                    const ReadOptions& readOptions,
                    const std::shared_ptr<ResourceSystem>& resourceSystem,
                    const std::shared_ptr<LogSystem>& logSystem)
                {
                    auto out = std::shared_ptr<Read>(new Read);
                    out->_init(fileInfo, readOptions, resourceSystem, logSystem);
                    return out;
                }

                bool Read::isRunning() const
                {
                    return _p->running;
                }

                std::future<Info> Read::getInfo()
                {
                    return _p->infoPromise.get_future();
                }

                void Read::seek(Time::Timestamp value)
                {
                    DJV_PRIVATE_PTR();
                    {
                        std::lock_guard<std::mutex> lock(_mutex);
                        p.seek = value;
                    }
                    p.queueCV.notify_one();
                }

                int Read::_decodeVideo(AVPacket * packet)
                {
                    DJV_PRIVATE_PTR();
                    int r = avcodec_send_packet(p.avCodecContext[p.avVideoStream], packet);
                    while (r >= 0)
                    {
                        r = avcodec_receive_frame(p.avCodecContext[p.avVideoStream], p.avFrame);
                        if (AVERROR(EAGAIN) == r)
                        {
                            r = 0;
                            break;
                            return 0;
                        }
                        else if (r < 0)
                        {
                            break;
                        }
                        
                        const Time::Timestamp pts = av_rescale_q(
                            p.avFrame->pts,
                            p.avFormatContext->streams[p.avVideoStream]->time_base,
                            av_get_time_base_q());

                        /*if (auto context = _context.lock())
                        {
                            std::stringstream ss;
                            ss << _fileInfo << ", decoded frame: " << pts;
                            _logSystem->log("djv::AV::IO::FFmpeg::Read", ss.str());
                        }*/

                        auto info = p.videoInfo.info;
                        if (!((0 == p.avFrame->sample_aspect_ratio.num && 1 == p.avFrame->sample_aspect_ratio.den) ||
                              0 == p.avFrame->sample_aspect_ratio.den))
                        {
                            info.pixelAspectRatio = p.avFrame->sample_aspect_ratio.num / static_cast<float>(p.avFrame->sample_aspect_ratio.den);
                        }
                        auto image = Image::Image::create(info);
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
                            std::lock_guard<std::mutex> lock(_mutex);
                            _videoQueue.addFrame(VideoFrame(pts, image));
                        }
                    }
                    return r;
                }

                int Read::_decodeAudio(AVPacket * packet)
                {
                    DJV_PRIVATE_PTR();
                    int r = avcodec_send_packet(p.avCodecContext[p.avAudioStream], packet);
                    while (r >= 0)
                    {
                        r = avcodec_receive_frame(p.avCodecContext[p.avAudioStream], p.avFrame);
                        if (r == AVERROR(EAGAIN))
                        {
                            r = 0;
                            break;
                        }
                        else if (r < 0)
                        {
                            break;
                        }

                        const Time::Timestamp pts = av_rescale_q(
                            p.avFrame->pts,
                            p.avFormatContext->streams[p.avAudioStream]->time_base,
                            av_get_time_base_q());

                        const auto & info = p.audioInfo.info;
                        auto audioData = Audio::Data::create(info, p.avFrame->nb_samples * info.channelCount);
                        switch (p.avCodecParameters[p.avAudioStream]->format)
                        {
                        case AV_SAMPLE_FMT_U8:
                        {
                            if (p.avCodecParameters[p.avAudioStream]->channels == info.channelCount)
                            {
                                memcpy(audioData->getData(), p.avFrame->data[0], audioData->getByteCount());
                            }
                            else
                            {
                                Audio::Data::extract(
                                    reinterpret_cast<uint8_t*>(p.avFrame->data[0]),
                                    reinterpret_cast<uint8_t*>(audioData->getData()),
                                    audioData->getSampleCount(),
                                    p.avCodecParameters[p.avAudioStream]->channels,
                                    info.channelCount);
                            }
                            break;
                        }
                        case AV_SAMPLE_FMT_S16:
                        {
                            if (p.avCodecParameters[p.avAudioStream]->channels == info.channelCount)
                            {
                                memcpy(audioData->getData(), p.avFrame->data[0], audioData->getByteCount());
                            }
                            else
                            {
                                Audio::Data::extract(
                                    reinterpret_cast<int16_t*>(p.avFrame->data[0]),
                                    reinterpret_cast<int16_t*>(audioData->getData()),
                                    audioData->getSampleCount(),
                                    p.avCodecParameters[p.avAudioStream]->channels,
                                    info.channelCount);
                            }
                            break;
                        }
                        case AV_SAMPLE_FMT_S32:
                        {
                            Audio::DataInfo s32Info = info;
                            s32Info.type = Audio::Type::S32;
                            auto s32Data = Audio::Data::create(s32Info, p.avFrame->nb_samples * info.channelCount);
                            if (p.avCodecParameters[p.avAudioStream]->channels == info.channelCount)
                            {
                                memcpy(s32Data->getData(), p.avFrame->data[0], s32Data->getByteCount());
                            }
                            else
                            {
                                Audio::Data::extract(
                                    reinterpret_cast<int32_t*>(p.avFrame->data[0]),
                                    reinterpret_cast<int32_t*>(s32Data->getData()),
                                    s32Data->getSampleCount(),
                                    p.avCodecParameters[p.avAudioStream]->channels,
                                    info.channelCount);
                            }
                            audioData = Audio::Data::convert(s32Data, Audio::Type::F32);
                            break;
                        }
                        case AV_SAMPLE_FMT_FLT:
                        {
                            if (p.avCodecParameters[p.avAudioStream]->channels == info.channelCount)
                            {
                                memcpy(audioData->getData(), p.avFrame->data[0], audioData->getByteCount());
                            }
                            else
                            {
                                Audio::Data::extract(
                                    reinterpret_cast<float*>(p.avFrame->data[0]),
                                    reinterpret_cast<float*>(audioData->getData()),
                                    audioData->getSampleCount(),
                                    p.avCodecParameters[p.avAudioStream]->channels,
                                    info.channelCount);
                            }
                            break;
                        }
                        case AV_SAMPLE_FMT_U8P:
                        {
                            const size_t channelCount = info.channelCount;
                            const uint8_t** c = new const uint8_t * [channelCount];
                            for (size_t i = 0; i < channelCount; ++i)
                            {
                                c[i] = reinterpret_cast<uint8_t*>(p.avFrame->data[i]);
                            }
                            Audio::Data::planarInterleave(
                                c,
                                reinterpret_cast<uint8_t*>(audioData->getData()),
                                audioData->getSampleCount(),
                                channelCount);
                            break;
                        }
                        case AV_SAMPLE_FMT_S16P:
                        {
                            const size_t channelCount = info.channelCount;
                            const int16_t** c = new const int16_t * [channelCount];
                            for (size_t i = 0; i < channelCount; ++i)
                            {
                                c[i] = reinterpret_cast<int16_t*>(p.avFrame->data[i]);
                            }
                            Audio::Data::planarInterleave(
                                c,
                                reinterpret_cast<int16_t*>(audioData->getData()),
                                audioData->getSampleCount(),
                                channelCount);
                            break;
                        }
                        case AV_SAMPLE_FMT_S32P:
                        {
                            Audio::DataInfo s32Info = info;
                            s32Info.type = Audio::Type::S32;
                            auto s32Data = Audio::Data::create(s32Info, p.avFrame->nb_samples * info.channelCount);
                            const size_t channelCount = info.channelCount;
                            const int32_t** c = new const int32_t * [channelCount];
                            for (size_t i = 0; i < channelCount; ++i)
                            {
                                c[i] = reinterpret_cast<int32_t*>(p.avFrame->data[i]);
                            }
                            Audio::Data::planarInterleave(
                                c,
                                reinterpret_cast<int32_t*>(s32Data->getData()),
                                s32Data->getSampleCount(),
                                channelCount);
                            audioData = Audio::Data::convert(s32Data, Audio::Type::F32);
                            break;
                        }
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
                            std::lock_guard<std::mutex> lock(_mutex);
                            _audioQueue.addFrame(AudioFrame(pts, audioData));
                        }
                    }
                    return r;
                }

                int Read::_seek(AVPacket* packet, int stream, Time::Timestamp& pts)
                {
                    DJV_PRIVATE_PTR();
                    int r = avcodec_send_packet(p.avCodecContext[stream], packet);
                    while (r >= 0)
                    {
                        r = avcodec_receive_frame(p.avCodecContext[stream], p.avFrame);
                        if (AVERROR(EAGAIN) == r)
                        {
                            r = 0;
                            break;
                        }
                        else if (r < 0)
                        {
                            break;
                        }
                        pts = av_rescale_q(
                            p.avFrame->pts,
                            p.avFormatContext->streams[stream]->time_base,
                            av_get_time_base_q());
                        /*if (auto context = _context.lock())
                        {
                            std::stringstream ss;
                            ss << _fileInfo << ", decoded frame: " << pts;
                            _logSystem->log("djv::AV::IO::FFmpeg::Read", ss.str());
                        }*/
                    }
                    return r;
                }

            } // namespace FFmpeg
        } // namespace IO
    } // namespace AV
} // namespace djv

