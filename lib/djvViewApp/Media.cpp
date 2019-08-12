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

#include <djvAV/AVSystem.h>

#include <djvCore/Context.h>
#include <djvCore/LogSystem.h>
#include <djvCore/Timer.h>

#include <condition_variable>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        namespace
        {
            //! \todo Should this be configurable?
            const size_t bufferCount = 30;
            const size_t videoQueueSize = 30;

            struct Track
            {
                std::shared_ptr<AV::IO::IRead> read;
                Frame::Range range;
            };
            
        } // namespace

        struct Media::Private
        {
            Context * context = nullptr;

            Core::FileSystem::FileInfo fileInfo;
            std::shared_ptr<ValueSubject<AV::IO::Info> > info;
            AV::IO::VideoInfo videoInfo;
            AV::IO::AudioInfo audioInfo;
            std::shared_ptr<ValueSubject<bool> > reload;
            std::shared_ptr<ValueSubject<size_t> > layer;
            std::shared_ptr<ValueSubject<Time::Speed> > speed;
            std::shared_ptr<ValueSubject<Time::Speed> > defaultSpeed;
            std::shared_ptr<ValueSubject<float> > realSpeed;
            std::shared_ptr<ValueSubject<bool> > playEveryFrame;
            std::shared_ptr<ValueSubject<Frame::Sequence> > sequence;
            std::shared_ptr<ValueSubject<Frame::Number> > currentFrame;
            std::shared_ptr<ValueSubject<std::shared_ptr<AV::Image::Image> > > currentImage;
            std::shared_ptr<ValueSubject<Playback> > playback;
            std::shared_ptr<ValueSubject<PlaybackMode> > playbackMode;
            std::shared_ptr<ValueSubject<bool> > inOutPointsEnabled;
            std::shared_ptr<ValueSubject<Frame::Number> > inPoint;
            std::shared_ptr<ValueSubject<Frame::Number> > outPoint;
            std::shared_ptr<ValueSubject<float> > volume;
            std::shared_ptr<ValueSubject<bool> > mute;
            std::shared_ptr<ValueSubject<size_t> > threadCount;
            std::shared_ptr<ListSubject<Frame::Range> > cachedFrames;

            std::shared_ptr<ValueSubject<size_t> > videoQueueMax;
            std::shared_ptr<ValueSubject<size_t> > videoQueueCount;
            std::shared_ptr<ValueSubject<size_t> > audioQueueMax;
            std::shared_ptr<ValueSubject<size_t> > audioQueueCount;
            std::shared_ptr<ValueSubject<size_t> > alQueueCount;
            std::shared_ptr<Time::Timer> queueTimer;
            std::condition_variable queueCV;
            std::vector<Track> tracks;
            int currentTrack = -1;

            AV::IO::Playback ioPlayback = AV::IO::Playback::Forward;
            ALuint alSource = 0;
            std::vector<ALuint> alBuffers;
            Frame::Number frameOffset = 0;
            std::chrono::system_clock::time_point startTime;
            std::chrono::system_clock::time_point realSpeedTime;
            size_t realSpeedFrameCount = 0;
            std::shared_ptr<Time::Timer> playbackTimer;
            std::shared_ptr<Time::Timer> realSpeedTimer;
            std::shared_ptr<Time::Timer> cachedFramesTimer;
            std::shared_ptr<Time::Timer> debugTimer;
            
            std::shared_ptr<AV::IO::IRead> getCurrentRead() const
            {
                std::shared_ptr<AV::IO::IRead> out;
                if (currentTrack != -1)
                {
                    out = tracks[currentTrack].read;
                }
                return out;
            }
        };

        void Media::_init(const Core::FileSystem::FileInfo& fileInfo, Context * context)
        {
            DJV_PRIVATE_PTR();
            p.context = context;

            p.fileInfo = fileInfo;
            p.info = ValueSubject<AV::IO::Info>::create();
            p.reload = ValueSubject<bool>::create();
            p.layer = ValueSubject<size_t>::create();
            p.speed = ValueSubject<Time::Speed>::create();
            p.defaultSpeed = ValueSubject<Time::Speed>::create();
            p.realSpeed = ValueSubject<float>::create();
            p.playEveryFrame = ValueSubject<bool>::create();
            p.sequence = ValueSubject<Frame::Sequence>::create();
            p.currentFrame = ValueSubject<Frame::Number>::create();
            p.currentImage = ValueSubject<std::shared_ptr<AV::Image::Image> >::create();
            p.playback = ValueSubject<Playback>::create();
            p.playbackMode = ValueSubject<PlaybackMode>::create(PlaybackMode::Loop);
            p.inOutPointsEnabled = ValueSubject<bool>::create(false);
            p.inPoint = ValueSubject<Frame::Number>::create(Frame::invalid);
            p.outPoint = ValueSubject<Frame::Number>::create(Frame::invalid);
            p.volume = ValueSubject<float>::create(1.f);
            p.mute = ValueSubject<bool>::create(false);
            p.threadCount = ValueSubject<size_t>::create(4);
            p.cachedFrames = ListSubject<Frame::Range>::create();

            p.videoQueueMax = ValueSubject<size_t>::create();
            p.audioQueueMax = ValueSubject<size_t>::create();
            p.videoQueueCount = ValueSubject<size_t>::create();
            p.audioQueueCount = ValueSubject<size_t>::create();
            p.alQueueCount = ValueSubject<size_t>::create();
            p.queueTimer = Time::Timer::create(context);
            p.queueTimer->setRepeating(true);

            p.playbackTimer = Time::Timer::create(context);
            p.playbackTimer->setRepeating(true);
            p.realSpeedTimer = Time::Timer::create(context);
            p.realSpeedTimer->setRepeating(true);
            p.cachedFramesTimer = Time::Timer::create(context);
            p.cachedFramesTimer->setRepeating(true);
            p.debugTimer = Time::Timer::create(context);
            p.debugTimer->setRepeating(true);

            alGetError();
            alGenSources(1, &p.alSource);
            ALenum error = alGetError();
            if (error != AL_NO_ERROR)
            {
                std::stringstream ss;
                ss << "djv::ViewApp::Media " << DJV_TEXT("cannot create OpenAL source") << ". " << AV::Audio::getALErrorString(error);
                auto logSystem = context->getSystemT<LogSystem>();
                logSystem->log("djv::ViewApp::Media", ss.str(), LogLevel::Error);
            }

            if (p.alSource)
            {
                p.alBuffers.resize(bufferCount);
                alGenBuffers(bufferCount, p.alBuffers.data());
                error = alGetError();
                if (error != AL_NO_ERROR)
                {
                    p.alBuffers.clear();
                    alDeleteSources(1, &p.alSource);
                    p.alSource = 0;
                    {
                        std::stringstream ss;
                        ss << "djv::ViewApp::Media " << DJV_TEXT("cannot create OpenAL buffers") << ". " << AV::Audio::getALErrorString(error);
                        auto logSystem = context->getSystemT<LogSystem>();
                        logSystem->log("djv::ViewApp::Media", ss.str(), LogLevel::Error);
                    }
                }
            }

            _open();
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
                alSourceStop(p.alSource);
                alDeleteSources(1, &p.alSource);
                p.alSource = 0;
            }
        }

        std::shared_ptr<Media> Media::create(const Core::FileSystem::FileInfo& fileInfo, Context* context)
        {
            auto out = std::shared_ptr<Media>(new Media);
            out->_init(fileInfo, context);
            return out;
        }

        const Core::FileSystem::FileInfo& Media::getFileInfo() const
        {
            return _p->fileInfo;
        }

        std::shared_ptr<IValueSubject<AV::IO::Info> > Media::observeInfo() const
        {
            return _p->info;
        }

        std::shared_ptr<Core::IValueSubject<bool> > Media::observeReload() const
        {
            return _p->reload;
        }

        void Media::reload()
        {
            _open();
        }

        std::shared_ptr<IValueSubject<size_t> > Media::observeLayer() const
        {
            return _p->layer;
        }

        void Media::setLayer(size_t value)
        {
            if (_p->layer->setIfChanged(value))
            {
                _open();
            }
        }

        void Media::nextLayer()
        {
            size_t layer = _p->layer->get();
            ++layer;
            if (layer >= _p->info->get().video.size())
            {
                layer = 0;
            }
            setLayer(layer);
        }

        void Media::prevLayer()
        {
            size_t layer = _p->layer->get();
            const size_t size = _p->info->get().video.size();
            if (layer > 0)
            {
                --layer;
            }
            else if (size > 0)
            {
                layer = size - 1;
            }
            else
            {
                layer = 0;
            }
            setLayer(layer);
        }

        std::shared_ptr<IValueSubject<std::shared_ptr<AV::Image::Image> > > Media::observeCurrentImage() const
        {
            return _p->currentImage;
        }

        std::shared_ptr<IValueSubject<Time::Speed> > Media::observeSpeed() const
        {
            return _p->speed;
        }

        std::shared_ptr<IValueSubject<Time::Speed> > Media::observeDefaultSpeed() const
        {
            return _p->defaultSpeed;
        }

        std::shared_ptr<IValueSubject<float> > Media::observeRealSpeed() const
        {
            return _p->realSpeed;
        }

        std::shared_ptr<IValueSubject<bool> > Media::observePlayEveryFrame() const
        {
            return _p->playEveryFrame;
        }

        std::shared_ptr<IValueSubject<Frame::Sequence> > Media::observeSequence() const
        {
            return _p->sequence;
        }

        std::shared_ptr<IValueSubject<Frame::Number> > Media::observeCurrentFrame() const
        {
            return _p->currentFrame;
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

        std::shared_ptr<IValueSubject<Frame::Number> > Media::observeInPoint() const
        {
            return _p->inPoint;
        }

        std::shared_ptr<IValueSubject<Frame::Number> > Media::observeOutPoint() const
        {
            return _p->outPoint;
        }

        void Media::setSpeed(const Time::Speed& value)
        {
            DJV_PRIVATE_PTR();
            if (p.speed->setIfChanged(value))
            {
                p.frameOffset = p.currentFrame->get();
                p.realSpeedFrameCount = 0;
                p.startTime = std::chrono::system_clock::now();
                p.realSpeedTime = p.startTime;
            }
        }

        void Media::setPlayEveryFrame(bool value)
        {
            _p->playEveryFrame->setIfChanged(value);
        }

        void Media::setCurrentFrame(Frame::Number value)
        {
            DJV_PRIVATE_PTR();
            Frame::Number start = 0;
            const size_t size = p.sequence->get().getSize();
            Frame::Number end = static_cast<Frame::Number>(size) - 1;
            if (p.inOutPointsEnabled->get())
            {
                start = p.inPoint->get();
                end = p.outPoint->get();
            }
            Frame::Number tmp = value;
            if (tmp > end)
            {
                tmp = 0;
            }
            if (tmp < 0)
            {
                tmp = end;
            }
            if (p.currentFrame->setIfChanged(tmp))
            {
                setPlayback(Playback::Stop);
                _seek(tmp);
            }
        }

        void Media::inPoint()
        {
            start();
        }

        void Media::outPoint()
        {
            end();
        }

        void Media::start()
        {
            setCurrentFrame(0);
        }

        void Media::end()
        {
            DJV_PRIVATE_PTR();
            const size_t size = p.sequence->get().getSize();
            const Frame::Number frame = size > 0 ? (size - 1) : 0;
            setCurrentFrame(frame);
        }

        void Media::nextFrame(size_t value)
        {
            DJV_PRIVATE_PTR();
            const Frame::Number frame = p.currentFrame->get();
            setCurrentFrame(frame + static_cast<Frame::Number>(value));
        }

        void Media::prevFrame(size_t value)
        {
            DJV_PRIVATE_PTR();
            const Frame::Number frame = p.currentFrame->get();
            setCurrentFrame(frame - static_cast<Frame::Number>(value));
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

        void Media::setInOutPointsEnabled(bool value)
        {
            _p->inOutPointsEnabled->setIfChanged(value);
        }

        void Media::setInPoint(Frame::Number value)
        {
            _p->inPoint->setIfChanged(value);
        }

        void Media::setOutPoint(Frame::Number value)
        {
            _p->outPoint->setIfChanged(value);
        }

        void Media::resetInPoint()
        {
            _p->inPoint->setIfChanged(0);
        }

        void Media::resetOutPoint()
        {
            _p->outPoint->setIfChanged(0);
        }

        std::shared_ptr<IValueSubject<float> > Media::observeVolume() const
        {
            return _p->volume;
        }

        std::shared_ptr<IValueSubject<bool> > Media::observeMute() const
        {
            return _p->mute;
        }

        void Media::setVolume(float value)
        {
            if (_p->volume->setIfChanged(Math::clamp(value, 0.f, 1.f)))
            {
                _volumeUpdate();
            }
        }

        void Media::setMute(bool value)
        {
            if (_p->mute->setIfChanged(value))
            {
                _volumeUpdate();
            }
        }

        std::shared_ptr<IValueSubject<size_t> > Media::observeThreadCount() const
        {
            return _p->threadCount;
        }

        void Media::setThreadCount(size_t value)
        {
            DJV_PRIVATE_PTR();
            if (p.threadCount->setIfChanged(value))
            {
                for (const auto& i : p.tracks)
                {
                    if (i.read)
                    {
                        i.read->setThreadCount(value);
                    }
                }
            }
        }

        bool Media::hasCache() const
        {
            DJV_PRIVATE_PTR();
            bool out = false;
            for (const auto& i : p.tracks)
            {
                if (i.read)
                {
                    out |= i.read->hasCache();
                }
            }
            return out;
        }

        std::shared_ptr<Core::IListSubject<Frame::Range> > Media::observeCachedFrames() const
        {
            return _p->cachedFrames;
        }

        void Media::setCacheEnabled(bool value)
        {
            DJV_PRIVATE_PTR();
            for (const auto& i : p.tracks)
            {
                if (i.read)
                {
                    i.read->setCacheEnabled(value);
                }
            }
        }

        void Media::setCacheMax(size_t value)
        {
            DJV_PRIVATE_PTR();
            for (const auto& i : p.tracks)
            {
                if (i.read)
                {
                    i.read->setCacheMax(value);
                }
            }
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

        std::shared_ptr<IValueSubject<size_t> > Media::observeALQueueCount() const
        {
            return _p->alQueueCount;
        }
        
        void Media::_open()
        {
            DJV_PRIVATE_PTR();
            auto context = p.context;
            try
            {
                AV::IO::ReadOptions options;
                options.layer = p.layer->get();
                options.videoQueueSize = videoQueueSize;
                auto io = context->getSystemT<AV::IO::System>();
                auto read = io->read(p.fileInfo, options);
                read->setThreadCount(p.threadCount->get());
                
                const auto info = read->getInfo().get();
                p.info->setIfChanged(info);
                Time::Speed speed;
                Frame::Sequence sequence;
                const auto& video = info.video;
                if (video.size())
                {
                    p.videoInfo = video[0];
                    speed = video[0].speed;
                    sequence = video[0].sequence;
                }
                const auto& audio = info.audio;
                if (audio.size())
                {
                    p.audioInfo = audio[0];
                }
                {
                    std::stringstream ss;
                    ss << p.fileInfo << " sequence: " << sequence.getSize();
                    auto logSystem = context->getSystemT<LogSystem>();
                    logSystem->log("djv::ViewApp::Media", ss.str());
                }
                p.info->setIfChanged(info);
                p.speed->setIfChanged(speed);
                p.defaultSpeed->setIfChanged(speed);
                p.sequence->setIfChanged(sequence);

                Track track;
                track.read = read;
                track.range = Frame::Range(1, sequence.getSize());
                p.tracks.push_back(track);
                
                const auto timeout = Time::getMilliseconds(Time::TimerValue::Fast);
                auto weak = std::weak_ptr<Media>(std::dynamic_pointer_cast<Media>(shared_from_this()));
                p.queueTimer->start(
                    timeout,
                    [weak, timeout](float)
                    {
                        if (auto media = weak.lock())
                        {
                            if (auto read = media->_p->getCurrentRead())
                            {
                                std::unique_lock<std::mutex> lock(read->getMutex());
                                if (media->_p->queueCV.wait_for(
                                    lock,
                                    timeout,
                                    [weak, read]
                                    {
                                        bool out = false;
                                        if (auto media = weak.lock())
                                        {
                                            out = read->getVideoQueue().hasFrames();
                                        }
                                        return out;
                                    }))
                                {
                                    auto& queue = read->getVideoQueue();
                                    auto frame = queue.getFrame();
                                    media->_p->currentImage->setIfChanged(frame.image);
                                }
                            }
                        }
                    });

                p.cachedFramesTimer->start(
                    Time::getMilliseconds(Time::TimerValue::Medium),
                    [weak](float)
                    {
                        if (auto media = weak.lock())
                        {
                            if (auto read = media->_p->getCurrentRead())
                            {
                                const auto& frames = read->getCachedFrames();
                                media->_p->cachedFrames->setIfChanged(frames);
                            }
                        }
                    });

                p.debugTimer->start(
                    Time::getMilliseconds(Time::TimerValue::Medium),
                    [weak](float)
                    {
                        if (auto media = weak.lock())
                        {
                            if (auto read = media->_p->getCurrentRead())
                            {
                                {
                                    std::lock_guard<std::mutex> lock(read->getMutex());
                                    auto& videoQueue = read->getVideoQueue();
                                    auto& audioQueue = read->getAudioQueue();
                                    media->_p->videoQueueMax->setAlways(videoQueue.getMax());
                                    media->_p->videoQueueCount->setAlways(videoQueue.getFrameCount());
                                    media->_p->audioQueueMax->setAlways(audioQueue.getMax());
                                    media->_p->audioQueueCount->setAlways(audioQueue.getFrameCount());
                                }
                                ALint queued = 0;
                                alGetSourcei(media->_p->alSource, AL_BUFFERS_QUEUED, &queued);
                                media->_p->alQueueCount->setAlways(queued);
                            }
                        }
                    });
            }
            catch (const std::exception& e)
            {
                std::stringstream ss;
                ss << "djv::ViewApp::Media " << DJV_TEXT("cannot open") << " '" << p.fileInfo << "'. " << e.what();
                auto logSystem = context->getSystemT<LogSystem>();
                logSystem->log("djv::ViewApp::Media", ss.str(), LogLevel::Error);
            }
            
            _seek(p.currentFrame->get());

            p.reload->setAlways(true);
        }
        
        void Media::_seek(Core::Frame::Number value)
        {
            DJV_PRIVATE_PTR();
            p.currentTrack = -1;
            int j = 0;
            for (const auto& i : p.tracks)
            {
                if (i.range.contains(value))
                {
                    p.currentTrack = j;
                    break;
                }
                ++j;
            }
            if (p.currentTrack != -1)
            {
                const auto& track = p.tracks[p.currentTrack];
                if (track.read)
                {
                    track.read->seek(value - track.range.min, p.ioPlayback);
                }
            }
        }

        void Media::_playbackUpdate()
        {
            DJV_PRIVATE_PTR();
            bool forward = false;
            switch (p.playback->get())
            {
            case Playback::Stop:
                if (p.alSource)
                {
                    alSourceStop(p.alSource);
                }
                p.playbackTimer->stop();
                p.realSpeedTimer->stop();
                _frameUpdate();
                _seek(p.currentFrame->get());
                break;
            case Playback::Forward: forward = true;
            case Playback::Reverse:
            {
                if (p.alSource)
                {
                    alSourcePlay(p.alSource);
                }
                p.ioPlayback = forward ? AV::IO::Playback::Forward : AV::IO::Playback::Reverse;
                p.frameOffset = p.currentFrame->get();
                p.startTime = std::chrono::system_clock::now();
                p.realSpeedTime = p.startTime;
                p.realSpeedFrameCount = 0;
                _seek(p.currentFrame->get());
                auto weak = std::weak_ptr<Media>(std::dynamic_pointer_cast<Media>(shared_from_this()));
                p.playbackTimer->start(
                    Time::getMilliseconds(Time::TimerValue::VeryFast),
                    [weak](float)
                {
                    if (auto media = weak.lock())
                    {
                        media->_playbackTick();
                    }
                });
                p.realSpeedTimer->start(
                    Time::getMilliseconds(Time::TimerValue::Slow),
                    [weak](float)
                    {
                        if (auto media = weak.lock())
                        {
                            const auto now = std::chrono::system_clock::now();
                            std::chrono::duration<double> delta = now - media->_p->realSpeedTime;
                            media->_p->realSpeed->setIfChanged(delta.count() ? (media->_p->realSpeedFrameCount / static_cast<float>(delta.count())) : 0.f);
                            media->_p->realSpeedTime = now;
                            media->_p->realSpeedFrameCount = 0;
                        }
                    });
                break;
            }
            default: break;
            }
        }

        void Media::_playbackTick()
        {
            DJV_PRIVATE_PTR();
            const Playback playback = p.playback->get();
            switch (playback)
            {
            case Playback::Forward:
            case Playback::Reverse:
            {
                Frame::Number frame = Frame::invalid;
                const auto& speed = p.speed->get();
                const auto now = std::chrono::system_clock::now();
                std::chrono::duration<double> delta = now - p.startTime;
                Frame::Number elapsed = static_cast<Frame::Number>(delta.count() * speed.toFloat());
                switch (playback)
                {
                case Playback::Forward: frame = p.frameOffset + elapsed; break;
                case Playback::Reverse: frame = p.frameOffset - elapsed; break;
                default: break;
                }

                Frame::Number start = 0;
                const Frame::Sequence& sequence = p.sequence->get();
                Frame::Number end = static_cast<Frame::Number>(sequence.getSize()) - 1;
                if (p.inOutPointsEnabled->get())
                {
                    start = p.inPoint->get();
                    end = p.outPoint->get();
                }
                if ((Playback::Forward == playback && frame >= end) ||
                    (Playback::Reverse == playback && frame <= start))
                {
                    switch (p.playbackMode->get())
                    {
                    case PlaybackMode::Once:
                        switch (p.playback->get())
                        {
                        case Playback::Forward: frame = end;   break;
                        case Playback::Reverse: frame = start; break;
                        default: break;
                        }
                        setPlayback(Playback::Stop);
                        setCurrentFrame(frame);
                        break;
                    case PlaybackMode::Loop:
                    {
                        Playback playback = p.playback->get();
                        switch (playback)
                        {
                        case Playback::Forward: frame = start; break;
                        case Playback::Reverse: frame = end;   break;
                        default: break;
                        }
                        setPlayback(Playback::Stop);
                        setCurrentFrame(frame);
                        setPlayback(playback);
                        break;
                    }
                    case PlaybackMode::PingPong:
                    {
                        Playback playback = p.playback->get();
                        switch (playback)
                        {
                        case Playback::Forward: frame = end;   break;
                        case Playback::Reverse: frame = start; break;
                        default: break;
                        }
                        setPlayback(Playback::Stop);
                        setCurrentFrame(frame);
                        setPlayback(Playback::Forward == playback ? Playback::Reverse : Playback::Forward);
                        break;
                    }
                    default: break;
                    }
                }
                p.currentFrame->setIfChanged(frame);
                _frameUpdate();
                break;
            }
            default: break;
            }
        }

        void Media::_frameUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.currentTrack != -1)
            {
                const auto& track = p.tracks[p.currentTrack];
                if (track.read)
                {
                    // Update the video queue.
                    {
                        std::lock_guard<std::mutex> lock(track.read->getMutex());
                        auto& queue = track.read->getVideoQueue();
                        const bool forward = Playback::Forward == p.playback->get();
                        const Frame::Number currentFrame = p.currentFrame->get() - track.range.min;
                        while (queue.hasFrames() && forward ?
                            (queue.getFrame().frame < currentFrame) :
                            (queue.getFrame().frame > currentFrame))
                        {
                            auto frame = queue.popFrame();
                            p.realSpeedFrameCount = p.realSpeedFrameCount + 1;
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
                        }
                    }

                    if (Playback::Forward == p.playback->get() && p.alSource)
                    {
                        // Fill the OpenAL queue with frames from the I/O queue.
                        ALint queued = 0;
                        alGetSourcei(p.alSource, AL_BUFFERS_QUEUED, &queued);
                        std::vector<AV::IO::AudioFrame> frames;
                        {
                            std::lock_guard<std::mutex> lock(track.read->getMutex());
                            auto& queue = track.read->getAudioQueue();
                            while (queue.hasFrames() &&
                                (frames.size() < (bufferCount - queued)) &&
                                (frames.size() < p.alBuffers.size()))
                            {
                                frames.push_back(queue.popFrame());
                            }
                        }
                        for (size_t i = 0; i < frames.size(); ++i)
                        {
                            auto data = frames[i].audio;
                            const AV::Audio::DataInfo info = data->getInfo();
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
            }
        }

        void Media::_volumeUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.alSource)
            {
                const float volume = !p.mute->get() ? p.volume->get() : 0.f;
                alSourcef(p.alSource, AL_GAIN, volume);
            }
        }

    } // namespace ViewApp
} // namespace djv

