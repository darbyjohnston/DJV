// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/TIFF.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace TIFF
            {
                void paletteLoad(
                    uint8_t *  in,
                    int        size,
                    int        bytes,
                    uint16_t * red,
                    uint16_t * green,
                    uint16_t * blue)
                {
                    switch (bytes)
                    {
                    case 1:
                    {
                        const uint8_t * inP = in + size - 1;
                        uint8_t * outP = in + (size_t(size) - 1) * 3;
                        for (int x = 0; x < size; ++x, outP -= 3)
                        {
                            const uint8_t index = *inP--;
                            outP[0] = static_cast<uint8_t>(red[index]);
                            outP[1] = static_cast<uint8_t>(green[index]);
                            outP[2] = static_cast<uint8_t>(blue[index]);
                        }
                    }
                    break;
                    case 2:
                    {
                        const uint16_t * inP = reinterpret_cast<const uint16_t *>(in) + size - 1;
                        uint16_t * outP = reinterpret_cast<uint16_t *>(in) + (size_t(size) - 1) * 3;
                        for (int x = 0; x < size; ++x, outP -= 3)
                        {
                            const uint16_t index = *inP--;
                            outP[0] = red  [index];
                            outP[1] = green[index];
                            outP[2] = blue [index];
                        }
                    }
                    break;
                    }
                }

                struct Plugin::Private
                {
                    Options options;
                };

                void Plugin::_init(const std::shared_ptr<Context>& context)
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

                std::shared_ptr<Plugin> Plugin::create(const std::shared_ptr<Context>& context)
                {
                    auto out = std::shared_ptr<Plugin>(new Plugin);
                    out->_init(context);
                    return out;
                }

                rapidjson::Value Plugin::getOptions(rapidjson::Document::AllocatorType& allocator) const
                {
                    return toJSON(_p->options, allocator);
                }

                void Plugin::setOptions(const rapidjson::Value & value)
                {
                    fromJSON(value, _p->options);
                }

                std::shared_ptr<IRead> Plugin::read(const FileSystem::FileInfo& fileInfo, const ReadOptions& options) const
                {
                    return Read::create(fileInfo, options, _textSystem, _resourceSystem, _logSystem);
                }

                std::shared_ptr<IWrite> Plugin::write(const FileSystem::FileInfo& fileInfo, const Info & info, const WriteOptions& options) const
                {
                    return Write::create(fileInfo, info, options, _p->options, _textSystem, _resourceSystem, _logSystem);
                }

            } // namespace TIFF
        } // namespace IO
    } // namespace AV

    rapidjson::Value toJSON(const AV::IO::TIFF::Options & value, rapidjson::Document::AllocatorType& allocator)
    {
        rapidjson::Value out(rapidjson::kObjectType);
        {
            std::stringstream ss;
            ss << value.compression;
            const std::string& s = ss.str();
            out.AddMember("Compression", rapidjson::Value(s.c_str(), s.size(), allocator), allocator);
        }
        return out;
    }

    void fromJSON(const rapidjson::Value & value, AV::IO::TIFF::Options & out)
    {
        if (value.IsObject())
        {
            for (const auto & i : value.GetObject())
            {
                if (0 == strcmp("Compression", i.name.GetString()) && i.value.IsString())
                {
                    std::stringstream ss(i.value.GetString());
                    ss >> out.compression;
                }
            }
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        AV::IO::TIFF,
        Compression,
        DJV_TEXT("tiff_compression_none"),
        DJV_TEXT("tiff_compression_rle"),
        DJV_TEXT("tiff_compression_lzw"));

} // namespace djv

