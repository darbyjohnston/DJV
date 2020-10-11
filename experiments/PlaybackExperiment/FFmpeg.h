// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include "IO.h"

struct AVPacket;

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

    void seek(double) override;

private:
    void _init(const djv::System::File::Info&);
    void _work();
    void _cleanup();

    enum class Decode
    {
        ReadFrame,
        SeekSkip,
        SeekFrame
    };

    int _decodeVideo(AVPacket*, Decode);
    int _decodeAudio(AVPacket*, Decode);

    DJV_PRIVATE();
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
