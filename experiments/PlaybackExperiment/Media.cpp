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
    _videoQueueSize = Core::Observer::ValueSubject<size_t>::create();
    _audioQueueSize = Core::Observer::ValueSubject<size_t>::create();
    _playback = Core::Observer::ValueSubject<Playback>::create();
    _currentTime = Core::Observer::ValueSubject<double>::create();

    auto ioSystem = context->getSystemT<IOSystem>();
    _read = ioSystem->read(fileInfo);
    _info->setIfChanged(_read->getInfo().get());

    _timer = System::Timer::create(context);
    _timer->setRepeating(true);
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

std::shared_ptr<Core::Observer::IValueSubject<double> > Media::observeCurrentTime() const
{
    return _currentTime;
}

void Media::setPlayback(Playback value)
{
    if (_playback->setIfChanged(value))
    {
        _read->setPlaybackDirection(Playback::Forward == value ? PlaybackDirection::Forward : PlaybackDirection::Reverse);
        switch (value)
        {
        case Playback::Stop:
            _timer->stop();
            break;
        case Playback::Forward:
        case Playback::Reverse:
        {
            _playbackStartTime = std::chrono::steady_clock::now();
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
            break;
        }
        default: break;
        }
    }
}

void Media::_tick()
{
    const auto now = std::chrono::steady_clock::now();
    const std::chrono::duration<double> delta = now - _playbackStartTime;
    _currentTime->setIfChanged(delta.count());

    _videoTick();
    _audioTick();
}

void Media::_videoTick()
{
    const int64_t frames = static_cast<int64_t>(
        _currentTime->get() * static_cast<double>(_info->get().videoSpeed.toFloat()));

    VideoFrame videoFrame;
    bool videoFrameValid = false;
    size_t videoQueueSize = 0;
    bool finished = false;
    {
        std::lock_guard<std::mutex> lock(_read->getMutex());
        auto& videoQueue = _read->getVideoQueue();
        finished = videoQueue.isFinished() && videoQueue.isEmpty();

        while (!videoQueue.isEmpty() && videoQueue.getFrame().frame < frames)
        {
            videoFrame = videoQueue.popFrame();
            videoFrameValid = true;
        }
        videoQueueSize = videoQueue.getCount();
    }

    if (videoFrameValid)
    {
        _image->setIfChanged(videoFrame.image);
    }

    _videoQueueSize->setIfChanged(videoQueueSize);

    if (finished)
    {
        _timer->stop();
    }
}

void Media::_audioTick()
{
    const int64_t samples = static_cast<int64_t>(
        _currentTime->get() * static_cast<double>(_info->get().audio.sampleRate));

    size_t audioQueueSize = 0;
    bool finished = false;
    {
        std::lock_guard<std::mutex> lock(_read->getMutex());
        auto& audioQueue = _read->getAudioQueue();
        finished = audioQueue.isFinished() && audioQueue.isEmpty();

        while (!audioQueue.isEmpty() && audioQueue.getFrame().sample < samples)
        {
            audioQueue.popFrame();
        }
        audioQueueSize = audioQueue.getCount();
    }

    _audioQueueSize->setIfChanged(audioQueueSize);

    if (finished)
    {
        _timer->stop();
    }
}
