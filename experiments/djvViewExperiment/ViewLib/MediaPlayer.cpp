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

#include <ViewLib/MediaPlayer.h>

#include <ViewLib/Context.h>

#include <AV/Audio.h>

#include <iostream>

namespace djv
{
    namespace ViewLib
    {
        struct MediaPlayer::Private
        {
            QPointer<Context> context;
            std::shared_ptr<AV::IO::Queue> queue;
            AV::IO::AudioInfo info;
            AV::IO::Duration duration = 0;
            AV::IO::Timestamp currentTime = 0;
            Enum::Playback playback = Enum::Playback::Stop;
            ALuint alSource = 0;
            std::vector<ALuint> alBuffers;
            int playbackTimer = 0;
            int startTimer = 0;
            int64_t queuedBytes = 0;
            int64_t timeOffset = 0;
        };
        
        MediaPlayer::MediaPlayer(const std::shared_ptr<AV::IO::Queue> & queue, const QPointer<Context> & context, QObject * parent) :
            QObject(parent),
            _p(new Private)
        {
            _p->context = context;
            _p->queue = queue;
        }
        
        MediaPlayer::~MediaPlayer()
        {}

        const AV::IO::AudioInfo & MediaPlayer::getInfo() const
        {
            return _p->info;
        }

        AV::IO::Timestamp MediaPlayer::getCurrentTime() const
        {
            return _p->currentTime;
        }

        Enum::Playback MediaPlayer::getPlayback() const
        {
            return _p->playback;
        }

        size_t MediaPlayer::getALUnqueuedBuffers() const
        {
            return _p->alBuffers.size();
        }

    } // namespace ViewLib
} // namespace djv

