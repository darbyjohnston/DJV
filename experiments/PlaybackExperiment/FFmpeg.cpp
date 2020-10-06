// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include "FFmpeg.h"

#include <djvSystem/LogSystem.h>
#include <djvSystem/TimerFunc.h>

extern "C"
{
#include <libavutil/dict.h>
#include <libavutil/imgutils.h>

} // extern "C"

using namespace djv;

namespace
{
    Audio::Type toAudioType(AVSampleFormat value)
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

} // namespace

void FFmpegIO::_init(
    const System::File::Info& info,
    const std::shared_ptr<System::LogSystem>& logSystem)
{
    IIO::_init(info, logSystem);
    _running = true;
    _thread = std::thread(
        [info, this]
        {
            try
            {
                _init(info);
                _work();
            }
            catch (const std::exception& e)
            {
                _infoPromise.set_value(IOInfo());
                _logSystem->log("FFmpegIO", e.what(), System::LogLevel::Error);
            }
            _cleanup();
        });
}

FFmpegIO::FFmpegIO()
{}

FFmpegIO::~FFmpegIO()
{
    _running = false;
    if (_thread.joinable())
    {
        _thread.join();
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
    return _infoPromise.get_future();
}

void FFmpegIO::seek(int64_t value)
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _videoQueue.clearFrames();
        _audioQueue.clearFrames();
        _seek = value;
    }
    _queueCV.notify_one();
}

void FFmpegIO::_init(const System::File::Info& info)
{
    int r = avformat_open_input(
        &_avFormatContext,
        _fileInfo.getFileName().c_str(),
        nullptr,
        nullptr);
    if (r < 0)
    {
        std::stringstream ss;
        ss << "Cannot open " << info.getFileName();
        throw std::runtime_error(ss.str());
    }
    r = avformat_find_stream_info(_avFormatContext, 0);
    if (r < 0)
    {
        std::stringstream ss;
        ss << "Cannot open " << info.getFileName();
        throw std::runtime_error(ss.str());
    }
    av_dump_format(_avFormatContext, 0, _fileInfo.getFileName().c_str(), 0);

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
        std::stringstream ss;
        ss << "Cannot open " << info.getFileName();
        throw std::runtime_error(ss.str());
    }

    _info.fileInfo = std::string(_fileInfo);

    if (_avVideoStream != -1 || _avAudioStream != -1)
    {
        _avFrame = av_frame_alloc();
    }

    size_t sequenceSize = 0;
    if (_avVideoStream != -1)
    {
        auto avVideoStream = _avFormatContext->streams[_avVideoStream];
        auto avVideoCodecParameters = avVideoStream->codecpar;
        auto avVideoCodec = avcodec_find_decoder(avVideoCodecParameters->codec_id);
        if (!avVideoCodec)
        {
            std::stringstream ss;
            ss << "Cannot open " << info.getFileName();
            throw std::runtime_error(ss.str());
        }
        _avCodecParameters[_avVideoStream] = avcodec_parameters_alloc();
        r = avcodec_parameters_copy(_avCodecParameters[_avVideoStream], avVideoCodecParameters);
        if (r < 0)
        {
            std::stringstream ss;
            ss << "Cannot open " << info.getFileName();
            throw std::runtime_error(ss.str());
        }
        _avCodecContext[_avVideoStream] = avcodec_alloc_context3(avVideoCodec);
        r = avcodec_parameters_to_context(_avCodecContext[_avVideoStream], _avCodecParameters[_avVideoStream]);
        if (r < 0)
        {
            std::stringstream ss;
            ss << "Cannot open " << info.getFileName();
            throw std::runtime_error(ss.str());
        }
        _avCodecContext[_avVideoStream]->thread_count = 1;
        _avCodecContext[_avVideoStream]->thread_type = FF_THREAD_SLICE;
        r = avcodec_open2(_avCodecContext[_avVideoStream], avVideoCodec, 0);
        if (r < 0)
        {
            std::stringstream ss;
            ss << "Cannot open " << info.getFileName();
            throw std::runtime_error(ss.str());
        }

        _avFrameRgb = av_frame_alloc();

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

        Image::Info imageInfo;
        imageInfo.size.w = _avCodecParameters[_avVideoStream]->width;
        imageInfo.size.h = _avCodecParameters[_avVideoStream]->height;
        imageInfo.type = Image::Type::RGBA_U8;
        imageInfo.codec = avVideoCodec->long_name;
        if (avVideoStream->duration != AV_NOPTS_VALUE)
        {
            AVRational r;
            r.num = avVideoStream->r_frame_rate.den;
            r.den = avVideoStream->r_frame_rate.num;
            sequenceSize = av_rescale_q(
                avVideoStream->duration,
                avVideoStream->time_base,
                r);
        }
        else if (_avFormatContext->duration != AV_NOPTS_VALUE)
        {
            AVRational r;
            r.num = avVideoStream->r_frame_rate.den;
            r.den = avVideoStream->r_frame_rate.num;
            sequenceSize = av_rescale_q(
                _avFormatContext->duration,
                av_get_time_base_q(),
                r);
        }
        _info.videoSpeed = Math::Rational(avVideoStream->r_frame_rate.num, avVideoStream->r_frame_rate.den);
        _info.videoSequence = Math::Frame::Sequence(Math::Frame::Range(1, sequenceSize));
        _info.video.push_back(imageInfo);
    }

    if (_avAudioStream != -1)
    {
        auto avAudioStream = _avFormatContext->streams[_avAudioStream];
        auto avAudioCodecParameters = avAudioStream->codecpar;
        Audio::Type audioType = toAudioType(static_cast<AVSampleFormat>(avAudioCodecParameters->format));
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
        _avCodecParameters[_avAudioStream] = avcodec_parameters_alloc();
        r = avcodec_parameters_copy(_avCodecParameters[_avAudioStream], avAudioCodecParameters);
        if (r < 0)
        {
            std::stringstream ss;
            ss << "Cannot open " << info.getFileName();
            throw std::runtime_error(ss.str());
        }
        _avCodecContext[_avAudioStream] = avcodec_alloc_context3(avAudioCodec);
        r = avcodec_parameters_to_context(_avCodecContext[_avAudioStream], _avCodecParameters[_avAudioStream]);
        if (r < 0)
        {
            std::stringstream ss;
            ss << "Cannot open " << info.getFileName();
            throw std::runtime_error(ss.str());
        }
        r = avcodec_open2(_avCodecContext[_avAudioStream], avAudioCodec, 0);
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
        else if (_avFormatContext->duration != AV_NOPTS_VALUE)
        {
            sampleCount = av_rescale_q(
                _avFormatContext->duration,
                av_get_time_base_q(),
                avAudioStream->time_base);
        }
        uint8_t channelCount = _avCodecParameters[_avAudioStream]->channels;
        switch (channelCount)
        {
        case 1:
        case 2:
        case 6:
        case 7:
        case 8: break;
        default: channelCount = 2; break;
        }
        _info.audio.channelCount = channelCount;
        _info.audio.type = audioType;
        _info.audio.sampleRate = _avCodecParameters[_avAudioStream]->sample_rate;
        _info.audio.sampleCount = sampleCount;
        _info.audio.codec = std::string(avAudioCodec->long_name);
    }

    _infoPromise.set_value(_info);
}

void FFmpegIO::_work()
{
    while (_running)
    {
        bool read = false;
        int64_t seek = seekNone;
        {
            std::unique_lock<std::mutex> lock(_mutex);
            if (_queueCV.wait_for(
                lock,
                System::getTimerDuration(System::TimerValue::Fast),
                [this]
                {
                    const bool video = _avVideoStream != -1 && (_videoQueue.isFinished() ? false : (_videoQueue.getCount() < _videoQueue.getMax()));
                    const bool audio = _avAudioStream != -1 && (_audioQueue.isFinished() ? false : (_audioQueue.getCount() < _audioQueue.getMax()));
                    return video || audio || _seek != seekNone;
                }))
            {
                read = true;
                if (_seek != seekNone)
                {
                    seek = _seek;
                    _seek = seekNone;
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
            if (seek != seekNone)
            {
                int64_t t = 0;
                int stream = -1;
                if (_avVideoStream != -1)
                {
                    stream = _avVideoStream;
                    AVRational r;
                    r.num = _info.videoSpeed.getDen();
                    r.den = _info.videoSpeed.getNum();
                    t = av_rescale_q(seek, r, _avFormatContext->streams[_avVideoStream]->time_base);
                    //t = av_rescale_q(seek, r, av_get_time_base_q());
                }
                else if (_avAudioStream != -1)
                {
                    stream = _avAudioStream;
                    AVRational r;
                    r.num = 1;
                    r.den = _info.audio.sampleRate;
                    t = av_rescale_q(seek, r, _avFormatContext->streams[_avAudioStream]->time_base);
                    //t = av_rescale_q(seek, r, av_get_time_base_q());
                }
                if (_avVideoStream != -1)
                {
                    avcodec_flush_buffers(_avCodecContext[_avVideoStream]);
                }
                if (_avAudioStream != -1)
                {
                    avcodec_flush_buffers(_avCodecContext[_avAudioStream]);
                }
                if (av_seek_frame(
                    _avFormatContext,
                    stream,
                    t,
                    AVSEEK_FLAG_BACKWARD) < 0)
                {
                    throw std::exception();
                }
                _videoTime = timeInvalid;
                _audioTime = timeInvalid;
                while ((_avVideoStream != -1 && _videoTime < seek - 1) ||
                    (_avAudioStream != -1 && _audioTime < seek - 1))
                {
                    if (av_read_frame(_avFormatContext, &packet) < 0)
                    {
                        if (_avVideoStream != -1)
                        {
                            DecodeVideo dv;
                            dv.seek = seek;
                            _decodeVideo(dv);
                            avcodec_flush_buffers(_avCodecContext[_avVideoStream]);
                        }
                        if (_avAudioStream != -1)
                        {
                            DecodeAudio da;
                            da.seek = seek;
                            _decodeAudio(da);
                            avcodec_flush_buffers(_avCodecContext[_avAudioStream]);
                        }
                        throw std::exception();
                    }
                    if (_avVideoStream == packet.stream_index)
                    {
                        DecodeVideo dv;
                        dv.packet = &packet;
                        dv.seek = seek;
                        if (_decodeVideo(dv) < 0)
                        {
                            throw std::exception();
                        }
                    }
                    else if (_avAudioStream == packet.stream_index)
                    {
                        DecodeAudio da;
                        da.packet = &packet;
                        da.seek = seek;
                        if (_decodeAudio(da) < 0)
                        {
                            throw std::exception();
                        }
                    }
                    av_packet_unref(&packet);
                }
            }
            if (read)
            {
                int r = av_read_frame(_avFormatContext, &packet);
                if (r < 0)
                {
                    if (_avVideoStream != -1)
                    {
                        DecodeVideo dv;
                        _decodeVideo(dv);
                        avcodec_flush_buffers(_avCodecContext[_avVideoStream]);
                    }
                    if (_avAudioStream != -1)
                    {
                        DecodeAudio da;
                        _decodeAudio(da);
                        avcodec_flush_buffers(_avCodecContext[_avAudioStream]);
                    }
                    throw std::exception();
                }
                if (_avVideoStream == packet.stream_index)
                {
                    DecodeVideo dv;
                    dv.packet = &packet;
                    if (_decodeVideo(dv) < 0)
                    {
                        throw std::exception();
                    }
                }
                else if (_avAudioStream == packet.stream_index)
                {
                    DecodeAudio da;
                    da.packet = &packet;
                    if (_decodeAudio(da) < 0)
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
    if (_swsContext)
    {
        sws_freeContext(_swsContext);
    }
    if (_avFrameRgb)
    {
        av_frame_free(&_avFrameRgb);
    }
    if (_avFrame)
    {
        av_frame_free(&_avFrame);
    }
    for (auto i : _avCodecContext)
    {
        avcodec_close(i.second);
        avcodec_free_context(&i.second);
    }
    for (auto i : _avCodecParameters)
    {
        avcodec_parameters_free(&i.second);
    }
    if (_avFormatContext)
    {
        avformat_close_input(&_avFormatContext);
    }
}

int FFmpegIO::_decodeVideo(const DecodeVideo& dv)
{
    int r = avcodec_send_packet(_avCodecContext[_avVideoStream], dv.packet);
    while (r >= 0)
    {
        r = avcodec_receive_frame(_avCodecContext[_avVideoStream], _avFrame);
        if (AVERROR(EAGAIN) == r)
        {
            r = 0;
            break;
        }
        else if (r < 0)
        {
            break;
        }

        AVRational r;
        r.num = _info.videoSpeed.getDen();
        r.den = _info.videoSpeed.getNum();
        _videoTime = av_rescale_q(
            _avFrame->pts,
            _avFormatContext->streams[_avVideoStream]->time_base,
            r);

        if (seekNone == dv.seek)
        {
            Image::Info imageInfo;
            if (_info.video.size())
            {
                imageInfo = _info.video[0];
            }
            if (!((0 == _avFrame->sample_aspect_ratio.num && 1 == _avFrame->sample_aspect_ratio.den) ||
                0 == _avFrame->sample_aspect_ratio.den))
            {
                imageInfo.pixelAspectRatio = _avFrame->sample_aspect_ratio.num / static_cast<float>(_avFrame->sample_aspect_ratio.den);
            }
            auto image = Image::Image::create(imageInfo);
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
                (uint8_t const* const*)_avFrame->data,
                _avFrame->linesize,
                0,
                _avCodecParameters[_avVideoStream]->height,
                _avFrameRgb->data,
                _avFrameRgb->linesize);

            {
                std::lock_guard<std::mutex> lock(_mutex);
                _videoQueue.addFrame(VideoFrame(_videoTime, image));
            }
        }
    }
    return r;
}

int FFmpegIO::_decodeAudio(const DecodeAudio& da)
{
    int r = avcodec_send_packet(_avCodecContext[_avAudioStream], da.packet);
    while (r >= 0)
    {
        r = avcodec_receive_frame(_avCodecContext[_avAudioStream], _avFrame);
        if (AVERROR(EAGAIN) == r)
        {
            r = 0;
            break;
        }
        else if (r < 0)
        {
            break;
        }

        AVRational r;
        r.num = _info.videoSpeed.getDen();
        r.den = _info.videoSpeed.getNum();
        _audioTime = av_rescale_q(
            _avFrame->pts,
            _avFormatContext->streams[_avAudioStream]->time_base,
            r);

        if (seekNone == da.seek)
        {
            auto audioInfo = _info.audio;
            audioInfo.sampleCount = _avFrame->nb_samples;
            auto audioData = Audio::Data::create(audioInfo);
            {
                std::lock_guard<std::mutex> lock(_mutex);
                _audioQueue.addFrame(AudioFrame(_audioTime, audioData));
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
