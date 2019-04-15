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

#include <djvViewApp/Media.h>

#include <djvCore/Context.h>
#include <djvCore/LogSystem.h>
#include <djvCore/Timer.h>

extern "C"
{
#include <libavutil/avutil.h>
#include <libavutil/rational.h>

} // extern "C"

#include <condition_variable>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        namespace
        {
            //! \todo [1.0 S] Should this be configurable?
            const size_t bufferCount = 30;

        } // namespace

        struct Media::Private
        {
            Context * context = nullptr;

            std::string fileName;
            std::shared_ptr<ValueSubject<AV::IO::Info> > info;
            AV::IO::VideoInfo videoInfo;
            AV::IO::AudioInfo audioInfo;
            std::shared_ptr<ValueSubject<Time::Timestamp> > duration;
            std::shared_ptr<ValueSubject<Time::Timestamp> > currentTime;
            std::shared_ptr<ValueSubject<std::shared_ptr<AV::Image::Image> > > currentImage;
            std::shared_ptr<ValueSubject<Playback> > playback;
            std::shared_ptr<ValueSubject<PlaybackMode> > playbackMode;
            std::shared_ptr<ValueSubject<bool> > inOutPointsEnabled;
            std::shared_ptr<ValueSubject<Time::Timestamp> > inPoint;
            std::shared_ptr<ValueSubject<Time::Timestamp> > outPoint;

            std::shared_ptr<ValueSubject<size_t> > videoQueueMax;
            std::shared_ptr<ValueSubject<size_t> > audioQueueMax;
            std::shared_ptr<ValueSubject<size_t> > videoQueueCount;
            std::shared_ptr<ValueSubject<size_t> > audioQueueCount;
            std::shared_ptr<ValueSubject<size_t> > alUnqueuedBuffers;
            std::shared_ptr<AV::IO::Queue> queue;
            std::shared_ptr<Time::Timer> queueTimer;
            std::condition_variable queueCV;
            std::shared_ptr<AV::IO::IRead> read;
            std::future<AV::IO::Info> infoFuture;
            std::shared_ptr<Time::Timer> infoTimer;
            std::chrono::system_clock::time_point startTime;
            
            ALuint alSource = 0;
            std::vector<ALuint> alBuffers;
            int64_t alBytes = 0;
            Time::Timestamp timeOffset = 0;
            
            std::shared_ptr<Time::Timer> playbackTimer;
            std::shared_ptr<Time::Timer> debugTimer;
        };

        void Media::_init(const std::string & fileName, Context * context)
        {
            DJV_PRIVATE_PTR();
            p.context = context;

            p.fileName = fileName;
            p.info = ValueSubject<AV::IO::Info>::create();
            p.duration = ValueSubject<Time::Timestamp>::create();
            p.currentTime = ValueSubject<Time::Timestamp>::create();
            p.currentImage = ValueSubject<std::shared_ptr<AV::Image::Image> >::create();
            p.playback = ValueSubject<Playback>::create();
            p.playbackMode = ValueSubject<PlaybackMode>::create(PlaybackMode::Loop);
            p.inOutPointsEnabled = ValueSubject<bool>::create(false);
            p.inPoint = ValueSubject<Time::Timestamp>::create(0);
            p.outPoint = ValueSubject<Time::Timestamp>::create(0);

            p.videoQueueMax = ValueSubject<size_t>::create();
            p.audioQueueMax = ValueSubject<size_t>::create();
            p.videoQueueCount = ValueSubject<size_t>::create();
            p.audioQueueCount = ValueSubject<size_t>::create();
            p.alUnqueuedBuffers = ValueSubject<size_t>::create();
            p.queue = AV::IO::Queue::create();
            p.queueTimer = Time::Timer::create(context);
            p.queueTimer->setRepeating(true);
            p.infoTimer = Time::Timer::create(context);
            p.infoTimer->setRepeating(true);

            p.playbackTimer = Time::Timer::create(context);
            p.playbackTimer->setRepeating(true);
            p.debugTimer = Time::Timer::create(context);
            p.debugTimer->setRepeating(true);

            alGetError();
            alGenSources(1, &p.alSource);
            ALenum error = AL_NO_ERROR;
            if ((error = alGetError()) != AL_NO_ERROR)
            {
                if (auto logSystem = context->getSystemT<LogSystem>())
                {
                    std::stringstream ss;
                    ss << "djv::ViewApp::Media " << DJV_TEXT("cannot create OpenAL source") << ". " << AV::Audio::getALErrorString(error);
                    logSystem->log("djv::ViewApp::Media", ss.str(), LogLevel::Error);
                }
            }

            if (p.alSource)
            {
                p.alBuffers.resize(bufferCount);
                alGenBuffers(bufferCount, p.alBuffers.data());
                if ((error = alGetError()) != AL_NO_ERROR)
                {
                    p.alBuffers.clear();
                    alDeleteSources(1, &p.alSource);
                    p.alSource = 0;
                    if (auto logSystem = context->getSystemT<LogSystem>())
                    {
                        std::stringstream ss;
                        ss << "djv::ViewApp::Media " << DJV_TEXT("cannot create OpenAL buffers") << ". " << AV::Audio::getALErrorString(error);
                        logSystem->log("djv::ViewApp::Media", ss.str(), LogLevel::Error);
                    }
                }
            }

            if (auto io = context->getSystemT<AV::IO::System>())
            {
                try
                {
                    p.read = io->read(fileName, p.queue);
                    p.infoFuture = p.read->getInfo();

                    const auto timeout = Time::getMilliseconds(Time::TimerValue::Fast);
                    p.infoTimer->start(
                        timeout,
                        [this, fileName, context](float)
                    {
                        DJV_PRIVATE_PTR();
                        if (p.infoFuture.valid() &&
                            p.infoFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                        {
                            p.infoTimer->stop();
                            const auto info = p.infoFuture.get();
                            Time::Timestamp duration = 0;
                            const auto & video = info.video;
                            if (video.size())
                            {
                                p.videoInfo = video[0];
                                duration = std::max(duration, video[0].duration);
                            }
                            const auto & audio = info.audio;
                            if (audio.size())
                            {
                                p.audioInfo = audio[0];
                                duration = std::max(duration, audio[0].duration);
                            }
                            if (auto logSystem = context->getSystemT<LogSystem>())
                            {
                                std::stringstream ss;
                                ss << fileName << " duration: " << duration;
                                logSystem->log("djv::ViewApp::Media", ss.str());
                            }
                            p.info->setIfChanged(info);
                            p.duration->setIfChanged(duration);
                        }
                    });

                    p.queueTimer->start(
                        timeout,
                        [this, timeout](float)
                    {
                        DJV_PRIVATE_PTR();
                        std::unique_lock<std::mutex> lock(p.queue->getMutex());
                        if (p.queueCV.wait_for(
                            lock,
                            timeout,
                            [this]
                        {
                            return _p->queue->hasVideo();
                        }))
                        {
                            p.currentImage->setIfChanged(p.queue->getVideo().second);
                        }
                    });

                    p.debugTimer->start(
                        Time::getMilliseconds(Time::TimerValue::Fast),
                        [this](float)
                    {
                        DJV_PRIVATE_PTR();
                        std::lock_guard<std::mutex> lock(p.queue->getMutex());
                        p.videoQueueMax->setIfChanged(p.queue->getVideoMax());
                        p.audioQueueMax->setIfChanged(p.queue->getAudioMax());
                        p.videoQueueCount->setIfChanged(p.queue->getVideoCount());
                        p.audioQueueCount->setIfChanged(p.queue->getAudioCount());
                        p.alUnqueuedBuffers->setIfChanged(p.alBuffers.size());
                    });
                }
                catch (const std::exception & e)
                {
                    if (auto logSystem = context->getSystemT<LogSystem>())
                    {
                        std::stringstream ss;
                        ss << "djv::ViewApp::Media " << DJV_TEXT("cannot open") << " '" << fileName << "'. " << e.what();
                        logSystem->log("djv::ViewApp::Media", ss.str(), LogLevel::Error);
                    }
                }
            }
        }

        Media::Media() :
            _p(new Private)
        {}

        Media::~Media()
        {
            DJV_PRIVATE_PTR();
            if (p.alBuffers.size())
            {
                alDeleteBuffers(bufferCount, p.alBuffers.data());
                p.alBuffers.clear();
            }
            if (p.alSource)
            {
                alDeleteSources(1, &p.alSource);
                p.alSource = 0;
            }
        }

        std::shared_ptr<Media> Media::create(const std::string & fileName, Context * context)
        {
            auto out = std::shared_ptr<Media>(new Media);
            out->_init(fileName, context);
            return out;
        }

        const std::string & Media::getFileName() const
        {
            return _p->fileName;
        }

        std::shared_ptr<IValueSubject<AV::IO::Info> > Media::observeInfo() const
        {
            return _p->info;
        }

        std::shared_ptr<IValueSubject<Time::Timestamp> > Media::observeDuration() const
        {
            return _p->duration;
        }

        std::shared_ptr<IValueSubject<Time::Timestamp> > Media::observeCurrentTime() const
        {
            return _p->currentTime;
        }

        std::shared_ptr<IValueSubject<std::shared_ptr<AV::Image::Image> > > Media::observeCurrentImage() const
        {
            return _p->currentImage;
        }

        std::shared_ptr<IValueSubject<Playback> > Media::observePlayback() const
        {
            return _p->playback;
        }

        std::shared_ptr<IValueSubject<PlaybackMode> > Media::observePlaybackMode() const
        {
            return _p->playbackMode;
        }

        std::shared_ptr<IValueSubject<bool> > Media::observeInOutPointsEnabled() const
        {
            return _p->inOutPointsEnabled;
        }

        std::shared_ptr<IValueSubject<Time::Timestamp> > Media::observeInPoint() const
        {
            return _p->inPoint;
        }

        std::shared_ptr<IValueSubject<Time::Timestamp> > Media::observeOutPoint() const
        {
            return _p->outPoint;
        }

        std::shared_ptr<IValueSubject<size_t> > Media::observeVideoQueueMax() const
        {
            return _p->videoQueueMax;
        }

        std::shared_ptr<IValueSubject<size_t> > Media::observeAudioQueueMax() const
        {
            return _p->audioQueueMax;
        }

        std::shared_ptr<IValueSubject<size_t> > Media::observeVideoQueueCount() const
        {
            return _p->videoQueueCount;
        }

        std::shared_ptr<IValueSubject<size_t> > Media::observeAudioQueueCount() const
        {
            return _p->audioQueueCount;
        }

        std::shared_ptr<IValueSubject<size_t> > Media::observeALUnqueuedBuffers() const
        {
            return _p->alUnqueuedBuffers;
        }

        void Media::setCurrentTime(Time::Timestamp value)
        {
            DJV_PRIVATE_PTR();
            Time::Timestamp start = 0;
            AVRational r;
            r.num = p.videoInfo.speed.getDuration();
            r.den = p.videoInfo.speed.getScale();
            const int64_t f = av_rescale_q(1, r, av_get_time_base_q());
            const Time::Timestamp duration = p.duration->get();
            Time::Timestamp end = duration - f;
            if (p.inOutPointsEnabled->get())
            {
                start = p.inPoint->get();
                end = p.outPoint->get();
            }
            Time::Timestamp tmp = value;
            while (duration && tmp > end)
            {
                tmp -= duration;
            }
            while (duration && tmp < 0)
            {
                tmp += duration;
            }
            if (p.currentTime->setIfChanged(tmp))
            {
                setPlayback(Playback::Stop);
                if (p.read)
                {
                    p.read->seek(tmp);
                }
            }
        }

        void Media::start()
        {
            setCurrentTime(0);
        }

        void Media::end()
        {
            DJV_PRIVATE_PTR();
            AVRational r;
            r.num = p.videoInfo.speed.getDuration();
            r.den = p.videoInfo.speed.getScale();
            setCurrentTime(p.duration->get() - av_rescale_q(1, r, av_get_time_base_q()));
        }

        void Media::nextFrame(size_t value)
        {
            DJV_PRIVATE_PTR();
            AVRational r;
            r.num = p.videoInfo.speed.getDuration();
            r.den = p.videoInfo.speed.getScale();
            setCurrentTime(p.currentTime->get() + av_rescale_q(value, r, av_get_time_base_q()));
        }

        void Media::prevFrame(size_t value)
        {
            DJV_PRIVATE_PTR();
            AVRational r;
            r.num = p.videoInfo.speed.getDuration();
            r.den = p.videoInfo.speed.getScale();
            setCurrentTime(p.currentTime->get() - av_rescale_q(value, r, av_get_time_base_q()));
        }

        void Media::setPlayback(Playback value)
        {
            if (_p->playback->setIfChanged(value))
            {
                _playbackUpdate();
            }
        }

        void Media::setPlaybackMode(PlaybackMode value)
        {
            _p->playbackMode->setIfChanged(value);
        }

        void Media::_playbackUpdate()
        {
            DJV_PRIVATE_PTR();
            switch (p.playback->get())
            {
            case Playback::Stop:
                if (p.alSource)
                {
                    alSourceStop(p.alSource);
                    alSourcei(p.alSource, AL_BUFFER, 0);
                    alSourcei(p.alSource, AL_BYTE_OFFSET, 0);
                    p.alBytes = 0;
                    p.alBuffers.clear();
                }
                p.playbackTimer->stop();
                break;
            case Playback::Forward:
            case Playback::Reverse:
                p.timeOffset = p.currentTime->get();
                _timeUpdate();
                p.startTime = std::chrono::system_clock::now();
                p.playbackTimer->start(
                    Time::getMilliseconds(Time::TimerValue::Fast),
                    [this](float)
                {
                    DJV_PRIVATE_PTR();
                    const Playback playback = p.playback->get();
                    switch (playback)
                    {
                    case Playback::Forward:
                    case Playback::Reverse:
                    {
                        Time::Timestamp time = 0;
                        const Time::Timestamp duration = p.duration->get();
                        AVRational r;
                        r.num = p.videoInfo.speed.getDuration();
                        r.den = p.videoInfo.speed.getScale();
                        const int64_t f = av_rescale_q(1, r, av_get_time_base_q());
                        if (Playback::Forward == playback && p.audioInfo.info.isValid() && p.alSource)
                        {
                            ALint offset = 0;
                            alGetSourcei(p.alSource, AL_BYTE_OFFSET, &offset);
                            Time::Timestamp pts = (p.alBytes + offset) / AV::Audio::getByteCount(p.audioInfo.info.type);
                            r.num = 1;
                            r.den = static_cast<int>(p.audioInfo.info.sampleRate);
                            time = p.timeOffset + av_rescale_q(pts, r, av_get_time_base_q());
                            /*{
                                std::stringstream ss;
                                ss << "audio time = " << time;
                                p.context->log("djv::ViewApp::Media", ss.str());
                            }*/
                        }
                        else
                        {
                            const auto now = std::chrono::system_clock::now();
                            const std::chrono::duration<double> delta = now - p.startTime;
                            Time::Timestamp pts = static_cast<Time::Timestamp>(delta.count() * p.videoInfo.speed.getScale() / p.videoInfo.speed.getDuration());
                            r.num = p.videoInfo.speed.getDuration();
                            r.den = p.videoInfo.speed.getScale();
                            switch (playback)
                            {
                            case Playback::Forward: time = p.timeOffset + av_rescale_q(pts, r, av_get_time_base_q()); break;
                            case Playback::Reverse: time = p.timeOffset - av_rescale_q(pts, r, av_get_time_base_q()); break;
                            default: break;
                            }
                            /*{
                                std::stringstream ss;
                                ss << "video time = " << time;
                                p.context->log("djv::ViewApp::Media", ss.str());
                            }*/
                        }

                        Time::Timestamp start = 0;
                        Time::Timestamp end = duration - f;
                        if (p.inOutPointsEnabled->get())
                        {
                            start = p.inPoint->get();
                            end = p.outPoint->get();
                        }
                        if (time < start || time > end)
                        {
                            switch (p.playbackMode->get())
                            {
                            case PlaybackMode::Once:
                                setPlayback(Playback::Stop);
                                time = Math::clamp(time, start, end);
                                setCurrentTime(time);
                                break;
                            case PlaybackMode::Loop:
                            {
                                Playback playback = p.playback->get();
                                setPlayback(Playback::Stop);
                                while (duration && time > end)
                                {
                                    time -= duration;
                                }
                                while (duration && time < start)
                                {
                                    time += duration;
                                }
                                setCurrentTime(time);
                                setPlayback(playback);
                                break;
                            }
                            case PlaybackMode::PingPong:
                            {
                                Playback playback = p.playback->get();
                                setPlayback(Playback::Stop);
                                time = Math::clamp(time, start, end);
                                setCurrentTime(time);
                                setPlayback(Playback::Forward == playback ? Playback::Reverse : Playback::Forward);
                                break;
                            }
                            default: break;
                            }
                        }
                        p.currentTime->setIfChanged(time);
                        if (Playback::Reverse == playback && p.read)
                        {
                            p.read->seek(time);
                        }
                        _timeUpdate();
                        break;
                    }
                    default: break;
                    }
                });
                if (p.alSource)
                {
                    alSourcePlay(p.alSource);
                }
                break;
            default: break;
            }
        }

        void Media::_timeUpdate()
        {
            DJV_PRIVATE_PTR();

            // Update the video queue.
            {
                std::lock_guard<std::mutex> lock(p.queue->getMutex());
                while (p.queue->hasVideo() && p.queue->getVideo().first < p.currentTime->get())
                {
                    p.queue->popVideo();
                }
            }

            // Unqueue the processed OpenAL buffers.
            if (p.alSource)
            {
                ALint processed = 0;
                alGetSourcei(p.alSource, AL_BUFFERS_PROCESSED, &processed);
                std::vector<ALuint> buffers;
                buffers.resize(processed);
                alSourceUnqueueBuffers(p.alSource, processed, buffers.data());
                for (const auto& buffer : buffers)
                {
                    p.alBuffers.push_back(buffer);
                    if (p.playback->get() != Playback::Stop)
                    {
                        ALint size = 0;
                        alGetBufferi(buffer, AL_SIZE, &size);
                        p.alBytes += size;
                    }
                }
            }

            if (Playback::Forward == p.playback->get() && p.audioInfo.info.isValid() && p.alSource)
            {
                // Fill the OpenAL queue with frames from the I/O queue.
                ALint queued = 0;
                alGetSourcei(p.alSource, AL_BUFFERS_QUEUED, &queued);
                std::vector<AV::IO::AudioFrame> frames;
                {
                    std::lock_guard<std::mutex> lock(p.queue->getMutex());
                    while (p.queue->hasAudio() &&
                       (frames.size() < (bufferCount - queued)) &&
                       (frames.size() < p.alBuffers.size()))
                    {
                        const auto audioFrame = p.queue->getAudio();
                        p.queue->popAudio();
                        frames.push_back(audioFrame);
                    }
                }
                for (size_t i = 0; i < frames.size(); ++i)
                {
                    auto data = AV::Audio::Data::convert(frames[i].second, AV::Audio::Type::S16);
                    const auto info = data->getInfo();
                    auto buffer = p.alBuffers.back();
                    p.alBuffers.pop_back();
                    alBufferData(
                        buffer,
                        AV::Audio::getALType(info.channelCount, info.type),
                        data->getData(),
                        data->getByteCount(),
                        static_cast<ALsizei>(info.sampleRate));
                    alSourceQueueBuffers(p.alSource, 1, &buffer);
                }
            }
        }

    } // namespace ViewApp
} // namespace djv

