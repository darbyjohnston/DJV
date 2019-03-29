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

#include <PlaybackSystem.h>

#include <Context.h>

#include <djvCore/Error.h>

#include <QTimerEvent>

using namespace djv;

namespace djv
{
    namespace AudioExperiment2
    {
        namespace
        {
            const size_t bufferCount = 100;
            const size_t timeout = 10;

            QStringList split(const ALCchar * value)
            {
                QStringList out;
                for (const ALCchar * p = value, *p2 = value; p2; ++p2)
                {
                    if (!*p2)
                    {
                        if (p2 > value && !*(p2 - 1))
                        {
                            break;
                        }
                        out.append(QString::fromLatin1(p, p2 - p));
                        p = p2;
                    }
                }
                return out;
            }

        } // namespace

        PlaybackSystem::PlaybackSystem(
            const std::shared_ptr<Util::AVQueue> & queue,
            const QPointer<Context> & context,
            QObject * parent) :
            QObject(parent),
            _context(context),
            _queue(queue)
        {
            //DJV_DEBUG("PlaybackSystem::PlaybackSystem");

            const ALCchar * devices = NULL;
            ALenum alEnum = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
            if (AL_TRUE == alEnum)
            {
                devices = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
                Q_FOREACH(auto s, split(devices))
                {
                    //DJV_DEBUG_PRINT("device = " << s);
                }
            }

            _alDevice = alcOpenDevice(devices);
            if (!_alDevice)
            {
                throw Core::Error("PlaybackSystem", "Cannot open OpenAL device");
            }
            _alContext = alcCreateContext(_alDevice, NULL);
            if (!_alContext)
            {
                throw Core::Error("PlaybackSystem", "Cannot create OpenAL context");
            }
            ALCboolean r = alcMakeContextCurrent(_alContext);
            if (AL_FALSE == r)
            {
                throw Core::Error("PlaybackSystem", "Cannot make OpenAL context current");
            }
            alGetError();
            alGenSources(1, &_alSource);
            ALenum error = AL_NO_ERROR;
            if ((error = alGetError()) != AL_NO_ERROR)
            {
                throw Core::Error("PlaybackSystem",
                    QString("Cannot create OpenAL source: %1").arg(AV::Audio::alErrorString(error)));
            }

            _alBuffers.resize(bufferCount);
            alGenBuffers(bufferCount, _alBuffers.data());
            if ((error = alGetError()) != AL_NO_ERROR)
            {
                throw Core::Error("PlaybackSystem",
                    QString("Cannot create OpenAL buffers: %1").arg(AV::Audio::alErrorString(error)));
            }
        }

        PlaybackSystem::~PlaybackSystem()
        {
            alcMakeContextCurrent(NULL);
            if (_alContext)
            {
                alcDestroyContext(_alContext);
            }
            if (_alDevice)
            {
                alcCloseDevice(_alDevice);
            }
        }

        void PlaybackSystem::setInfo(const Util::AudioInfo & value)
        {
            if (value == _info)
                return;
            _info = value;
            Q_EMIT infoChanged(_info);
        }

        void PlaybackSystem::setDuration(int64_t value)
        {
            if (value == _duration)
                return;
            _duration = value;
            Q_EMIT durationChanged(_duration);
        }

        void PlaybackSystem::setCurrentTime(int64_t value)
        {
            if (value == _currentTime)
                return;
            _currentTime = value;
            setPlayback(Util::PLAYBACK_STOP);
            _timeOffset = _currentTime;
            timeUpdate();
            Q_EMIT currentTimeChanged(_currentTime);
        }

        void PlaybackSystem::setPlayback(Util::PLAYBACK value)
        {
            if (value == _playback)
                return;
            _playback = value;
            playbackUpdate();
            Q_EMIT playbackChanged(_playback);
        }

        void PlaybackSystem::timerEvent(QTimerEvent *)
        {
            //DJV_DEBUG("PlaybackSystem::timerEvent");
            switch (_playback)
            {
            case Util::PLAYBACK_FORWARD:
            {
                ALint offset = 0;
                alGetSourcei(_alSource, AL_BYTE_OFFSET, &offset);
                //DJV_DEBUG_PRINT("al offset = " << offset);
                AVRational r;
                r.num = 1;
                r.den = static_cast<int>(_info.sampleRate);
                auto time = _timeOffset + av_rescale_q(
                    (_queuedBytes + offset) / AV::Audio::byteCount(_info.type),
                    r,
                    AV::FFmpeg::timeBaseQ());
                //DJV_DEBUG_PRINT("time = " << time);
                _currentTime = time;
                timeUpdate();
                Q_EMIT currentTimeChanged(_currentTime);
                break;
            }
            default: break;
            }
        }

        void PlaybackSystem::playbackUpdate()
        {
            //DJV_DEBUG("PlaybackSystem::playbackUpdate");
            //DJV_DEBUG_PRINT("playback = " << _playback);
            switch (_playback)
            {
            case Util::PLAYBACK_FORWARD:
                timeUpdate();
                if (_playbackTimer)
                {
                    killTimer(_playbackTimer);
                }
                _playbackTimer = startTimer(timeout);
                alSourcePlay(_alSource);
                break;
            case Util::PLAYBACK_STOP:
                alSourceStop(_alSource);
                if (_playbackTimer)
                {
                    killTimer(_playbackTimer);
                    _playbackTimer = 0;
                }
                timeUpdate();
                _queuedBytes = 0;
                _timeOffset = _currentTime;
                alSourcei(_alSource, AL_BYTE_OFFSET, 0);
                break;
            default: break;
            }
        }

        void PlaybackSystem::timeUpdate()
        {
            //DJV_DEBUG("PlaybackSystem::timeUpdate");
            //DJV_DEBUG_PRINT("current time = " << _currentTime);
            //DJV_DEBUG_PRINT("playback = " << _playback);
            //DJV_DEBUG_PRINT("buffers = " << _alBuffers.size());

            // Update the video queue.
            {
                std::lock_guard<std::mutex> lock(_queue->mutex);
                //DJV_DEBUG_PRINT("video queue = " << _queue->video.size());
                while (!_queue->video.empty() && _queue->video.front().first < _currentTime)
                {
                    _queue->video.pop_front();
                }
                //DJV_DEBUG_PRINT("video queue = " << _queue->video.size());
            }

            // Unqueue the processed OpenAL buffers.
            ALint processed = 0;
            alGetSourcei(_alSource, AL_BUFFERS_PROCESSED, &processed);
            //DJV_DEBUG_PRINT("al processed = " << processed);
            std::vector<ALuint> buffers;
            buffers.resize(processed);
            alSourceUnqueueBuffers(_alSource, processed, buffers.data());
            for (const auto & buffer : buffers)
            {
                _alBuffers.push_back(buffer);
                if (Util::PLAYBACK_STOP != _playback)
                {
                    ALint size = 0;
                    alGetBufferi(buffer, AL_SIZE, &size);
                    _queuedBytes += size;
                }
            }
            //DJV_DEBUG_PRINT("al buffers = " << _alBuffers.size());

            if (Util::PLAYBACK_STOP != _playback)
            {
                // Fill the OpenAL queue with frames from the I/O queue.
                ALint queued = 0;
                alGetSourcei(_alSource, AL_BUFFERS_QUEUED, &queued);
                //DJV_DEBUG_PRINT("al queued = " << queued);
                std::vector<Util::AudioFrame> frames;
                {
                    std::lock_guard<std::mutex> lock(_queue->mutex);
                    //DJV_DEBUG_PRINT("audio queue = " << _queue->audio.size());
                    while (!_queue->audio.empty() && frames.size() < bufferCount - queued && frames.size() < _alBuffers.size())
                    {
                        frames.push_back(_queue->audio.front());
                        _queue->audio.pop_front();
                    }
                }
                //DJV_DEBUG_PRINT("audio frames = " << frames.size());
                for (size_t i = 0; i < frames.size(); ++i)
                {
                    auto data = AV::AudioData::convert(*frames[i].second, AV::Audio::S16);
                    auto buffer = _alBuffers.back();
                    _alBuffers.pop_back();
                    alBufferData(
                        buffer,
                        AV::Audio::toAL(data.channels(), data.type()),
                        data.data(),
                        data.byteCount(),
                        static_cast<ALsizei>(data.sampleRate()));
                    alSourceQueueBuffers(_alSource, 1, &buffer);
                }
            }
        }

    } // namespace AudioExperiment2
} // namespace djv
