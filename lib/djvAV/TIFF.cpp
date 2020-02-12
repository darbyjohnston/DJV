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

                Plugin::Plugin() :
                    _p(new Private)
                {}

                std::shared_ptr<Plugin> Plugin::create(const std::shared_ptr<Context>& context)
                {
                    auto out = std::shared_ptr<Plugin>(new Plugin);
                    out->_init(
                        pluginName,
                        DJV_TEXT("plugin_tiff_io"),
                        fileExtensions,
                        context);
                    return out;
                }

                picojson::value Plugin::getOptions() const
                {
                    return toJSON(_p->options);
                }

                void Plugin::setOptions(const picojson::value & value)
                {
                    fromJSON(value, _p->options);
                }

                std::shared_ptr<IRead> Plugin::read(const FileSystem::FileInfo& fileInfo, const ReadOptions& options) const
                {
                    return Read::create(fileInfo, options, _resourceSystem, _logSystem);
                }

                std::shared_ptr<IWrite> Plugin::write(const FileSystem::FileInfo& fileInfo, const Info & info, const WriteOptions& options) const
                {
                    return Write::create(fileInfo, info, options, _p->options, _resourceSystem, _logSystem);
                }

            } // namespace TIFF
        } // namespace IO
    } // namespace AV

    picojson::value toJSON(const AV::IO::TIFF::Options & value)
    {
        picojson::value out(picojson::object_type, true);
        {
            std::stringstream ss;
            ss << value.compression;
            out.get<picojson::object>()["Compression"] = picojson::value(ss.str());
        }
        return out;
    }

    void fromJSON(const picojson::value & value, AV::IO::TIFF::Options & out)
    {
        if (value.is<picojson::object>())
        {
            for (const auto & i : value.get<picojson::object>())
            {
                if ("Compression" == i.first)
                {
                    std::stringstream ss(i.second.get<std::string>());
                    ss >> out.compression;
                }
            }
        }
        else
        {
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        AV::IO::TIFF,
        Compression,
        DJV_TEXT("None"),
        DJV_TEXT("format_tiff_compression_rle"),
        DJV_TEXT("format_tiff_compression_lzw"));

} // namespace djv

