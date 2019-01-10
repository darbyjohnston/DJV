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

#include <djvViewLib/Media.h>

#include <djvAV/FFmpeg.h>

#include <djvCore/Context.h>
#include <djvCore/Timer.h>

#include <condition_variable>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        namespace
        {
            //! \todo [1.0 S] Should this be configurable?
            const size_t bufferCount = 100;

        } // namespace

        struct Media::Private
        {
            Context * context = nullptr;
            std::string fileName;
            std::shared_ptr<ValueSubject<AV::IO::Info> > info;
            AV::IO::VideoInfo videoInfo;
            AV::IO::AudioInfo audioInfo;
            std::shared_ptr<ValueSubject<Time::Duration> > duration;
            std::shared_ptr<ValueSubject<Time::Timestamp> > currentTime;
            std::shared_ptr<ValueSubject<Playback> > playback;
            std::shared_ptr<ValueSubject<std::shared_ptr<AV::Image::Image> > > currentImage;
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
            int64_t queuedBytes = 0;
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
            p.duration = ValueSubject<Time::Duration>::create();
            p.currentTime = ValueSubject<Time::Timestamp>::create();
            p.playback = ValueSubject<Playback>::create();
            p.currentImage = ValueSubject<std::shared_ptr<AV::Image::Image> >::create();
            p.videoQueueMax = ValueSubject<size_t>::create();
            p.audioQueueMax = ValueSubject<size_t>::create();
            p.videoQueueCount = ValueSubject<size_t>::create();
            p.audioQueueCount = ValueSubject<size_t>::create();
            p.alUnqueuedBuffers = ValueSubject<size_t>::create();
            p.queue = AV::IO::Queue::create();
            p.queue->setCloseOnFinish(false);
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
                std::stringstream ss;
                ss << "djv::ViewLib::Media: " << DJV_TEXT("cannot create OpenAL source") << ": " << AV::Audio::getALErrorString(error);
                throw std::runtime_error(ss.str());
            }

            p.alBuffers.resize(bufferCount);
            alGenBuffers(bufferCount, p.alBuffers.data());
            if ((error = alGetError()) != AL_NO_ERROR)
            {
                std::stringstream ss;
                ss << "djv::ViewLib::Media: " << DJV_TEXT("cannot create OpenAL buffers") << ": " << AV::Audio::getALErrorString(error);
                throw std::runtime_error(ss.str());
            }

            if (auto io = context->getSystemT<AV::IO::System>().lock())
            {
                try
                {
                    p.read = io->read(fileName, p.queue);
                    p.infoFuture = p.read->getInfo();
                    const auto timeout = Time::Timer::getMilliseconds(Time::Timer::Value::Fast);
                    p.infoTimer->start(
                        timeout,
                        [this, fileName](float)
                    {
                        DJV_PRIVATE_PTR();
                        if (p.infoFuture.valid() &&
                            p.infoFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                        {
                            p.infoTimer->stop();
                            auto info = p.infoFuture.get();
                            Time::Duration duration = 0;
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
                            {
                                std::stringstream ss;
                                ss << fileName << " duration: " << duration;
                                p.context->log("djv::ViewLib::Media", ss.str());
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
                        Time::Timer::getMilliseconds(Time::Timer::Value::Slow),
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
                    std::stringstream ss;
                    ss << "djv::ViewLib::Media: " << DJV_TEXT("cannot open") << ": " << fileName << ": " << e.what();
                    context->log("djv::ViewLib::Media", ss.str(), LogLevel::Error);
                }
            }
        }

        Media::Media() :
            _p(new Private)
        {}

        Media::~Media()
        {}

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

        std::shared_ptr<IValueSubject<Time::Duration> > Media::observeDuration() const
        {
            return _p->duration;
        }

        std::shared_ptr<IValueSubject<Time::Timestamp> > Media::observeCurrentTime() const
        {
            return _p->currentTime;
        }

        std::shared_ptr<IValueSubject<Playback> > Media::observePlayback() const
        {
            return _p->playback;
        }

        std::shared_ptr<IValueSubject<std::shared_ptr<AV::Image::Image> > > Media::observeCurrentImage() const
        {
            return _p->currentImage;
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
            if (p.currentTime->setIfChanged(value))
            {
                if (p.read)
                {
                    p.read->seek(value);
                }
                setPlayback(Playback::Stop);
                p.timeOffset = value;
                _timeUpdate();
            }
        }

        void Media::setPlayback(Playback value)
        {
            if (_p->playback->setIfChanged(value))
            {
                _playbackUpdate();
            }
        }

        void Media::_playbackUpdate()
        {
            DJV_PRIVATE_PTR();
            switch (p.playback->get())
            {
            case Playback::Forward:
            {
                _timeUpdate();
                p.startTime = std::chrono::system_clock::now();
                p.playbackTimer->start(
                    Time::Timer::getMilliseconds(Time::Timer::Value::Fast),
                    [this](float)
                {
                    DJV_PRIVATE_PTR();
                    switch (p.playback->get())
                    {
                    case Playback::Forward:
                    {
                        /*ALint alSourceState = 0;
                        alGetSourcei(p.alSource, AL_SOURCE_STATE, &alSourceState);
                        if ((AL_INITIAL == alSourceState || AL_STOPPED == alSourceState) && p.queue->getAudioCount() == 100)
                        {
                            alSourcePlay(p.alSource);
                        }*/
                        Time::Timestamp pts = 0;
                        AVRational r;
                        Time::Timestamp time = 0;
                        if (p.audioInfo.info.isValid())
                        {
                            ALint offset = 0;
                            alGetSourcei(p.alSource, AL_BYTE_OFFSET, &offset);
                            pts = (p.queuedBytes + offset) / AV::Audio::getByteCount(p.audioInfo.info.type);
                            r.num = 1;
                            r.den = static_cast<int>(p.audioInfo.info.sampleRate);
                            time = p.timeOffset + av_rescale_q(pts, r, AV::IO::FFmpeg::getTimeBaseQ());
                            /*{
                                std::stringstream ss;
                                ss << "audio time = " << time;
                                p.context->log("djv::ViewLib::Media", ss.str());
                            }*/
                        }
                        else
                        {
                            const auto now = std::chrono::system_clock::now();
                            const std::chrono::duration<double> delta = now - p.startTime;
                            auto pts = static_cast<Time::Timestamp>(delta.count() * p.videoInfo.speed.getScale() / p.videoInfo.speed.getDuration());
                            r.num = p.videoInfo.speed.getDuration();
                            r.den = p.videoInfo.speed.getScale();
                            time = p.timeOffset + av_rescale_q(pts, r, AV::IO::FFmpeg::getTimeBaseQ());
                            /*{
                                std::stringstream ss;
                                ss << "video time = " << time;
                                p.context->log("djv::ViewLib::Media", ss.str());
                            }*/
                        }
                        p.currentTime->setIfChanged(time);
                        _timeUpdate();
                        break;
                    }
                    default: break;
                    }
                });
                //ALint alSourceState = 0;
                //alGetSourcei(p.alSource, AL_SOURCE_STATE, &alSourceState);
                //if (AL_INITIAL == alSourceState || AL_STOPPED == alSourceState)
                {
                    alSourcePlay(p.alSource);
                }
                break;
            }
            case Playback::Stop:
                alSourceStop(p.alSource);
                p.playbackTimer->stop();
                _timeUpdate();
                p.queuedBytes = 0;
                p.timeOffset = p.currentTime->get();
                alSourcei(p.alSource, AL_BYTE_OFFSET, 0);
                break;
            default: break;
            }
        }

        void Media::_timeUpdate()
        {
            // Update the video queue.
            DJV_PRIVATE_PTR();
            {
                std::lock_guard<std::mutex> lock(p.queue->getMutex());
                while (p.queue->hasVideo() && p.queue->getVideo().first < p.currentTime->get())
                {
                    p.queue->popVideo();
                }
            }

            // Unqueue the processed OpenAL buffers.
            ALint processed = 0;
            alGetSourcei(p.alSource, AL_BUFFERS_PROCESSED, &processed);
            std::vector<ALuint> buffers;
            buffers.resize(processed);
            alSourceUnqueueBuffers(p.alSource, processed, buffers.data());
            for (const auto & buffer : buffers)
            {
                p.alBuffers.push_back(buffer);
                if (Playback::Stop != p.playback->get())
                {
                    ALint size = 0;
                    alGetBufferi(buffer, AL_SIZE, &size);
                    p.queuedBytes += size;
                }
            }

            if (Playback::Stop != p.playback->get())
            {
                // Fill the OpenAL queue with frames from the I/O queue.
                ALint queued = 0;
                alGetSourcei(p.alSource, AL_BUFFERS_QUEUED, &queued);
                std::vector<AV::IO::AudioFrame> frames;
                {
                    std::lock_guard<std::mutex> lock(p.queue->getMutex());
                    while (p.queue->hasAudio() && frames.size() < bufferCount - queued && frames.size() < p.alBuffers.size())
                    {
                        frames.push_back(p.queue->getAudio());
                        p.queue->popAudio();
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

    } // namespace ViewLib
} // namespace djv

