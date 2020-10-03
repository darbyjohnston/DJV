// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

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
        namespace Command
        {
            class ICommand;

        } // namespace Command
    } // namespace Core

    namespace Image
    {
        class Image;
        class Info;

    } // namespace Image

    namespace System
    {
        class Context;

        namespace File
        {
            class Info;

        } // namespace File
    } // namespace System

    namespace ViewApp
    {
        class AnnotatePrimitive;
        
        //! This class provides a media object.
        class Media : public std::enable_shared_from_this<Media>
        {
            DJV_NON_COPYABLE(Media);

        protected:
            void _init(
                const System::File::Info&,
                const std::shared_ptr<System::Context>&);
            Media();

        public:
            ~Media();

            //! Create a new media object.
            static std::shared_ptr<Media> create(
                const System::File::Info&,
                const std::shared_ptr<System::Context>&);

            //! \name File
            ///@{

            bool isValid() const;
            const System::File::Info& getFileInfo() const;

            std::shared_ptr<Core::Observer::IValueSubject<AV::IO::Info> > observeInfo() const;
            std::shared_ptr<Core::Observer::IValueSubject<bool> > observeReload() const;

            void reload();

            ///@}

            //! \name Layers
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<std::pair<std::vector<Image::Info>, int> > > observeLayers() const;

            void setLayer(int);
            void nextLayer();
            void prevLayer();

            ///@}

            //! \name Image
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<std::shared_ptr<Image::Image> > > observeCurrentImage() const;

            ///@}

            //! \name Playback
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<Math::Rational> > observeSpeed() const;
            std::shared_ptr<Core::Observer::IValueSubject<PlaybackSpeed> > observePlaybackSpeed() const;
            std::shared_ptr<Core::Observer::IValueSubject<Math::Rational> > observeDefaultSpeed() const;
            std::shared_ptr<Core::Observer::IValueSubject<Math::Rational> > observeCustomSpeed() const;
            std::shared_ptr<Core::Observer::IValueSubject<float> > observeRealSpeed() const;
            std::shared_ptr<Core::Observer::IValueSubject<bool> > observePlayEveryFrame() const;
            std::shared_ptr<Core::Observer::IValueSubject<Math::Frame::Sequence> > observeSequence() const;
            std::shared_ptr<Core::Observer::IValueSubject<Math::Frame::Index> > observeCurrentFrame() const;
            std::shared_ptr<Core::Observer::IValueSubject<Playback> > observePlayback() const;
            std::shared_ptr<Core::Observer::IValueSubject<PlaybackMode> > observePlaybackMode() const;
            std::shared_ptr<Core::Observer::IValueSubject<AV::IO::InOutPoints> > observeInOutPoints() const;

            void setPlaybackSpeed(PlaybackSpeed);
            void setCustomSpeed(const Math::Rational&);
            void setPlayEveryFrame(bool);
            void setCurrentFrame(Math::Frame::Index, bool inOutPoints = true);
            void setPlayback(Playback);
            void setPlaybackMode(PlaybackMode);
            void inPoint();
            void outPoint();
            void start();
            void end();
            void nextFrame(size_t = 1);
            void prevFrame(size_t = 1);
            void setInOutPoints(const AV::IO::InOutPoints&);
            void setInPoint();
            void setOutPoint();
            void resetInPoint();
            void resetOutPoint();

            ///@}

            //! \name Audio
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<bool> > observeAudioEnabled() const;
            std::shared_ptr<Core::Observer::IValueSubject<float> > observeVolume() const;
            std::shared_ptr<Core::Observer::IValueSubject<bool> > observeMute() const;

            void setVolume(float);
            void setMute(bool);

            ///@}

            //! \name I/O
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<size_t> > observeThreadCount() const;

            void setThreadCount(size_t);

            ///@}

            //! \name Cache
            ///@{

            bool hasCache() const;
            size_t getCacheMaxByteCount() const;
            size_t getCacheByteCount() const;

            std::shared_ptr<Core::Observer::IValueSubject<Math::Frame::Sequence> > observeCacheSequence() const;
            std::shared_ptr<Core::Observer::IValueSubject<Math::Frame::Sequence> > observeCachedFrames() const;

            void setCacheEnabled(bool);
            void setCacheMaxByteCount(size_t);

            ///@}

            //! \name Annotations
            ///@{
            
            std::shared_ptr<Core::Observer::IListSubject<std::shared_ptr<AnnotatePrimitive> > > observeAnnotations() const;

            void addAnnotation(const std::shared_ptr<AnnotatePrimitive>&);
            void removeAnnotation(const std::shared_ptr<AnnotatePrimitive>&);
            void clearAnnotations();

            ///@}

            //! \name Undo
            ///@{

            void pushCommand(const std::shared_ptr<Core::Command::ICommand>&);

            std::shared_ptr<Core::Observer::IValueSubject<bool> > observeHasUndo() const;
            std::shared_ptr<Core::Observer::IValueSubject<bool> > observeHasRedo() const;
            void undo();
            void redo();

            ///@}

            //! \name Debugging
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<size_t> > observeVideoQueueMax() const;
            std::shared_ptr<Core::Observer::IValueSubject<size_t> > observeAudioQueueMax() const;
            std::shared_ptr<Core::Observer::IValueSubject<size_t> > observeVideoQueueCount() const;
            std::shared_ptr<Core::Observer::IValueSubject<size_t> > observeAudioQueueCount() const;

            ///@}

        private:
            bool _hasAudio() const;
            bool _isAudioEnabled() const;
            bool _hasAudioSyncPlayback() const;
            void _open();
            void _setSpeed(const Math::Rational&);
            void _setCurrentFrame(Math::Frame::Index);
            void _seek(Math::Frame::Index);
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

