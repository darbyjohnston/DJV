// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include "FFmpeg.h"

#include "FFmpegFunc.h"

#include <djvSystem/LogSystem.h>
#include <djvSystem/TimerFunc.h>

#include <djvMath/RationalFunc.h>

extern "C"
{
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/dict.h>
#include <libavutil/imgutils.h>

} // extern "C"

#include <atomic>
#include <thread>

using namespace djv;

struct FFmpegIO::Private
{
    IOInfo info;
    std::promise<IOInfo> infoPromise;
    std::thread thread;
    std::atomic<bool> running;
    std::condition_variable queueCV;

    AVFormatContext* avFormatContext = nullptr;
    int avVideoStream = -1;
    int avAudioStream = -1;
    std::map<int, AVCodecParameters*> avCodecParameters;
    std::map<int, AVCodecContext*> avCodecContext;
    AVFrame* avFrame = nullptr;
    AVFrame* avFrameRgb = nullptr;
    SwsContext* swsContext = nullptr;

    Timestamp videoTimestamp = timestampInvalid;
    Timestamp audioTimestamp = timestampInvalid;
};

void FFmpegIO::_init(
    const System::File::Info& info,
    const std::shared_ptr<System::LogSystem>& logSystem)
{
    IIO::_init(info, logSystem);
    DJV_PRIVATE_PTR();
    p.running = true;
    p.thread = std::thread(
        [info, this]
        {
            try
            {
                _init(info);
                _work();
            }
            catch (const std::exception& e)
            {
                _p->infoPromise.set_value(IOInfo());
                _logSystem->log("FFmpegIO", e.what(), System::LogLevel::Error);
            }
            _cleanup();
        });
}

FFmpegIO::FFmpegIO() :
    _p(new Private)
{}

FFmpegIO::~FFmpegIO()
{
    DJV_PRIVATE_PTR();
    p.running = false;
    if (p.thread.joinable())
    {
        p.thread.join();
    }
}

std::shared_ptr<FFmpegIO> FFmpegIO::create(
    const System::File::Info& info,
    const std::shared_ptr<System::LogSystem>& logSystem)
{
    auto out = std::shared_ptr<FFmpegIO>(new FFmpegIO);
    out->_init(info, logSystem);
    return out;
}

std::future<IOInfo> FFmpegIO::getInfo()
{
    return _p->infoPromise.get_future();
}

void FFmpegIO::seek(Timestamp value)
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _seek = value;
    }
    _p->queueCV.notify_one();
}

void FFmpegIO::_init(const System::File::Info& info)
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

    p.info.fileInfo = std::string(_fileInfo);

    if (p.avVideoStream != -1 || p.avAudioStream != -1)
    {
        p.avFrame = av_frame_alloc();
    }

    size_t sequenceSize = 0;
    if (p.avVideoStream != -1)
    {
        auto avVideoStream = p.avFormatContext->streams[p.avVideoStream];
        auto avVideoCodecParameters = avVideoStream->codecpar;
        auto avVideoCodec = avcodec_find_decoder(avVideoCodecParameters->codec_id);
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
        p.avCodecContext[p.avVideoStream]->thread_count = 1;
        p.avCodecContext[p.avVideoStream]->thread_type = FF_THREAD_SLICE;
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
        if (avVideoStream->duration != AV_NOPTS_VALUE)
        {
            sequenceSize = av_rescale_q(
                avVideoStream->duration,
                avVideoStream->time_base,
                av_inv_q(avVideoStream->r_frame_rate));
        }
        else if (p.avFormatContext->duration != AV_NOPTS_VALUE)
        {
            sequenceSize = av_rescale_q(
                p.avFormatContext->duration,
                av_get_time_base_q(),
                av_inv_q(avVideoStream->r_frame_rate));
        }
        p.info.videoSpeed = Math::Rational(avVideoStream->r_frame_rate.num, avVideoStream->r_frame_rate.den);
        p.info.videoSequence = Math::Frame::Sequence(Math::Frame::Range(1, sequenceSize));
        p.info.video.push_back(imageInfo);
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

        size_t sampleCount = 0;
        if (avAudioStream->duration != AV_NOPTS_VALUE)
        {
            sampleCount = avAudioStream->duration;
        }
        else if (p.avFormatContext->duration != AV_NOPTS_VALUE)
        {
            sampleCount = av_rescale_q(
                p.avFormatContext->duration,
                av_get_time_base_q(),
                avAudioStream->time_base);
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
        p.info.audio.channelCount = channelCount;
        p.info.audio.type = audioType;
        p.info.audio.sampleRate = p.avCodecParameters[p.avAudioStream]->sample_rate;
        p.info.audio.codec = std::string(avAudioCodec->long_name);
        p.info.audioSampleCount = sampleCount;
    }

    p.infoPromise.set_value(p.info);
}

void FFmpegIO::_work()
{
    DJV_PRIVATE_PTR();
    while (p.running)
    {
        bool read = false;
        Timestamp seek = seekNone;
        {
            std::unique_lock<std::mutex> lock(_mutex);
            if (p.queueCV.wait_for(
                lock,
                System::getTimerDuration(System::TimerValue::Fast),
                [this]
                {
                    const bool video = _p->avVideoStream != -1 && (_videoQueue.isFinished() ? false : (_videoQueue.getCount() < _videoQueue.getMax()));
                    const bool audio = _p->avAudioStream != -1 && (_audioQueue.isFinished() ? false : (_audioQueue.getCount() < _audioQueue.getMax()));
                    return video || audio || _seek != seekNone;
                }))
            {
                read = true;
                if (_seek != seekNone)
                {
                    seek = _seek;
                    _seek = seekNone;
                    _videoQueue.setSeek(true);
                    _videoQueue.setFinished(false);
                    _videoQueue.clearFrames();
                    _audioQueue.setSeek(true);
                    _audioQueue.setFinished(false);
                    _audioQueue.clearFrames();
                }
            }
        }

        AVPacket packet;
        try
        {
            if (seek != seekNone)
            {
                Timestamp ts = 0;
                int stream = -1;
                Timestamp sub = 0;
                if (p.avVideoStream != -1)
                {
                    stream = p.avVideoStream;
                    ts = av_rescale_q(seek, toFFmpeg(timebase), p.avFormatContext->streams[p.avVideoStream]->time_base);
                    sub = av_rescale_q(1, toFFmpeg(p.info.videoSpeed.swap()), toFFmpeg(timebase));
                }
                else if (p.avAudioStream != -1)
                {
                    stream = p.avAudioStream;
                    ts = av_rescale_q(seek, toFFmpeg(timebase), p.avFormatContext->streams[p.avVideoStream]->time_base);
                    sub = av_rescale_q(1, av_make_q(p.info.audio.sampleRate, 1), toFFmpeg(timebase));
                }
                if (p.avVideoStream != -1)
                {
                    avcodec_flush_buffers(p.avCodecContext[p.avVideoStream]);
                }
                if (p.avAudioStream != -1)
                {
                    avcodec_flush_buffers(p.avCodecContext[p.avAudioStream]);
                }
                if (av_seek_frame(
                    p.avFormatContext,
                    stream,
                    ts,
                    AVSEEK_FLAG_BACKWARD) < 0)
                {
                    throw std::exception();
                }
                //std::cout << "FFmpeg seek: " << seek << " / " << ts << std::endl;
                p.videoTimestamp = timestampInvalid;
                p.audioTimestamp = timestampInvalid;
                while ((p.avVideoStream != -1 && p.videoTimestamp < seek - sub) ||
                    (p.avAudioStream != -1 && p.audioTimestamp < seek - sub))
                {
                    if (av_read_frame(p.avFormatContext, &packet) < 0)
                    {
                        if (p.avVideoStream != -1)
                        {
                            _decodeVideo(nullptr, Decode::False);
                            avcodec_flush_buffers(p.avCodecContext[p.avVideoStream]);
                        }
                        if (p.avAudioStream != -1)
                        {
                            _decodeAudio(nullptr, Decode::False);
                            avcodec_flush_buffers(p.avCodecContext[p.avAudioStream]);
                        }
                        throw std::exception();
                    }
                    if (p.avVideoStream == packet.stream_index)
                    {
                        if (_decodeVideo(&packet, Decode::False) < 0)
                        {
                            throw std::exception();
                        }
                    }
                    else if (p.avAudioStream == packet.stream_index)
                    {
                        if (_decodeAudio(&packet, Decode::False) < 0)
                        {
                            throw std::exception();
                        }
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
                        _decodeVideo(nullptr, Decode::True);
                        avcodec_flush_buffers(p.avCodecContext[p.avVideoStream]);
                    }
                    if (p.avAudioStream != -1)
                    {
                        _decodeAudio(nullptr, Decode::True);
                        avcodec_flush_buffers(p.avCodecContext[p.avAudioStream]);
                    }
                    throw std::exception();
                }
                if (p.avVideoStream == packet.stream_index)
                {
                    if (_decodeVideo(&packet, Decode::True) < 0)
                    {
                        throw std::exception();
                    }
                }
                else if (p.avAudioStream == packet.stream_index)
                {
                    if (_decodeAudio(&packet, Decode::True) < 0)
                    {
                        throw std::exception();
                    }
                }
                av_packet_unref(&packet);
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
    }
}

void FFmpegIO::_cleanup()
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

int FFmpegIO::_decodeVideo(AVPacket* packet, Decode decode)
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

        p.videoTimestamp = av_rescale_q(
            p.avFrame->pts,
            p.avFormatContext->streams[p.avVideoStream]->time_base,
            toFFmpeg(timebase));
        //std::cout << "FFmpeg video: " << p.videoTimestamp << " / " << static_cast<int>(decode) << std::endl;

        if (Decode::True == decode)
        {
            Image::Info imageInfo;
            if (p.info.video.size())
            {
                imageInfo = p.info.video[0];
            }
            if (!((0 == p.avFrame->sample_aspect_ratio.num && 1 == p.avFrame->sample_aspect_ratio.den) ||
                0 == p.avFrame->sample_aspect_ratio.den))
            {
                imageInfo.pixelAspectRatio = p.avFrame->sample_aspect_ratio.num / static_cast<float>(p.avFrame->sample_aspect_ratio.den);
            }
            auto image = Image::Data::create(imageInfo);
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
                (uint8_t const* const*)p.avFrame->data,
                p.avFrame->linesize,
                0,
                p.avCodecParameters[p.avVideoStream]->height,
                p.avFrameRgb->data,
                p.avFrameRgb->linesize);

            {
                std::lock_guard<std::mutex> lock(_mutex);
                _videoQueue.addFrame(VideoFrame(p.videoTimestamp, image));
            }
        }
    }
    return r;
}

int FFmpegIO::_decodeAudio(AVPacket* packet, Decode decode)
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

        p.audioTimestamp = av_rescale_q(
            p.avFrame->pts,
            p.avFormatContext->streams[p.avAudioStream]->time_base,
            toFFmpeg(timebase));
        //std::cout << "FFmpeg audio: " << p.audioTimestamp << " / " << static_cast<int>(decode) << std::endl;

        if (Decode::True == decode)
        {
            auto audioData = Audio::Data::create(p.info.audio, p.avFrame->nb_samples);
            extractAudio(
                p.avFrame->data,
                p.avCodecParameters[p.avAudioStream]->format,
                p.avCodecParameters[p.avAudioStream]->channels,
                audioData);
            {
                std::lock_guard<std::mutex> lock(_mutex);
                _audioQueue.addFrame(AudioFrame(p.audioTimestamp, audioData));
            }
        }
    }
    return r;
}

FFmpegPlugin::FFmpegPlugin(const std::shared_ptr<System::LogSystem>& logSystem) :
    IIOPlugin(logSystem)
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
    
std::shared_ptr<IIO> FFmpegPlugin::read(const System::File::Info& info)
{
    return FFmpegIO::create(info, _logSystem);
}
