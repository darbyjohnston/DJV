// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include "Enum.h"

#include <djvSystem/ISystem.h>

#include <djvImage/Image.h>

#include <djvAudio/AudioData.h>

#include <djvMath/FrameNumber.h>
#include <djvMath/Rational.h>

#include <djvSystem/FileInfo.h>

#include <future>
#include <mutex>
#include <queue>

class IOInfo
{
public:
    IOInfo();

    djv::System::File::Info       fileInfo;
    djv::Math::Rational           videoSpeed;
    djv::Math::Frame::Sequence    videoSequence;
    std::vector<djv::Image::Info> video;
    djv::Audio::Info              audio;

    bool operator == (const IOInfo&) const;
};

class VideoFrame
{
public:
    VideoFrame();
    VideoFrame(djv::Math::Frame::Index, const std::shared_ptr<djv::Image::Image>&);

    djv::Math::Frame::Index            frame = 0;
    std::shared_ptr<djv::Image::Image> image;

    bool operator == (const VideoFrame&) const;
};

class AudioFrame
{
public:
    AudioFrame();
    explicit AudioFrame(int64_t, const std::shared_ptr<djv::Audio::Data>&);

    int64_t                           sample = 0;
    std::shared_ptr<djv::Audio::Data> audio;

    bool operator == (const AudioFrame&) const;
};

template<typename T>
class IOQueue
{
public:
    IOQueue(size_t max);

    size_t getMax() const;
    void setMax(size_t);

    bool isEmpty() const;
    size_t getCount() const;
    T getFrame() const;

    void addFrame(const T&);
    T popFrame();
    void clearFrames();

    bool isFinished() const;
    void setFinished(bool);

private:
    size_t _max = 0;
    std::queue<T> _queue;
    bool _finished = false;
};

typedef IOQueue<VideoFrame> VideoQueue;
typedef IOQueue<AudioFrame> AudioQueue;

class IIO : public std::enable_shared_from_this<IIO>
{
    DJV_NON_COPYABLE(IIO);

protected:
    void _init(
        const djv::System::File::Info&,
        const std::shared_ptr<djv::System::LogSystem>&);
    IIO();

public:
    virtual ~IIO() = 0;

    virtual std::future<IOInfo> getInfo() = 0;

    std::mutex& getMutex();
    VideoQueue& getVideoQueue();
    AudioQueue& getAudioQueue();

    void setPlaybackDirection(PlaybackDirection);
    void seek(int64_t);

protected:
    std::shared_ptr<djv::System::LogSystem> _logSystem;
    djv::System::File::Info _fileInfo;
    std::mutex _mutex;
    VideoQueue _videoQueue;
    AudioQueue _audioQueue;
    PlaybackDirection _playbackDirection = PlaybackDirection::Forward;
    int64_t _seek = -1;
};

class IIOPlugin : public std::enable_shared_from_this<IIOPlugin>
{
    DJV_NON_COPYABLE(IIOPlugin);

public:
    IIOPlugin(const std::shared_ptr<djv::System::LogSystem>&);
    virtual ~IIOPlugin() = 0;

    virtual bool canRead(const djv::System::File::Info&) = 0;
    virtual std::shared_ptr<IIO> read(const djv::System::File::Info&) = 0;

protected:
    std::shared_ptr<djv::System::LogSystem> _logSystem;
};

class IOSystem : public djv::System::ISystem
{
    DJV_NON_COPYABLE(IOSystem);

protected:
    void _init(const std::shared_ptr<djv::System::Context>&);
    IOSystem();

public:
    ~IOSystem() override;

    static std::shared_ptr<IOSystem> create(const std::shared_ptr<djv::System::Context>&);

    std::shared_ptr<IIO> read(const djv::System::File::Info&);

private:
    std::vector<std::shared_ptr<IIOPlugin> > _plugins;
};

#include "IOInline.h"
