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

VideoFrame::VideoFrame(Math::Frame::Number frame, const std::shared_ptr<Image::Image>& image) :
    frame(frame),
    image(image)
{}

bool VideoFrame::operator == (const VideoFrame& other) const
{
    return frame == other.frame &&
        image == other.image;
}

AudioFrame::AudioFrame()
{}

AudioFrame::AudioFrame(const std::shared_ptr<Audio::Data>& audio) :
    audio(audio)
{}

bool AudioFrame::operator == (const AudioFrame& other) const
{
    return audio == other.audio;
}

void IIO::_init(
    const System::File::Info& fileInfo,
    const std::shared_ptr<djv::System::LogSystem>& logSystem)
{
    _logSystem = logSystem;
    _fileInfo = fileInfo;
}

IIO::IIO()
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

IIOPlugin::IIOPlugin(const std::shared_ptr<djv::System::LogSystem>& logSystem) :
    _logSystem(logSystem)
{}

IIOPlugin::~IIOPlugin()
{}

void IOSystem::_init(const std::shared_ptr<System::Context>& context)
{
    ISystem::_init("IOSystem", context);

    auto logSystem = context->getSystemT<System::LogSystem>();
    _plugins.push_back(FFmpegPlugin::create(logSystem));
}

IOSystem::IOSystem()
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
    std::shared_ptr<IIO> out;
    for (const auto& i : _plugins)
    {
        if (i->canRead(fileInfo))
        {
            out = i->read(fileInfo);
            break;
        }
    }
    return out;
}
