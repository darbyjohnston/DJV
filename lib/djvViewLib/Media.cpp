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

#include <djvViewLib/Context.h>

#include <djvAV/FFmpeg.h>

#include <djvCore/Timer.h>

#include <condition_variable>
#include <sstream>

namespace djv
{
    namespace ViewLib
    {
        namespace
        {
            const size_t bufferCount = 100;
            const size_t timeout = 10;

        } // namespace

        struct Media::Private
        {
            std::weak_ptr<Context> context;
            std::string fileName;
            std::shared_ptr<Core::ValueSubject<AV::IO::Info> > info;
            AV::IO::VideoInfo videoInfo;
            AV::IO::AudioInfo audioInfo;
            std::shared_ptr<Core::ValueSubject<AV::Duration> > duration;
            std::shared_ptr<Core::ValueSubject<AV::Timestamp> > currentTime;
            std::shared_ptr<Core::ValueSubject<Enum::Playback> > playback;
            std::shared_ptr<Core::ValueSubject<std::shared_ptr<AV::Image> > > currentImage;
            std::shared_ptr<Core::ValueSubject<size_t> > alUnqueuedBuffers;
            std::shared_ptr<AV::IO::ReadQueue> queue;
            std::shared_ptr<Core::Timer> queueTimer;
            std::condition_variable queueCV;
            std::shared_ptr<AV::IO::IRead> read;
            std::future<AV::IO::Info> infoFuture;
            std::shared_ptr<Core::Timer> infoTimer;
            ALuint alSource = 0;
            std::vector<ALuint> alBuffers;
            int64_t queuedBytes = 0;
            AV::Timestamp timeOffset = 0;
            std::shared_ptr<Core::Timer> playbackTimer;
        };

        void Media::_init(const std::string & fileName, const std::shared_ptr<Context> & context)
        {
            _p->context = context;
            _p->fileName = fileName;
            _p->info = Core::ValueSubject<AV::IO::Info>::create();
            _p->duration = Core::ValueSubject<AV::Duration>::create();
            _p->currentTime = Core::ValueSubject<AV::Timestamp>::create();
            _p->playback = Core::ValueSubject<Enum::Playback>::create();
            _p->currentImage = Core::ValueSubject<std::shared_ptr<AV::Image >>::create();
            _p->alUnqueuedBuffers = Core::ValueSubject<size_t>::create();
            _p->queue = AV::IO::ReadQueue::create();
            _p->queueTimer = Core::Timer::create(context);
            _p->queueTimer->setRepeating(true);
            _p->infoTimer = Core::Timer::create(context);
            _p->infoTimer->setRepeating(true);
            _p->playbackTimer = Core::Timer::create(context);
            _p->playbackTimer->setRepeating(true);

            alGetError();
            alGenSources(1, &_p->alSource);
            ALenum error = AL_NO_ERROR;
            if ((error = alGetError()) != AL_NO_ERROR)
            {
                std::stringstream ss;
                ss << "Cannot create OpenAL source: " << AV::Audio::getALErrorString(error);
                throw Core::Error("djv::ViewLib::Media", ss.str());
            }

            _p->alBuffers.resize(bufferCount);
            alGenBuffers(bufferCount, _p->alBuffers.data());
            if ((error = alGetError()) != AL_NO_ERROR)
            {
                std::stringstream ss;
                ss << "Cannot create OpenAL buffers: " << AV::Audio::getALErrorString(error);
                throw Core::Error("djv::ViewLib::Media", ss.str());
            }

            try
            {
                _p->read = context->getSystemT<AV::IO::System>()->read(fileName, _p->queue);
                _p->infoFuture = _p->read->getInfo();
                _p->infoTimer->start(
                    std::chrono::milliseconds(timeout),
                    [this](float)
                {
                    if (_p->infoFuture.valid() &&
                        _p->infoFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                    {
                        _p->infoTimer->stop();
                        auto info = _p->infoFuture.get();
                        AV::Duration duration = 0;
                        const auto & video = info.getVideo();
                        if (video.size())
                        {
                            _p->videoInfo = video[0];
                            duration = std::max(duration, video[0].getDuration());
                        }
                        const auto & audio = info.getAudio();
                        if (audio.size())
                        {
                            _p->audioInfo = audio[0];
                            duration = std::max(duration, audio[0].getDuration());
                        }
                        _p->info->setIfChanged(info);
                        _p->duration->setIfChanged(duration);
                    }
                });

                _p->queueTimer->start(
                    std::chrono::milliseconds(timeout),
                    [this](float)
                {
                    std::unique_lock<std::mutex> lock(_p->queue->getMutex());
                    if (_p->queueCV.wait_for(
                        lock,
                        std::chrono::milliseconds(timeout),
                        [this]
                    {
                        return _p->queue->hasVideoFrames();
                    }))
                    {
                        _p->currentImage->setIfChanged(_p->queue->getVideoFrame().second);
                    }
                });
            }
            catch (const std::exception & e)
            {
                std::stringstream ss;
                ss << "Cannot open " << fileName << ": " << e.what();
                context->log("djv::ViewLib::Media", ss.str(), Core::LogLevel::Error);
            }
        }

        Media::Media() :
            _p(new Private)
        {}

        Media::~Media()
        {}

        std::shared_ptr<Media> Media::create(const std::string & fileName, std::shared_ptr<Context> & context)
        {
            auto out = std::shared_ptr<Media>(new Media);
            out->_init(fileName, context);
            return out;
        }

        const std::string & Media::getFileName() const
        {
            return _p->fileName;
        }

        std::shared_ptr<Core::IValueSubject<AV::IO::Info> > Media::getInfo() const
        {
            return _p->info;
        }

        std::shared_ptr<Core::IValueSubject<AV::Duration> > Media::getDuration() const
        {
            return _p->duration;
        }

        std::shared_ptr<Core::IValueSubject<AV::Timestamp> > Media::getCurrentTime() const
        {
            return _p->currentTime;
        }

        std::shared_ptr<Core::IValueSubject<Enum::Playback> > Media::getPlayback() const
        {
            return _p->playback;
        }

        std::shared_ptr<Core::IValueSubject<std::shared_ptr<AV::Image> > > Media::getCurrentImage() const
        {
            return _p->currentImage;
        }

        std::shared_ptr<Core::IValueSubject<size_t> > Media::getALUnqueuedBuffers() const
        {
            return _p->alUnqueuedBuffers;
        }

        void Media::setCurrentTime(AV::Timestamp value)
        {
            if (_p->currentTime->setIfChanged(value))
            {
                setPlayback(Enum::Playback::Stop);
                _p->timeOffset = _p->currentTime->get();
                if (_p->read)
                {
                    _p->read->seek(value);
                }
                _timeUpdate();
            }
        }

        void Media::setPlayback(Enum::Playback value)
        {
            if (_p->playback->setIfChanged(value))
            {
                _playbackUpdate();
            }
        }

        void Media::_playbackUpdate()
        {
            switch (_p->playback->get())
            {
            case Enum::Playback::Forward:
            {
                _timeUpdate();
                _p->playbackTimer->start(
                    std::chrono::milliseconds(timeout),
                    [this](float)
                {
                    switch (_p->playback->get())
                    {
                    case Enum::Playback::Forward:
                    {
                        /*ALint alSourceState = 0;
                        alGetSourcei(_p->alSource, AL_SOURCE_STATE, &alSourceState);
                        if ((AL_INITIAL == alSourceState || AL_STOPPED == alSourceState) && _p->queue->getAudioFrameCount() == 100)
                        {
                            alSourcePlay(_p->alSource);
                        }*/
                        ALint offset = 0;
                        alGetSourcei(_p->alSource, AL_BYTE_OFFSET, &offset);
                        AVRational r;
                        r.num = 1;
                        r.den = static_cast<int>(_p->audioInfo.getInfo().getSampleRate());
                        auto time = _p->timeOffset + av_rescale_q(
                            (_p->queuedBytes + offset) / AV::Audio::getByteCount(_p->audioInfo.getInfo().getType()),
                            r,
                            AV::IO::FFmpeg::getTimeBaseQ());
                        _p->currentTime->setIfChanged(time);
                        _timeUpdate();
                        break;
                    }
                    default: break;
                    }
                });
                //ALint alSourceState = 0;
                //alGetSourcei(_p->alSource, AL_SOURCE_STATE, &alSourceState);
                //if (AL_INITIAL == alSourceState || AL_STOPPED == alSourceState)
                {
                    alSourcePlay(_p->alSource);
                }
                break;
            }
            case Enum::Playback::Stop:
                alSourceStop(_p->alSource);
                _p->playbackTimer->stop();
                _timeUpdate();
                _p->queuedBytes = 0;
                _p->timeOffset = _p->currentTime->get();
                alSourcei(_p->alSource, AL_BYTE_OFFSET, 0);
                break;
            default: break;
            }
        }

        void Media::_timeUpdate()
        {
            // Update the video queue.
            {
                std::lock_guard<std::mutex> lock(_p->queue->getMutex());
                while (_p->queue->hasVideoFrames() && _p->queue->getVideoFrame().first < _p->currentTime->get())
                {
                    _p->queue->popVideoFrame();
                }
            }

            // Unqueue the processed OpenAL buffers.
            ALint processed = 0;
            alGetSourcei(_p->alSource, AL_BUFFERS_PROCESSED, &processed);
            std::vector<ALuint> buffers;
            buffers.resize(processed);
            alSourceUnqueueBuffers(_p->alSource, processed, buffers.data());
            for (const auto & buffer : buffers)
            {
                _p->alBuffers.push_back(buffer);
                if (Enum::Playback::Stop != _p->playback->get())
                {
                    ALint size = 0;
                    alGetBufferi(buffer, AL_SIZE, &size);
                    _p->queuedBytes += size;
                }
            }

            if (Enum::Playback::Stop != _p->playback->get())
            {
                // Fill the OpenAL queue with frames from the I/O queue.
                ALint queued = 0;
                alGetSourcei(_p->alSource, AL_BUFFERS_QUEUED, &queued);
                std::vector<AV::IO::AudioFrame> frames;
                {
                    std::lock_guard<std::mutex> lock(_p->queue->getMutex());
                    while (_p->queue->hasAudioFrames() && frames.size() < bufferCount - queued && frames.size() < _p->alBuffers.size())
                    {
                        frames.push_back(_p->queue->getAudioFrame());
                        _p->queue->popAudioFrame();
                    }
                }
                for (size_t i = 0; i < frames.size(); ++i)
                {
                    auto data = AV::Audio::Data::convert(frames[i].second, AV::Audio::Type::S16);
                    const auto info = data->getInfo();
                    auto buffer = _p->alBuffers.back();
                    _p->alBuffers.pop_back();
                    alBufferData(
                        buffer,
                        AV::Audio::getALType(info.getChannels(), info.getType()),
                        data->getData(),
                        data->getByteCount(),
                        static_cast<ALsizei>(info.getSampleRate()));
                    alSourceQueueBuffers(_p->alSource, 1, &buffer);
                }
            }
        }

    } // namespace ViewLib
} // namespace djv

