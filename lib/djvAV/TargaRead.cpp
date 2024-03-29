// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/Targa.h>

#include <djvSystem/File.h>
#include <djvSystem/FileIO.h>
#include <djvSystem/TextSystem.h>

#include <djvCore/StringFormat.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace Targa
        {
            Read::Read()
            {}

            Read::~Read()
            {
                _finish();
            }

            std::shared_ptr<Read> Read::create(
                const System::File::Info& fileInfo,
                const IO::ReadOptions& readOptions,
                const std::shared_ptr<System::TextSystem>& textSystem,
                const std::shared_ptr<System::ResourceSystem>& resourceSystem,
                const std::shared_ptr<System::LogSystem>& logSystem)
            {
                auto out = std::shared_ptr<Read>(new Read);
                out->_init(fileInfo, readOptions, textSystem, resourceSystem, logSystem);
                return out;
            }

            IO::Info Read::_readInfo(const std::string& fileName)
            {
                auto io = System::File::IO::create();
                return _open(fileName, io);
            }

            namespace
            {
                const uint8_t* readRle(
                    const uint8_t* in,
                    const uint8_t* end,
                    uint8_t*       out,
                    size_t         size,
                    size_t         channels)
                {
                    const uint8_t* const outEnd = out + size * channels;
                    while (out < outEnd)
                    {
                        // Information.
                        if (in > end)
                        {
                            return nullptr;
                        }
                        const uint8_t count  = (*in & 0x7f) + 1;
                        const bool    run    = (*in & 0x80) ? true : false;
                        const size_t  length = run ? 1 : count;
                        ++in;

                        // Unpack.
                        if (in + length * channels > end)
                        {
                            return nullptr;
                        }
                        if (run)
                        {
                            for (size_t j = 0; j < count; ++j, out += channels)
                            {
                                switch (channels)
                                {
                                case 4:
                                    out[3] = in[3];
                                    out[2] = in[2];
                                    out[1] = in[1];
                                    out[0] = in[0];
                                    break;
                                case 3:
                                    out[2] = in[2];
                                    out[1] = in[1];
                                    out[0] = in[0];
                                    break;
                                case 2:
                                    out[1] = in[1];
                                    out[0] = in[0];
                                    break;
                                case 1:
                                    out[0] = in[0];
                                    break;
                                default: break;
                                }
                            }
                            in += channels;
                        }
                        else
                        {
                            for (size_t j = 0; j < count; ++j, in += channels, out += channels)
                            {
                                switch (channels)
                                {
                                case 4:
                                    out[3] = in[3];
                                    out[2] = in[2];
                                    out[1] = in[1];
                                    out[0] = in[0];
                                    break;
                                case 3:
                                    out[2] = in[2];
                                    out[1] = in[1];
                                    out[0] = in[0];
                                    break;
                                case 2:
                                    out[1] = in[1];
                                    out[0] = in[0];
                                    break;
                                case 1:
                                    out[0] = in[0];
                                    break;
                                default: break;
                                }
                            }
                        }
                    }
                    return in;
                }

            } // namespace

            std::shared_ptr<Image::Data> Read::_readImage(const std::string& fileName)
            {
                std::shared_ptr<Image::Data> out;
                auto io = System::File::IO::create();
                const auto info = _open(fileName, io);
                out = Image::Data::create(info.video[0]);
                out->setPluginName(pluginName);

                const Image::Info& imageInfo = info.video[0];
                const size_t channels = Image::getChannelCount(imageInfo.type);
                if (!_compression)
                {
                    io->read(out->getData(), out->getDataByteCount());
                }
                else
                {
                    const size_t tmpSize = io->getSize() - io->getPos();
                    std::vector<uint8_t> tmp(tmpSize);
                    io->read(tmp.data(), tmpSize);
                    const uint8_t* p = tmp.data();
                    const uint8_t* const end = p + tmpSize;
                    for (uint16_t y = 0; y < imageInfo.size.h; ++y)
                    {
                        p = readRle(
                            p,
                            end,
                            out->getData(0, y),
                            imageInfo.size.w,
                            channels);
                        if (!p)
                        {
                            throw System::File::Error(String::Format("{0}: {1}").
                                arg(fileName).
                                arg(_textSystem->getText(DJV_TEXT("error_read_scanline"))));
                        }
                    }
                }

                if (_bgr)
                {
                    for (uint16_t y = 0; y < imageInfo.size.h; ++y)
                    {
                        uint8_t* p = out->getData(0, y);
                        for (uint16_t x = 0; x < imageInfo.size.w; ++x, p += channels)
                        {
                            const uint8_t tmp = p[0];
                            p[0] = p[2];
                            p[2] = tmp;
                        }
                    }
                }

                return out;
            }

            namespace
            {
                class Header
                {
                public:
                    Header();

                    void read(
                        const std::shared_ptr<System::File::IO>&,
                        Image::Info&,
                        bool& bgr,
                        bool& compression,
                        const std::shared_ptr<System::TextSystem>&);

                private:
                    struct Data
                    {
                        uint8_t  idSize;
                        uint8_t  cmapType;
                        uint8_t  imageType;
                        uint16_t cmapStart;
                        uint16_t cmapSize;
                        uint8_t  cmapBits;
                        uint16_t x;
                        uint16_t y;
                        uint16_t width;
                        uint16_t height;
                        uint8_t  pixelBits;
                        uint8_t  descriptor;
                    };
                    Data _data;
                };

                Header::Header()
                {
                    _data.idSize = 0;
                    _data.cmapType = 0;
                    _data.imageType = 0;
                    _data.cmapStart = 0;
                    _data.cmapSize = 0;
                    _data.cmapBits = 0;
                    _data.x = 0;
                    _data.y = 0;
                    _data.width = 0;
                    _data.height = 0;
                    _data.pixelBits = 0;
                    _data.descriptor = 0;
                }

                void Header::read(
                    const std::shared_ptr<System::File::IO>& io,
                    Image::Info& info,
                    bool& bgr,
                    bool& compression,
                    const std::shared_ptr<System::TextSystem>& textSystem)
                {
                    // Read.
                    io->readU8(&_data.idSize);
                    io->readU8(&_data.cmapType);
                    io->readU8(&_data.imageType);
                    io->readU16(&_data.cmapStart);
                    io->readU16(&_data.cmapSize);
                    io->readU8(&_data.cmapBits);
                    io->readU16(&_data.x);
                    io->readU16(&_data.y);
                    io->readU16(&_data.width);
                    io->readU16(&_data.height);
                    io->readU8(&_data.pixelBits);
                    io->readU8(&_data.descriptor);
                    io->seek(_data.idSize);

                    // Information.
                    info.size.w = _data.width;
                    info.size.h = _data.height;
                    info.layout.mirror.x = (_data.descriptor >> 4) & 1;
                    info.layout.mirror.y = !((_data.descriptor >> 5) & 1);
                    const int alphaBits = _data.descriptor & 15;
                    switch (alphaBits)
                    {
                    case 0:
                    case 8: break;
                    default:
                        throw System::File::Error(String::Format("{0}: {1}").
                            arg(io->getFileName()).
                            arg(textSystem->getText(DJV_TEXT("error_file_not_supported"))));
                    }
                    switch (_data.imageType)
                    {
                    case 3:
                    case 11:
                        switch (_data.pixelBits)
                        {
                        case 8:  info.type = Image::Type::L_U8; break;
                        case 16: info.type = Image::Type::LA_U8; break;
                        default: break;
                        }
                        break;
                    case 2:
                    case 10:
                        switch (_data.pixelBits)
                        {
                        case 24:
                            info.type = Image::Type::RGB_U8;
                            bgr = true;
                            break;
                        case 32:
                            info.type = Image::Type::RGBA_U8;
                            bgr = true;
                            break;
                        default: break;
                        }
                        break;
                    default: break;
                    }
                    if (Image::Type::None == info.type)
                    {
                        throw System::File::Error(String::Format("{0}: {1}").
                            arg(io->getFileName()).
                            arg(textSystem->getText(DJV_TEXT("error_file_not_supported"))));
                    }
                    const int bits = _data.pixelBits + alphaBits;
                    if (bits < (Image::getChannelCount(info.type) * 8) || (bits % 8) != 0)
                    {
                        throw System::File::Error(String::Format("{0}: {1}").
                            arg(io->getFileName()).
                            arg(textSystem->getText(DJV_TEXT("error_file_not_supported"))));
                    }
                    compression =
                        10 == _data.imageType ||
                        11 == _data.imageType;
                    info.layout.endian = Memory::Endian::LSB;

                    const size_t ioSize = io->getSize();
                    const size_t ioPos = io->getPos();
                    const size_t fileDataByteCount = ioSize > 0 ? (ioSize - ioPos) : 0;
                    const size_t dataByteCount = info.getDataByteCount();
                    if (!compression && dataByteCount > fileDataByteCount)
                    {
                        throw System::File::Error(String::Format("{0}: {1}").
                            arg(io->getFileName()).
                            arg(textSystem->getText(DJV_TEXT("error_incomplete_file"))));
                    }
                }

            } // namespace

            IO::Info Read::_open(const std::string& fileName, const std::shared_ptr<System::File::IO>& io)
            {
                io->setEndianConversion(Memory::getEndian() != Memory::Endian::LSB);
                io->open(fileName, System::File::Mode::Read);
                Image::Info imageInfo;
                Header().read(io, imageInfo, _bgr, _compression, _textSystem);
                IO::Info info;
                info.fileName = fileName;
                info.videoSpeed = _speed;
                info.videoSequence = _sequence;
                info.video.push_back(imageInfo);
                return info;
            }

        } // namespace Targa
    } // namespace AV
} // namespace djv

