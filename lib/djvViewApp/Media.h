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

#pragma once

#include <djvViewApp/Enum.h>

#include <djvAV/IO.h>

#include <djvCore/ListObserver.h>
#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace Core
    {
        class Context;

    } // namespace Core

    namespace ViewApp
    {
        class Media : public std::enable_shared_from_this<Media>
        {
            DJV_NON_COPYABLE(Media);

        protected:
            void _init(const std::string &, size_t videoMax, size_t audioMax, Core::Context *);
            Media();

        public:
            ~Media();

            static std::shared_ptr<Media> create(const std::string&, Core::Context*);
            static std::shared_ptr<Media> create(const std::string&, size_t videoMax, size_t audioMax, Core::Context*);

            const std::string & getFileName() const;
            std::shared_ptr<Core::IValueSubject<AV::IO::Info> > observeInfo() const;
            std::shared_ptr<Core::IValueSubject<Core::Time::Timestamp> > observeDuration() const;
            std::shared_ptr<Core::IValueSubject<Core::Time::Timestamp> > observeCurrentTime() const;
            std::shared_ptr<Core::IValueSubject<std::shared_ptr<AV::Image::Image> > > observeCurrentImage() const;
            std::shared_ptr<Core::IValueSubject<Playback> > observePlayback() const;
            std::shared_ptr<Core::IValueSubject<PlaybackMode> > observePlaybackMode() const;
            std::shared_ptr<Core::IValueSubject<bool> > observeInOutPointsEnabled() const;
            std::shared_ptr<Core::IValueSubject<Core::Time::Timestamp> > observeInPoint() const;
            std::shared_ptr<Core::IValueSubject<Core::Time::Timestamp> > observeOutPoint() const;

            std::shared_ptr<Core::IValueSubject<size_t> > observeVideoQueueMax() const;
            std::shared_ptr<Core::IValueSubject<size_t> > observeAudioQueueMax() const;
            std::shared_ptr<Core::IValueSubject<size_t> > observeVideoQueueCount() const;
            std::shared_ptr<Core::IValueSubject<size_t> > observeAudioQueueCount() const;
            std::shared_ptr<Core::IValueSubject<size_t> > observeALUnqueuedBuffers() const;

            void setCurrentTime(Core::Time::Timestamp);
            void setPlayback(Playback);
            void setPlaybackMode(PlaybackMode);
            void inPoint();
            void outPoint();
            void start();
            void end();
            void nextFrame(size_t = 1);
            void prevFrame(size_t = 1);
            void setInOutPointsEnabled(bool);
            void setInPoint(Core::Time::Timestamp);
            void setOutPoint(Core::Time::Timestamp);
            void resetInPoint();
            void resetOutPoint();

        private:
            DJV_PRIVATE();

            void _playbackUpdate();
            void _timeUpdate();
        };

    } // namespace ViewApp
} // namespace djv

