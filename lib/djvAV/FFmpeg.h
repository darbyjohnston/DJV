// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/IOPlugin.h>

#if defined(DJV_PLATFORM_LINUX)
#define __STDC_CONSTANT_MACROS
#endif // DJV_PLATFORM_LINUX

extern "C"
{
#include <libavcodec/avcodec.h>

} // extern "C"

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            //! This namespace provides FFmpeg image and audio I/O.
            //!
            //! References:
            //! - https://ffmpeg.org
            namespace FFmpeg
            {
                static const std::string pluginName = "FFmpeg";
                static const std::set<std::string> fileExtensions =
                {
                    ".avi",
                    ".dv",
                    ".gif",
                    ".flv",
                    ".m2v",
                    ".mkv",
                    ".mov",
                    ".mpg",
                    ".mpeg",
                    ".mp3",
                    ".mp4",
                    ".m4v",
                    ".mxf",
                    ".ps",
                    ".ts",
                    ".vob",
                    ".wav",
                    ".webp"
                };

                //! This struct provides the FFmpeg file I/O optioms.
                struct Options
                {
                    size_t threadCount = 4;
                    
                    bool operator == (const Options&) const;
                };

                //! This class provides the FFmpeg file reader.
                class Read : public IRead
                {
                    DJV_NON_COPYABLE(Read);

                protected:
                    void _init(
                        const System::File::Info&,
                        const ReadOptions&,
                        const Options&,
                        const std::shared_ptr<System::TextSystem>&,
                        const std::shared_ptr<System::ResourceSystem>&,
                        const std::shared_ptr<System::LogSystem>&);
                    Read();

                public:
                    ~Read() override;

                    static std::shared_ptr<Read> create(
                        const System::File::Info&,
                        const ReadOptions&,
                        const Options&,
                        const std::shared_ptr<System::TextSystem>&,
                        const std::shared_ptr<System::ResourceSystem>&,
                        const std::shared_ptr<System::LogSystem>&);

                    bool isRunning() const override;

                    std::future<Info> getInfo() override;

                    void seek(int64_t, Direction) override;

                private:
                    struct DecodeVideo
                    {
                        AVPacket*           packet       = nullptr;
                        Math::Frame::Number seek         = -1;
                        bool                cacheEnabled = false;
                    };
                    int _decodeVideo(const DecodeVideo&, Math::Frame::Number&);

                    struct DecodeAudio
                    {
                        AVPacket*           packet = nullptr;
                        Math::Frame::Number seek   = -1;
                    };
                    int _decodeAudio(const DecodeAudio&, Math::Frame::Number&);

                    DJV_PRIVATE();
                };

                //! This class provides the FFmpeg file I/O plugin.
                class Plugin : public IPlugin
                {
                    DJV_NON_COPYABLE(Plugin);

                protected:
                    void _init(const std::shared_ptr<System::Context>&);

                    Plugin();

                public:
                    static std::shared_ptr<Plugin> create(const std::shared_ptr<System::Context>&);

                    rapidjson::Value getOptions(rapidjson::Document::AllocatorType&) const override;
                    void setOptions(const rapidjson::Value&) override;

                    std::shared_ptr<IRead> read(const System::File::Info&, const ReadOptions&) const override;

                private:
                    DJV_PRIVATE();
                };

            } // namespace FFmpeg
        } // namespace IO
    } // namespace AV
} // namespace djv
