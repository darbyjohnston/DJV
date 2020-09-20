// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/OpenEXR.h>

#include <djvAV/OpenEXRFunc.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace OpenEXR
            {
                Channel::Channel()
                {}

                Channel::Channel(
                    const std::string& name,
                    Image::DataType    type,
                    const glm::ivec2&  sampling) :
                    name(name),
                    type(type),
                    sampling(sampling)
                {}

                Layer::Layer(
                    const std::vector<Channel>& channels,
                    bool                        luminanceChroma) :
                    channels(channels),
                    luminanceChroma(luminanceChroma)
                {
                    std::vector<std::string> names;
                    for (const auto& i : channels)
                    {
                        names.push_back(i.name);
                    }
                    name = getLayerName(names);
                }
                    
                bool Options::operator == (const Options& other) const
                {
                    return
                        threadCount == other.threadCount &&
                        channels == other.channels &&
                        compression == other.compression &&
                        dwaCompressionLevel == other.dwaCompressionLevel;
                }
                
                struct Plugin::Private
                {
                    Options options;
                };

                Plugin::Plugin() :
                    _p(new Private)
                {}

                std::shared_ptr<Plugin> Plugin::create(const std::shared_ptr<System::Context>& context)
                {
                    auto out = std::shared_ptr<Plugin>(new Plugin);
                    Imf::setGlobalThreadCount(out->_p->options.threadCount);
                    out->_init(
                        pluginName,
                        DJV_TEXT("plugin_openexr_io"),
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
                    DJV_PRIVATE_PTR();
                    fromJSON(value, p.options);
                    Imf::setGlobalThreadCount(p.options.threadCount);
                }

                std::shared_ptr<IRead> Plugin::read(const System::File::Info& fileInfo, const ReadOptions& options) const
                {
                    return Read::create(fileInfo, options, _p->options, _textSystem, _resourceSystem, _logSystem);
                }

                std::shared_ptr<IWrite> Plugin::write(const System::File::Info& fileInfo, const Info& info, const WriteOptions& options) const
                {
                    return Write::create(fileInfo, info, options, _p->options, _textSystem, _resourceSystem, _logSystem);
                }

            } // namespace OpenEXR
        } // namespace IO
    } // namespace AV
} // namespace djv

