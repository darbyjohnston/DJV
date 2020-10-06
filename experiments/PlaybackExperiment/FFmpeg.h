// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include "IO.h"

extern "C"
{
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#include <atomic>
#include <thread>

class FFmpegIO : public IIO
{
    DJV_NON_COPYABLE(FFmpegIO);

protected:
    void _init(
        const djv::System::File::Info&,
        const std::shared_ptr<djv::System::LogSystem>&);
    FFmpegIO();

public:
    ~FFmpegIO() override;

    static std::shared_ptr<FFmpegIO> create(
        const djv::System::File::Info&,
        const std::shared_ptr<djv::System::LogSystem>&);

    std::future<IOInfo> getInfo() override;

    void seek(int64_t) override;

private:
    void _init(const djv::System::File::Info&);
    void _work();
    void _cleanup();

    struct DecodeVideo
    {
        AVPacket* packet = nullptr;
        int64_t seek = seekNone;
    };
    int _decodeVideo(const DecodeVideo&);

    struct DecodeAudio
    {
        AVPacket* packet = nullptr;
        int64_t seek = seekNone;
    };
    int _decodeAudio(const DecodeAudio&);

    IOInfo _info;
    std::promise<IOInfo> _infoPromise;
    std::thread _thread;
    std::atomic<bool> _running;
    std::condition_variable _queueCV;

    AVFormatContext* _avFormatContext = nullptr;
    int _avVideoStream = -1;
    int _avAudioStream = -1;
    std::map<int, AVCodecParameters*> _avCodecParameters;
    std::map<int, AVCodecContext*> _avCodecContext;
    AVFrame* _avFrame = nullptr;
    AVFrame* _avFrameRgb = nullptr;
    SwsContext* _swsContext = nullptr;

    int64_t _videoTime = timeInvalid;
    int64_t _audioTime = timeInvalid;
};

class FFmpegPlugin : public IIOPlugin
{
    DJV_NON_COPYABLE(FFmpegPlugin);

protected:
    FFmpegPlugin(const std::shared_ptr<djv::System::LogSystem>&);

public:
    ~FFmpegPlugin() override;

    static std::shared_ptr<FFmpegPlugin> create(const std::shared_ptr<djv::System::LogSystem>&);

    bool canRead(const djv::System::File::Info&) override;
    std::shared_ptr<IIO> read(const djv::System::File::Info&) override;
};
