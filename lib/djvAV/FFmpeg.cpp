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

#include <djvAV/FFmpeg.h>

#include <djvCore/Context.h>
#include <djvCore/FileSystem.h>
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
                    case AV_SAMPLE_FMT_S16:  out = Audio::Type::S16; break;
                    case AV_SAMPLE_FMT_S32:  out = Audio::Type::S32; break;
                    case AV_SAMPLE_FMT_FLT:  out = Audio::Type::F32; break;
                    case AV_SAMPLE_FMT_DBL:  out = Audio::Type::F64; break;
                    case AV_SAMPLE_FMT_S16P: out = Audio::Type::S16; break;
                    case AV_SAMPLE_FMT_S32P: out = Audio::Type::S32; break;
                    case AV_SAMPLE_FMT_FLTP: out = Audio::Type::F32; break;
                    case AV_SAMPLE_FMT_DBLP: out = Audio::Type::F64; break;
                    default: break;
                    }
                    return out;
                }

                std::string toString(AVSampleFormat value)
                {
                    std::map<AVSampleFormat, std::string> data =
                    {
                        { AV_SAMPLE_FMT_NONE, DJV_TEXT("av_sample_format_None") },
                        { AV_SAMPLE_FMT_S16, DJV_TEXT("av_sample_format_s16") },
                        { AV_SAMPLE_FMT_S32, DJV_TEXT("av_sample_format_s32") },
                        { AV_SAMPLE_FMT_FLT, DJV_TEXT("av_sample_format_float") },
                        { AV_SAMPLE_FMT_DBL, DJV_TEXT("av_sample_format_double") },
                        { AV_SAMPLE_FMT_S16P, DJV_TEXT("av_sample_format_s16_planar") },
                        { AV_SAMPLE_FMT_S32P, DJV_TEXT("av_sample_format_s32_planar") },
                        { AV_SAMPLE_FMT_FLTP, DJV_TEXT("av_sample_format_float_planar") },
                        { AV_SAMPLE_FMT_DBLP, DJV_TEXT("av_sample_format_double_planar") }
                    };
                    const auto i = data.find(value);
                    return i != data.end() ? i->second : DJV_TEXT("error_unknown");
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

                struct Plugin::Private
                {
                    Options options;
                };

                void Plugin::_init(const std::shared_ptr<Context>& context)
                {
                    IPlugin::_init(
                        pluginName,
                        DJV_TEXT("plugin_ffmpeg_io"),
                        fileExtensions,
                        context);
                        
                    _logSystem = context->getSystemT<LogSystem>();
                    av_log_set_level(AV_LOG_ERROR);
                    av_log_set_callback(avLogCallback);
               }

                Plugin::Plugin() :
                    _p(new Private)
                {}

                std::shared_ptr<Plugin> Plugin::create(const std::shared_ptr<Context>& context)
                {
                    auto out = std::shared_ptr<Plugin>(new Plugin);
                    out->_init(context);
                    return out;
                }

                picojson::value Plugin::getOptions() const
                {
                    DJV_PRIVATE_PTR();
                    return toJSON(p.options);
                }

                void Plugin::setOptions(const picojson::value& value)
                {
                    DJV_PRIVATE_PTR();
                    fromJSON(value, p.options);
                }

                std::shared_ptr<IRead> Plugin::read(const FileSystem::FileInfo& fileInfo, const ReadOptions& options) const
                {
                    DJV_PRIVATE_PTR();
                    return Read::create(fileInfo, options, p.options, _resourceSystem, _logSystem);
                }

            } // namespace FFmpeg
        } // namespace IO
    } // namespace AV

    picojson::value toJSON(const AV::IO::FFmpeg::Options& value)
    {
        picojson::value out(picojson::object_type, true);
        {
            out.get<picojson::object>()["ThreadCount"] = toJSON(value.threadCount);
        }
        return out;
    }

    void fromJSON(const picojson::value& value, AV::IO::FFmpeg::Options& out)
    {
        if (value.is<picojson::object>())
        {
            for (const auto& i : value.get<picojson::object>())
            {
                if ("ThreadCount" == i.first)
                {
                    fromJSON(i.second, out.threadCount);
                }
            }
        }
        else
        {
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

} // namespace djv

