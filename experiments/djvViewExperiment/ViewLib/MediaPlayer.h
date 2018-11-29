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

#pragma once

#include <ViewLib/Enum.h>

#include <AV/IO.h>

#include <QPointer>

namespace djv
{
    namespace ViewLib
    {
        class Context;

        class MediaPlayer : public QObject
        {
            Q_OBJECT

        public:
            MediaPlayer(const std::shared_ptr<AV::IO::Queue> &, const QPointer<Context> &, QObject * parent = nullptr);
            ~MediaPlayer() override;

            const AV::IO::AudioInfo & getInfo() const;
            AV::IO::Timestamp getCurrentTime() const;
            Enum::Playback getPlayback() const;
            size_t getALUnqueuedBuffers() const;

        public Q_SLOTS:
            void setInfo(const AV::IO::AudioInfo &);
            void setCurrentTime(AV::IO::Timestamp);
            void setPlayback(Enum::Playback);

        Q_SIGNALS:
            void infoChanged(const AV::IO::AudioInfo &);
            void currentTimeChanged(AV::IO::Timestamp);
            void playbackChanged(Enum::Playback);

        protected:
            void timerEvent(QTimerEvent *) override;

        private:
            void playbackUpdate();
            void timeUpdate();
            
        private:
            DJV_PRIVATE();
        };

    } // namespace ViewLib
} // namespace djv

