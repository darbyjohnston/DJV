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

#include <RtAudio.h>

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
            void _init(const Core::FileSystem::FileInfo&, const std::shared_ptr<Core::Context>&);
            Media();

        public:
            ~Media();

            //! Create a new media object.
            static std::shared_ptr<Media> create(const Core::FileSystem::FileInfo&, const std::shared_ptr<Core::Context>&);

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
            std::shared_ptr<Core::IValueSubject<Core::Frame::Sequence> > observeSequence() const;
            std::shared_ptr<Core::IValueSubject<Core::Frame::Index> > observeCurrentFrame() const;
            std::shared_ptr<Core::IValueSubject<Playback> > observePlayback() const;
            std::shared_ptr<Core::IValueSubject<PlaybackMode> > observePlaybackMode() const;
            std::shared_ptr<Core::IValueSubject<bool> > observeInOutPointsEnabled() const;
            std::shared_ptr<Core::IValueSubject<Core::Frame::Index> > observeInPoint() const;
            std::shared_ptr<Core::IValueSubject<Core::Frame::Index> > observeOutPoint() const;

            void setSpeed(const Core::Time::Speed&);
            void setPlayEveryFrame(bool);
            void setCurrentFrame(Core::Frame::Index, bool inOutPoints = true);
            void setPlayback(Playback);
            void setPlaybackMode(PlaybackMode);
            void inPoint();
            void outPoint();
            void start();
            void end();
            void nextFrame(size_t = 1);
            void prevFrame(size_t = 1);
            void setInOutPointsEnabled(bool);
            void setInPoint();
            void setInPoint(Core::Frame::Index);
            void setOutPoint();
            void setOutPoint(Core::Frame::Index);
            void resetInPoint();
            void resetOutPoint();

            ///@}

            //! \name Audio
            ///@{

            std::shared_ptr<Core::IValueSubject<bool> > observeAudioEnabled() const;
            std::shared_ptr<Core::IValueSubject<float> > observeVolume() const;
            std::shared_ptr<Core::IValueSubject<bool> > observeMute() const;

            void setVolume(float);
            void setMute(bool);

            ///@}

            //! \name I/O
            ///@{

            std::shared_ptr<Core::IValueSubject<size_t> > observeThreadCount() const;

            void setThreadCount(size_t);

            ///@}

            //! \name Memory Cache
            ///@{

            bool hasCache() const;
            size_t getCacheMaxByteCount() const;
            size_t getCacheByteCount() const;
            std::shared_ptr<Core::IListSubject<Core::Frame::Range> > observeCachedFrames() const;

            void setCacheEnabled(bool);
            void setCacheMaxByteCount(size_t);

            ///@}

            //! \name Debugging
            ///@{

            std::shared_ptr<Core::IValueSubject<size_t> > observeVideoQueueMax() const;
            std::shared_ptr<Core::IValueSubject<size_t> > observeAudioQueueMax() const;
            std::shared_ptr<Core::IValueSubject<size_t> > observeVideoQueueCount() const;
            std::shared_ptr<Core::IValueSubject<size_t> > observeAudioQueueCount() const;

            ///@}

        private:
            bool _hasAudio() const;
            bool _isAudioEnabled() const;
            bool _hasAudioSyncPlayback() const;
            void _open();
            void _setCurrentFrame(Core::Frame::Index);
            void _seek(Core::Frame::Index);
            void _playbackUpdate();
            void _playbackTick();
            void _startAudioStream();
            void _stopAudioStream();
            void _queueUpdate();

            static int _rtAudioCallback(
                void* outputBuffer,
                void* inputBuffer,
                unsigned int nFrames,
                double streamTime,
                RtAudioStreamStatus status,
                void* userData);
            static void _rtAudioErrorCallback(
                RtAudioError::Type type,
                const std::string& errorText);

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

