// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/Cineon.h>

#include <djvCore/FileIO.h>
#include <djvCore/FileSystem.h>
#include <djvCore/Memory.h>
#include <djvCore/String.h>
#include <djvCore/StringFormat.h>
#include <djvCore/TextSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace Cineon
            {
                namespace
                {
                    void zero(int32_t* value)
                    {
                        *((uint32_t*)value) = 0x80000000;
                    }

                    void zero(float* value)
                    {
                        *((uint32_t*)value) = 0x7F800000;
                    }

                    void zero(char* value, size_t size)
                    {
                        memset(value, 0, size);
                    }

                } // namespace

                void zero(Header& header)
                {
                    memset(&header.file, 0xff, sizeof(Header::File));
                    zero(header.file.version, 8);
                    zero(header.file.name, 100);
                    zero(header.file.time, 24);

                    memset(&header.image, 0xff, sizeof(Header::Image));

                    for (uint8_t i = 0; i < 8; ++i)
                    {
                        zero(&header.image.channel[i].lowData);
                        zero(&header.image.channel[i].lowQuantity);
                        zero(&header.image.channel[i].highData);
                        zero(&header.image.channel[i].highQuantity);
                    }

                    memset(&header.source, 0xff, sizeof(Header::Source));
                    zero(&header.source.offset[0]);
                    zero(&header.source.offset[1]);
                    zero(header.source.file, 100);
                    zero(header.source.time, 24);
                    zero(header.source.inputDevice, 64);
                    zero(header.source.inputModel, 32);
                    zero(header.source.inputSerial, 32);
                    zero(&header.source.inputPitch[0]);
                    zero(&header.source.inputPitch[1]);
                    zero(&header.source.gamma);

                    memset(&header.film, 0xff, sizeof(Header::Film));
                    zero(header.film.format, 32);
                    zero(&header.film.frameRate);
                    zero(header.film.frameId, 32);
                    zero(header.film.slate, 200);
                }

                void convertEndian(Header& header)
                {
                    Memory::endian(&header.file.imageOffset, 1, 4);
                    Memory::endian(&header.file.headerSize, 1, 4);
                    Memory::endian(&header.file.industryHeaderSize, 1, 4);
                    Memory::endian(&header.file.userHeaderSize, 1, 4);
                    Memory::endian(&header.file.size, 1, 4);

                    for (uint8_t i = 0; i < 8; ++i)
                    {
                        Memory::endian(header.image.channel[i].size, 2, 4);
                        Memory::endian(&header.image.channel[i].lowData, 1, 4);
                        Memory::endian(&header.image.channel[i].lowQuantity, 1, 4);
                        Memory::endian(&header.image.channel[i].highData, 1, 4);
                        Memory::endian(&header.image.channel[i].highQuantity, 1, 4);
                    }

                    Memory::endian(header.image.white, 2, 4);
                    Memory::endian(header.image.red, 2, 4);
                    Memory::endian(header.image.green, 2, 4);
                    Memory::endian(header.image.blue, 2, 4);
                    Memory::endian(&header.image.linePadding, 1, 4);
                    Memory::endian(&header.image.channelPadding, 1, 4);

                    Memory::endian(header.source.offset, 2, 4);
                    Memory::endian(header.source.inputPitch, 2, 4);
                    Memory::endian(&header.source.gamma, 1, 4);

                    Memory::endian(&header.film.prefix, 1, 4);
                    Memory::endian(&header.film.count, 1, 4);
                    Memory::endian(&header.film.frame, 1, 4);
                    Memory::endian(&header.film.frameRate, 1, 4);
                }

                bool isValid(const char* in, size_t size)
                {
                    const char _minChar = 32;
                    const char _maxChar = 126;
                    const char* p = in;
                    const char* const end = p + size;
                    for (; *p && p < end; ++p)
                    {
                        if (*p < _minChar || *p > _maxChar)
                        {
                            return false;
                        }
                    }
                    return size ? (in[0] != 0) : false;
                }

                std::string toString(const char* in, size_t size)
                {
                    const char* p = in;
                    const char* const end = p + size;
                    for (; *p && p < end; ++p)
                        ;
                    return std::string(in, p - in);
                }

                size_t fromString(
                    const std::string& string,
                    char*              out,
                    size_t             maxLen,
                    bool               terminate)
                {
                    DJV_ASSERT(maxLen >= 0);
                    const char* c = string.c_str();
                    const size_t length = std::min(string.length(), maxLen - static_cast<int>(terminate));
                    size_t i = 0;
                    for (; i < length; ++i)
                    {
                        out[i] = c[i];
                    }
                    if (terminate)
                    {
                        out[i++] = 0;
                    }
                    return i;
                }

                namespace
                {
                    bool isValid(const uint8_t* in)
                    {
                        return *in != 0xff;
                    }

                    // Constants to catch uninitialized values.
                    const int32_t _intMax   = 1000000;
                    const float   _floatMax = 1000000.F;
                    const float   _minSpeed = .000001F;

                    bool isValid(const uint32_t* in)
                    {
                        return
                            *in != 0xffffffff &&
                            *in < static_cast<uint32_t>(_intMax);
                    }

                    bool isValid(const int32_t* in)
                    {
                        return
                            *in != static_cast<int32_t>(0x80000000) &&
                            *in > -_intMax &&
                            *in < _intMax;
                    }

                    bool isValid(const float* in)
                    {
                        return
                            *(reinterpret_cast<const uint32_t*>(in)) != 0x7F800000 &&
                            *in > -_floatMax &&
                            *in < _floatMax;
                    }

                } // namespace

                Header read(
                    const std::shared_ptr<FileSystem::FileIO>& io,
                    Info& info,
                    ColorProfile& colorProfile,
                    const std::shared_ptr<TextSystem>& textSystem)
                {
                    Header out;
                    zero(out);

                    info.fileName = io->getFileName();

                    // Read the file section of the header.
                    io->read(&out.file, sizeof(Header::File));

                    // Check the magic number.
                    bool flipEndian = false;
                    if (magic[0] == out.file.magic)
                        ;
                    else if (magic[1] == out.file.magic)
                    {
                        flipEndian = true;
                    }
                    else
                    {
                        throw FileSystem::Error(String::Format("{0}: {1}").
                            arg(io->getFileName()).
                            arg(textSystem->getText(DJV_TEXT("error_bad_magic_number"))));
                    }

                    // Read the rest of the header.
                    io->read(&out.image, sizeof(Header::Image));
                    io->read(&out.source, sizeof(Header::Source));
                    io->read(&out.film, sizeof(Header::Film));

                    // Flip the endian of the data if necessary.
                    if (flipEndian)
                    {
                        io->setEndianConversion(true);
                        convertEndian(out);
                        info.video[0].layout.endian = Memory::opposite(Memory::getEndian());
                    }

                    // Read the image section of the header.
                    if (!out.image.channels)
                    {
                        throw FileSystem::Error(String::Format("{0}: {1}").
                            arg(io->getFileName()).
                            arg(textSystem->getText(DJV_TEXT("error_no_image_channels"))));
                    }
                    uint8_t i = 1;
                    for (; i < out.image.channels; ++i)
                    {
                        if ((out.image.channel[i].size[0] != out.image.channel[0].size[0]) ||
                            (out.image.channel[i].size[1] != out.image.channel[0].size[1]))
                        {
                            break;
                        }
                        if (out.image.channel[i].bitDepth != out.image.channel[0].bitDepth)
                        {
                            break;
                        }
                    }
                    if (i < out.image.channels)
                    {
                        throw FileSystem::Error(String::Format("{0}: {1}").
                            arg(io->getFileName()).
                            arg(textSystem->getText(DJV_TEXT("error_image_channels_same_size_and_bit_depth"))));
                    }
                    Image::Type imageType = Image::Type::None;
                    switch (out.image.channels)
                    {
                    case 3:
                        switch (out.image.channel[0].bitDepth)
                        {
                        case 10:
                            imageType = Image::Type::RGB_U10;
                            break;
                        default: break;
                        }
                        break;
                    default: break;
                    }
                    if (Image::Type::None == imageType)
                    {
                        throw FileSystem::Error(String::Format("{0}: {1}").
                            arg(io->getFileName()).
                            arg(textSystem->getText(DJV_TEXT("error_unsupported_bit_depth"))));
                    }
                    if (isValid(&out.image.linePadding) && out.image.linePadding)
                    {
                        throw FileSystem::Error(String::Format("{0}: {1}").
                            arg(io->getFileName()).
                            arg(textSystem->getText(DJV_TEXT("error_line_padding_unsupported"))));
                    }
                    if (isValid(&out.image.channelPadding) && out.image.channelPadding)
                    {
                        throw FileSystem::Error(String::Format("{0}: {1}").
                            arg(io->getFileName()).
                            arg(textSystem->getText(DJV_TEXT("error_channel_padding_unsupported"))));
                    }

                    // Collect information.
                    info.video[0].type = imageType;
                    info.video[0].size.w = out.image.channel[0].size[0];
                    info.video[0].size.h = out.image.channel[0].size[1];
                    if (io->getSize() - out.file.imageOffset != info.video[0].getDataByteCount())
                    {
                        throw FileSystem::Error(String::Format("{0}: {1}").
                            arg(io->getFileName()).
                            arg(textSystem->getText(DJV_TEXT("error_incomplete_file"))));
                    }
                    switch (static_cast<Orient>(out.image.orient))
                    {
                    case Orient::LeftRightBottomTop:
                        info.video[0].layout.mirror.y = true;
                        break;
                    case Orient::RightLeftTopBottom:
                        info.video[0].layout.mirror.x = true;
                        break;
                    case Orient::RightLeftBottomTop:
                        info.video[0].layout.mirror.x = true;
                        info.video[0].layout.mirror.y = true;
                        break;
                    case Orient::TopBottomLeftRight:
                    case Orient::TopBottomRightLeft:
                    case Orient::BottomTopLeftRight:
                    case Orient::BottomTopRightLeft:
                        //! \todo Implement these image orientations.
                        break;
                    default: break;
                    }
                    if (isValid(out.file.time, 24))
                    {
                        info.tags.set("Time", toString(out.file.time, 24));
                    }
                    if (isValid(&out.source.offset[0]) && isValid(&out.source.offset[1]))
                    {
                        std::stringstream ss;
                        ss << out.source.offset[0] << " " << out.source.offset[1];
                        info.tags.set("Source Offset", ss.str());
                    }
                    if (isValid(out.source.file, 100))
                    {
                        info.tags.set("Source File", toString(out.source.file, 100));
                    }
                    if (isValid(out.source.time, 24))
                    {
                        info.tags.set("Source Time", toString(out.source.time, 24));
                    }
                    if (isValid(out.source.inputDevice, 64))
                    {
                        info.tags.set("Source Input Device", toString(out.source.inputDevice, 64));
                    }
                    if (isValid(out.source.inputModel, 32))
                    {
                        info.tags.set("Source Input Model", toString(out.source.inputModel, 32));
                    }
                    if (isValid(out.source.inputSerial, 32))
                    {
                        info.tags.set("Source Input Serial", toString(out.source.inputSerial, 32));
                    }
                    if (isValid(&out.source.inputPitch[0]) && isValid(&out.source.inputPitch[1]))
                    {
                        std::stringstream ss;
                        ss << out.source.inputPitch[0] << " " << out.source.inputPitch[1];
                        info.tags.set("Source Input Pitch", ss.str());
                    }
                    if (isValid(&out.source.gamma))
                    {
                        std::stringstream ss;
                        ss << out.source.gamma;
                        info.tags.set("Source Gamma", ss.str());
                    }
                    if (isValid(&out.film.id) &&
                        isValid(&out.film.type) &&
                        isValid(&out.film.offset) &&
                        isValid(&out.film.prefix) &&
                        isValid(&out.film.count))
                    {
                        info.tags.set( "Keycode", Time::keycodeToString(
                            out.film.id,
                            out.film.type,
                            out.film.prefix,
                            out.film.count,
                            out.film.offset));
                    }
                    if (isValid(out.film.format, 32))
                    {
                        info.tags.set("Film Format", toString(out.film.format, 32));
                    }
                    if (isValid(&out.film.frame))
                    {
                        std::stringstream ss;
                        ss << out.film.frame;
                        info.tags.set("Film Frame", ss.str());
                    }
                    if (isValid(&out.film.frameRate) && out.film.frameRate >= _minSpeed)
                    {
                        info.videoSpeed = Time::fromSpeed(out.film.frameRate);
                        std::stringstream ss;
                        ss << out.film.frameRate;
                        info.tags.set("Film Frame Rate", ss.str());
                    }
                    if (isValid(out.film.frameId, 32))
                    {
                        info.tags.set("Film Frame ID", toString(out.film.frameId, 32));
                    }
                    if (isValid(out.film.slate, 200))
                    {
                        info.tags.set("Film Slate", toString(out.film.slate, 200));
                    }
                    switch (static_cast<Descriptor>(out.image.channel[0].descriptor[1]))
                    {
                    case Descriptor::RedFilmPrint: colorProfile = ColorProfile::FilmPrint; break;
                    default:                       colorProfile = ColorProfile::Raw;       break;
                    }

                    // Set the file position.
                    if (out.file.imageOffset)
                    {
                        io->setPos(out.file.imageOffset);
                    }
                    const size_t ioSize = io->getSize();
                    const size_t ioPos = io->getPos();
                    const size_t fileDataByteCount = ioSize > 0 ? (ioSize - ioPos) : 0;
                    const size_t dataByteCount = info.video[0].getDataByteCount();
                    if (dataByteCount > fileDataByteCount)
                    {
                        throw FileSystem::Error(String::Format("{0}: {1}").
                            arg(io->getFileName()).
                            arg(textSystem->getText(DJV_TEXT("error_incomplete_file"))));
                    }

                    return out;
                }

                void write(
                    const std::shared_ptr<FileSystem::FileIO>& io,
                    const Info& info,
                    ColorProfile colorProfile)
                {
                    Header header;
                    zero(header);
                    
                    // Set the file section.
                    header.file.imageOffset        = 2048;
                    header.file.headerSize         = 1024;
                    header.file.industryHeaderSize = 1024;
                    header.file.userHeaderSize     = 0;

                    // Set the image section.
                    header.image.orient = static_cast<uint8_t>(Orient::LeftRightTopBottom);
                    header.image.channels = 3;
                    if (ColorProfile::FilmPrint == colorProfile)
                    {
                        header.image.channel[0].descriptor[1] = static_cast<uint8_t>(Descriptor::RedFilmPrint);
                        header.image.channel[1].descriptor[1] = static_cast<uint8_t>(Descriptor::GreenFilmPrint);
                        header.image.channel[2].descriptor[1] = static_cast<uint8_t>(Descriptor::BlueFilmPrint);
                    }
                    else
                    {
                        header.image.channel[0].descriptor[1] = static_cast<uint8_t>(Descriptor::Luminance);
                        header.image.channel[1].descriptor[1] = static_cast<uint8_t>(Descriptor::Luminance);
                        header.image.channel[2].descriptor[1] = static_cast<uint8_t>(Descriptor::Luminance);
                    }
                    const uint8_t bitDepth = 10;
                    for (uint8_t i = 0; i < header.image.channels; ++i)
                    {
                        header.image.channel[i].descriptor[0] = 0;
                        header.image.channel[i].bitDepth = bitDepth;
                        header.image.channel[i].size[0] = info.video[0].size.w;
                        header.image.channel[i].size[1] = info.video[0].size.h;

                        header.image.channel[i].lowData = 0;

                        switch (bitDepth)
                        {
                        case  8: header.image.channel[i].highData = Image::U8Range.getMax();  break;
                        case 10: header.image.channel[i].highData = Image::U10Range.getMax(); break;
                        case 12: header.image.channel[i].highData = Image::U12Range.getMax(); break;
                        case 16: header.image.channel[i].highData = Image::U16Range.getMax(); break;
                        default: break;
                        }
                    }
                    header.image.interleave     = 0;
                    header.image.packing        = 5;
                    header.image.dataSign       = 0;
                    header.image.dataSense      = 0;
                    header.image.linePadding    = 0;
                    header.image.channelPadding = 0;

                    // Set the tags.
                    fromString(info.video[0].name, header.file.name, 100, false);
                    if (info.tags.contains("Time"))
                    {
                        fromString(info.tags.get("Time"), header.file.time, 24, false);
                    }
                    if (info.tags.contains("Source Offset"))
                    {
                        std::stringstream ss(info.tags.get("Source Offset"));
                        ss >> header.source.offset[0];
                        ss >> header.source.offset[1];
                    }
                    if (info.tags.contains("Source File"))
                    {
                        fromString(info.tags.get("Source File"), header.source.file, 100, false);
                    }
                    if (info.tags.contains("Source Time"))
                    {
                        fromString(info.tags.get("Source Time"), header.source.time, 24, false);
                    }
                    if (info.tags.contains("Source Input Device"))
                    {
                        fromString(info.tags.get("Source Input Device"), header.source.inputDevice, 64, false);
                    }
                    if (info.tags.contains("Source Input Model"))
                    {
                        fromString(info.tags.get("Source Input Model"), header.source.inputModel, 32, false);
                    }
                    if (info.tags.contains("Source Input Serial"))
                    {
                        fromString(info.tags.get("Source Input Serial"), header.source.inputSerial, 32, false);
                    }
                    if (info.tags.contains("Source Input Pitch"))
                    {
                        std::stringstream ss(info.tags.get("Source Input Pitch"));
                        ss >> header.source.inputPitch[0];
                        ss >> header.source.inputPitch[1];
                    }
                    if (info.tags.contains("Source Gamma"))
                    {
                        std::stringstream ss(info.tags.get("Source Gamma"));
                        ss >> header.source.gamma;
                    }
                    if (info.tags.contains("Keycode"))
                    {
                        int id     = 0;
                        int type   = 0;
                        int prefix = 0;
                        int count  = 0;
                        int offset = 0;
                        Time::stringToKeycode(info.tags.get("Keycode"), id, type, prefix, count, offset);
                        header.film.id     = id;
                        header.film.type   = type;
                        header.film.offset = offset;
                        header.film.prefix = prefix;
                        header.film.count  = count;
                    }
                    if (info.tags.contains("Film Format"))
                    {
                        fromString(info.tags.get("Film Format"), header.film.format, 32, false);
                    }
                    if (info.tags.contains("Film Frame"))
                    {
                        std::stringstream ss(info.tags.get("Film Frame"));
                        ss >> header.film.frame;
                    }
                    if (info.tags.contains("Film Frame Rate"))
                    {
                        std::stringstream ss(info.tags.get("Film Frame Rate"));
                        ss >> header.film.frameRate;
                    }
                    if (info.tags.contains("Film Frame ID"))
                    {
                        fromString(info.tags.get("Film Frame ID"), header.film.frameId, 32, false);
                    }
                    if (info.tags.contains("Film Slate"))
                    {
                        fromString(info.tags.get("Film Slate"), header.film.slate, 200, false);
                    }

                    // Write the header.
                    const bool flipEndian = Memory::getEndian() != Memory::Endian::MSB;
                    io->setEndianConversion(flipEndian);
                    if (flipEndian)
                    {
                        convertEndian(header);
                        header.file.magic = magic[1];
                    }
                    else
                    {
                        header.file.magic = magic[0];
                    }
                    io->write(&header.file, sizeof(Header::File));
                    io->write(&header.image, sizeof(Header::Image));
                    io->write(&header.source, sizeof(Header::Source));
                    io->write(&header.film, sizeof(Header::Film));
                }

                void writeFinish(const std::shared_ptr<FileSystem::FileIO>& io)
                {
                    const uint32_t size = static_cast<uint32_t>(io->getPos());
                    io->setPos(20);
                    io->writeU32(size);
                }

                struct Plugin::Private
                {
                };

                Plugin::Plugin() :
                    _p(new Private)
                {}

                std::shared_ptr<Plugin> Plugin::create(const std::shared_ptr<Context>& context)
                {
                    auto out = std::shared_ptr<Plugin>(new Plugin);
                    out->_init(
                        pluginName,
                        DJV_TEXT("plugin_cineon_io"),
                        fileExtensions,
                        context);
                    return out;
                }

                std::shared_ptr<IRead> Plugin::read(const FileSystem::FileInfo& fileInfo, const ReadOptions& options) const
                {
                    return Read::create(fileInfo, options, _textSystem, _resourceSystem, _logSystem);
                }

                std::shared_ptr<IWrite> Plugin::write(const FileSystem::FileInfo& fileInfo, const Info& info, const WriteOptions& options) const
                {
                    return Write::create(fileInfo, info, options, _textSystem, _resourceSystem, _logSystem);
                }
                
                DJV_ENUM_HELPERS_IMPLEMENTATION(ColorProfile);
                DJV_ENUM_HELPERS_IMPLEMENTATION(Orient);
                DJV_ENUM_HELPERS_IMPLEMENTATION(Descriptor);

            } // namespace Cineon
        } // namespace IO
    } // namespace AV
} // namespace djv

