// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include "Enum.h"
#include "IO.h"

#include <djvUI/Window.h>

#include <djvAV/Time.h>

#include <djvImage/Image.h>

#include <djvSystem/FileInfo.h>

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
    std::shared_ptr<djv::Core::Observer::IValueSubject<double> > observeCurrentTime() const;

    void setPlayback(Playback);

    void seek(double);

private:
    void _tick();
    void _videoTick();
    void _audioTick();

    DJV_PRIVATE();
};
