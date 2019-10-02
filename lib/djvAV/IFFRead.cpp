//------------------------------------------------------------------------------
// Copyright (c) 2008-2009 Mikael Sundell
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
#include <djvCore/FileSystem.h>

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

                    size_t readRle(FileSystem::FileIO& io, uint8_t* out, size_t size)
                    {
                        const size_t pos = io.getPos();
                        const uint8_t* const end = out + size;
                        while (out < end)
                        {
                            // Information.
                            uint8_t in = 0;
                            io.readU8(&in);
                            const uint8_t count = (in & 0x7f) + 1;
                            const bool run = (in & 0x80) ? true : false;

                            // Find runs.
                            if (!run)
                            {
                                // Verbatim.
                                io.readU8(out, count);
                                out += count;
                            }
                            else
                            {
                                // Duplicate.
                                io.readU8(&in);
                                const uint8_t p = in;
                                for (int i = 0; i < count; i++)
                                {
                                    *out++ = p;
                                }
                            }
                        }
                        return io.getPos() - pos;
                    }

                } // namespace

                Read::Read()
                {}

                Read::~Read()
                {
                    _finish();
                }

                std::shared_ptr<Read> Read::create(
                    const FileSystem::FileInfo& fileInfo,
                    const ReadOptions& readOptions,
                    const std::shared_ptr<ResourceSystem>& resourceSystem,
                    const std::shared_ptr<LogSystem>& logSystem)
                {
                    auto out = std::shared_ptr<Read>(new Read);
                    out->_init(fileInfo, readOptions, resourceSystem, logSystem);
                    return out;
                }

                Info Read::_readInfo(const std::string & fileName)
                {
                    FileSystem::FileIO io;
                    return _open(fileName, io);
                }

                std::shared_ptr<Image::Image> Read::_readImage(const std::string & fileName)
                {
                    std::shared_ptr<Image::Image> out;
                    FileSystem::FileIO io;
                    const auto info = _open(fileName, io);
                    out = Image::Image::create(info.video[0].info);
                    out->setPluginName(pluginName);

                    uint8_t type[4];
                    uint8_t pixels[32];
                    uint32_t size;
                    uint32_t chunkSize;
                    uint32_t tilesRgba = _tiles;

                    const size_t channelByteCount = Image::getByteCount(Image::getDataType(info.video[0].info.type));
                    const size_t byteCount = Image::getByteCount(info.video[0].info.type);

                    // Read FOR4 <size> TBMP block
                    while (!io.isEOF())
                    {
                        // Get type.
                        io.read(&type, 4);

                        // Get length.
                        io.readU32(&size, 1);
                        chunkSize = getAlignSize(size, 4);

                        if (type[0] == 'A' &&
                            type[1] == 'U' &&
                            type[2] == 'T' &&
                            type[3] == 'H')
                        {
                            std::vector<uint8_t> buf(chunkSize);
                            io.readU8(buf.data(), chunkSize);
                            std::string s;
                            for (const auto& i : buf)
                            {
                                s.push_back(i);
                            }

                            // Get tag.
                            Tags tags;
                            tags.setTag("Creator", s);

                            // Set tag.
                            out->setTags(tags);
                        }
                        else if (type[0] == 'F' &&
                            type[1] == 'O' &&
                            type[2] == 'R' &&
                            type[3] == '4')
                        {
                            // Get type.
                            io.read(&type, 4);

                            // Check if TBMP.
                            if (type[0] == 'T' &&
                                type[1] == 'B' &&
                                type[2] == 'M' &&
                                type[3] == 'P')
                            {
                                // Read RGBA and ZBUF block.
                                while (!io.isEOF())
                                {
                                    // Get type.
                                    io.read(&type, 4);

                                    // Get length.
                                    io.readU32(&size, 1);
                                    chunkSize = getAlignSize(size, 4);

                                    // Tiles and RGBA.
                                    if (tilesRgba &&
                                        type[0] == 'R' &&
                                        type[1] == 'G' &&
                                        type[2] == 'B' &&
                                        type[3] == 'A')
                                    {
                                        // Set image size.
                                        uint32_t imageSize = size;

                                        // Get tile coordinates.
                                        uint16_t xmin;
                                        uint16_t xmax;
                                        uint16_t ymin;
                                        uint16_t ymax;
                                        io.readU16(&xmin, 1);
                                        io.readU16(&ymin, 1);
                                        io.readU16(&xmax, 1);
                                        io.readU16(&ymax, 1);

                                        if (xmin > xmax ||
                                            ymin > ymax ||
                                            xmax >= info.video[0].info.size.w ||
                                            ymax >= info.video[0].info.size.h)
                                        {
                                            throw FileSystem::Error(DJV_TEXT("File not supported."));
                                        }

                                        // NOTE: tile w = xmax - xmin + 1
                                        //       tile h = ymax - ymin + 1

                                        uint32_t tw = xmax - xmin + 1;
                                        uint32_t th = ymax - ymin + 1;

                                        if (!tw || !th)
                                        {
                                            throw FileSystem::Error(DJV_TEXT("File not supported."));
                                        }

                                        bool tile_compress = false;

                                        // If tile compression fails to be less than
                                        // image data stored uncompressed, the tile
                                        // is written uncompressed.

                                        // Set channels.
                                        uint8_t channels = Image::getChannelCount(info.video[0].info.type);

                                        // Set tile pixels.

                                        // Append xmin, xmax, ymin and ymax.
                                        uint32_t tileSize =
                                            tw * th * channels *
                                            Image::getByteCount(Image::getDataType(info.video[0].info.type)) + 8;

                                        // Test compressed.
                                        if (tileSize > imageSize)
                                        {
                                            tile_compress = true;
                                        }

                                        // Handle 8-bit data.
                                        if (info.video[0].info.type == Image::Type::RGB_U8 ||
                                            info.video[0].info.type == Image::Type::RGBA_U8)
                                        {
                                            // Tile compress.
                                            if (tile_compress)
                                            {
                                                int p = 0;

                                                // Map: RGB(A)8 BGRA to RGBA
                                                for (int c = (channels * channelByteCount) - 1;
                                                    c >= 0;
                                                    --c)
                                                {
                                                    std::vector<uint8_t> in(static_cast<size_t>(tw) * static_cast<size_t>(th));
                                                    uint8_t* inP = in.data();

                                                    // Uncompress.
                                                    p += readRle(io, in.data(), static_cast<size_t>(tw) * static_cast<size_t>(th));

                                                    for (uint16_t py = ymin; py <= ymax; py++)
                                                    {
                                                        uint8_t* out_dy = out->getData(0, py);

                                                        for (uint16_t px = xmin; px <= xmax; px++)
                                                        {
                                                            uint8_t* outP = out_dy + px * byteCount + c;
                                                            *outP++ = *inP++;
                                                        }
                                                    }
                                                }

                                                // Test.
                                                if (p != imageSize - 8)
                                                {
                                                    throw FileSystem::Error(DJV_TEXT("File not supported."));
                                                }
                                            }
                                            else
                                            {
                                                for (uint16_t py = ymin; py <= ymax; py++)
                                                {
                                                    uint8_t* out_dy = out->getData(xmin, py);

                                                    // Tile scanline.
                                                    std::vector<uint8_t> scanline(tw * byteCount);
                                                    uint8_t* outP = scanline.data();

                                                    // Set bytes.
                                                    for (uint16_t px = xmin; px <= xmax; px++)
                                                    {
                                                        // Get pixels.
                                                        io.read(&pixels, byteCount);

                                                        if (size < static_cast<uint32_t>(byteCount))
                                                        {
                                                            throw FileSystem::Error(DJV_TEXT("File not supported."));
                                                        }

                                                        size -= byteCount;

                                                        // Map: RGB(A)8 ABGR to ARGB
                                                        for (int c = channels - 1; c >= 0; --c)
                                                        {
                                                            uint8_t pixel;
                                                            uint8_t* inP = pixels + c * channelByteCount;
                                                            memcpy(&pixel, inP, 1);
                                                            *outP++ = pixel;
                                                        }
                                                    }

                                                    // Copy data.
                                                    memcpy(out_dy, scanline.data(), tw * byteCount);
                                                }
                                            }
                                        }
                                        // Handle 16-bit data.
                                        else if (
                                            info.video[0].info.type == Image::Type::RGB_U16 ||
                                            info.video[0].info.type == Image::Type::RGBA_U16)
                                        {
                                            if (tile_compress)
                                            {
                                                // Set bytes.
                                                int p = 0;

                                                // Set map.
                                                int* map = NULL;

                                                if (Memory::getEndian() == Memory::Endian::LSB)
                                                {
                                                    int rgb16[] = { 0, 2, 4, 1, 3, 5 };
                                                    int rgba16[] = { 0, 2, 4, 7, 1, 3, 5, 6 };

                                                    if (info.video[0].info.type == Image::Type::RGB_U16)
                                                    {
                                                        map = rgb16;
                                                    }
                                                    else
                                                    {
                                                        map = rgba16;
                                                    }
                                                }
                                                else
                                                {
                                                    int rgb16[] = { 1, 3, 5, 0, 2, 4 };
                                                    int rgba16[] = { 1, 3, 5, 7, 0, 2, 4, 6 };

                                                    if (info.video[0].info.type == Image::Type::RGB_U16)
                                                    {
                                                        map = rgb16;
                                                    }
                                                    else
                                                    {
                                                        map = rgba16;
                                                    }
                                                }

                                                // Map: RGB(A)8 BGRA to RGBA
                                                for (int c = (channels * channelByteCount) - 1; c >= 0; --c)
                                                {
                                                    int mc = map[c];

                                                    std::vector<uint8_t> in(static_cast<size_t>(tw) * static_cast<size_t>(th));
                                                    uint8_t* inP = in.data();

                                                    // Uncompress.
                                                    p += readRle(io, in.data(), static_cast<size_t>(tw) * static_cast<size_t>(th));

                                                    for (uint16_t py = ymin; py <= ymax; py++)
                                                    {
                                                        uint8_t* out_dy = out->getData(0, py);

                                                        for (uint16_t px = xmin; px <= xmax; px++)
                                                        {
                                                            uint8_t* outP = out_dy + px * byteCount + mc;
                                                            *outP++ = *inP++;
                                                        }
                                                    }
                                                }

                                                // Test.
                                                if (p != imageSize - 8)
                                                {
                                                    throw FileSystem::Error(DJV_TEXT("File not supported."));
                                                }
                                            }
                                            else
                                            {
                                                for (uint16_t py = ymin; py <= ymax; py++)
                                                {
                                                    uint8_t* out_dy = out->getData(xmin, py);

                                                    // Tile scanline.
                                                    std::vector<uint16_t> scanline(tw * byteCount);
                                                    uint16_t* outP = scanline.data();

                                                    // Set bytes.
                                                    for (uint16_t px = xmin; px <= xmax; px++)
                                                    {
                                                        // Get pixels.
                                                        io.read(&pixels, byteCount);

                                                        if (size < static_cast<uint32_t>(byteCount))
                                                        {
                                                            throw FileSystem::Error(DJV_TEXT("File not supported."));
                                                        }

                                                        size -= byteCount;

                                                        // Map: RGB8 ABGR to ARGB
                                                        for (int c = channels - 1; c >= 0; --c)
                                                        {
                                                            uint16_t pixel;
                                                            uint8_t* in = pixels + c * channelByteCount;

                                                            if (Memory::getEndian() == Memory::Endian::LSB)
                                                            {
                                                                Memory::endian(in, &pixel, 1, 2);
                                                            }
                                                            else
                                                            {
                                                                memcpy(&pixel, in, 2);
                                                            }

                                                            // Set pixel.
                                                            *outP++ = pixel;
                                                        }
                                                    }

                                                    // Copy data.
                                                    memcpy(out_dy, scanline.data(), tw * byteCount);
                                                }
                                            }
                                        }
                                        else
                                        {
                                            io.seek(chunkSize);
                                        }

                                        // Seek to align to chunksize.
                                        size = chunkSize - imageSize;

                                        if (size)
                                        {
                                            io.seek(size);
                                        }

                                        tilesRgba--;
                                    }
                                    else
                                    {
                                        io.seek(chunkSize);
                                    }

                                    if (tilesRgba)
                                    {
                                        continue;
                                    }
                                    else
                                    {
                                        break;
                                    }

                                }

                                // TBMP done, break.
                                break;
                            }

                        }
                        else
                        {
                            // Skip to the next block.
                            io.seek(chunkSize);
                        }
                    }

                    return out;
                }

                namespace
                {
                    class Header
                    {
                    public:
                        void read(FileSystem::FileIO&, Image::Info&, int& tiles, bool& compression);

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
                        };
                        Data _data;
                    };

                    void Header::read(
                        FileSystem::FileIO& io,
                        Image::Info& info,
                        int& tiles,
                        bool& compression)
                    {
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
                                                throw FileSystem::Error(DJV_TEXT("Error reading header."));
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
                                                throw FileSystem::Error(DJV_TEXT("File not supported."));
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

                        info.layout.mirror.y = true;
                    }

                } // namespace

                Info Read::_open(const std::string & fileName, FileSystem::FileIO & io)
                {
                    io.setEndian(Memory::getEndian() != Memory::Endian::MSB);
                    io.open(fileName, FileSystem::FileIO::Mode::Read);
                    Image::Info imageInfo;
                    Header().read(io, imageInfo, _tiles, _compression);
                    auto info = Info(fileName, VideoInfo(imageInfo, _speed, _sequence));
                    return info;
                }

            } // namespace IFF
        } // namespace IO
    } // namespace AV
} // namespace djv

