// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

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
                    //! \todo How can we translate this?
                    std::map<AVSampleFormat, std::string> data =
                    {
                        { AV_SAMPLE_FMT_NONE, DJV_TEXT("av_sample_format_none") },
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

                void extractAudio(
                    uint8_t** inData,
                    int inFormat,
                    uint8_t inChannelCount,
                    std::shared_ptr<Audio::Data> out)
                {
                    const uint8_t outChannelCount = out->getChannelCount();
                    switch (inFormat)
                    {
                    case AV_SAMPLE_FMT_S16:
                    {
                        if (inChannelCount == outChannelCount)
                        {
                            memcpy(out->getData(), inData[0], out->getByteCount());
                        }
                        else
                        {
                            Audio::Data::extract(
                                reinterpret_cast<int16_t*>(inData[0]),
                                reinterpret_cast<int16_t*>(out->getData()),
                                out->getSampleCount(),
                                inChannelCount,
                                outChannelCount);
                        }
                        break;
                    }
                    case AV_SAMPLE_FMT_S32:
                    {
                        if (inChannelCount == outChannelCount)
                        {
                            memcpy(out->getData(), inData[0], out->getByteCount());
                        }
                        else
                        {
                            Audio::Data::extract(
                                reinterpret_cast<int32_t*>(inData[0]),
                                reinterpret_cast<int32_t*>(out->getData()),
                                out->getSampleCount(),
                                inChannelCount,
                                outChannelCount);
                        }
                        break;
                    }
                    case AV_SAMPLE_FMT_FLT:
                    {
                        if (inChannelCount == outChannelCount)
                        {
                            memcpy(out->getData(), inData[0], out->getByteCount());
                        }
                        else
                        {
                            Audio::Data::extract(
                                reinterpret_cast<float*>(inData[0]),
                                reinterpret_cast<float*>(out->getData()),
                                out->getSampleCount(),
                                inChannelCount,
                                outChannelCount);
                        }
                        break;
                    }
                    case AV_SAMPLE_FMT_DBL:
                    {
                        if (inChannelCount == outChannelCount)
                        {
                            memcpy(out->getData(), inData[0], out->getByteCount());
                        }
                        else
                        {
                            Audio::Data::extract(
                                reinterpret_cast<double*>(inData[0]),
                                reinterpret_cast<double*>(out->getData()),
                                out->getSampleCount(),
                                inChannelCount,
                                outChannelCount);
                        }
                        break;
                    }
                    case AV_SAMPLE_FMT_S16P:
                    {
                        const int16_t** c = new const int16_t * [outChannelCount];
                        for (int i = 0; i < outChannelCount; ++i)
                        {
                            c[i] = reinterpret_cast<int16_t*>(inData[i]);
                        }
                        Audio::Data::planarInterleave(
                            c,
                            reinterpret_cast<int16_t*>(out->getData()),
                            out->getSampleCount(),
                            outChannelCount);
                        delete[] c;
                        break;
                    }
                    case AV_SAMPLE_FMT_S32P:
                    {
                        const int32_t** c = new const int32_t * [outChannelCount];
                        for (int i = 0; i < outChannelCount; ++i)
                        {
                            c[i] = reinterpret_cast<int32_t*>(inData[i]);
                        }
                        Audio::Data::planarInterleave(
                            c,
                            reinterpret_cast<int32_t*>(out->getData()),
                            out->getSampleCount(),
                            outChannelCount);
                        delete[] c;
                        break;
                    }
                    case AV_SAMPLE_FMT_FLTP:
                    {
                        const float** c = new const float* [outChannelCount];
                        for (int i = 0; i < outChannelCount; ++i)
                        {
                            c[i] = reinterpret_cast<float*>(inData[i]);
                        }
                        Audio::Data::planarInterleave(
                            c,
                            reinterpret_cast<float*>(out->getData()),
                            out->getSampleCount(),
                            outChannelCount);
                        delete[] c;
                        break;
                    }
                    case AV_SAMPLE_FMT_DBLP:
                    {
                        const double** c = new const double* [outChannelCount];
                        for (int i = 0; i < outChannelCount; ++i)
                        {
                            c[i] = reinterpret_cast<double*>(inData[i]);
                        }
                        Audio::Data::planarInterleave(
                            c,
                            reinterpret_cast<double*>(out->getData()),
                            out->getSampleCount(),
                            outChannelCount);
                        delete[] c;
                        break;
                    }
                    default: break;
                    }
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

                rapidjson::Value Plugin::getOptions(rapidjson::Document::AllocatorType& allocator) const
                {
                    DJV_PRIVATE_PTR();
                    return toJSON(p.options, allocator);
                }

                void Plugin::setOptions(const rapidjson::Value& value)
                {
                    DJV_PRIVATE_PTR();
                    fromJSON(value, p.options);
                }

                std::shared_ptr<IRead> Plugin::read(const FileSystem::FileInfo& fileInfo, const ReadOptions& options) const
                {
                    DJV_PRIVATE_PTR();
                    return Read::create(fileInfo, options, p.options, _textSystem, _resourceSystem, _logSystem);
                }

            } // namespace FFmpeg
        } // namespace IO
    } // namespace AV

    rapidjson::Value toJSON(const AV::IO::FFmpeg::Options& value, rapidjson::Document::AllocatorType& allocator)
    {
        rapidjson::Value out(rapidjson::kObjectType);
        {
            out.AddMember("ThreadCount", toJSON(value.threadCount, allocator), allocator);
        }
        return out;
    }

    void fromJSON(const rapidjson::Value& value, AV::IO::FFmpeg::Options& out)
    {
        if (value.IsObject())
        {
            for (const auto& i : value.GetObject())
            {
                if (0 == strcmp("ThreadCount", i.name.GetString()))
                {
                    fromJSON(i.value, out.threadCount);
                }
            }
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

} // namespace djv

