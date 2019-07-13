//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvAV/FFmpeg.h>

#include <djvCore/LogSystem.h>
#include <djvCore/String.h>

extern "C"
{
#include <libavformat/avformat.h>
}

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace FFmpeg
            {
                Audio::Type toAudioType(AVSampleFormat value)
                {
                    Audio::Type out = Audio::Type::None;
                    switch (value)
                    {
                    case AV_SAMPLE_FMT_U8:   out = Audio::Type::U8;  break;
                    case AV_SAMPLE_FMT_S16:  out = Audio::Type::S16; break;
                    case AV_SAMPLE_FMT_S32:  out = Audio::Type::S32; break;
                    case AV_SAMPLE_FMT_FLT:  out = Audio::Type::F32; break;
                    case AV_SAMPLE_FMT_U8P:  out = Audio::Type::U8;  break;
                    case AV_SAMPLE_FMT_S16P: out = Audio::Type::S16; break;
                    case AV_SAMPLE_FMT_S32P: out = Audio::Type::S32; break;
                    case AV_SAMPLE_FMT_FLTP: out = Audio::Type::F32; break;
                    default: break;
                    }
                    return out;
                }

                std::string toString(AVSampleFormat value)
                {
                    std::map<AVSampleFormat, std::string> data =
                    {
                        { AV_SAMPLE_FMT_NONE, DJV_TEXT("None") },
                        { AV_SAMPLE_FMT_U8, DJV_TEXT("U8") },
                        { AV_SAMPLE_FMT_S16, DJV_TEXT("S16") },
                        { AV_SAMPLE_FMT_S32, DJV_TEXT("S32") },
                        { AV_SAMPLE_FMT_FLT, DJV_TEXT("Float") },
                        { AV_SAMPLE_FMT_DBL, DJV_TEXT("Double") },
                        { AV_SAMPLE_FMT_U8P, DJV_TEXT("U8 Planar") },
                        { AV_SAMPLE_FMT_S16P, DJV_TEXT("S16 Planar") },
                        { AV_SAMPLE_FMT_S32P, DJV_TEXT("S32 Planar") },
                        { AV_SAMPLE_FMT_FLTP, DJV_TEXT("Float Planar") },
                        { AV_SAMPLE_FMT_DBLP, DJV_TEXT("Double Planar") }
                    };
                    const auto i = data.find(value);
                    return i != data.end() ? i->second : DJV_TEXT("Unknown");
                }

                std::string getErrorString(int r)
                {
                    char buf[String::cStringLength];
                    av_strerror(r, buf, String::cStringLength);
                    return std::string(buf);
                }

                namespace
                {
                    std::weak_ptr<LogSystem> _logSystem;

                    void avLogCallback(void * ptr, int level, const char * fmt, va_list vl)
                    {
                        if (level > av_log_get_level())
                            return;
                        if (auto logSystem = _logSystem.lock())
                        {
                            char s[String::cStringLength] = "";
                            vsnprintf(s, String::cStringLength, fmt, vl);
                            logSystem->log("djv::AV::IO::FFmpeg::Plugin", s);
                        }
                    }

                } // namespace

                void Plugin::_init(
                    const std::shared_ptr<ResourceSystem>& resourceSystem,
                    const std::shared_ptr<LogSystem>& logSystem)
                {
                    IPlugin::_init(
                        pluginName,
                        DJV_TEXT("This plugin provides FFmpeg image and audio I/O."),
                        fileExtensions,
                        resourceSystem,
                        logSystem);
                        
                    _logSystem = logSystem;
                    av_log_set_level(AV_LOG_ERROR);
                    av_log_set_callback(avLogCallback);
               }

                Plugin::Plugin()
                {}

                std::shared_ptr<Plugin> Plugin::create(
                    const std::shared_ptr<ResourceSystem>& resourceSystem,
                    const std::shared_ptr<LogSystem>& logSystem)
                {
                    auto out = std::shared_ptr<Plugin>(new Plugin);
                    out->_init(resourceSystem, logSystem);
                    return out;
                }

                std::shared_ptr<IRead> Plugin::read(const FileSystem::FileInfo& fileInfo, const ReadOptions& options) const
                {
                    return Read::create(fileInfo, options, _resourceSystem, _logSystem);
                }

            } // namespace FFmpeg
        } // namespace IO
    } // namespace AV
} // namespace djv

