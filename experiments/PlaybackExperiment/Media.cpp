// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include "Media.h"

#include <djvSystem/Context.h>
#include <djvSystem/LogSystem.h>
#include <djvSystem/TimerFunc.h>

using namespace djv;

void Media::_init(
    const djv::System::File::Info& fileInfo,
    const std::shared_ptr<System::Context>& context)
{
    _context = context;
    _fileInfo = fileInfo;
    _info = Core::Observer::ValueSubject<IOInfo>::create();
    _image = Core::Observer::ValueSubject<std::shared_ptr<Image::Image> >::create();
    _videoQueueSize = Core::Observer::ValueSubject<size_t>::create();
    _audioQueueSize = Core::Observer::ValueSubject<size_t>::create();
    _playback = Core::Observer::ValueSubject<Playback>::create();
    _currentFrame = Core::Observer::ValueSubject<Math::Frame::Index>::create();

    auto ioSystem = context->getSystemT<IOSystem>();
    _read = ioSystem->read(fileInfo);
    if (_read)
    {
        _info->setIfChanged(_read->getInfo().get());
    }

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

std::shared_ptr<Core::Observer::IValueSubject<size_t> > Media::observeVideoQueueSize() const
{
    return _videoQueueSize;
}

std::shared_ptr<Core::Observer::IValueSubject<size_t> > Media::observeAudioQueueSize() const
{
    return _audioQueueSize;
}

std::shared_ptr<Core::Observer::IValueSubject<Playback> > Media::observePlayback() const
{
    return _playback;
}

std::shared_ptr<Core::Observer::IValueSubject<Math::Frame::Index> > Media::observeCurrentFrame() const
{
    return _currentFrame;
}

void Media::setPlayback(Playback value)
{
    if (_read && _playback->setIfChanged(value))
    {
        _read->setPlaybackDirection(Playback::Forward == value ? PlaybackDirection::Forward : PlaybackDirection::Reverse);
        switch (value)
        {
        case Playback::Forward:
        case Playback::Reverse:
            _playbackStartFrame = _currentFrame->get();
            _playbackStartTime = std::chrono::steady_clock::now();
            break;
        default: break;
        }
    }
}

void Media::seek(djv::Math::Frame::Index value)
{
    if (_read)
    {
        _read->seek(value);
        _playbackStartFrame = value;
        _playbackTime = 0.0;
        switch (_playback->get())
        {
        case Playback::Forward:
        case Playback::Reverse:
            _playbackStartTime = std::chrono::steady_clock::now();
            break;
        default: break;
        }
    }
}

void Media::_tick()
{
    if (_read)
    {
        switch (_playback->get())
        {
        case Playback::Forward:
        case Playback::Reverse:
        {
            const auto now = std::chrono::steady_clock::now();
            _playbackTime = std::chrono::duration<double>(now - _playbackStartTime).count();
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
    const int64_t currentFrame = _playbackStartFrame + static_cast<int64_t>(
        _playbackTime * static_cast<double>(_info->get().videoSpeed.toFloat()));

    VideoFrame frame;
    bool valid = false;
    size_t queueSize = 0;
    bool finished = false;
    {
        std::lock_guard<std::mutex> lock(_read->getMutex());
        auto& queue = _read->getVideoQueue();
        finished = queue.isFinished() && queue.isEmpty();

        //while (!queue.isEmpty() && queue.getFrame().frame <= currentFrame)
        //{
        //    frame = queue.popFrame();
        //    valid = true;
        //}
        if (!queue.isEmpty() && queue.getFrame().frame <= currentFrame)
        {
            frame = queue.popFrame();
            valid = true;
        }
        queueSize = queue.getCount();
    }

    if (valid)
    {
        _image->setIfChanged(frame.image);
        _currentFrame->setIfChanged(frame.frame);
        _videoQueueSize->setAlways(queueSize);
    }
}

void Media::_audioTick()
{
    const int64_t samples = static_cast<int64_t>(
        _playbackTime * static_cast<double>(_info->get().audio.sampleRate));

    bool valid = false;
    size_t queueSize = 0;
    bool finished = false;
    {
        std::lock_guard<std::mutex> lock(_read->getMutex());
        auto& queue = _read->getAudioQueue();
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
        _audioQueueSize->setAlways(queueSize);
    }
}
