// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/FFmpeg.h>

#include <djvAV/FFmpegFunc.h>

#include <djvSystem/File.h>
#include <djvSystem/Context.h>
#include <djvSystem/LogSystem.h>

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
                bool Options::operator == (const Options& other) const
                {
                    return threadCount == other.threadCount;
                }
                
                namespace
                {
                    std::weak_ptr<System::LogSystem> _logSystem;

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

                void Plugin::_init(const std::shared_ptr<System::Context>& context)
                {
                    IPlugin::_init(
                        pluginName,
                        DJV_TEXT("plugin_ffmpeg_io"),
                        fileExtensions,
                        context);
                        
                    _logSystem = context->getSystemT<System::LogSystem>();
                    av_log_set_level(AV_LOG_ERROR);
                    av_log_set_callback(avLogCallback);
               }

                Plugin::Plugin() :
                    _p(new Private)
                {}

                std::shared_ptr<Plugin> Plugin::create(const std::shared_ptr<System::Context>& context)
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

                std::shared_ptr<IRead> Plugin::read(const System::File::Info& fileInfo, const ReadOptions& options) const
                {
                    DJV_PRIVATE_PTR();
                    return Read::create(fileInfo, options, p.options, _textSystem, _resourceSystem, _logSystem);
                }

            } // namespace FFmpeg
        } // namespace IO
    } // namespace AV
} // namespace djv

