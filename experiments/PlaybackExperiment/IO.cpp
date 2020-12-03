// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include "IO.h"

#include "FFmpeg.h"

#include <djvSystem/Context.h>
#include <djvSystem/LogSystem.h>

using namespace djv;

namespace IO
{
    FrameInfo::FrameInfo()
    {}
    
    FrameInfo::FrameInfo(
        Timestamp                timestamp,
        djv::Math::Frame::Number frame,
        int32_t                  timecode) :
        timestamp(timestamp),
        frame(frame),
        timecode(timecode)
    {}

    bool FrameInfo::operator == (const FrameInfo & other) const
    {
        return timestamp == other.timestamp;
    }

    bool FrameInfo::operator != (const FrameInfo& other) const
    {
        return !(timestamp == other.timestamp);
    }

    bool FrameInfo::operator < (const FrameInfo& other) const
    {
        return timestamp < other.timestamp;
    }

    VideoFrame::VideoFrame()
    {}

    bool VideoFrame::operator == (const VideoFrame& other) const
    {
        return info == other.info &&
            data == other.data &&
            seekFrame == other.seekFrame;
    }

    AudioFrame::AudioFrame()
    {}

    bool AudioFrame::operator == (const AudioFrame& other) const
    {
        return info == other.info &&
            data == other.data &&
            seekFrame == other.seekFrame;
    }

    Info::Info()
    {}

    std::shared_ptr<Info> Info::create()
    {
        return std::shared_ptr<Info>(new Info);
    }

    bool Info::operator == (const Info& other) const
    {
        return fileInfo == other.fileInfo &&
            video == other.video &&
            videoSpeed == other.videoSpeed &&
            videoFrameInfo == other.videoFrameInfo &&
            audio == other.audio &&
            audioFrameInfo == other.audioFrameInfo;
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

    void IRead::_init(
        const System::File::Info& fileInfo,
        const std::shared_ptr<System::LogSystem>& logSystem)
    {
        IIO::_init(fileInfo, logSystem);
    }

    IRead::IRead()
    {}

    IRead::~IRead()
    {}

    void IRead::setPlaybackDirection(PlaybackDirection value)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _playbackDirection = value;
    }

    IPlugin::IPlugin(const std::shared_ptr<System::LogSystem>& logSystem) :
        _logSystem(logSystem)
    {}

    IPlugin::~IPlugin()
    {}

    struct IOSystem::Private
    {
        std::vector<std::shared_ptr<IPlugin> > plugins;
    };

    void IOSystem::_init(const std::shared_ptr<System::Context>& context)
    {
        ISystem::_init("IO::IOSystem", context);
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

    std::shared_ptr<IRead> IOSystem::read(const System::File::Info& fileInfo)
    {
        DJV_PRIVATE_PTR();
        std::shared_ptr<IRead> out;
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

} // namespace IO
