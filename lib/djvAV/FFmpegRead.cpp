// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/FFmpeg.h>

#include <djvCore/FileSystem.h>
#include <djvCore/LogSystem.h>
#include <djvCore/StringFormat.h>
#include <djvCore/TextSystem.h>
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
                    Options options;
                    Info info;
                    std::promise<Info> infoPromise;
                    std::condition_variable queueCV;
                    int64_t seek = Frame::invalid;
                    Direction direction = Direction::Forward;
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
                    const ReadOptions& readOptions,
                    const Options& options,
                    const std::shared_ptr<TextSystem>& textSystem,
                    const std::shared_ptr<ResourceSystem>& resourceSystem,
                    const std::shared_ptr<LogSystem>& logSystem)
                {
                    IRead::_init(fileInfo, readOptions, textSystem, resourceSystem, logSystem);
                    DJV_PRIVATE_PTR();
                    p.options = options;
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
                                throw FileSystem::Error(String::Format("{0}: {1}").
                                    arg(_fileInfo.getFileName()).
                                    arg(FFmpeg::getErrorString(r)));
                            }
                            r = avformat_find_stream_info(p.avFormatContext, 0);
                            if (r < 0)
                            {
                                throw FileSystem::Error(String::Format("{0}: {1}").
                                    arg(_fileInfo.getFileName()).
                                    arg(FFmpeg::getErrorString(r)));
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
                                throw FileSystem::Error(String::Format("{0}: {1}").
                                    arg(_fileInfo.getFileName()).
                                    arg(_textSystem->getText(DJV_TEXT("error_no_streams"))));
                            }

                            p.info.fileName = std::string(_fileInfo);

                            if (p.avVideoStream != -1 || p.avAudioStream != -1)
                            {
                                // Initialize the buffers.
                                p.avFrame = av_frame_alloc();
                            }

                            size_t sequenceSize = 0;
                            if (p.avVideoStream != -1)
                            {
                                // Find the codec for the video stream.
                                auto avVideoStream = p.avFormatContext->streams[p.avVideoStream];
                                auto avVideoCodecParameters = avVideoStream->codecpar;
                                auto avVideoCodec = avcodec_find_decoder(avVideoCodecParameters->codec_id);
                                if (!avVideoCodec)
                                {
                                    throw FileSystem::Error(String::Format("{0}: {1}").
                                        arg(_fileInfo.getFileName()).
                                        arg(_textSystem->getText(DJV_TEXT("error_no_video_codecs"))));
                                }
                                p.avCodecParameters[p.avVideoStream] = avcodec_parameters_alloc();
                                r = avcodec_parameters_copy(p.avCodecParameters[p.avVideoStream], avVideoCodecParameters);
                                if (r < 0)
                                {
                                    throw FileSystem::Error(String::Format("{0}: {1}").
                                        arg(_fileInfo.getFileName()).
                                        arg(FFmpeg::getErrorString(r)));
                                }
                                p.avCodecContext[p.avVideoStream] = avcodec_alloc_context3(avVideoCodec);
                                r = avcodec_parameters_to_context(p.avCodecContext[p.avVideoStream], p.avCodecParameters[p.avVideoStream]);
                                if (r < 0)
                                {
                                    throw FileSystem::Error(String::Format("{0}: {1}").
                                        arg(_fileInfo.getFileName()).
                                        arg(FFmpeg::getErrorString(r)));
                                }
                                p.avCodecContext[p.avVideoStream]->thread_count = p.options.threadCount;
                                p.avCodecContext[p.avVideoStream]->thread_type = FF_THREAD_SLICE;
                                r = avcodec_open2(p.avCodecContext[p.avVideoStream], avVideoCodec, 0);
                                if (r < 0)
                                {
                                    throw FileSystem::Error(String::Format("{0}: {1}").
                                        arg(_fileInfo.getFileName()).
                                        arg(FFmpeg::getErrorString(r)));
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
                                Image::Info imageInfo;
                                imageInfo.size.w = p.avCodecParameters[p.avVideoStream]->width;
                                imageInfo.size.h = p.avCodecParameters[p.avVideoStream]->height;
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
                                else if (p.avFormatContext->duration != AV_NOPTS_VALUE)
                                {
                                    AVRational r;
                                    r.num = avVideoStream->r_frame_rate.den;
                                    r.den = avVideoStream->r_frame_rate.num;
                                    sequenceSize = av_rescale_q(
                                        p.avFormatContext->duration,
                                        av_get_time_base_q(),
                                        r);
                                }
                                p.info.videoSpeed = Math::Rational(avVideoStream->r_frame_rate.num, avVideoStream->r_frame_rate.den);
                                p.info.videoSequence = Frame::Sequence(Frame::Range(1, sequenceSize));
                                p.info.video.push_back(imageInfo);
                                /*{
                                    std::stringstream ss;
                                    ss << _fileInfo << ": image size " << imageInfo.size << "\n";
                                    ss << _fileInfo << ": pixel type " << imageInfo.type << "\n";
                                    ss << _fileInfo << ": duration " << duration << "\n";
                                    ss << _fileInfo << ": speed " << p.info.videoSpeed << "\n";
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
                                    throw FileSystem::Error(String::Format("{0}: {1}").
                                        arg(_fileInfo.getFileName()).
                                        arg(_textSystem->getText(DJV_TEXT("error_unsupported_audio_format"))));
                                }
                                auto avAudioCodec = avcodec_find_decoder(avAudioCodecParameters->codec_id);
                                if (!avAudioCodec)
                                {
                                    throw FileSystem::Error(String::Format("{0}: {1}").
                                        arg(_fileInfo.getFileName()).
                                        arg(_textSystem->getText(DJV_TEXT("error_no_audio_codecs"))));
                                }
                                p.avCodecParameters[p.avAudioStream] = avcodec_parameters_alloc();
                                r = avcodec_parameters_copy(p.avCodecParameters[p.avAudioStream], avAudioCodecParameters);
                                if (r < 0)
                                {
                                    throw FileSystem::Error(String::Format("{0}: {1}").
                                        arg(_fileInfo.getFileName()).
                                        arg(FFmpeg::getErrorString(r)));
                                }
                                p.avCodecContext[p.avAudioStream] = avcodec_alloc_context3(avAudioCodec);
                                r = avcodec_parameters_to_context(p.avCodecContext[p.avAudioStream], p.avCodecParameters[p.avAudioStream]);
                                if (r < 0)
                                {
                                    throw FileSystem::Error(String::Format("{0}: {1}").
                                        arg(_fileInfo.getFileName()).
                                        arg(FFmpeg::getErrorString(r)));
                                }
                                r = avcodec_open2(p.avCodecContext[p.avAudioStream], avAudioCodec, 0);
                                if (r < 0)
                                {
                                    throw FileSystem::Error(String::Format("{0}: {1}").
                                        arg(_fileInfo.getFileName()).
                                        arg(FFmpeg::getErrorString(r)));
                                }

                                // Get information.
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
                                p.info.audio.sampleCount = sampleCount;
                                p.info.audio.codec = std::string(avAudioCodec->long_name);
                            }

                            AVDictionaryEntry* tag = nullptr;
                            while ((tag = av_dict_get(p.avFormatContext->metadata, "", tag, AV_DICT_IGNORE_SUFFIX)))
                            {
                                p.info.tags.set(tag->key, tag->value);
                            }

                            p.infoPromise.set_value(p.info);

                            while (p.running)
                            {
                                //! \todo Implement me!
                                /*bool cacheEnabled = false;
                                size_t cacheMaxByteCount = 0;
                                {
                                    std::lock_guard<std::mutex> lock(_mutex);
                                    cacheEnabled = _cacheEnabled;
                                    cacheMaxByteCount = _cacheMaxByteCount;
                                    _cachedFrames = _getCachedFrames(_cache);
                                }
                                if (!cacheEnabled)
                                {
                                    _cache.clear();
                                }
                                if (info.video.size() && _options.layer < info.video.size())
                                {
                                    const size_t dataByteCount = info.video[_options.layer].info.getDataByteCount();
                                    _cache.setMax(cacheMaxByteCount / dataByteCount);
                                }
                                else
                                {
                                    _cache.setMax(0);
                                }*/

                                bool read = false;
                                int64_t seek = Frame::invalid;
                                {
                                    //const std::vector<Frame::Number> cachedFrames = _cache.getKeys();
                                    std::unique_lock<std::mutex> lock(_mutex);
                                    if (p.queueCV.wait_for(
                                        lock,
                                        Time::getTime(Time::TimerValue::Fast),
                                        [this, sequenceSize]
                                        //[this, sequenceSize, cacheEnabled, &cachedFrames]
                                    {
                                        DJV_PRIVATE_PTR();
                                        const bool video = p.avVideoStream != -1 && (_videoQueue.isFinished() ? false : (_videoQueue.getCount() < _videoQueue.getMax()));
                                        const bool audio = p.avAudioStream != -1 && (_audioQueue.isFinished() ? false : (_audioQueue.getCount() < _audioQueue.getMax()));

                                        /*bool cache = false;
                                        if (cacheEnabled && !_videoQueue.isFinished() && !_audioQueue.isFinished())
                                        {
                                            const size_t cacheMax = _cache.getMax();
                                            cache |= _cache.getSize() < cacheMax;
                                            if (_videoQueue.getFrameCount())
                                            {
                                                const auto& frame = _videoQueue.getFrame();
                                                for (const auto& i : cachedFrames)
                                                {
                                                    if (i < frame.frame || i > frame.frame + cacheMax)
                                                    {
                                                        cache = true;
                                                        break;
                                                    }
                                                }
                                            }
                                        }*/
                                        
                                        return video || audio || p.seek != Frame::invalid || p.direction != _direction;
                                        //return video || audio || p.seek != Frame::invalid || p.direction != _direction || cache;
                                    }))
                                    {
                                        read = true;
                                        if (p.direction != _direction)
                                        {
                                            p.direction = _direction;
                                            _videoQueue.setFinished(false);
                                            _videoQueue.clearFrames();
                                            _audioQueue.setFinished(false);
                                            _audioQueue.clearFrames();
                                        }
                                        if (p.seek != Frame::invalid)
                                        {
                                            seek = p.seek;
                                            p.seek = Frame::invalid;
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
                                    if (seek != Frame::invalid)
                                    {
                                        int64_t t = 0;
                                        int stream = -1;
                                        if (p.avVideoStream != -1)
                                        {
                                            stream = p.avVideoStream;
                                            AVRational r;
                                            r.num = p.info.videoSpeed.getDen();
                                            r.den = p.info.videoSpeed.getNum();
                                            t = av_rescale_q(seek, r, p.avFormatContext->streams[p.avVideoStream]->time_base);
                                            //t = av_rescale_q(seek, r, av_get_time_base_q());
                                        }
                                        else if (p.avAudioStream != -1)
                                        {
                                            stream = p.avAudioStream;
                                            AVRational r;
                                            r.num = 1;
                                            r.den = p.info.audio.sampleRate;
                                            t = av_rescale_q(seek, r, p.avFormatContext->streams[p.avAudioStream]->time_base);
                                            //t = av_rescale_q(seek, r, av_get_time_base_q());
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
                                            t,
                                            AVSEEK_FLAG_BACKWARD) < 0)
                                        {
                                            throw std::exception();
                                        }
                                        Frame::Number videoFrame = Frame::invalid;
                                        Frame::Number audioFrame = Frame::invalid;
                                        while (videoFrame < seek - 1 || audioFrame < seek - 1)
                                        {
                                            if (av_read_frame(p.avFormatContext, &packet) < 0)
                                            {
                                                if (p.avVideoStream != -1)
                                                {
                                                    DecodeVideo dv;
                                                    //dv.cacheEnabled = cacheEnabled;
                                                    dv.seek         = seek;
                                                    _decodeVideo(dv, videoFrame);
                                                    avcodec_flush_buffers(p.avCodecContext[p.avVideoStream]);
                                                }
                                                if (p.avAudioStream != -1)
                                                {
                                                    DecodeAudio da;
                                                    da.seek = seek;
                                                    _decodeAudio(da, audioFrame);
                                                    avcodec_flush_buffers(p.avCodecContext[p.avAudioStream]);
                                                }
                                                throw std::exception();
                                            }
                                            if (p.avVideoStream == packet.stream_index)
                                            {
                                                DecodeVideo dv;
                                                dv.packet       = &packet;
                                                dv.seek         = seek;
                                                //dv.cacheEnabled = cacheEnabled;
                                                if (_decodeVideo(dv, videoFrame) < 0)
                                                {
                                                    throw std::exception();
                                                }
                                            }
                                            else if (p.avAudioStream == packet.stream_index)
                                            {
                                                DecodeAudio da;
                                                da.packet = &packet;
                                                da.seek   = seek;
                                                if (_decodeAudio(da, audioFrame) < 0)
                                                {
                                                    throw std::exception();
                                                }
                                            }
                                            av_packet_unref(&packet);
                                        }
                                    }
                                    if (read)
                                    {
                                        Frame::Number videoFrame = Frame::invalid;
                                        Frame::Number audioFrame = Frame::invalid;
                                        int r = av_read_frame(p.avFormatContext, &packet);
                                        if (r < 0)
                                        {
                                            if (p.avVideoStream != -1)
                                            {
                                                DecodeVideo dv;
                                                //dv.cacheEnabled = cacheEnabled;
                                                _decodeVideo(dv, videoFrame);
                                                avcodec_flush_buffers(p.avCodecContext[p.avVideoStream]);
                                            }
                                            if (p.avAudioStream != -1)
                                            {
                                                DecodeAudio da;
                                                _decodeAudio(da, audioFrame);
                                                avcodec_flush_buffers(p.avCodecContext[p.avAudioStream]);
                                            }
                                            throw std::exception();
                                        }
                                        if (p.avVideoStream == packet.stream_index)
                                        {
                                            DecodeVideo dv;
                                            dv.packet       = &packet;
                                            //dv.cacheEnabled = cacheEnabled;
                                            if (_decodeVideo(dv, videoFrame) < 0)
                                            {
                                                throw std::exception();
                                            }
                                        }
                                        else if (p.avAudioStream == packet.stream_index)
                                        {
                                            DecodeAudio da;
                                            da.packet = &packet;
                                            if (_decodeAudio(da, audioFrame) < 0)
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
                        catch (const std::exception& e)
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
                    const Options& options,
                    const std::shared_ptr<TextSystem>& textSystem,
                    const std::shared_ptr<ResourceSystem>& resourceSystem,
                    const std::shared_ptr<LogSystem>& logSystem)
                {
                    auto out = std::shared_ptr<Read>(new Read);
                    out->_init(fileInfo, readOptions, options, textSystem, resourceSystem, logSystem);
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

                void Read::seek(Frame::Number value, Direction)
                {
                    DJV_PRIVATE_PTR();
                    {
                        std::lock_guard<std::mutex> lock(_mutex);
                        _videoQueue.clearFrames();
                        _audioQueue.clearFrames();
                        p.seek = value;
                    }
                    p.queueCV.notify_one();
                }

                int Read::_decodeVideo(const DecodeVideo& dv, Frame::Number& frame)
                {
                    DJV_PRIVATE_PTR();
                    int r = avcodec_send_packet(p.avCodecContext[p.avVideoStream], dv.packet);
                    while (r >= 0)
                    {
                        r = avcodec_receive_frame(p.avCodecContext[p.avVideoStream], p.avFrame);
                        if (AVERROR(EAGAIN) == r)
                        {
                            r = 0;
                            return 0;
                        }
                        else if (r < 0)
                        {
                            break;
                        }
                        
                        AVRational r;
                        r.num = p.info.videoSpeed.getDen();
                        r.den = p.info.videoSpeed.getNum();
                        frame = av_rescale_q(
                            p.avFrame->pts,
                            p.avFormatContext->streams[p.avVideoStream]->time_base,
                            r);
                        //std::cout << "decode video = " << frame << std::endl;

                        if (Frame::invalid == dv.seek || frame >= dv.seek)
                        {
                            std::shared_ptr<Image::Image> image;
                            if (dv.cacheEnabled && _cache.get(frame, image))
                            {}
                            else
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
                                image = Image::Image::create(imageInfo);
                                image->setPluginName(pluginName);
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
                                if (dv.cacheEnabled)
                                {
                                    _cache.add(frame, image);
                                }
                            }
                            {
                                std::lock_guard<std::mutex> lock(_mutex);
                                if (Frame::invalid == p.seek)
                                {
                                    _videoQueue.addFrame(VideoFrame(frame, image));
                                }
                            }
                        }
                    }
                    return r;
                }

                int Read::_decodeAudio(const DecodeAudio& da, Frame::Number& frame)
                {
                    DJV_PRIVATE_PTR();
                    int r = avcodec_send_packet(p.avCodecContext[p.avAudioStream], da.packet);
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

                        AVRational r;
                        r.num = p.info.videoSpeed.getDen();
                        r.den = p.info.videoSpeed.getNum();
                        frame = av_rescale_q(
                            p.avFrame->pts,
                            p.avFormatContext->streams[p.avAudioStream]->time_base,
                            r);
                        //std::cout << "decode audio = " << frame << std::endl;

                        if (Frame::invalid == da.seek || frame >= da.seek)
                        {
                            auto audioInfo = p.info.audio;
                            audioInfo.sampleCount = p.avFrame->nb_samples;
                            auto audioData = Audio::Data::create(audioInfo);
                            extractAudio(
                                p.avFrame->data,
                                p.avCodecParameters[p.avAudioStream]->format,
                                p.avCodecParameters[p.avAudioStream]->channels,
                                audioData);
                            {
                                std::lock_guard<std::mutex> lock(_mutex);
                                if (Frame::invalid == p.seek)
                                {
                                    _audioQueue.addFrame(AudioFrame(audioData));
                                }
                            }
                        }
                    }
                    return r;
                }

            } // namespace FFmpeg
        } // namespace IO
    } // namespace AV
} // namespace djv

