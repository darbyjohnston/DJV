// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/TIFF.h>

#include <djvAV/TIFFFunc.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace TIFF
            {
                bool Options::operator == (const Options& other) const
                {
                    return compression == other.compression;
                }
                
                struct Plugin::Private
                {
                    Options options;
                };

                void Plugin::_init(const std::shared_ptr<System::Context>& context)
                {
                    ISequencePlugin::_init(
                        pluginName,
                        DJV_TEXT("plugin_tiff_io"),
                        fileExtensions,
                        context);
                    TIFFSetErrorHandler(nullptr);
                    TIFFSetWarningHandler(nullptr);
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
                    return toJSON(_p->options, allocator);
                }

                void Plugin::setOptions(const rapidjson::Value& value)
                {
                    fromJSON(value, _p->options);
                }

                std::shared_ptr<IRead> Plugin::read(const System::File::Info& fileInfo, const ReadOptions& options) const
                {
                    return Read::create(fileInfo, options, _textSystem, _resourceSystem, _logSystem);
                }

                std::shared_ptr<IWrite> Plugin::write(const System::File::Info& fileInfo, const Info& info, const WriteOptions& options) const
                {
                    return Write::create(fileInfo, info, options, _p->options, _textSystem, _resourceSystem, _logSystem);
                }

            } // namespace TIFF
        } // namespace IO
    } // namespace AV
} // namespace djv
