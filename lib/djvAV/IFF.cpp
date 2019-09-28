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

#include <djvAV/IFF.h>

#include <djvCore/FileIO.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace IFF
            {
                namespace
                {
                    int getTileWidth()
                    {
                        return 64;
                    }

                    int getTileHeight()
                    {
                        return 64;
                    }

                    Image::Size getTileSize(uint32_t width, uint32_t height)
                    {
                        const uint32_t tw = getTileWidth();
                        const uint32_t th = getTileHeight();
                        return Image::Size(
                            (width  + tw - 1) / tw,
                            (height + th - 1) / th);
                    }

                    struct Header
                    {
                        void read(FileSystem::FileIO&, Image::Info&, int& tiles, bool& compression);
                        void write(FileSystem::FileIO&, const Image::Info&, bool compression);

                    private:
                        struct Data
                        {
                            uint32_t x = 0;
                            uint32_t y = 0;
                            uint32_t width = 0;
                            uint32_t height = 0;
                            uint8_t  pixelBits = 0;
                            uint8_t  pixelChannels = 0;
                            uint16_t tiles = 0;
                        }
                        _data;
                    };

                    void Header::read(
                        FileSystem::FileIO& io,
                        Image::Info& info,
                        int& tiles,
                        bool& compression)
                    {
                        info.layout.mirror.y = true;

                        uint8_t  type[4];
                        uint32_t size;
                        uint32_t chunksize;
                        uint32_t tbhdsize;
                        uint32_t flags;
                        uint32_t compressed;
                        uint16_t bytes;
                        uint16_t prnum;
                        uint16_t prden;

                        // Read FOR4 <size> CIMG.
                        while (!io.isEOF())
                        {
                            // Get type.
                            io.read(&type, 4);

                            // Get length.
                            io.readU32(&size, 1);
                            chunksize = getAlignSize(size, 4);

                            if (type[0] == 'F' &&
                                type[1] == 'O' &&
                                type[2] == 'R' &&
                                type[3] == '4')
                            {
                                // Get type
                                io.read(&type, 4);

                                // Check if CIMG.
                                if (type[0] == 'C' &&
                                    type[1] == 'I' &&
                                    type[2] == 'M' &&
                                    type[3] == 'G')
                                {
                                    // Read TBHD.
                                    while (!io.isEOF())
                                    {
                                        // Get type
                                        io.read(&type, 4);

                                        // Get length
                                        io.readU32(&size, 1);
                                        chunksize = getAlignSize(size, 4);

                                        if (type[0] == 'T' &&
                                            type[1] == 'B' &&
                                            type[2] == 'H' &&
                                            type[3] == 'D')
                                        {
                                            tbhdsize = size;

                                            // Test if size if correct.
                                            if (tbhdsize != 24 && tbhdsize != 32)
                                            {
                                                throw std::runtime_error(DJV_TEXT("Error reading header."));
                                            }

                                            // Set data.
                                            Data data;

                                            // Get width and height.
                                            io.readU32(&data.width, 1);
                                            io.readU32(&data.height, 1);
                                            info.size = Image::Size(data.width, data.height);

                                            // Get prnum and prdeb
                                            io.readU16(&prnum, 1);
                                            io.readU16(&prden, 1);

                                            // Get flags, bytes, tiles and compressed.
                                            io.readU32(&flags, 1);
                                            io.readU16(&bytes, 1);

                                            // Get tiles.
                                            io.readU16(&data.tiles, 1);
                                            tiles = data.tiles;

                                            // Get compressed.
                                            io.readU32(&compressed, 1);

                                            // 0 no compression
                                            // 1 RLE compression
                                            // 2 QRL (not supported)
                                            // 3 QR4 (not supported)
                                            if (compressed > 1)
                                            {
                                                // no compression or non-rle compression not
                                                // supported
                                                throw std::runtime_error(DJV_TEXT("File not supported."));
                                            }

                                            // Get compressed.
                                            compression = compressed;

                                            // Set XY.
                                            if (tbhdsize == 32)
                                            {
                                                io.readU32(&data.x, 1);
                                                io.readU32(&data.y, 1);
                                            }
                                            else
                                            {
                                                data.x = 0;
                                                data.y = 0;
                                            }

                                            // Test format.
                                            if (flags & 0x00000003)
                                            {
                                                // Test if grayscale is set, if throw assert.
                                                DJV_ASSERT(!(flags & 0x00000010));
                                                bool alpha = false;

                                                // Test for RGB channels.
                                                if (flags & 0x00000001)
                                                {
                                                    data.pixelChannels = 3;
                                                }

                                                // Test for Alpha channel.
                                                if (flags & 0x00000002)
                                                {
                                                    data.pixelChannels++;
                                                    alpha = true;
                                                }

                                                // Test pixel bits.
                                                if (flags & 0x00002000)
                                                {
                                                    data.pixelBits = 16; // 12bit represented as 16bit

                                                    if (!alpha)
                                                    {
                                                        info.type = Image::Type::RGB_U16;
                                                    }
                                                    else
                                                    {
                                                        info.type = Image::Type::RGBA_U16;
                                                    }
                                                }
                                                else
                                                {
                                                    if (!bytes)
                                                    {
                                                        data.pixelBits = 8; // 8bit

                                                        if (!alpha)
                                                        {
                                                            info.type = Image::Type::RGB_U8;
                                                        }
                                                        else
                                                        {
                                                            info.type = Image::Type::RGBA_U8;
                                                        }
                                                    }
                                                    else
                                                    {
                                                        data.pixelBits = 16; // 16bit

                                                        if (!alpha)
                                                        {
                                                            info.type = Image::Type::RGB_U16;
                                                        }
                                                        else
                                                        {
                                                            info.type = Image::Type::RGBA_U16;
                                                        }
                                                    }
                                                }

                                                // Test bits.
                                                const int bits = data.pixelChannels * data.pixelBits;
                                                DJV_ASSERT(
                                                    bits == (Image::getChannelCount(info.type) *
                                                        data.pixelBits) &&
                                                        (bits % data.pixelBits) == 0);
                                            }

                                            // Z format.
                                            else if (flags & 0x00000004)
                                            {
                                                data.pixelChannels = 1;
                                                data.pixelBits = 32; // 32bit

                                                // NOTE: Z_F32 support - not supported
                                                // info->pixel = PIXEL(Pixel::Z_F32);

                                                DJV_ASSERT(bytes == 0);
                                            }

                                            // Set data.
                                            _data = data;

                                            // TBHD done, break.
                                            break;
                                        }

                                        // Skip to the next block.
                                        io.seek(chunksize);
                                    }

                                    // Test if supported else skip to next block.
                                    if (_data.width > 0 &&
                                        _data.height > 0 &&
                                        _data.pixelBits > 0 &&
                                        _data.pixelChannels > 0)
                                    {
                                        break;
                                    }
                                }
                            }

                            // Skip to the next block.
                            io.seek(chunksize);
                        }
                    }

                    void Header::write(
                        FileSystem::FileIO& io,
                        const Image::Info& info,
                        bool compression)
                    {
                        // FOR4 <size> CIMG    - 4 + 4 + 4 = 16 bytes
                        // TBHD <size> <data>  - 4 + 4 + 32 = 40 bytes

                        uint32_t length = 0;
                        uint32_t flags = 0;

                        // Information.

                        const int channels = Image::getChannelCount(info.type);
                        int bytes = 0;

                        _data.width = info.size.w;
                        _data.height = info.size.h;
                        _data.pixelChannels = channels;
                        _data.pixelBits = 1 == channels ? (1 == info.size.h ? 1 : 2) : 3;

                        // Write.

                        // 'FOR4' type
                        io.writeU8('F');
                        io.writeU8('O');
                        io.writeU8('R');
                        io.writeU8('4');

                        // 'FOR4' length
                        // NOTE: only reserved for now.
                        io.writeU32(length);

                        // 'CIMG' type
                        io.writeU8('C');
                        io.writeU8('I');
                        io.writeU8('M');
                        io.writeU8('G');

                        // 'TBHD' type
                        io.writeU8('T');
                        io.writeU8('B');
                        io.writeU8('H');
                        io.writeU8('D');

                        // 'TBHD' length
                        length = 32;
                        io.writeU32(length);

                        // Set data.
                        // NOTE: 32 bytes.

                        // Set width and height.
                        io.writeU32(_data.width);
                        io.writeU32(_data.height);

                        // Set prnum and prden.
                        io.writeU16(1);
                        io.writeU16(1);

                        // Set flags.
                        switch (info.type)
                        {
                        case Image::Type::RGB_U8:
                            // RGB 8.
                            flags = 0x00000001;
                            bytes = 0;
                            break;

                        case Image::Type::RGBA_U8:
                            // RGBA 8.
                            flags = 0x00000003;
                            bytes = 0;
                            break;

                        case Image::Type::RGB_U16:
                            // RGB 16.
                            flags = 0x00000001;
                            bytes = 1;
                            break;

                        case Image::Type::RGBA_U16:
                            // RGBA 16.
                            flags = 0x00000003;
                            bytes = 1;
                            break;

                        default:
                            break;
                        }

                        io.writeU32(flags);
                        io.writeU16(bytes);

                        // Set tiles.
                        Image::Size size = getTileSize(_data.width, _data.height);
                        io.writeU16(size.w * size.h);

                        // Set compressed.
                        // 0 no compression
                        // 1 RLE compression
                        // 2 QRL (not supported)
                        // 3 QR4 (not supported)
                        if (!compression)
                        {
                            io.writeU32(0);
                        }
                        else
                        {
                            io.writeU32(1);
                        }

                        // Set x and y.
                        io.writeU32(0);
                        io.writeU32(0);
                    }

                } // namespace

                uint32_t getAlignSize(uint32_t size, uint32_t alignment)
                {
                    uint32_t mod = size % alignment;
                    if (mod)
                    {
                        mod = alignment - mod;
                        size += mod;
                    }
                    return size;
                }

                void readHeader(
                    FileSystem::FileIO& io,
                    Image::Info&        info,
                    int&                tiles,
                    bool&               compression)
                {
                    Header().read(io, info, tiles, compression);
                }

                void writeHeader(
                    FileSystem::FileIO& io,
                    const Image::Info&  info,
                    bool                compression)
                {
                    Header().write(io, info, compression);
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
                        DJV_TEXT("This plugin provides IFF image I/O."),
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

            } // namespace IFF
        } // namespace IO
    } // namespace AV

    picojson::value toJSON(const AV::IO::IFF::Options & value)
    {
        picojson::value out(picojson::object_type, true);
        {
            std::stringstream ss;
            ss << value.compression;
            out.get<picojson::object>()["Compression"] = picojson::value(ss.str());
        }
        return out;
    }

    void fromJSON(const picojson::value & value, AV::IO::IFF::Options & out)
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
            throw std::invalid_argument(DJV_TEXT("Cannot parse the value."));
        }
    }

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        AV::IO::IFF,
        Compression,
        DJV_TEXT("None"),
        DJV_TEXT("RLE"));

} // namespace djv

