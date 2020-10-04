// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

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

private:
    void _tick();

    djv::System::File::Info _fileInfo;
    std::shared_ptr<IIO> _read;
    std::shared_ptr<djv::Core::Observer::ValueSubject<IOInfo> > _info;
    std::shared_ptr<djv::Core::Observer::ValueSubject<std::shared_ptr<djv::Image::Image> > > _image;
    std::shared_ptr<djv::System::Timer> _timer;
};
