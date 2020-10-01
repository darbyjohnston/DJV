// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/RLA.h>

#include <djvSystem/File.h>
#include <djvSystem/FileIO.h>
#include <djvSystem/TextSystem.h>

#include <djvCore/StringFormat.h>

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
                    const System::File::Info& fileInfo,
                    const ReadOptions& readOptions,
                    const std::shared_ptr<System::TextSystem>& textSystem,
                    const std::shared_ptr<System::ResourceSystem>& resourceSystem,
                    const std::shared_ptr<System::LogSystem>& logSystem)
                {
                    auto out = std::shared_ptr<Read>(new Read);
                    out->_init(fileInfo, readOptions, textSystem, resourceSystem, logSystem);
                    return out;
                }

                Info Read::_readInfo(const std::string& fileName)
                {
                    auto io = System::File::IO::create();
                    return _open(fileName, io);
                }

                namespace
                {
                    void readRle(
                        const std::shared_ptr<System::File::IO>& io,
                        uint8_t* out,
                        size_t size,
                        size_t channels,
                        size_t bytes)
                    {
                        int16_t _size = 0;
                        io->read16(&_size);
                        std::vector<uint8_t> buf;
                        buf.resize(_size);
                        io->read(buf.data(), _size);
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
                        const std::shared_ptr<System::File::IO>& io,
                        uint8_t* out,
                        size_t size,
                        size_t channels)
                    {
                        int16_t _size = 0;
                        io->read16(&_size);
                        std::vector<uint8_t> buf(_size);
                        io->read(buf.data(), _size);
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

                std::shared_ptr<Image::Image> Read::_readImage(const std::string& fileName)
                {
                    std::shared_ptr<Image::Image> out;
                    auto io = System::File::IO::create();
                    const auto info = _open(fileName, io);
                    out = Image::Image::create(info.video[0]);
                    out->setPluginName(pluginName);

                    const size_t w = info.video[0].size.w;
                    const size_t h = info.video[0].size.h;
                    const size_t channels = Image::getChannelCount(info.video[0].type);
                    const size_t bytes = Image::getByteCount(Image::getDataType(info.video[0].type));
                    const Image::DataType dataType = Image::getDataType(info.video[0].type);
                    uint8_t* dataP = out->getData();
                    for (uint16_t y = 0; y < h; ++y, dataP += w * channels * bytes)
                    {
                        io->setPos(_rleOffset[y]);
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

                Info Read::_open(const std::string& fileName, const std::shared_ptr<System::File::IO>& io)
                {
                    // Open the file.
                    io->setEndianConversion(Memory::getEndian() != Memory::Endian::MSB);
                    io->open(fileName, System::File::Mode::Read);

                    // Read the header.
                    Header header;
                    io->read(&header, sizeof(Header));
                    if (io->hasEndianConversion())
                    {
                        endian(&header);
                    }
                    const int w = header.active[1] - header.active[0] + 1;
                    const int h = header.active[3] - header.active[2] + 1;

                    // Read the scanline table.
                    _rleOffset.resize(h);
                    io->read32(_rleOffset.data(), h);

                    // Get file information.
                    if (header.matteChannels > 1)
                    {
                        throw System::File::Error(String::Format("{0}: {1}").
                            arg(fileName).
                            arg(_textSystem->getText(DJV_TEXT("error_file_not_supported"))));
                    }
                    if (header.matteChannelType != header.colorChannelType)
                    {
                        throw System::File::Error(String::Format("{0}: {1}").
                            arg(fileName).
                            arg(_textSystem->getText(DJV_TEXT("error_file_not_supported"))));
                    }
                    if (header.matteBitDepth != header.colorBitDepth)
                    {
                        throw System::File::Error(String::Format("{0}: {1}").
                            arg(fileName).
                            arg(_textSystem->getText(DJV_TEXT("error_file_not_supported"))));
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
                            throw System::File::Error(String::Format("{0}: {1}").
                                arg(fileName).
                                arg(_textSystem->getText(DJV_TEXT("error_file_not_supported"))));
                        }
                    }
                    if (Image::Type::None == type)
                    {
                        throw System::File::Error(String::Format("{0}: {1}").
                            arg(fileName).
                            arg(_textSystem->getText(DJV_TEXT("error_file_not_supported"))));
                    }
                    if (header.field)
                    {
                        throw System::File::Error(String::Format("{0}: {1}").
                            arg(fileName).
                            arg(_textSystem->getText(DJV_TEXT("error_file_not_supported"))));
                    }

                    Info info;
                    info.fileName = fileName;
                    info.videoSpeed = _speed;
                    info.videoSequence = _sequence;
                    info.video.push_back(Image::Info(w, h, type, Image::Mirror(false, true)));
                    return info;
                }

            } // namespace RLA
        } // namespace IO
    } // namespace AV
} // namespace djv
