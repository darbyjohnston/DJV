// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include "IO.h"

#include "FFmpeg.h"

#include <djvSystem/Context.h>
#include <djvSystem/LogSystem.h>

using namespace djv;

IOInfo::IOInfo()
{}

bool IOInfo::operator == (const IOInfo& other) const
{
    return fileInfo == other.fileInfo &&
        videoSpeed == other.videoSpeed &&
        videoSequence == other.videoSequence &&
        video == other.video &&
        audio == other.audio;
}

VideoFrame::VideoFrame()
{}

VideoFrame::VideoFrame(
    Timestamp timestamp,
    const std::shared_ptr<Image::Image>& image) :
    timestamp(timestamp),
    image(image)
{}

bool VideoFrame::operator == (const VideoFrame& other) const
{
    return timestamp == other.timestamp &&
        image == other.image;
}

AudioFrame::AudioFrame()
{}

AudioFrame::AudioFrame(
    Timestamp timestamp,
    const std::shared_ptr<Audio::Data>& audio) :
    timestamp(timestamp),
    audio(audio)
{}

bool AudioFrame::operator == (const AudioFrame& other) const
{
    return
        timestamp == other.timestamp &&
        audio == other.audio;
}

void IIO::_init(
    const System::File::Info& fileInfo,
    const std::shared_ptr<System::LogSystem>& logSystem)
{
    _logSystem = logSystem;
    _fileInfo = fileInfo;
}

IIO::IIO() :
    _videoQueue(10),
    _audioQueue(10)
{}

IIO::~IIO()
{}

std::mutex& IIO::getMutex()
{
    return _mutex;
}

VideoQueue& IIO::getVideoQueue()
{
    return _videoQueue;
}

AudioQueue& IIO::getAudioQueue()
{
    return _audioQueue;
}

void IIO::setPlaybackDirection(PlaybackDirection value)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _playbackDirection = value;
}

IIOPlugin::IIOPlugin(const std::shared_ptr<System::LogSystem>& logSystem) :
    _logSystem(logSystem)
{}

IIOPlugin::~IIOPlugin()
{}

struct IOSystem::Private
{
    std::vector<std::shared_ptr<IIOPlugin> > plugins;
};

void IOSystem::_init(const std::shared_ptr<System::Context>& context)
{
    ISystem::_init("IOSystem", context);
    DJV_PRIVATE_PTR();

    auto logSystem = context->getSystemT<System::LogSystem>();
    p.plugins.push_back(FFmpegPlugin::create(logSystem));
}

IOSystem::IOSystem() :
    _p(new Private)
{}

IOSystem::~IOSystem()
{}

std::shared_ptr<IOSystem> IOSystem::create(const std::shared_ptr<System::Context>& context)
{
    auto out = std::shared_ptr<IOSystem>(new IOSystem);
    out->_init(context);
    return out;
}

std::shared_ptr<IIO> IOSystem::read(const System::File::Info& fileInfo)
{
    DJV_PRIVATE_PTR();
    std::shared_ptr<IIO> out;
    for (const auto& i : p.plugins)
    {
        if (i->canRead(fileInfo))
        {
            out = i->read(fileInfo);
            break;
        }
    }
    return out;
}
