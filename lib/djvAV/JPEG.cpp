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

                picojson::value Plugin::getOptions() const
                {
                    return toJSON(_p->options);
                }

                void Plugin::setOptions(const picojson::value& value)
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
    
    picojson::value toJSON(const AV::IO::JPEG::Options& value)
    {
        picojson::value out(picojson::object_type, true);
        {
            std::stringstream ss;
            ss << value.quality;
            out.get<picojson::object>()["Quality"] = picojson::value(ss.str());
        }
        return out;
    }

    void fromJSON(const picojson::value& value, AV::IO::JPEG::Options& out)
    {
        if (value.is<picojson::object>())
        {
            for (const auto& i : value.get<picojson::object>())
            {
                if ("Quality" == i.first)
                {
                    std::stringstream ss(i.second.get<std::string>());
                    ss >> out.quality;
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

