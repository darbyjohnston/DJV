// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include "FFmpeg.h"

#include <djvAudio/Data.h>

#include <djvSystem/LogSystem.h>
#include <djvSystem/Timer.h>

#include <djvMath/Rational.h>

#include <djvCore/Memory.h>

extern "C"
{
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/dict.h>
#include <libavutil/imgutils.h>
#include <libavutil/rational.h>

} // extern "C"

#include <atomic>
#include <thread>

using namespace djv;

namespace IO
{
    struct FFmpegRead::Private
    {
        std::shared_ptr<Info> info;
        std::promise< std::shared_ptr<Info> > infoPromise;
        std::thread thread;
        std::atomic<bool> running;
        std::condition_variable queueCV;
        Timestamp currentVideoTimestamp = timestampInvalid;
        Timestamp currentAudioTimestamp = timestampInvalid;

        AVFormatContext* avFormatContext = nullptr;
        int avVideoStream = -1;
        int avAudioStream = -1;
        std::map<int, AVCodecParameters*> avCodecParameters;
        std::map<int, AVCodecContext*> avCodecContext;
        AVFrame* avFrame = nullptr;
        AVFrame* avFrameRgb = nullptr;
        SwsContext* swsContext = nullptr;
    };

    void FFmpegRead::_init(
        const System::File::Info& fileInfo,
        const std::shared_ptr<System::LogSystem>& logSystem)
    {
        IIO::_init(fileInfo, logSystem);
        DJV_PRIVATE_PTR();
        p.running = true;
        p.thread = std::thread(
            [fileInfo, this]
            {
                try
                {
                    _init(fileInfo);
                    _work();
                }
                catch (const std::exception& e)
                {
                    _p->infoPromise.set_value(nullptr);
                    _logSystem->log("FFmpegRead", e.what(), System::LogLevel::Error);
                }
                _cleanup();
            });
    }

    FFmpegRead::FFmpegRead() :
        _p(new Private)
    {}

    FFmpegRead::~FFmpegRead()
    {
        DJV_PRIVATE_PTR();
        p.running = false;
        if (p.thread.joinable())
        {
            p.thread.join();
        }
    }

    std::shared_ptr<FFmpegRead> FFmpegRead::create(
        const System::File::Info& info,
        const std::shared_ptr<System::LogSystem>& logSystem)
    {
        auto out = std::shared_ptr<FFmpegRead>(new FFmpegRead);
        out->_init(info, logSystem);
        return out;
    }

    std::future< std::shared_ptr<Info> > FFmpegRead::getInfo()
    {
        return _p->infoPromise.get_future();
    }

    void FFmpegRead::seek(Timestamp value)
    {
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _seekTimestamp = value;
        }
        _p->queueCV.notify_one();
    }

    void FFmpegRead::_init(const System::File::Info& info)
    {
        DJV_PRIVATE_PTR();
        int r = avformat_open_input(
            &p.avFormatContext,
            _fileInfo.getFileName().c_str(),
            nullptr,
            nullptr);
        if (r < 0)
        {
            std::stringstream ss;
            ss << "Cannot open " << info.getFileName();
            throw std::runtime_error(ss.str());
        }
        r = avformat_find_stream_info(p.avFormatContext, 0);
        if (r < 0)
        {
            std::stringstream ss;
            ss << "Cannot open " << info.getFileName();
            throw std::runtime_error(ss.str());
        }
        av_dump_format(p.avFormatContext, 0, _fileInfo.getFileName().c_str(), 0);

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
            ss << "Cannot open " << info.getFileName();
            throw std::runtime_error(ss.str());
        }

        p.info = IO::Info::create();
        p.info->fileInfo = std::string(_fileInfo);

        if (p.avVideoStream != -1 || p.avAudioStream != -1)
        {
            p.avFrame = av_frame_alloc();
        }

        if (p.avVideoStream != -1)
        {
            auto avVideoStream = p.avFormatContext->streams[p.avVideoStream];
            auto avVideoCodecParameters = avVideoStream->codecpar;
            auto avVideoCodec = avcodec_find_decoder(avVideoCodecParameters->codec_id);
            //auto avVideoCodec = avcodec_find_decoder_by_name("h264_dxva2");
            //auto avVideoCodec = avcodec_find_decoder_by_name("h264_d3d11va");
            if (!avVideoCodec)
            {
                std::stringstream ss;
                ss << "Cannot open " << info.getFileName();
                throw std::runtime_error(ss.str());
            }
            p.avCodecParameters[p.avVideoStream] = avcodec_parameters_alloc();
            r = avcodec_parameters_copy(p.avCodecParameters[p.avVideoStream], avVideoCodecParameters);
            if (r < 0)
            {
                std::stringstream ss;
                ss << "Cannot open " << info.getFileName();
                throw std::runtime_error(ss.str());
            }
            p.avCodecContext[p.avVideoStream] = avcodec_alloc_context3(avVideoCodec);
            r = avcodec_parameters_to_context(p.avCodecContext[p.avVideoStream], p.avCodecParameters[p.avVideoStream]);
            if (r < 0)
            {
                std::stringstream ss;
                ss << "Cannot open " << info.getFileName();
                throw std::runtime_error(ss.str());
            }
            p.avCodecContext[p.avVideoStream]->thread_count = 8;
            p.avCodecContext[p.avVideoStream]->thread_type = FF_THREAD_FRAME; //FF_THREAD_SLICE;
            r = avcodec_open2(p.avCodecContext[p.avVideoStream], avVideoCodec, 0);
            if (r < 0)
            {
                std::stringstream ss;
                ss << "Cannot open " << info.getFileName();
                throw std::runtime_error(ss.str());
            }

            p.avFrameRgb = av_frame_alloc();

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

            Image::Info imageInfo;
            imageInfo.size.w = p.avCodecParameters[p.avVideoStream]->width;
            imageInfo.size.h = p.avCodecParameters[p.avVideoStream]->height;
            imageInfo.type = Image::Type::RGBA_U8;
            imageInfo.codec = avVideoCodec->long_name;
            p.info->video.push_back(imageInfo);
            p.info->videoSpeed = Math::IntRational(avVideoStream->r_frame_rate.num, avVideoStream->r_frame_rate.den);
            if (avVideoStream->duration != AV_NOPTS_VALUE)
            {
                AVRational r;
                r.num = IO::timebase.getNum();
                r.den = IO::timebase.getDen();
                p.info->videoDuration = av_rescale_q(
                    avVideoStream->duration,
                    avVideoStream->time_base,
                    r);
            }
            else if (p.avFormatContext->duration != AV_NOPTS_VALUE)
            {
                AVRational r;
                r.num = IO::timebase.getNum();
                r.den = IO::timebase.getDen();
                p.info->videoDuration = av_rescale_q(
                    p.avFormatContext->duration,
                    av_get_time_base_q(),
                    r);
            }
        }

        if (p.avAudioStream != -1)
        {
            auto avAudioStream = p.avFormatContext->streams[p.avAudioStream];
            auto avAudioCodecParameters = avAudioStream->codecpar;
            Audio::Type audioType = fromFFmpeg(static_cast<AVSampleFormat>(avAudioCodecParameters->format));
            if (Audio::Type::None == audioType)
            {
                std::stringstream ss;
                ss << "Cannot open " << info.getFileName();
                throw std::runtime_error(ss.str());
            }
            auto avAudioCodec = avcodec_find_decoder(avAudioCodecParameters->codec_id);
            if (!avAudioCodec)
            {
                std::stringstream ss;
                ss << "Cannot open " << info.getFileName();
                throw std::runtime_error(ss.str());
            }
            p.avCodecParameters[p.avAudioStream] = avcodec_parameters_alloc();
            r = avcodec_parameters_copy(p.avCodecParameters[p.avAudioStream], avAudioCodecParameters);
            if (r < 0)
            {
                std::stringstream ss;
                ss << "Cannot open " << info.getFileName();
                throw std::runtime_error(ss.str());
            }
            p.avCodecContext[p.avAudioStream] = avcodec_alloc_context3(avAudioCodec);
            r = avcodec_parameters_to_context(p.avCodecContext[p.avAudioStream], p.avCodecParameters[p.avAudioStream]);
            if (r < 0)
            {
                std::stringstream ss;
                ss << "Cannot open " << info.getFileName();
                throw std::runtime_error(ss.str());
            }
            r = avcodec_open2(p.avCodecContext[p.avAudioStream], avAudioCodec, 0);
            if (r < 0)
            {
                std::stringstream ss;
                ss << "Cannot open " << info.getFileName();
                throw std::runtime_error(ss.str());
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
            p.info->audio.channelCount = channelCount;
            p.info->audio.type = audioType;
            p.info->audio.sampleRate = p.avCodecParameters[p.avAudioStream]->sample_rate;
            p.info->audio.codec = std::string(avAudioCodec->long_name);
            if (avAudioStream->duration != AV_NOPTS_VALUE)
            {
                AVRational r;
                r.num = IO::timebase.getNum();
                r.den = IO::timebase.getDen();
                p.info->audioDuration = av_rescale_q(
                    avAudioStream->duration,
                    avAudioStream->time_base,
                    toFFmpeg(IO::timebase));
            }
            else if (p.avFormatContext->duration != AV_NOPTS_VALUE)
            {
                AVRational r;
                r.num = IO::timebase.getNum();
                r.den = IO::timebase.getDen();
                p.info->audioDuration = av_rescale_q(
                    p.avFormatContext->duration,
                    av_get_time_base_q(),
                    toFFmpeg(IO::timebase));
            }
        }

        p.infoPromise.set_value(p.info);
    }

    void FFmpegRead::_work()
    {
        DJV_PRIVATE_PTR();
        while (p.running)
        {
            bool read = false;
            Timestamp seekTimestamp = seekNone;
            {
                std::unique_lock<std::mutex> lock(_mutex);
                if (p.queueCV.wait_for(
                    lock,
                    System::getTimerDuration(System::TimerValue::Fast),
                    [this]
                    {
                        const bool video = _p->avVideoStream != -1 &&
                            (_videoQueue.isFinished() ? false : (_videoQueue.getCount() < _videoQueue.getMax()));
                        const bool audio = _p->avAudioStream != -1 &&
                            (_audioQueue.isFinished() ? false : (_audioQueue.getCount() < _audioQueue.getMax()));
                        return video || audio || _seekTimestamp != seekNone;
                    }))
                {
                    read = true;
                    if (_seekTimestamp != seekNone)
                    {
                        seekTimestamp = _seekTimestamp;
                        _seekTimestamp = seekNone;
                        _videoQueue.setFinished(false);
                        _videoQueue.clearFrames();
                        _audioQueue.setFinished(false);
                        _audioQueue.clearFrames();
                    }
                }
            }

            AVPacket packet;
            std::queue<VideoFrame> videoFrames;
            std::queue<AudioFrame> audioFrames;
            try
            {
                if (seekTimestamp != seekNone)
                {
                    p.currentVideoTimestamp = timestampInvalid;
                    p.currentAudioTimestamp = timestampInvalid;
                    _seek(&packet, seekTimestamp, videoFrames, audioFrames);
                }
                else if (read)
                {
                    _read(&packet, seekNone, videoFrames, audioFrames);
                }
            }
            catch (const std::exception&)
            {
                av_packet_unref(&packet);
                {
                    std::lock_guard<std::mutex> lock(_mutex);
                    _videoQueue.setFinished(true);
                    _audioQueue.setFinished(true);
                }
            }
            if (!videoFrames.empty() || !audioFrames.empty())
            {
                std::lock_guard<std::mutex> lock(_mutex);
                while (!videoFrames.empty())
                {
                    _videoQueue.addFrame(videoFrames.front());
                    videoFrames.pop();
                }
                while (!audioFrames.empty())
                {
                    _audioQueue.addFrame(audioFrames.front());
                    audioFrames.pop();
                }
            }
        }
    }

    void FFmpegRead::_cleanup()
    {
        DJV_PRIVATE_PTR();
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
    }

    void FFmpegRead::_seek(
        AVPacket* packet,
        Timestamp seekTimestamp,
        std::queue<VideoFrame>& videoFrames,
        std::queue<AudioFrame>& audioFrames)
    {
        DJV_PRIVATE_PTR();
        //std::cout << "FFmpeg seek: " << seekTimestamp << std::endl;
        if (p.avVideoStream != -1)
        {
            avcodec_flush_buffers(p.avCodecContext[p.avVideoStream]);
        }
        if (p.avAudioStream != -1)
        {
            avcodec_flush_buffers(p.avCodecContext[p.avAudioStream]);
        }
        if (p.avVideoStream != -1)
        {
            if (av_seek_frame(
                p.avFormatContext,
                p.avVideoStream,
                av_rescale_q(
                    seekTimestamp,
                    toFFmpeg(timebase),
                    p.avFormatContext->streams[p.avVideoStream]->time_base),
                AVSEEK_FLAG_BACKWARD) < 0)
            {
                throw std::exception();
            }
        }
        else if (p.avAudioStream != -1)
        {
            if (av_seek_frame(
                p.avFormatContext,
                p.avAudioStream,
                av_rescale_q(
                    seekTimestamp,
                    toFFmpeg(timebase),
                    p.avFormatContext->streams[p.avAudioStream]->time_base),
                AVSEEK_FLAG_BACKWARD) < 0)
            {
                throw std::exception();
            }
        }
        while ((p.avVideoStream != -1 && p.currentVideoTimestamp < seekTimestamp) ||
            (p.avAudioStream != -1 && p.currentAudioTimestamp < seekTimestamp))
        {
            _read(packet, seekTimestamp, videoFrames, audioFrames);
        }
    }

    void FFmpegRead::_read(
        AVPacket* packet,
        Timestamp seekTimestamp,
        std::queue<VideoFrame>& videoFrames,
        std::queue<AudioFrame>& audioFrames)
    {
        DJV_PRIVATE_PTR();
        if (av_read_frame(p.avFormatContext, packet) < 0)
        {
            if (p.avVideoStream != -1)
            {
                _readVideo(nullptr, seekTimestamp, videoFrames);
                avcodec_flush_buffers(p.avCodecContext[p.avVideoStream]);
            }
            if (p.avAudioStream != -1)
            {
                _readAudio(nullptr, seekTimestamp, audioFrames);
                avcodec_flush_buffers(p.avCodecContext[p.avAudioStream]);
            }
            throw std::exception();
        }
        if (p.avVideoStream == packet->stream_index)
        {
            if (_readVideo(packet, seekTimestamp, videoFrames) < 0)
            {
                throw std::exception();
            }
        }
        else if (p.avAudioStream == packet->stream_index)
        {
            if (_readAudio(packet, seekTimestamp, audioFrames) < 0)
            {
                throw std::exception();
            }
        }
        av_packet_unref(packet);
    }

    int FFmpegRead::_readVideo(AVPacket* packet, Timestamp seekTimestamp, std::queue<VideoFrame>& frames)
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
            }
            else if (r < 0)
            {
                break;
            }

            p.currentVideoTimestamp = av_rescale_q(
                p.avFrame->pts,
                p.avFormatContext->streams[p.avVideoStream]->time_base,
                toFFmpeg(timebase));

            SeekFrame seekFrame = SeekFrame::False;
            if (seekNone == seekTimestamp || p.currentVideoTimestamp >= seekTimestamp)
            {
                Image::Info imageInfo;
                if (p.info->video.size())
                {
                    imageInfo = p.info->video[0];
                }
                if (!((0 == p.avFrame->sample_aspect_ratio.num && 1 == p.avFrame->sample_aspect_ratio.den) ||
                    0 == p.avFrame->sample_aspect_ratio.den))
                {
                    imageInfo.pixelAspectRatio = p.avFrame->sample_aspect_ratio.num / static_cast<float>(p.avFrame->sample_aspect_ratio.den);
                }
                auto imageData = Image::Data::create(imageInfo);
                av_image_fill_arrays(
                    p.avFrameRgb->data,
                    p.avFrameRgb->linesize,
                    imageData->getData(),
                    AV_PIX_FMT_RGBA,
                    imageData->getWidth(),
                    imageData->getHeight(),
                    1);
                sws_scale(
                    p.swsContext,
                    (uint8_t const* const*)p.avFrame->data,
                    p.avFrame->linesize,
                    0,
                    p.avCodecParameters[p.avVideoStream]->height,
                    p.avFrameRgb->data,
                    p.avFrameRgb->linesize);

                seekFrame = seekTimestamp != seekNone && frames.empty() ? SeekFrame::True : SeekFrame::False;
                VideoFrame videoFrame;
                videoFrame.info.timestamp = p.currentVideoTimestamp;
                videoFrame.data = imageData;
                videoFrame.seekFrame = seekFrame;
                frames.push(videoFrame);
            }

            /*std::cout << "FFmpeg video: " << p.currentVideoTimestamp;
            if (SeekFrame::True == seekFrame)
            {
                std::cout << " seek";
            }
            std::cout << std::endl;*/
        }
        return r;
    }

    int FFmpegRead::_readAudio(AVPacket* packet, Timestamp seekTimestamp, std::queue<AudioFrame>& frames)
    {
        DJV_PRIVATE_PTR();
        int r = avcodec_send_packet(p.avCodecContext[p.avAudioStream], packet);
        while (r >= 0)
        {
            r = avcodec_receive_frame(p.avCodecContext[p.avAudioStream], p.avFrame);
            if (AVERROR(EAGAIN) == r)
            {
                r = 0;
                break;
            }
            else if (r < 0)
            {
                break;
            }

            p.currentAudioTimestamp = av_rescale_q(
                p.avFrame->pts,
                p.avFormatContext->streams[p.avAudioStream]->time_base,
                toFFmpeg(timebase));

            SeekFrame seekFrame = SeekFrame::False;
            if (seekNone == seekTimestamp || p.currentAudioTimestamp >= seekTimestamp)
            {
                auto audioData = Audio::Data::create(p.info->audio, p.avFrame->nb_samples);
                extractAudio(
                    p.avFrame->data,
                    p.avCodecParameters[p.avAudioStream]->format,
                    p.avCodecParameters[p.avAudioStream]->channels,
                    audioData);

                seekFrame = seekTimestamp != seekNone && frames.empty() ? SeekFrame::True : SeekFrame::False;
                AudioFrame audioFrame;
                audioFrame.info.timestamp = p.currentAudioTimestamp;
                audioFrame.data = audioData;
                audioFrame.seekFrame = seekFrame;
                frames.push(audioFrame);
            }

            /*std::cout << "FFmpeg audio: " << p.currentAudioTimestamp;
            if (SeekFrame::True == seekFrame)
            {
                std::cout << " seek";
            }
            std::cout << std::endl;*/
        }
        return r;
    }

    FFmpegPlugin::FFmpegPlugin(const std::shared_ptr<System::LogSystem>& logSystem) :
        IPlugin(logSystem)
    {}

    FFmpegPlugin::~FFmpegPlugin()
    {}

    std::shared_ptr<FFmpegPlugin> FFmpegPlugin::create(
        const std::shared_ptr<System::LogSystem>& logSystem)
    {
        auto out = std::shared_ptr<FFmpegPlugin>(new FFmpegPlugin(logSystem));
        return out;
    }

    bool FFmpegPlugin::canRead(const System::File::Info&)
    {
        return true;
    }

    std::shared_ptr<IRead> FFmpegPlugin::read(const System::File::Info& info)
    {
        return FFmpegRead::create(info, _logSystem);
    }

    AVRational toFFmpeg(const Math::IntRational& value)
    {
        return av_make_q(value.getNum(), value.getDen());
    }

    Math::IntRational fromFFmpeg(const AVRational& value)
    {
        return Math::IntRational(value.num, value.den);
    }

    Audio::Type fromFFmpeg(AVSampleFormat value)
    {
        Audio::Type out = Audio::Type::None;
        switch (value)
        {
        case AV_SAMPLE_FMT_S16:  out = Audio::Type::S16; break;
        case AV_SAMPLE_FMT_S32:  out = Audio::Type::S32; break;
        case AV_SAMPLE_FMT_FLT:  out = Audio::Type::F32; break;
        case AV_SAMPLE_FMT_DBL:  out = Audio::Type::F64; break;
        case AV_SAMPLE_FMT_S16P: out = Audio::Type::S16; break;
        case AV_SAMPLE_FMT_S32P: out = Audio::Type::S32; break;
        case AV_SAMPLE_FMT_FLTP: out = Audio::Type::F32; break;
        case AV_SAMPLE_FMT_DBLP: out = Audio::Type::F64; break;
        default: break;
        }
        return out;
    }

    void extractAudio(
        uint8_t** inData,
        int inFormat,
        uint8_t inChannelCount,
        std::shared_ptr<Audio::Data> out)
    {
        const uint8_t outChannelCount = out->getChannelCount();
        switch (inFormat)
        {
        case AV_SAMPLE_FMT_S16:
        {
            if (inChannelCount == outChannelCount)
            {
                memcpy(out->getData(), inData[0], out->getByteCount());
            }
            else
            {
                Audio::extract(
                    reinterpret_cast<int16_t*>(inData[0]),
                    reinterpret_cast<int16_t*>(out->getData()),
                    out->getSampleCount(),
                    inChannelCount,
                    outChannelCount);
            }
            break;
        }
        case AV_SAMPLE_FMT_S32:
        {
            if (inChannelCount == outChannelCount)
            {
                memcpy(out->getData(), inData[0], out->getByteCount());
            }
            else
            {
                Audio::extract(
                    reinterpret_cast<int32_t*>(inData[0]),
                    reinterpret_cast<int32_t*>(out->getData()),
                    out->getSampleCount(),
                    inChannelCount,
                    outChannelCount);
            }
            break;
        }
        case AV_SAMPLE_FMT_FLT:
        {
            if (inChannelCount == outChannelCount)
            {
                memcpy(out->getData(), inData[0], out->getByteCount());
            }
            else
            {
                Audio::extract(
                    reinterpret_cast<float*>(inData[0]),
                    reinterpret_cast<float*>(out->getData()),
                    out->getSampleCount(),
                    inChannelCount,
                    outChannelCount);
            }
            break;
        }
        case AV_SAMPLE_FMT_DBL:
        {
            if (inChannelCount == outChannelCount)
            {
                memcpy(out->getData(), inData[0], out->getByteCount());
            }
            else
            {
                Audio::extract(
                    reinterpret_cast<double*>(inData[0]),
                    reinterpret_cast<double*>(out->getData()),
                    out->getSampleCount(),
                    inChannelCount,
                    outChannelCount);
            }
            break;
        }
        case AV_SAMPLE_FMT_S16P:
        {
            const int16_t** c = new const int16_t * [outChannelCount];
            for (int i = 0; i < outChannelCount; ++i)
            {
                c[i] = reinterpret_cast<int16_t*>(inData[i]);
            }
            Audio::planarInterleave(
                c,
                reinterpret_cast<int16_t*>(out->getData()),
                outChannelCount,
                out->getSampleCount());
            delete[] c;
            break;
        }
        case AV_SAMPLE_FMT_S32P:
        {
            const int32_t** c = new const int32_t * [outChannelCount];
            for (int i = 0; i < outChannelCount; ++i)
            {
                c[i] = reinterpret_cast<int32_t*>(inData[i]);
            }
            Audio::planarInterleave(
                c,
                reinterpret_cast<int32_t*>(out->getData()),
                outChannelCount,
                out->getSampleCount());
            delete[] c;
            break;
        }
        case AV_SAMPLE_FMT_FLTP:
        {
            const float** c = new const float* [outChannelCount];
            for (int i = 0; i < outChannelCount; ++i)
            {
                c[i] = reinterpret_cast<float*>(inData[i]);
            }
            Audio::planarInterleave(
                c,
                reinterpret_cast<float*>(out->getData()),
                outChannelCount,
                out->getSampleCount());
            delete[] c;
            break;
        }
        case AV_SAMPLE_FMT_DBLP:
        {
            const double** c = new const double* [outChannelCount];
            for (int i = 0; i < outChannelCount; ++i)
            {
                c[i] = reinterpret_cast<double*>(inData[i]);
            }
            Audio::planarInterleave(
                c,
                reinterpret_cast<double*>(out->getData()),
                outChannelCount,
                out->getSampleCount());
            delete[] c;
            break;
        }
        default: break;
        }
    }

} // namespace IO
