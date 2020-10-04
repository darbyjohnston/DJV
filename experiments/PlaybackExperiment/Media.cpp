// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include "Media.h"

#include <djvSystem/Context.h>
#include <djvSystem/TimerFunc.h>

using namespace djv;

void Media::_init(
    const djv::System::File::Info& fileInfo,
    const std::shared_ptr<System::Context>& context)
{
    _fileInfo = fileInfo;
    _info = Core::Observer::ValueSubject<IOInfo>::create();
    _image = Core::Observer::ValueSubject<std::shared_ptr<Image::Image> >::create();

    auto ioSystem = context->getSystemT<IOSystem>();
    _read = ioSystem->read(fileInfo);
    _info->setIfChanged(_read->getInfo().get());

    _timer = System::Timer::create(context);
    _timer->setRepeating(true);
    auto weak = std::weak_ptr<Media>(shared_from_this());
    _timer->start(
        System::getTimerDuration(System::TimerValue::Fast),
        [weak](const std::chrono::steady_clock::time_point&, const Core::Time::Duration&)
        {
            if (auto media = weak.lock())
            {
                media->_tick();
            }
        });
}

Media::Media()
{}

Media::~Media()
{}

std::shared_ptr<Media> Media::create(
    const djv::System::File::Info& fileInfo,
    const std::shared_ptr<System::Context>& context)
{
    auto out = std::shared_ptr<Media>(new Media);
    out->_init(fileInfo, context);
    return out;
}

const System::File::Info& Media::getFileInfo() const
{
    return _fileInfo;
}

std::shared_ptr<Core::Observer::IValueSubject<IOInfo> > Media::observeInfo() const
{
    return _info;
}

std::shared_ptr<Core::Observer::IValueSubject<std::shared_ptr<Image::Image> > > Media::observeCurrentImage() const
{
    return _image;
}

void Media::_tick()
{
    VideoFrame videoFrame;
    bool videoFrameValid = false;
    {
        std::lock_guard<std::mutex> lock(_read->getMutex());
        auto& videoQueue = _read->getVideoQueue();
        if (!videoQueue.isEmpty())
        {
            videoFrame = videoQueue.popFrame();
            videoFrameValid = true;
        }
    }
    if (videoFrameValid)
    {
        _image->setIfChanged(videoFrame.image);
    }
}
