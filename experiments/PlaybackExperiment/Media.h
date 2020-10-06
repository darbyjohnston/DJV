// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include "Enum.h"
#include "IO.h"

#include <djvUI/Window.h>

#include <djvImage/Image.h>

#include <djvSystem/FileInfo.h>
#include <djvSystem/Timer.h>

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

    std::shared_ptr<djv::Core::Observer::IValueSubject<IOInfo> > observeInfo() const;
    std::shared_ptr<djv::Core::Observer::IValueSubject<std::shared_ptr<djv::Image::Image> > > observeCurrentImage() const;
    std::shared_ptr<djv::Core::Observer::IValueSubject<size_t> > observeVideoQueueSize() const;
    std::shared_ptr<djv::Core::Observer::IValueSubject<size_t> > observeAudioQueueSize() const;

    std::shared_ptr<djv::Core::Observer::IValueSubject<Playback> > observePlayback() const;
    std::shared_ptr<djv::Core::Observer::IValueSubject<djv::Math::Frame::Index> > observeCurrentFrame() const;

    void setPlayback(Playback);
    void seek(djv::Math::Frame::Index);

private:
    void _tick();
    void _videoTick();
    void _audioTick();

    std::weak_ptr<djv::System::Context> _context;
    djv::System::File::Info _fileInfo;
    std::shared_ptr<IIO> _read;
    std::shared_ptr<djv::Core::Observer::ValueSubject<IOInfo> > _info;
    std::shared_ptr<djv::Core::Observer::ValueSubject<std::shared_ptr<djv::Image::Image> > > _image;
    std::shared_ptr<djv::Core::Observer::ValueSubject<size_t> > _videoQueueSize;
    std::shared_ptr<djv::Core::Observer::ValueSubject<size_t> > _audioQueueSize;
    std::shared_ptr<djv::Core::Observer::ValueSubject<Playback> > _playback;
    std::shared_ptr<djv::Core::Observer::ValueSubject<djv::Math::Frame::Index> > _currentFrame;
    double _playbackTime = 0.0;
    djv::Math::Frame::Index _playbackStartFrame = 0;
    std::chrono::steady_clock::time_point _playbackStartTime;
    std::shared_ptr<djv::System::Timer> _timer;
};
