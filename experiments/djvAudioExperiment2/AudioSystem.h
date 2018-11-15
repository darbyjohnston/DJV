//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
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

#pragma once

#include <IO.h>

#include <QObject>
#include <QPointer>

#include <AL/al.h>
#include <AL/alc.h>

#include <future>

namespace djv
{
    namespace AudioExperiment2
    {
        class Context;

        class AudioSystem : public QObject
        {
            Q_OBJECT

        public:
            AudioSystem(
                const QPointer<IO> &,
                const QPointer<Context> &,
                QObject * parent = nullptr);
            ~AudioSystem() override;

            const Util::AudioInfo & info() const { return _info; }
            float duration() const { return _duration; }
            float currentTime() const { return _currentTime; }
            Util::PLAYBACK playback() const { return _playback; }

        public Q_SLOTS:
            void setInfo(const Util::AudioInfo &);
            void setDuration(int64_t);
            void setCurrentTime(int64_t);
            void setPlayback(Util::PLAYBACK);

        Q_SIGNALS:
            void infoChanged(const Util::AudioInfo &);
            void durationChanged(int64_t);
            void currentTimeChanged(int64_t);
            void playbackChanged(Util::PLAYBACK);

        protected:
            void timerEvent(QTimerEvent *);

        private:
            void playbackUpdate();
            void timeUpdate();

            QPointer<Context> _context;
            QPointer<IO> _io;
            std::future<IOInfo> _ioInfoFuture;
            int _ioInfoTimer = 0;
            Util::AudioInfo _info;
            int64_t _duration = 0;
            int64_t _currentTime = 0;
            Util::PLAYBACK _playback = Util::PLAYBACK_STOP;
            ALCdevice * _alDevice = nullptr;
            ALCcontext * _alContext = nullptr;
            ALuint _alSource = 0;
            std::vector<ALuint> _alBuffers;
            int _playbackTimer = 0;
            int64_t _queuedBytes = 0;
            int64_t _timeOffset = 0;
        };

    } // namespace AudioExperiment2
} // namespace djv
