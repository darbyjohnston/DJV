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

        namespace FileSystem
        {
            class FileInfo;

        } // namespace FileSystem
    } // namespace Core

    namespace ViewApp
    {
        //! This class provides a media object.
        class Media : public std::enable_shared_from_this<Media>
        {
            DJV_NON_COPYABLE(Media);

        protected:
            void _init(const Core::FileSystem::FileInfo&, Core::Context *);
            Media();

        public:
            ~Media();

            //! Create a new media object.
            static std::shared_ptr<Media> create(const Core::FileSystem::FileInfo&, Core::Context*);

            //! \name File
            ///@{

            const Core::FileSystem::FileInfo& getFileInfo() const;

            std::shared_ptr<Core::IValueSubject<AV::IO::Info> > observeInfo() const;
            std::shared_ptr<Core::IValueSubject<bool> > observeReload() const;

            void reload();

            ///@}

            //! \name Layers
            ///@{

            std::shared_ptr<Core::IValueSubject<size_t> > observeLayer() const;

            void setLayer(size_t);
            void nextLayer();
            void prevLayer();

            ///@}

            //! \name Image
            ///@{

            std::shared_ptr<Core::IValueSubject<std::shared_ptr<AV::Image::Image> > > observeCurrentImage() const;

            ///@}

            //! \name Playback
            ///@{

            std::shared_ptr<Core::IValueSubject<Core::Time::Speed> > observeSpeed() const;
            std::shared_ptr<Core::IValueSubject<Core::Time::Speed> > observeDefaultSpeed() const;
            std::shared_ptr<Core::IValueSubject<float> > observeRealSpeed() const;
            std::shared_ptr<Core::IValueSubject<bool> > observePlayEveryFrame() const;
            std::shared_ptr<Core::IValueSubject<Core::Time::Timestamp> > observeDuration() const;
            std::shared_ptr<Core::IValueSubject<Core::Time::Timestamp> > observeCurrentTime() const;
            std::shared_ptr<Core::IValueSubject<Playback> > observePlayback() const;
            std::shared_ptr<Core::IValueSubject<PlaybackMode> > observePlaybackMode() const;
            std::shared_ptr<Core::IValueSubject<bool> > observeInOutPointsEnabled() const;
            std::shared_ptr<Core::IValueSubject<Core::Time::Timestamp> > observeInPoint() const;
            std::shared_ptr<Core::IValueSubject<Core::Time::Timestamp> > observeOutPoint() const;
            std::shared_ptr<Core::IValueSubject<size_t> > observeThreadCount() const;

            void setSpeed(const Core::Time::Speed&);
            void setPlayEveryFrame(bool);
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
            void setThreadCount(size_t);

            ///@}

            //! \name Audio
            ///@{

            std::shared_ptr<Core::IValueSubject<float> > observeVolume() const;
            std::shared_ptr<Core::IValueSubject<bool> > observeMute() const;

            void setVolume(float);
            void setMute(bool);

            ///@}

            //! \name Cache
            ///@{

            std::shared_ptr<Core::IValueSubject<bool> > observeHasCache() const;
            std::shared_ptr<Core::IValueSubject<bool> > observeCacheEnabled() const;
            std::shared_ptr<Core::IValueSubject<int> > observeCacheMax() const;
            std::shared_ptr<Core::IListSubject<Core::Time::TimestampRange> > observeCachedTimestamps() const;

            void setCacheEnabled(bool);
            void setCacheMax(int);

            ///@}

            //! \name Debugging
            ///@{

            std::shared_ptr<Core::IValueSubject<size_t> > observeVideoQueueMax() const;
            std::shared_ptr<Core::IValueSubject<size_t> > observeAudioQueueMax() const;
            std::shared_ptr<Core::IValueSubject<size_t> > observeVideoQueueCount() const;
            std::shared_ptr<Core::IValueSubject<size_t> > observeAudioQueueCount() const;
            std::shared_ptr<Core::IValueSubject<size_t> > observeALQueueCount() const;

            ///@}

        private:
            DJV_PRIVATE();

            void _open();
            void _playbackUpdate();
            void _playbackTick();
            void _timeUpdate();
            void _volumeUpdate();
        };

    } // namespace ViewApp
} // namespace djv

