//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

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

                    picojson::value getOptions() const override;
                    void setOptions(const picojson::value&) override;

                    std::shared_ptr<IRead> read(const Core::FileSystem::FileInfo&, const ReadOptions&) const override;

                private:
                    DJV_PRIVATE();
                };

            } // namespace FFmpeg
        } // namespace IO
    } // namespace AV

    picojson::value toJSON(const AV::IO::FFmpeg::Options&);

    //! Throws:
    //! - std::exception
    void fromJSON(const picojson::value&, AV::IO::FFmpeg::Options&);

} // namespace djv

#include <djvAV/FFmpegInline.h>

