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

#include <djvAV/RLA.h>

#include <djvCore/FileIO.h>
#include <djvCore/FileSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace RLA
            {
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

                namespace
                {
                    void readRle(
                        FileSystem::FileIO& io,
                        uint8_t*            out,
                        size_t              size,
                        size_t              channels,
                        size_t              bytes)
                    {
                        int16_t _size = 0;
                        io.read16(&_size);
                        std::vector<uint8_t> buf;
                        buf.resize(_size);
                        io.read(buf.data(), _size);
                        const uint8_t* p = buf.data();
                        for (size_t b = 0; b < bytes; ++b)
                        {
                            uint8_t* outP = out + (Memory::Endian::LSB == Memory::getEndian() ? (bytes - 1 - b) : b);
                            const size_t outInc = channels * bytes;
                            for (size_t i = 0; i < size;)
                            {
                                int count = *((const int8_t*)p);
                                ++p;
                                if (count >= 0)
                                {
                                    ++count;
                                    for (int j = 0; j < count; ++j, outP += outInc)
                                    {
                                        *outP = *p;
                                    }
                                    ++p;
                                }
                                else
                                {
                                    count = -count;
                                    for (int j = 0; j < count; ++j, ++p, outP += outInc)
                                    {
                                        *outP = *p;
                                    }
                                }
                                i += count;
                            }
                        }
                    }

                    void readFloat(
                        FileSystem::FileIO& io,
                        uint8_t*            out,
                        size_t              size,
                        size_t              channels)
                    {
                        int16_t _size = 0;
                        io.read16(&_size);
                        std::vector<uint8_t> buf(_size);
                        io.read(buf.data(), _size);
                        const uint8_t* p = buf.data();
                        const size_t outInc = channels * 4;
                        if (Memory::Endian::LSB == Memory::getEndian())
                        {
                            for (size_t i = 0; i < size; ++i, p += 4, out += outInc)
                            {
                                out[0] = p[3];
                                out[1] = p[2];
                                out[2] = p[1];
                                out[3] = p[0];
                            }
                        }
                        else
                        {
                            for (size_t i = 0; i < size; ++i, p += 4, out += outInc)
                            {
                                out[0] = p[0];
                                out[1] = p[1];
                                out[2] = p[2];
                                out[3] = p[3];
                            }
                        }
                    }

                } // namespace

                std::shared_ptr<Image::Image> Read::_readImage(const std::string & fileName)
                {
                    std::shared_ptr<Image::Image> out;
                    FileSystem::FileIO io;
                    const auto info = _open(fileName, io);
                    out = Image::Image::create(info.video[0].info);
                    out->setPluginName(pluginName);

                    const size_t w = info.video[0].info.size.w;
                    const size_t h = info.video[0].info.size.h;
                    const size_t channels = Image::getChannelCount(info.video[0].info.type);
                    const size_t bytes = Image::getByteCount(Image::getDataType(info.video[0].info.type));
                    const Image::DataType dataType = Image::getDataType(info.video[0].info.type);
                    uint8_t* dataP = out->getData();
                    for (uint16_t y = 0; y < h; ++y, dataP += w * channels * bytes)
                    {
                        io.setPos(_rleOffset[y]);
                        for (int c = 0; c < channels; ++c)
                        {
                            if (Image::DataType::F32 == dataType)
                            {
                                readFloat(io, dataP + c * bytes, w, channels);
                            }
                            else
                            {
                                readRle(io, dataP + c * bytes, w, channels, bytes);
                            }
                        }
                    }

                    return out;
                }

                namespace
                {
                    struct Header
                    {
                        int16_t dimensions[4]; // Left, right, bottom, top.
                        int16_t active[4];
                        int16_t frame;
                        int16_t colorChannelType;
                        int16_t colorChannels;
                        int16_t matteChannels;
                        int16_t auxChannels;
                        int16_t version;
                        char    gamma[16];
                        char    chroma[3][24];
                        char    whitepoint[24];
                        int32_t job;
                        char    fileName[128];
                        char    description[128];
                        char    progam[64];
                        char    machine[32];
                        char    user[32];
                        char    date[20];
                        char    aspect[24];
                        char    aspectRatio[8];
                        char    colorFormat[32];
                        int16_t field;
                        char    renderTime[12];
                        char    filter[32];
                        int16_t colorBitDepth;
                        int16_t matteChannelType;
                        int16_t matteBitDepth;
                        int16_t auxChannelType;
                        int16_t auxBitDepth;
                        char    auxFormat[32];
                        char    pad[36];
                        int32_t offset;
                    };

                    void endian(Header* in)
                    {
                        Memory::endian(&in->dimensions, 4, 2);
                        Memory::endian(&in->active, 4, 2);
                        Memory::endian(&in->frame, 1, 2);
                        Memory::endian(&in->colorChannelType, 1, 2);
                        Memory::endian(&in->colorChannels, 1, 2);
                        Memory::endian(&in->matteChannels, 1, 2);
                        Memory::endian(&in->auxChannels, 1, 2);
                        Memory::endian(&in->version, 1, 2);
                        Memory::endian(&in->job, 1, 4);
                        Memory::endian(&in->field, 1, 2);
                        Memory::endian(&in->colorBitDepth, 1, 2);
                        Memory::endian(&in->matteChannelType, 1, 2);
                        Memory::endian(&in->matteBitDepth, 1, 2);
                        Memory::endian(&in->auxChannelType, 1, 2);
                        Memory::endian(&in->auxBitDepth, 1, 2);
                        Memory::endian(&in->offset, 1, 4);
                    }

                } // namespace

                Info Read::_open(const std::string & fileName, FileSystem::FileIO& io)
                {
                    // Open the file.
                    io.setEndian(Memory::getEndian() != Memory::Endian::MSB);
                    io.open(fileName, FileSystem::FileIO::Mode::Read);

                    // Read the header.
                    Header header;
                    io.read(&header, sizeof(Header));
                    if (io.getEndian())
                    {
                        endian(&header);
                    }
                    const int w = header.active[1] - header.active[0] + 1;
                    const int h = header.active[3] - header.active[2] + 1;

                    // Read the scanline table.
                    _rleOffset.resize(h);
                    io.read32(_rleOffset.data(), h);

                    // Get file information.
                    if (header.matteChannels > 1)
                    {
                        throw FileSystem::Error(DJV_TEXT("File not supported."));
                    }
                    if (header.matteChannelType != header.colorChannelType)
                    {
                        throw FileSystem::Error(DJV_TEXT("File not supported."));
                    }
                    if (header.matteBitDepth != header.colorBitDepth)
                    {
                        throw FileSystem::Error(DJV_TEXT("File not supported."));
                    }
                    Image::Type type = Image::Type::None;
                    if (3 == header.colorChannelType)
                    {
                        type = Image::getFloatType(header.colorChannels + header.matteChannels, header.colorBitDepth);
                    }
                    else
                    {
                        type = Image::getIntType(header.colorChannels + header.matteChannels, header.colorBitDepth);
                        if (Image::DataType::U32 == Image::getDataType(type))
                        {
                            throw FileSystem::Error(DJV_TEXT("File not supported."));
                        }
                    }
                    if (Image::Type::None == type)
                    {
                        throw FileSystem::Error(DJV_TEXT("File not supported."));
                    }
                    if (header.field)
                    {
                        throw FileSystem::Error(DJV_TEXT("File not supported."));
                    }

                    auto imageInfo = Image::Info(w, h, type, Image::Mirror(false, true));
                    auto info = Info(fileName, VideoInfo(imageInfo, _speed, _sequence));
                    return info;
                }

            } // namespace RLA
        } // namespace IO
    } // namespace AV
} // namespace djv
