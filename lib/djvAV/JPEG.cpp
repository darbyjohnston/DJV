// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/JPEG.h>

#include <djvCore/Context.h>
#include <djvCore/String.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace JPEG
            {
                struct Plugin::Private
                {
                    Options options;
                };

                Plugin::Plugin() :
                    _p(new Private)
                {}

                Plugin::~Plugin()
                {}

                std::shared_ptr<Plugin> Plugin::create(const std::shared_ptr<Context>& context)
                {
                    auto out = std::shared_ptr<Plugin>(new Plugin);
                    out->_init(
                        pluginName,
                        DJV_TEXT("plugin_jpeg_io"),
                        fileExtensions,
                        context);
                    return out;
                }

                rapidjson::Value Plugin::getOptions(rapidjson::Document::AllocatorType& allocator) const
                {
                    return toJSON(_p->options, allocator);
                }

                void Plugin::setOptions(const rapidjson::Value& value)
                {
                    fromJSON(value, _p->options);
                }

                std::shared_ptr<IRead> Plugin::read(const FileSystem::FileInfo& fileInfo, const ReadOptions& options) const
                {
                    return Read::create(fileInfo, options, _textSystem, _resourceSystem, _logSystem);
                }

                std::shared_ptr<IWrite> Plugin::write(const FileSystem::FileInfo& fileInfo, const Info& info, const WriteOptions& options) const
                {
                    return Write::create(fileInfo, info, options, _p->options, _textSystem, _resourceSystem, _logSystem);
                }

                extern "C"
                {
                    void djvJPEGError(j_common_ptr in)
                    {
                        auto error = reinterpret_cast<JPEGErrorStruct *>(in->err);
                        char message[JMSG_LENGTH_MAX] = "";
                        in->err->format_message(in, message);
                        error->messages.push_back(message);
                        ::longjmp(error->jump, 1);
                    }

                    void djvJPEGWarning(j_common_ptr in, int level)
                    {
                        if (level > 0)
                        {
                            //! \todo Should we send "trace" messages to the debug log?
                            return;
                        }
                        auto error = reinterpret_cast<JPEGErrorStruct *>(in->err);
                        char message[JMSG_LENGTH_MAX] = "";
                        in->err->format_message(in, message);
                        error->messages.push_back(message);
                    }

                } // extern "C"

            } // namespace JPEG
        } // namespace IO
    } // namespace AV
    
    rapidjson::Value toJSON(const AV::IO::JPEG::Options& value, rapidjson::Document::AllocatorType& allocator)
    {
        rapidjson::Value out(rapidjson::kObjectType);
        {
            out.AddMember("Quality", rapidjson::Value(value.quality), allocator);
        }
        return out;
    }

    void fromJSON(const rapidjson::Value& value, AV::IO::JPEG::Options& out)
    {
        if (value.IsObject())
        {
            for (const auto& i : value.GetObject())
            {
                if (0 == strcmp("Quality", i.name.GetString()) && i.value.IsInt())
                {
                    out.quality = i.value.GetInt();
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

