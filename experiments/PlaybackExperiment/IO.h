// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include "Enum.h"

#include <djvSystem/ISystem.h>

#include <djvImage/Data.h>

#include <djvAudio/Data.h>

#include <djvMath/FrameNumber.h>
#include <djvMath/Rational.h>

#include <djvSystem/FileInfo.h>

#include <future>
#include <mutex>
#include <queue>

namespace IO
{
    const djv::Math::IntRational timebase(1, 705600000);
    typedef int64_t Timestamp;
    const Timestamp timestampInvalid = std::numeric_limits<int64_t>::min();
    const Timestamp seekNone = -1;

    enum class SeekFrame
    {
        False,
        True
    };

    class FrameInfo
    {
    public:
        FrameInfo();
        explicit FrameInfo(
            Timestamp timestamp,
            int32_t   timecode  = 0);

        Timestamp timestamp = 0;
        int32_t   timecode  = 0;

        bool operator == (const FrameInfo&) const;
        bool operator != (const FrameInfo&) const;
        bool operator < (const FrameInfo&) const;
    };

    class VideoFrame
    {
    public:
        VideoFrame();

        FrameInfo                         info;
        std::shared_ptr<djv::Image::Data> data;
        SeekFrame                         seekFrame = SeekFrame::False;

        bool operator == (const VideoFrame&) const;
    };

    class AudioFrame
    {
    public:
        AudioFrame();

        FrameInfo                         info;
        std::shared_ptr<djv::Audio::Data> data;
        SeekFrame                         seekFrame = SeekFrame::False;

        bool operator == (const AudioFrame&) const;
    };

    class Info : public std::enable_shared_from_this<Info>
    {
        DJV_NON_COPYABLE(Info);

    protected:
        Info();

    public:
        static std::shared_ptr<Info> create();

        djv::System::File::Info       fileInfo;
        std::vector<djv::Image::Info> video;
        djv::Math::IntRational        videoSpeed;
        int64_t                       videoDuration = 0;
        djv::Audio::Info              audio;
        int64_t                       audioDuration = 0;

        bool operator == (const Info&) const;
    };

    template<typename T>
    class Queue
    {
    public:
        Queue(size_t max);

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

    typedef Queue<VideoFrame> VideoQueue;
    typedef Queue<AudioFrame> AudioQueue;

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

        std::mutex& getMutex();
        VideoQueue& getVideoQueue();
        AudioQueue& getAudioQueue();

    protected:
        std::shared_ptr<djv::System::LogSystem> _logSystem;
        djv::System::File::Info _fileInfo;
        std::mutex _mutex;
        VideoQueue _videoQueue;
        AudioQueue _audioQueue;
    };

    class IRead : public IIO
    {
        DJV_NON_COPYABLE(IRead);

    protected:
        void _init(
            const djv::System::File::Info&,
            const std::shared_ptr<djv::System::LogSystem>&);
        IRead();

    public:
        virtual ~IRead() = 0;

        virtual std::future<std::shared_ptr<Info> > getInfo() = 0;

        void setPlaybackDirection(PlaybackDirection);
        virtual void seek(Timestamp) = 0;

    protected:
        PlaybackDirection _playbackDirection = PlaybackDirection::Forward;
        Timestamp _seekTimestamp = seekNone;
    };

    class IPlugin : public std::enable_shared_from_this<IPlugin>
    {
        DJV_NON_COPYABLE(IPlugin);

    public:
        IPlugin(const std::shared_ptr<djv::System::LogSystem>&);
        virtual ~IPlugin() = 0;

        virtual bool canRead(const djv::System::File::Info&) = 0;
        virtual std::shared_ptr<IRead> read(const djv::System::File::Info&) = 0;

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

        std::shared_ptr<IRead> read(const djv::System::File::Info&);

    private:
        DJV_PRIVATE();
    };

} // namespace IO

#include "IOInline.h"
