// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include "Media.h"

#include <djvSystem/Context.h>
#include <djvSystem/LogSystem.h>
#include <djvSystem/Timer.h>
#include <djvSystem/TimerFunc.h>

using namespace djv;

struct Media::Private
{
    std::weak_ptr<System::Context> context;
    System::File::Info fileInfo;
    std::shared_ptr<IIO> read;
    std::shared_ptr<Core::Observer::ValueSubject<IOInfo> > info;
    std::shared_ptr<Core::Observer::ValueSubject<std::shared_ptr<Image::Image> > > image;
    std::shared_ptr<Core::Observer::ValueSubject<size_t> > videoQueueSize;
    std::shared_ptr<Core::Observer::ValueSubject<size_t> > audioQueueSize;
    std::shared_ptr<Core::Observer::ValueSubject<Playback> > playback;
    std::shared_ptr<Core::Observer::ValueSubject<bool> > playbackEveryFrame;
    std::shared_ptr<Core::Observer::ValueSubject<Math::Frame::Index> > currentFrame;
    double playbackTime = 0.0;
    Math::Frame::Index playbackStartFrame = 0;
    std::chrono::steady_clock::time_point playbackStartTime;
    std::shared_ptr<System::Timer> timer;
};

void Media::_init(
    const System::File::Info& fileInfo,
    const std::shared_ptr<System::Context>& context)
{
    DJV_PRIVATE_PTR();
    p.context = context;
    p.fileInfo = fileInfo;
    p.info = Core::Observer::ValueSubject<IOInfo>::create();
    p.image = Core::Observer::ValueSubject<std::shared_ptr<Image::Image> >::create();
    p.videoQueueSize = Core::Observer::ValueSubject<size_t>::create();
    p.audioQueueSize = Core::Observer::ValueSubject<size_t>::create();
    p.playback = Core::Observer::ValueSubject<Playback>::create();
    p.playbackEveryFrame = Core::Observer::ValueSubject<bool>::create(false);
    p.currentFrame = Core::Observer::ValueSubject<Math::Frame::Index>::create();

    auto ioSystem = context->getSystemT<IOSystem>();
    p.read = ioSystem->read(fileInfo);
    if (p.read)
    {
        p.info->setIfChanged(p.read->getInfo().get());
    }

    p.timer = System::Timer::create(context);
    p.timer->setRepeating(true);
    auto weak = std::weak_ptr<Media>(shared_from_this());
    p.timer->start(
        System::getTimerDuration(System::TimerValue::Fast),
        [weak](const std::chrono::steady_clock::time_point&, const Core::Time::Duration&)
        {
            if (auto media = weak.lock())
            {
                media->_tick();
            }
        });
}

Media::Media() :
    _p(new Private)
{}

Media::~Media()
{}

std::shared_ptr<Media> Media::create(
    const System::File::Info& fileInfo,
    const std::shared_ptr<System::Context>& context)
{
    auto out = std::shared_ptr<Media>(new Media);
    out->_init(fileInfo, context);
    return out;
}

const System::File::Info& Media::getFileInfo() const
{
    return _p->fileInfo;
}

std::shared_ptr<Core::Observer::IValueSubject<IOInfo> > Media::observeInfo() const
{
    return _p->info;
}

std::shared_ptr<Core::Observer::IValueSubject<std::shared_ptr<Image::Image> > > Media::observeCurrentImage() const
{
    return _p->image;
}

std::shared_ptr<Core::Observer::IValueSubject<size_t> > Media::observeVideoQueueSize() const
{
    return _p->videoQueueSize;
}

std::shared_ptr<Core::Observer::IValueSubject<size_t> > Media::observeAudioQueueSize() const
{
    return _p->audioQueueSize;
}

std::shared_ptr<Core::Observer::IValueSubject<Playback> > Media::observePlayback() const
{
    return _p->playback;
}

std::shared_ptr<Core::Observer::IValueSubject<bool> > Media::observePlaybackEveryFrame() const
{
    return _p->playbackEveryFrame;
}

std::shared_ptr<Core::Observer::IValueSubject<Math::Frame::Index> > Media::observeCurrentFrame() const
{
    return _p->currentFrame;
}

void Media::setPlayback(Playback value)
{
    DJV_PRIVATE_PTR();
    if (p.read && p.playback->setIfChanged(value))
    {
        p.read->setPlaybackDirection(Playback::Forward == value ? PlaybackDirection::Forward : PlaybackDirection::Reverse);
        switch (value)
        {
        case Playback::Forward:
        case Playback::Reverse:
            p.playbackStartFrame = p.currentFrame->get();
            p.playbackStartTime = std::chrono::steady_clock::now();
            break;
        default: break;
        }
    }
}

void Media::setPlaybackEveryFrame(bool value)
{
    _p->playbackEveryFrame->setIfChanged(value);
}

void Media::seek(Math::Frame::Index value)
{
    DJV_PRIVATE_PTR();
    if (p.read)
    {
        p.read->seek(value);
        p.playbackStartFrame = value;
        p.playbackTime = 0.0;
        switch (p.playback->get())
        {
        case Playback::Forward:
        case Playback::Reverse:
            p.playbackStartTime = std::chrono::steady_clock::now();
            break;
        default: break;
        }
    }
}

void Media::_tick()
{
    DJV_PRIVATE_PTR();
    if (p.read)
    {
        switch (p.playback->get())
        {
        case Playback::Forward:
        case Playback::Reverse:
        {
            const auto now = std::chrono::steady_clock::now();
            p.playbackTime = std::chrono::duration<double>(now - p.playbackStartTime).count();
            break;
        }
        default: break;
        }
        _videoTick();
        _audioTick();
    }
}

void Media::_videoTick()
{
    DJV_PRIVATE_PTR();

    const int64_t currentFrame = p.playbackStartFrame + static_cast<int64_t>(
        p.playbackTime * static_cast<double>(p.info->get().videoSpeed.toFloat()));

    VideoFrame frame;
    bool valid = false;
    size_t queueSize = 0;
    bool finished = false;
    {
        std::lock_guard<std::mutex> lock(p.read->getMutex());
        auto& queue = p.read->getVideoQueue();
        finished = queue.isFinished() && queue.isEmpty();

        if (!p.playbackEveryFrame->get())
        {
            while (!queue.isEmpty() && queue.getFrame().frame <= currentFrame)
            {
                frame = queue.popFrame();
                valid = true;
            }
        }
        else
        {
            if (!queue.isEmpty() && queue.getFrame().frame <= currentFrame)
            {
                frame = queue.popFrame();
                valid = true;
            }
        }
        queueSize = queue.getCount();
    }

    if (valid)
    {
        //std::cout << "frame: " << frame.frame << std::endl;
        p.image->setIfChanged(frame.image);
        p.currentFrame->setIfChanged(frame.frame);
        p.videoQueueSize->setAlways(queueSize);
    }
}

void Media::_audioTick()
{
    DJV_PRIVATE_PTR();

    const int64_t samples = static_cast<int64_t>(
        p.playbackTime * static_cast<double>(p.info->get().audio.sampleRate));

    bool valid = false;
    size_t queueSize = 0;
    bool finished = false;
    {
        std::lock_guard<std::mutex> lock(p.read->getMutex());
        auto& queue = p.read->getAudioQueue();
        finished = queue.isFinished() && queue.isEmpty();

        while (!queue.isEmpty() && queue.getFrame().sample < samples)
        {
            queue.popFrame();
            valid = true;
        }
        queueSize = queue.getCount();
    }

    if (valid)
    {
        p.audioQueueSize->setAlways(queueSize);
    }
}
