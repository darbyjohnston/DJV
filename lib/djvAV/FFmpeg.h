// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/IO.h>

#include <djvCore/Frame.h>

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

                Audio::Type toAudioType(AVSampleFormat);
                std::string toString(AVSampleFormat);

                std::string getErrorString(int);

                //! This struct provides the FFmpeg file I/O optioms.
                struct Options
                {
                    size_t threadCount = 4;
                };

                //! This class provides the FFmpeg file reader.
                class Read : public IRead
                {
                    DJV_NON_COPYABLE(Read);

                protected:
                    void _init(
                        const Core::FileSystem::FileInfo&,
                        const ReadOptions&,
                        const Options&,
                        const std::shared_ptr<Core::TextSystem>&,
                        const std::shared_ptr<Core::ResourceSystem>&,
                        const std::shared_ptr<Core::LogSystem>&);
                    Read();

                public:
                    ~Read() override;

                    static std::shared_ptr<Read> create(
                        const Core::FileSystem::FileInfo&,
                        const ReadOptions&,
                        const Options&,
                        const std::shared_ptr<Core::TextSystem>&,
                        const std::shared_ptr<Core::ResourceSystem>&,
                        const std::shared_ptr<Core::LogSystem>&);

                    bool isRunning() const override;

                    std::future<Info> getInfo() override;

                    void seek(int64_t, Direction) override;

                private:
                    struct DecodeVideo
                    {
                        AVPacket*           packet       = nullptr;
                        Core::Frame::Number seek         = -1;
                        bool                cacheEnabled = false;
                    };
                    int _decodeVideo(const DecodeVideo&, Core::Frame::Number&);

                    struct DecodeAudio
                    {
                        AVPacket*           packet = nullptr;
                        Core::Frame::Number seek   = -1;
                    };
                    int _decodeAudio(const DecodeAudio&, Core::Frame::Number&);

                    DJV_PRIVATE();
                };

                //! This class provides the FFmpeg file I/O plugin.
                class Plugin : public IPlugin
                {
                    DJV_NON_COPYABLE(Plugin);

                protected:
                    void _init(const std::shared_ptr<Core::Context>&);

                    Plugin();

                public:
                    static std::shared_ptr<Plugin> create(const std::shared_ptr<Core::Context>&);

                    rapidjson::Value getOptions(rapidjson::Document::AllocatorType&) const override;
                    void setOptions(const rapidjson::Value&) override;

                    std::shared_ptr<IRead> read(const Core::FileSystem::FileInfo&, const ReadOptions&) const override;

                private:
                    DJV_PRIVATE();
                };

            } // namespace FFmpeg
        } // namespace IO
    } // namespace AV

    rapidjson::Value toJSON(const AV::IO::FFmpeg::Options&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, AV::IO::FFmpeg::Options&);

} // namespace djv
