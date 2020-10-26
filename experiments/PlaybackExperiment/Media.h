// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include "Enum.h"
#include "IO.h"

#include <djvAV/Time.h>

#include <djvImage/Data.h>

#include <djvSystem/FileInfo.h>
#include <djvSystem/ISystem.h>

#include <djvCore/ValueObserver.h>

class Media : public std::enable_shared_from_this<Media>
{
    DJV_NON_COPYABLE(Media);

protected:
    void _init(
        const djv::System::File::Info&,
        const std::shared_ptr<djv::System::Context>&);
    Media();

public:
    ~Media();

    static std::shared_ptr<Media> create(
        const djv::System::File::Info&,
        const std::shared_ptr<djv::System::Context>&);

    const djv::System::File::Info& getFileInfo() const;
    const IOInfo& getIOInfo() const;

    std::shared_ptr<djv::Core::Observer::IValueSubject<std::shared_ptr<djv::Image::Data> > > observeImage() const;
    std::shared_ptr<djv::Core::Observer::IValueSubject<size_t> > observeVideoQueueSize() const;
    std::shared_ptr<djv::Core::Observer::IValueSubject<size_t> > observeAudioQueueSize() const;

    std::shared_ptr<djv::Core::Observer::IValueSubject<Playback> > observePlayback() const;
    std::shared_ptr<djv::Core::Observer::IValueSubject<Timestamp> > observeTimestamp() const;
    std::shared_ptr<djv::Core::Observer::IValueSubject<float> > observeFPS() const;
    std::shared_ptr<djv::Core::Observer::IValueSubject<float> > observeAudioVolume() const;
    std::shared_ptr<djv::Core::Observer::IValueSubject<bool> > observeAudioMute() const;

    void setPlayback(Playback);
    void seek(Timestamp);
    void frame(Frame);

private:
    void _tick();
    void _tickNoAudio();
    void _tickAudio();

    bool _hasAudioPlayback() const;
    void _startAudio();
    void _stopAudio();
    int _audioCallback(
        void* outputBuffer,
        void* inputBuffer,
        unsigned int nFrames,
        double streamTime,
        RtAudioStreamStatus status);
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

    void _error(const std::string& message, const std::string& what);

    DJV_PRIVATE();
};
