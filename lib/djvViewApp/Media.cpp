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

#include <RtAudio.h>

#include <condition_variable>
#include <queue>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        namespace
        {
            //! \todo Should this be configurable?
            const size_t bufferFrameCount = 256;
            const size_t videoQueueSize = 30;
            
        } // namespace

        struct Media::Private
        {
            std::weak_ptr<Context> context;

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
            std::shared_ptr<ValueSubject<Frame::Index> > currentFrame;
            std::shared_ptr<ValueSubject<std::shared_ptr<AV::Image::Image> > > currentImage;
            std::shared_ptr<ValueSubject<Playback> > playback;
            std::shared_ptr<ValueSubject<PlaybackMode> > playbackMode;
            std::shared_ptr<ValueSubject<bool> > inOutPointsEnabled;
            std::shared_ptr<ValueSubject<Frame::Index> > inPoint;
            std::shared_ptr<ValueSubject<Frame::Index> > outPoint;
            std::shared_ptr<ValueSubject<bool> > audioEnabled;
            std::shared_ptr<ValueSubject<float> > volume;
            std::shared_ptr<ValueSubject<bool> > mute;
            std::shared_ptr<ValueSubject<size_t> > threadCount;
            std::shared_ptr<ValueSubject<Frame::Sequence> > cacheSequence;
            std::shared_ptr<ValueSubject<Frame::Sequence> > cachedFrames;

            std::shared_ptr<ValueSubject<size_t> > videoQueueMax;
            std::shared_ptr<ValueSubject<size_t> > videoQueueCount;
            std::shared_ptr<ValueSubject<size_t> > audioQueueMax;
            std::shared_ptr<ValueSubject<size_t> > audioQueueCount;
            std::shared_ptr<AV::IO::IRead> read;

            AV::IO::Direction ioDirection = AV::IO::Direction::Forward;
            std::unique_ptr<RtAudio> rtAudio;
            std::shared_ptr<AV::Audio::Data> audioData;
            size_t audioDataSamplesOffset = 0;
            size_t audioDataSamplesCount = 0;
            std::chrono::system_clock::time_point audioDataSamplesTime;
            Frame::Index frameOffset = 0;
            std::chrono::system_clock::time_point startTime;
            std::chrono::system_clock::time_point realSpeedTime;
            size_t realSpeedFrameCount = 0;
            std::chrono::system_clock::time_point playEveryFrameTime;
            std::shared_ptr<Time::Timer> queueTimer;
            std::shared_ptr<Time::Timer> playbackTimer;
            std::shared_ptr<Time::Timer> realSpeedTimer;
            std::shared_ptr<Time::Timer> cacheTimer;
            std::shared_ptr<Time::Timer> debugTimer;
        };

        void Media::_init(
            const Core::FileSystem::FileInfo& fileInfo,
            const std::shared_ptr<Core::Context>& context)
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
            p.currentFrame = ValueSubject<Frame::Index>::create();
            p.currentImage = ValueSubject<std::shared_ptr<AV::Image::Image> >::create();
            p.playback = ValueSubject<Playback>::create();
            p.playbackMode = ValueSubject<PlaybackMode>::create(PlaybackMode::Loop);
            p.inOutPointsEnabled = ValueSubject<bool>::create(false);
            p.inPoint = ValueSubject<Frame::Index>::create(Frame::invalid);
            p.outPoint = ValueSubject<Frame::Index>::create(Frame::invalid);
            p.volume = ValueSubject<float>::create(1.F);
            p.audioEnabled = ValueSubject<bool>::create(false);
            p.mute = ValueSubject<bool>::create(false);
            p.threadCount = ValueSubject<size_t>::create(4);
            p.cacheSequence = ValueSubject<Frame::Sequence>::create();
            p.cachedFrames = ValueSubject<Frame::Sequence>::create();

            p.videoQueueMax = ValueSubject<size_t>::create();
            p.audioQueueMax = ValueSubject<size_t>::create();
            p.videoQueueCount = ValueSubject<size_t>::create();
            p.audioQueueCount = ValueSubject<size_t>::create();

            p.queueTimer = Time::Timer::create(context);
            p.queueTimer->setRepeating(true);
            p.playbackTimer = Time::Timer::create(context);
            p.playbackTimer->setRepeating(true);
            p.realSpeedTimer = Time::Timer::create(context);
            p.realSpeedTimer->setRepeating(true);
            p.cacheTimer = Time::Timer::create(context);
            p.cacheTimer->setRepeating(true);
            p.debugTimer = Time::Timer::create(context);
            p.debugTimer->setRepeating(true);

            try
            {
                p.rtAudio.reset(new RtAudio);
            }
            catch (const std::exception& e)
            {
                std::stringstream ss;
                ss << DJV_TEXT("Audio cannot be initialized") << ". " << e.what();
                auto logSystem = context->getSystemT<LogSystem>();
                logSystem->log("djv::ViewApp::Media", ss.str(), LogLevel::Error);
            }

            _open();

            auto weak = std::weak_ptr<Media>(std::dynamic_pointer_cast<Media>(shared_from_this()));
            p.queueTimer->start(
                Time::getMilliseconds(Time::TimerValue::VeryFast),
                [weak](float)
                {
                    if (auto media = weak.lock())
                    {
                        media->_queueUpdate();
                    }
                });
        }

        Media::Media() :
            _p(new Private)
        {}

        Media::~Media()
        {
            DJV_PRIVATE_PTR();
            p.rtAudio.reset();
        }

        std::shared_ptr<Media> Media::create(
            const Core::FileSystem::FileInfo& fileInfo,
            const std::shared_ptr<Context>& context)
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

        std::shared_ptr<IValueSubject<Frame::Index> > Media::observeCurrentFrame() const
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

        std::shared_ptr<IValueSubject<Frame::Index> > Media::observeInPoint() const
        {
            return _p->inPoint;
        }

        std::shared_ptr<IValueSubject<Frame::Index> > Media::observeOutPoint() const
        {
            return _p->outPoint;
        }

        void Media::setSpeed(const Time::Speed& value)
        {
            DJV_PRIVATE_PTR();
            if (p.speed->setIfChanged(value))
            {
                _seek(p.currentFrame->get());
                p.audioEnabled->setIfChanged(_isAudioEnabled());
                if (_hasAudioSyncPlayback())
                {
                    _startAudioStream();
                }
            }
        }

        void Media::setPlayEveryFrame(bool value)
        {
            DJV_PRIVATE_PTR();
            if (_p->playEveryFrame->setIfChanged(value))
            {
                _seek(p.currentFrame->get());
                p.audioEnabled->setIfChanged(_isAudioEnabled());
                if (_hasAudioSyncPlayback())
                {
                    _startAudioStream();
                }
            }
        }

        void Media::setCurrentFrame(Frame::Index value, bool inOutPoints)
        {
            DJV_PRIVATE_PTR();
            Frame::Index start = 0;
            const size_t size = p.sequence->get().getSize();
            Frame::Index end = size > 0 ? (static_cast<Frame::Index>(size) - 1) : 0;
            if (inOutPoints && p.inOutPointsEnabled->get())
            {
                start = p.inPoint->get();
                end = p.outPoint->get();
            }
            Frame::Index tmp = value;
            while (tmp > end)
            {
                tmp -= size;
            }
            while (tmp < start)
            {
                tmp += size;
            }
            if (p.currentFrame->setIfChanged(tmp))
            {
                setPlayback(Playback::Stop);
                _seek(p.currentFrame->get());
            }
        }

        void Media::inPoint()
        {
            DJV_PRIVATE_PTR();
            setCurrentFrame(p.inOutPointsEnabled->get() ? p.inPoint->get() : 0);
        }

        void Media::outPoint()
        {
            DJV_PRIVATE_PTR();
            const size_t size = p.sequence->get().getSize();
            const Frame::Index end = size > 0 ? (size - 1) : 0;
            setCurrentFrame(p.inOutPointsEnabled->get() ? p.outPoint->get() : end, false);
        }

        void Media::start()
        {
            setCurrentFrame(0, false);
        }

        void Media::end()
        {
            DJV_PRIVATE_PTR();
            const size_t size = p.sequence->get().getSize();
            const Frame::Index frame = size > 0 ? (size - 1) : 0;
            setCurrentFrame(frame, false);
        }

        void Media::nextFrame(size_t value)
        {
            DJV_PRIVATE_PTR();
            const Frame::Index frame = p.currentFrame->get();
            setCurrentFrame(frame + static_cast<Frame::Index>(value));
        }

        void Media::prevFrame(size_t value)
        {
            DJV_PRIVATE_PTR();
            const Frame::Index frame = p.currentFrame->get();
            setCurrentFrame(frame - static_cast<Frame::Index>(value));
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
            DJV_PRIVATE_PTR();
            if (p.inOutPointsEnabled->setIfChanged(value))
            {
                _seek(p.currentFrame->get());
                if (_hasAudioSyncPlayback())
                {
                    _startAudioStream();
                }
            }
        }

        void Media::setInPoint()
        {
            setInOutPointsEnabled(true);
            setInPoint(_p->currentFrame->get());
        }

        void Media::setInPoint(Frame::Index value)
        {
            if (_p->inPoint->setIfChanged(value))
            {
                _p->outPoint->setIfChanged(std::max(_p->inPoint->get(), _p->outPoint->get()));
            }
        }

        void Media::setOutPoint()
        {
            setInOutPointsEnabled(true);
            setOutPoint(_p->currentFrame->get());
        }

        void Media::setOutPoint(Frame::Index value)
        {
            if (_p->outPoint->setIfChanged(value))
            {
                _p->inPoint->setIfChanged(std::min(_p->inPoint->get(), _p->outPoint->get()));
            }
        }

        void Media::resetInPoint()
        {
            DJV_PRIVATE_PTR();
            if (_p->inPoint->setIfChanged(0))
            {
                const size_t size = p.sequence->get().getSize();
                if (_p->outPoint->get() == (static_cast<Frame::Index>(size) - 1))
                {
                    setInOutPointsEnabled(false);
                }
            }
        }

        void Media::resetOutPoint()
        {
            DJV_PRIVATE_PTR();
            const size_t size = p.sequence->get().getSize();
            if (p.outPoint->setIfChanged(size > 0 ? (static_cast<Frame::Index>(size) - 1) : 0))
            {
                if (_p->inPoint->get() == 0)
                {
                    setInOutPointsEnabled(false);
                }
            }
        }

        std::shared_ptr<IValueSubject<bool> > Media::observeAudioEnabled() const
        {
            return _p->audioEnabled;
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
            _p->volume->setIfChanged(Math::clamp(value, 0.F, 1.F));
        }

        void Media::setMute(bool value)
        {
            _p->mute->setIfChanged(value);
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
                if (p.read)
                {
                    p.read->setThreadCount(value);
                }
            }
        }

        bool Media::hasCache() const
        {
            DJV_PRIVATE_PTR();
            return p.read ? p.read->hasCache() : false;
        }

        size_t Media::getCacheMaxByteCount() const
        {
            DJV_PRIVATE_PTR();
            return p.read ? p.read->getCacheMaxByteCount() : 0;
        }

        size_t Media::getCacheByteCount() const
        {
            DJV_PRIVATE_PTR();
            return p.read ? p.read->getCacheByteCount() : 0;
        }

        std::shared_ptr<Core::IValueSubject<Frame::Sequence> > Media::observeCacheSequence() const
        {
            return _p->cacheSequence;
        }

        std::shared_ptr<Core::IValueSubject<Frame::Sequence> > Media::observeCachedFrames() const
        {
            return _p->cachedFrames;
        }

        void Media::setCacheEnabled(bool value)
        {
            DJV_PRIVATE_PTR();
            if (p.read)
            {
                p.read->setCacheEnabled(value);
            }
        }

        void Media::setCacheMaxByteCount(size_t value)
        {
            DJV_PRIVATE_PTR();
            if (p.read)
            {
                p.read->setCacheMaxByteCount(value);
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

        bool Media::_hasAudio() const
        {
            DJV_PRIVATE_PTR();
            return p.audioInfo.info.isValid() && p.rtAudio;
        }

        bool Media::_isAudioEnabled() const
        {
            DJV_PRIVATE_PTR();
            return _hasAudio() &&
                p.speed->get() == p.defaultSpeed->get() &&
                !p.playEveryFrame->get();
        }

        bool Media::_hasAudioSyncPlayback() const
        {
            DJV_PRIVATE_PTR();
            return _isAudioEnabled() &&
                Playback::Forward == p.playback->get();
        }

        void Media::_open()
        {
            DJV_PRIVATE_PTR();
            if (auto context = p.context.lock())
            {
                try
                {
                    AV::IO::ReadOptions options;
                    options.layer = p.layer->get();
                    options.videoQueueSize = videoQueueSize;
                    auto io = context->getSystemT<AV::IO::System>();
                    p.read = io->read(p.fileInfo, options);
                    p.read->setThreadCount(p.threadCount->get());
                    
                    const auto info = p.read->getInfo().get();
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
                    p.inPoint->setIfChanged(0);
                    const size_t sequenceSize = sequence.getSize();
                    p.outPoint->setIfChanged(sequenceSize > 0 ? (static_cast<Frame::Index>(sequenceSize) - 1) : 0);
                    if (_hasAudio())
                    {
                        if (p.rtAudio->isStreamOpen())
                        {
                            p.rtAudio->closeStream();
                        }
                        RtAudio::StreamParameters rtParameters;
                        rtParameters.deviceId = p.rtAudio->getDefaultOutputDevice();
                        rtParameters.nChannels = p.audioInfo.info.channelCount;
                        unsigned int rtBufferFrames = bufferFrameCount;
                        try
                        {
                            p.rtAudio->openStream(
                                &rtParameters,
                                nullptr,
                                AV::Audio::toRtAudio(p.audioInfo.info.type),
                                p.audioInfo.info.sampleRate,
                                &rtBufferFrames,
                                _rtAudioCallback,
                                this,
                                nullptr,
                                _rtAudioErrorCallback);
                        }
                        catch (const std::exception& e)
                        {
                            std::stringstream ss;
                            ss << DJV_TEXT("The audio stream cannot be opened") << ". " << e.what();
                            auto logSystem = context->getSystemT<LogSystem>();
                            logSystem->log("djv::ViewApp::Media", ss.str(), LogLevel::Error);
                        }
                    }
                    p.audioEnabled->setIfChanged(_isAudioEnabled());

                    auto weak = std::weak_ptr<Media>(std::dynamic_pointer_cast<Media>(shared_from_this()));
                    p.cacheTimer->start(
                        Time::getMilliseconds(Time::TimerValue::Medium),
                        [weak](float)
                        {
                            if (auto media = weak.lock())
                            {
                                if (media->_p->read)
                                {
                                    const auto& sequence = media->_p->read->getCacheSequence();
                                    const auto& frames = media->_p->read->getCachedFrames();
                                    media->_p->cacheSequence->setIfChanged(sequence);
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
                                if (media->_p->read)
                                {
                                    size_t videoQueueMax   = 0;
                                    size_t videoQueueCount = 0;
                                    size_t audioQueueMax   = 0;
                                    size_t audioQueueCount = 0;
                                    {
                                        std::lock_guard<std::mutex> lock(media->_p->read->getMutex());
                                        const auto& videoQueue = media->_p->read->getVideoQueue();
                                        const auto& audioQueue = media->_p->read->getAudioQueue();
                                        videoQueueMax   = videoQueue.getMax();
                                        videoQueueCount = videoQueue.getCount();
                                        audioQueueMax   = audioQueue.getMax();
                                        audioQueueCount = audioQueue.getCount();
                                    }
                                    media->_p->videoQueueMax->setAlways(videoQueueMax);
                                    media->_p->videoQueueCount->setAlways(videoQueueCount);
                                    media->_p->audioQueueMax->setAlways(audioQueueMax);
                                    media->_p->audioQueueCount->setAlways(audioQueueCount);
                                }
                            }
                        });
                }
                catch (const std::exception& e)
                {
                    std::stringstream ss;
                    ss << DJV_TEXT("The file") << " '" << p.fileInfo << "' " << DJV_TEXT("cannot be read") <<  ". " << e.what();
                    auto logSystem = context->getSystemT<LogSystem>();
                    logSystem->log("djv::ViewApp::Media", ss.str(), LogLevel::Error);
                }
                
                _seek(p.currentFrame->get());

                p.reload->setAlways(true);
            }
        }

        void Media::_setCurrentFrame(Frame::Index value)
        {
            DJV_PRIVATE_PTR();
            Frame::Index start = 0;
            const Frame::Sequence& sequence = p.sequence->get();
            const size_t size = sequence.getSize();
            Frame::Index end = size > 0 ? (static_cast<Frame::Index>(size) - 1) : 0;
            if (p.inOutPointsEnabled->get())
            {
                start = p.inPoint->get();
                end   = p.outPoint->get();
            }
            switch (p.playback->get())
            {
            case Playback::Forward:
                if (value > end)
                {
                    switch (p.playbackMode->get())
                    {
                    case PlaybackMode::Once:
                        setPlayback(Playback::Stop);
                        break;
                    case PlaybackMode::Loop:
                    {
                        setPlayback(Playback::Stop);
                        setCurrentFrame(start);
                        setPlayback(Playback::Forward);
                        break;
                    }
                    case PlaybackMode::PingPong:
                    {
                        setPlayback(Playback::Stop);
                        setPlayback(Playback::Reverse);
                        break;
                    }
                    default: break;
                    }
                }
                else if (value < start)
                {
                    setCurrentFrame(start);
                    setPlayback(Playback::Forward);
                }
                else
                {
                    p.currentFrame->setIfChanged(value);
                }
                break;
            case Playback::Reverse:
                if (value < start)
                {
                    switch (p.playbackMode->get())
                    {
                    case PlaybackMode::Once:
                        setPlayback(Playback::Stop);
                        break;
                    case PlaybackMode::Loop:
                    {
                        setPlayback(Playback::Stop);
                        setCurrentFrame(end);
                        setPlayback(Playback::Reverse);
                        break;
                    }
                    case PlaybackMode::PingPong:
                    {
                        setPlayback(Playback::Stop);
                        setPlayback(Playback::Forward);
                        break;
                    }
                    default: break;
                    }
                }
                else if (value > end)
                {
                    setCurrentFrame(end);
                    setPlayback(Playback::Reverse);
                }
                else
                {
                    p.currentFrame->setIfChanged(value);
                }
                break;
            default: break;
            }
        }

        void Media::_seek(Frame::Index value)
        {
            DJV_PRIVATE_PTR();
            if (auto context = p.context.lock())
            {
                if (p.read)
                {
                    p.read->seek(value, p.ioDirection);
                }
                p.audioData.reset();
                p.audioDataSamplesOffset = 0;
                p.audioDataSamplesCount = 0;
                const auto now = std::chrono::system_clock::now();
                p.audioDataSamplesTime = now;
                p.frameOffset = p.currentFrame->get();
                p.startTime = now;
                p.realSpeedTime = p.startTime;
                p.realSpeedFrameCount = 0;
                p.playEveryFrameTime = now;
                _stopAudioStream();
            }
        }

        void Media::_playbackUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = p.context.lock())
            {
                bool forward = false;
                switch (p.playback->get())
                {
                case Playback::Stop:
                    _stopAudioStream();
                    p.playbackTimer->stop();
                    p.realSpeedTimer->stop();
                    _seek(p.currentFrame->get());
                    break;
                case Playback::Forward: // Forward or reverse.
                    forward = true;
                case Playback::Reverse:
                {
                    p.ioDirection = forward ? AV::IO::Direction::Forward : AV::IO::Direction::Reverse;
                    _seek(p.currentFrame->get());
                    p.audioData.reset();
                    p.audioDataSamplesOffset = 0;
                    p.audioDataSamplesCount = 0;
                    const auto now = std::chrono::system_clock::now();
                    p.audioDataSamplesTime = now;
                    p.frameOffset = p.currentFrame->get();
                    p.startTime = now;
                    p.realSpeedTime = p.startTime;
                    p.realSpeedFrameCount = 0;
                    p.playEveryFrameTime = now;
                    if (_hasAudioSyncPlayback())
                    {
                        _startAudioStream();
                    }
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
                                media->_p->realSpeed->setIfChanged(delta.count() ? (media->_p->realSpeedFrameCount / static_cast<float>(delta.count())) : 0.F);
                                media->_p->realSpeedTime = now;
                                media->_p->realSpeedFrameCount = 0;
                            }
                        });
                    break;
                }
                default: break;
                }
            }
        }

        void Media::_playbackTick()
        {
            DJV_PRIVATE_PTR();
            const Playback playback = p.playback->get();
            bool forward = false;
            switch (playback)
            {
            case Playback::Forward: // Forward or reverse.
                forward = true;
            case Playback::Reverse:
            {
                const auto& speed = p.speed->get();
                const auto now = std::chrono::system_clock::now();
                if (_hasAudioSyncPlayback())
                {
                    if (p.audioDataSamplesCount)
                    {
                        std::chrono::duration<double> delta = now - p.audioDataSamplesTime;
                        Frame::Index frame = p.frameOffset +
                            Time::scale(
                                p.audioDataSamplesCount,
                                Math::Rational(1, static_cast<int>(p.audioInfo.info.sampleRate)),
                                speed.swap()) +
                            delta.count() * speed.toFloat();
                        _setCurrentFrame(frame);
                    }
                }
                else if (p.playEveryFrame->get())
                {
                }
                else
                {
                    std::chrono::duration<double> delta = now - p.startTime;
                    Frame::Index elapsed = static_cast<Frame::Index>(delta.count() * speed.toFloat());
                    Frame::Index frame = Frame::invalid;
                    switch (playback)
                    {
                    case Playback::Forward: frame = p.frameOffset + elapsed; break;
                    case Playback::Reverse: frame = p.frameOffset - elapsed; break;
                    default: break;
                    }
                    _setCurrentFrame(frame);
                }
                break;
            }
            default: break;
            }
        }

        void Media::_startAudioStream()
        {
            DJV_PRIVATE_PTR();
            if (auto context = p.context.lock())
            {
                try
                {
                    p.rtAudio->startStream();
                }
                catch (const std::exception& e)
                {
                    std::stringstream ss;
                    ss << DJV_TEXT("Cannot start audio stream") << ". " << e.what();
                    auto logSystem = context->getSystemT<LogSystem>();
                    logSystem->log("djv::ViewApp::Media", ss.str(), LogLevel::Error);
                }
            }
        }

        void Media::_stopAudioStream()
        {
            DJV_PRIVATE_PTR();
            if (auto context = p.context.lock())
            {
                if (_hasAudio() && p.rtAudio->isStreamRunning())
                {
                    try
                    {
                        p.rtAudio->abortStream();
                        p.rtAudio->setStreamTime(0.0);
                    }
                    catch (const std::exception& e)
                    {
                        std::stringstream ss;
                        ss << DJV_TEXT("Cannot stop audio stream") << ". " << e.what();
                        auto logSystem = context->getSystemT<LogSystem>();
                        logSystem->log("djv::ViewApp::Media", ss.str(), LogLevel::Error);
                    }
                }
            }
        }

        void Media::_queueUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.read)
            {
                // Update the video queue.
                const Playback playback = p.playback->get();
                const auto now = std::chrono::system_clock::now();
                const std::chrono::duration<double> playEveryFrameDelta = now - p.playEveryFrameTime;
                const float frameTime = 1.F / p.speed->get().toFloat();
                const bool playEveryFrameAdvance = playEveryFrameDelta.count() > frameTime;
                const Frame::Index currentFrame = p.currentFrame->get();
                AV::IO::VideoFrame frame;
                bool gotFrame = false;
                {
                    std::lock_guard<std::mutex> lock(p.read->getMutex());
                    auto& queue = p.read->getVideoQueue();
                    if (p.playEveryFrame->get())
                    {
                        if (playback != Playback::Stop && !queue.isEmpty() && playEveryFrameAdvance)
                        {
                            frame = queue.popFrame();
                            gotFrame = true;
                            p.playEveryFrameTime += std::chrono::milliseconds(static_cast<size_t>(1000 * frameTime));
                            p.realSpeedFrameCount = p.realSpeedFrameCount + 1;
                        }
                    }
                    else
                    {
                        while (!queue.isEmpty() &&
                            (AV::IO::Direction::Forward == p.ioDirection ?
                                (queue.getFrame().frame < currentFrame) :
                                (queue.getFrame().frame > currentFrame)))
                        {
                            frame = queue.popFrame();
                            gotFrame = true;
                            p.realSpeedFrameCount = p.realSpeedFrameCount + 1;
                        }
                    }
                    if (!gotFrame && !queue.isEmpty())
                    {
                        frame = queue.getFrame();
                    }
                }
                if (frame.image)
                {
                    p.currentImage->setIfChanged(frame.image);
                    if (p.playEveryFrame->get())
                    {
                        _setCurrentFrame(frame.frame);
                    }
                }

                // Update the audio queue.
                if (_hasAudio() && !_hasAudioSyncPlayback())
                {
                    std::lock_guard<std::mutex> lock(p.read->getMutex());
                    auto& queue = p.read->getAudioQueue();
                    while (queue.getCount() > queue.getMax())
                    {
                        queue.popFrame();
                    }
                }
            }
        }
        
        int Media::_rtAudioCallback(
            void* outputBuffer,
            void* inputBuffer,
            unsigned int nFrames,
            double streamTime,
            RtAudioStreamStatus status,
            void* userData)
        {
            Media* media = reinterpret_cast<Media*>(userData);
            const auto& info = media->_p->audioInfo;

            size_t outputSampleCount = static_cast<size_t>(nFrames);
            size_t sampleCount = 0;
            const size_t sampleByteCount = info.info.channelCount * AV::Audio::getByteCount(info.info.type);
            const float volume = !media->_p->mute->get() ? media->_p->volume->get() : 0.F;

            if (media->_p->audioData)
            {
                sampleCount += media->_p->audioData->getSampleCount() - media->_p->audioDataSamplesOffset;
            }

            std::vector<AV::IO::AudioFrame> frames;
            {
                std::lock_guard<std::mutex> lock(media->_p->read->getMutex());
                auto& queue = media->_p->read->getAudioQueue();
                while (!queue.isEmpty() && sampleCount < outputSampleCount)
                {
                    auto frame = queue.getFrame();
                    frames.push_back(frame);
                    queue.popFrame();
                    sampleCount += frame.audio->getSampleCount();
                }
            }

            uint8_t* p = reinterpret_cast<uint8_t*>(outputBuffer);
            if (media->_p->audioData)
            {
                const size_t size = std::min(media->_p->audioData->getSampleCount() - media->_p->audioDataSamplesOffset, outputSampleCount);
                //memcpy(
                //    p,
                //    media->_p->audioData->getData() + media->_p->audioDataSamplesOffset * sampleByteCount,
                //    size * sampleByteCount);
                AV::Audio::Data::volume(
                    media->_p->audioData->getData() + media->_p->audioDataSamplesOffset * sampleByteCount,
                    p,
                    volume,
                    size,
                    info.info.channelCount,
                    info.info.type);
                p += size * sampleByteCount;
                media->_p->audioDataSamplesOffset += size;
                media->_p->audioDataSamplesCount += size;
                media->_p->audioDataSamplesTime = std::chrono::system_clock::now();
                outputSampleCount -= size;
                if (media->_p->audioDataSamplesOffset >= media->_p->audioData->getSampleCount())
                {
                    media->_p->audioData.reset();
                    media->_p->audioDataSamplesOffset = 0;
                }
            }

            for (const auto& i : frames)
            {
                media->_p->audioData = i.audio;
                size_t size = std::min(i.audio->getSampleCount(), outputSampleCount);
                //memcpy(
                //    p,
                //    i.audio->getData(),
                //    size * sampleByteCount);
                AV::Audio::Data::volume(
                    i.audio->getData(),
                    p,
                    volume,
                    size,
                    info.info.channelCount,
                    info.info.type);
                p += size * sampleByteCount;
                media->_p->audioDataSamplesOffset = size;
                media->_p->audioDataSamplesCount += size;
                media->_p->audioDataSamplesTime = std::chrono::system_clock::now();
                outputSampleCount -= size;
            }

            const size_t zero = (nFrames * sampleByteCount) - (p - reinterpret_cast<uint8_t*>(outputBuffer));
            if (zero)
            {
                //! \todo Is this the correct way to clear the audio data?
                memset(p, 0, zero);
            }

            return 0;
        }

        void Media::_rtAudioErrorCallback(
            RtAudioError::Type type,
            const std::string& errorText)
        {}

    } // namespace ViewApp
} // namespace djv

