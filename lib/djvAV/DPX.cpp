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

#include <djvAV/DPX.h>

#include <djvCore/FileIO.h>
#include <djvCore/Memory.h>
#include <djvCore/String.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace DPX
            {
                namespace
                {
                    void zero(char* in, int size)
                    {
                        memset(in, 0, size);
                    }

                } // namespace

                void zero(Header& header)
                {
                    memset(&header.file, 0xff, sizeof(Header::File));
                    zero(header.file.version, 8);
                    zero(header.file.name, 100);
                    zero(header.file.time, 24);
                    zero(header.file.creator, 100);
                    zero(header.file.project, 200);
                    zero(header.file.copyright, 200);

                    memset(&header.image, 0xff, sizeof(Header::Image));

                    memset(&header.source, 0xff, sizeof(Header::Source));
                    zero(header.source.file, 100);
                    zero(header.source.time, 24);
                    zero(header.source.inputDevice, 32);
                    zero(header.source.inputSerial, 32);

                    memset(&header.film, 0xff, sizeof(Header::Film));
                    zero(header.film.id, 2);
                    zero(header.film.type, 2);
                    zero(header.film.offset, 2);
                    zero(header.film.prefix, 6);
                    zero(header.film.count, 4);
                    zero(header.film.format, 32);
                    zero(header.film.frameId, 32);
                    zero(header.film.slate, 100);

                    memset(&header.tv, 0xff, sizeof(Header::TV));
                }

                void convertEndian(Header& header)
                {
                    Memory::endian(&header.file.imageOffset, 1, 4);
                    Memory::endian(&header.file.size, 1, 4);
                    Memory::endian(&header.file.dittoKey, 1, 4);
                    Memory::endian(&header.file.headerSize, 1, 4);
                    Memory::endian(&header.file.industryHeaderSize, 1, 4);
                    Memory::endian(&header.file.userHeaderSize, 1, 4);
                    Memory::endian(&header.file.encryptionKey, 1, 4);

                    Memory::endian(&header.image.orient, 1, 2);
                    Memory::endian(&header.image.elemSize, 1, 2);
                    Memory::endian(header.image.size, 2, 4);
                    for (size_t i = 0; i < 8; ++i)
                    {
                        Memory::endian(&header.image.elem[i].dataSign, 1, 4);
                        Memory::endian(&header.image.elem[i].lowData, 1, 4);
                        Memory::endian(&header.image.elem[i].lowQuantity, 1, 4);
                        Memory::endian(&header.image.elem[i].highData, 1, 4);
                        Memory::endian(&header.image.elem[i].highQuantity, 1, 4);
                        Memory::endian(&header.image.elem[i].packing, 1, 2);
                        Memory::endian(&header.image.elem[i].encoding, 1, 2);
                        Memory::endian(&header.image.elem[i].dataOffset, 1, 4);
                        Memory::endian(&header.image.elem[i].linePadding, 1, 4);
                        Memory::endian(&header.image.elem[i].elemPadding, 1, 4);
                    }

                    Memory::endian(header.source.offset, 2, 4);
                    Memory::endian(header.source.center, 2, 4);
                    Memory::endian(header.source.size, 2, 4);
                    Memory::endian(header.source.border, 4, 2);
                    Memory::endian(header.source.pixelAspect, 2, 4);
                    Memory::endian(header.source.scanSize, 2, 4);

                    Memory::endian(&header.film.frame, 1, 4);
                    Memory::endian(&header.film.sequence, 1, 4);
                    Memory::endian(&header.film.hold, 1, 4);
                    Memory::endian(&header.film.frameRate, 1, 4);
                    Memory::endian(&header.film.shutter, 1, 4);

                    Memory::endian(&header.tv.timecode, 1, 4);
                    Memory::endian(&header.tv.userBits, 1, 4);
                    Memory::endian(header.tv.sampleRate, 2, 4);
                    Memory::endian(&header.tv.frameRate, 1, 4);
                    Memory::endian(&header.tv.timeOffset, 1, 4);
                    Memory::endian(&header.tv.gamma, 1, 4);
                    Memory::endian(&header.tv.blackLevel, 1, 4);
                    Memory::endian(&header.tv.blackGain, 1, 4);
                    Memory::endian(&header.tv.breakpoint, 1, 4);
                    Memory::endian(&header.tv.whiteLevel, 1, 4);
                    Memory::endian(&header.tv.integrationTimes, 1, 4);
                }

                namespace
                {
                    //! These hard-coded values are meant to catch uninitialized values.
                    const int32_t _intMax   = 1000000;
                    const float   _floatMax = 1000000.f;
                    const char    _minChar  = 32;
                    const char    _maxChar  = 126;
                    const float   _minSpeed = .000001f;

                    bool isValid(const uint8_t* in)
                    {
                        return *in != 0xff;
                    }

                    bool isValid(const uint16_t* in)
                    {
                        return *in != 0xffff;
                    }

                    bool isValid(const uint32_t* in)
                    {
                        return *in != 0xffffffff && *in < static_cast<uint32_t>(_intMax);
                    }

                    bool isValid(const float* in)
                    {
                        return
                            *((uint32_t*)in) != 0xffffffff &&
                            *in > -_floatMax &&
                            *in < _floatMax;
                    }

                    bool isValid(const char* in, int size)
                    {
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
                        const size_t length =
                            maxLen ?
                            std::min(
                                string.length(),
                                maxLen - static_cast<int>(terminate)) :
                                (string.length() + static_cast<int>(terminate));
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

                } // namespace

                Header read(FileSystem::FileIO& io, Info& info, ColorProfile& colorProfile)
                {
                    Header out;
                    zero(out);

                    // Read the file section of the header.
                    io.read(&out.file, sizeof(Header::File));

                    // Check the magic number.
                    Memory::Endian fileEndian = Memory::Endian::First;
                    if (0 == memcmp(&out.file.magic, magic[0], 4))
                    {
                        fileEndian = Memory::Endian::MSB;
                    }
                    else if (0 == memcmp(&out.file.magic, magic[1], 4))
                    {
                        fileEndian = Memory::Endian::LSB;
                    }
                    else
                    {
                        std::stringstream s;
                        s << DJV_TEXT("Unknown magic number.");
                        throw std::runtime_error(s.str());
                    }

                    // Read the reset of the header.
                    io.read(&out.image, sizeof(Header::Image));
                    io.read(&out.source, sizeof(Header::Source));
                    io.read(&out.film, sizeof(Header::Film));
                    io.read(&out.tv, sizeof(Header::TV));

                    // Flip the endian of the data if necessary.
                    if (fileEndian != Memory::getEndian())
                    {
                        io.setEndian(true);
                        convertEndian(out);
                        info.video[0].info.layout.endian = Memory::opposite(Memory::getEndian());
                    }

                    // Collect the information.
                    if (out.image.elemSize != 1)
                    {
                        std::stringstream s;
                        s << DJV_TEXT("Unsupported file.");
                        throw std::runtime_error(s.str());
                    }
                    info.video[0].info.size.w = out.image.size[0];
                    info.video[0].info.size.h = out.image.size[1];

                    switch (static_cast<Orient>(out.image.orient))
                    {
                    case Orient::LeftRightBottomTop:
                        info.video[0].info.layout.mirror.y = true;
                        break;
                    case Orient::RightLeftTopBottom:
                        info.video[0].info.layout.mirror.x = true;
                        break;
                    case Orient::RightLeftBottomTop:
                        info.video[0].info.layout.mirror.x = true;
                        info.video[0].info.layout.mirror.y = true;
                        break;
                    default: break;
                    }

                    info.video[0].info.type = Image::Type::None;
                    switch (static_cast<Components>(out.image.elem[0].packing))
                    {
                    case Components::Pack:
                    {
                        uint8_t channels = 0;
                        switch (static_cast<Descriptor>(out.image.elem[0].descriptor))
                        {
                        case Descriptor::L:    channels = 1; break;
                        case Descriptor::RGB:  channels = 3; break;
                        case Descriptor::RGBA: channels = 4; break;
                        }
                        info.video[0].info.type = Image::getIntType(channels, out.image.elem[0].bitDepth);
                    }
                    break;
                    case Components::TypeA:
                        switch (out.image.elem[0].bitDepth)
                        {
                        case 10:
                            if (Descriptor::RGB == static_cast<Descriptor>(out.image.elem[0].descriptor))
                            {
                                info.video[0].info.type = Image::Type::RGB_U10;
                                info.video[0].info.layout.alignment = 4;
                            }
                            break;
                        case 16:
                        {
                            uint8_t channels = 0;
                            switch (static_cast<Descriptor>(out.image.elem[0].descriptor))
                            {
                            case Descriptor::L:    channels = 1; break;
                            case Descriptor::RGB:  channels = 3; break;
                            case Descriptor::RGBA: channels = 4; break;
                            }
                            info.video[0].info.type = Image::getIntType(channels, out.image.elem[0].bitDepth);
                        }
                        break;
                        }
                        break;
                    }
                    if (Image::Type::None == info.video[0].info.type)
                    {
                        std::stringstream s;
                        s << DJV_TEXT("Unsupported file.");
                        throw std::runtime_error(s.str());
                    }

                    if (out.image.elem[0].encoding)
                    {
                        std::stringstream s;
                        s << DJV_TEXT("Unsupported file.");
                        throw std::runtime_error(s.str());
                    }

                    if (isValid(&out.image.elem[0].linePadding) && out.image.elem[0].linePadding)
                    {
                        std::stringstream s;
                        s << DJV_TEXT("Unsupported file.");
                        throw std::runtime_error(s.str());
                    }

                    if (Transfer::FilmPrint == static_cast<Transfer>(out.image.elem[0].transfer))
                    {
                        colorProfile = ColorProfile::FilmPrint;
                    }

                    if (isValid(out.file.time, 24))
                    {
                        info.tags.setTag("Time", toString(out.file.time, 24));
                    }
                    if (isValid(out.file.creator, 100))
                    {
                        info.tags.setTag("Creator", toString(out.file.creator, 100));
                    }
                    if (isValid(out.file.project, 200))
                    {
                        info.tags.setTag("Project", toString(out.file.project, 200));
                    }
                    if (isValid(out.file.copyright, 200))
                    {
                        info.tags.setTag("Copyright", toString(out.file.copyright, 200));
                    }

                    if (isValid(&out.source.offset[0]) && isValid(&out.source.offset[1]))
                    {
                        std::stringstream ss;
                        ss << out.source.offset[0] << " " << out.source.offset[1];
                        info.tags.setTag("Source Offset", ss.str());
                    }
                    if (isValid(&out.source.center[0]) && isValid(&out.source.center[1]))
                    {
                        std::stringstream ss;
                        ss << out.source.center[0] << " " << out.source.center[1];
                        info.tags.setTag("Source Center", ss.str());
                    }
                    if (isValid(&out.source.size[0]) && isValid(&out.source.size[1]))
                    {
                        std::stringstream ss;
                        ss << out.source.size[0] << " " << out.source.size[1];
                        info.tags.setTag("Source Size", ss.str());
                    }
                    if (isValid(out.source.file, 100))
                    {
                        info.tags.setTag("Source File", toString(out.source.file, 100));
                    }
                    if (isValid(out.source.time, 24))
                    {
                        info.tags.setTag("Source Time", toString(out.source.time, 24));
                    }
                    if (isValid(out.source.inputDevice, 32))
                    {
                        info.tags.setTag("Source Input Device", toString(out.source.inputDevice, 32));
                    }
                    if (isValid(out.source.inputSerial, 32))
                    {
                        info.tags.setTag("Source Input Serial", toString(out.source.inputSerial, 32));
                    }
                    if (isValid(&out.source.border[0]) && isValid(&out.source.border[1]) &&
                        isValid(&out.source.border[2]) && isValid(&out.source.border[3]))
                    {
                        std::stringstream ss;
                        ss << out.source.border[0] << " ";
                        ss << out.source.border[1] << " ";
                        ss << out.source.border[2] << " ";
                        ss << out.source.border[3];
                        info.tags.setTag("Source Border", ss.str());
                    }
                    if (isValid(&out.source.pixelAspect[0]) && isValid(&out.source.pixelAspect[1]))
                    {
                        std::stringstream ss;
                        ss << out.source.pixelAspect[0] << " " << out.source.pixelAspect[1];
                        info.tags.setTag("Source Pixel Aspect", ss.str());
                    }
                    if (isValid(&out.source.scanSize[0]) && isValid(&out.source.scanSize[1]))
                    {
                        std::stringstream ss;
                        ss << out.source.scanSize[0] << " " << out.source.scanSize[1];
                        info.tags.setTag("Source Scan Size", ss.str());
                    }

                    if (isValid(out.film.id, 2) && isValid(out.film.type, 2) &&
                        isValid(out.film.offset, 2) && isValid(out.film.prefix, 6) &&
                        isValid(out.film.count, 4))
                    {
                        info.tags.setTag("Keycode", Time::keycodeToString(
                            std::stoi(std::string(out.film.id, 2)),
                            std::stoi(std::string(out.film.type, 2)),
                            std::stoi(std::string(out.film.prefix, 6)),
                            std::stoi(std::string(out.film.count, 4)),
                            std::stoi(std::string(out.film.offset, 2))));
                    }
                    if (isValid(out.film.format, 32))
                    {
                        info.tags.setTag("Film Format", std::string(out.film.format, 32));
                    }
                    if (isValid(&out.film.frame))
                    {
                        std::stringstream ss;
                        ss << out.film.frame;
                        info.tags.setTag("Film Frame", ss.str());
                    }
                    if (isValid(&out.film.sequence))
                    {
                        std::stringstream ss;
                        ss << out.film.sequence;
                        info.tags.setTag("Film Sequence", ss.str());
                    }
                    if (isValid(&out.film.hold))
                    {
                        std::stringstream ss;
                        ss << out.film.hold;
                        info.tags.setTag("Film Hold", ss.str());
                    }
                    if (isValid(&out.film.frameRate) && out.film.frameRate > _minSpeed)
                    {
                        info.video[0].speed = Time::Speed(out.film.frameRate);
                        std::stringstream ss;
                        ss << out.film.frameRate;
                        info.tags.setTag("Film Frame Rate", ss.str());
                    }
                    if (isValid(&out.film.shutter))
                    {
                        std::stringstream ss;
                        ss << out.film.shutter;
                        info.tags.setTag("Film Shutter", ss.str());
                    }
                    if (isValid(out.film.frameId, 32))
                    {
                        info.tags.setTag("Film Frame ID", std::string(out.film.frameId, 32));
                    }
                    if (isValid(out.film.slate, 100))
                    {
                        info.tags.setTag("Film Slate", std::string(out.film.slate, 100));
                    }

                    if (isValid(&out.tv.timecode))
                    {
                        info.tags.setTag("Timecode", Time::timecodeToString(out.tv.timecode));
                    }
                    if (isValid(&out.tv.interlace))
                    {
                        std::stringstream ss;
                        ss << static_cast<unsigned int>(out.tv.interlace);
                        info.tags.setTag("TV Interlace", ss.str());
                    }
                    if (isValid(&out.tv.field))
                    {
                        std::stringstream ss;
                        ss << static_cast<unsigned int>(out.tv.field);
                        info.tags.setTag("TV Field", ss.str());
                    }
                    if (isValid(&out.tv.videoSignal))
                    {
                        std::stringstream ss;
                        ss << static_cast<unsigned int>(out.tv.videoSignal);
                        info.tags.setTag("TV Video Signal", ss.str());
                    }
                    if (isValid(&out.tv.sampleRate[0]) && isValid(&out.tv.sampleRate[1]))
                    {
                        std::stringstream ss;
                        ss << out.tv.sampleRate[0] << " " << out.tv.sampleRate[1];
                        info.tags.setTag("TV Sample Rate", ss.str());
                    }
                    if (isValid(&out.tv.frameRate) && out.tv.frameRate > _minSpeed)
                    {
                        info.video[0].speed = Time::Speed(out.tv.frameRate);
                        std::stringstream ss;
                        ss << out.tv.frameRate;
                        info.tags.setTag("TV Frame Rate", ss.str());
                    }
                    if (isValid(&out.tv.timeOffset))
                    {
                        std::stringstream ss;
                        ss << out.tv.timeOffset;
                        info.tags.setTag("TV Time Offset", ss.str());
                    }
                    if (isValid(&out.tv.gamma))
                    {
                        std::stringstream ss;
                        ss << out.tv.gamma;
                        info.tags.setTag("TV Gamma", ss.str());
                    }
                    if (isValid(&out.tv.blackLevel))
                    {
                        std::stringstream ss;
                        ss << out.tv.blackLevel;
                        info.tags.setTag("TV Black Level", ss.str());
                    }
                    if (isValid(&out.tv.blackGain))
                    {
                        std::stringstream ss;
                        ss << out.tv.blackGain;
                        info.tags.setTag("TV Black Gain", ss.str());
                    }
                    if (isValid(&out.tv.breakpoint))
                    {
                        std::stringstream ss;
                        ss << out.tv.breakpoint;
                        info.tags.setTag("TV Breakpoint", ss.str());
                    }
                    if (isValid(&out.tv.whiteLevel))
                    {
                        std::stringstream ss;
                        ss << out.tv.whiteLevel;
                        info.tags.setTag("TV White Level", ss.str());
                    }
                    if (isValid(&out.tv.integrationTimes))
                    {
                        std::stringstream ss;
                        ss << out.tv.integrationTimes;
                        info.tags.setTag("TV Integration Times", ss.str());
                    }

                    // Set the file position.
                    if (out.file.imageOffset)
                    {
                        io.setPos(out.file.imageOffset);
                    }

                    return out;
                }

                void write(FileSystem::FileIO& io, const Info& info, Version version, Endian endian, ColorProfile colorProfile)
                {
                    Header header;
                    zero(header);

                    switch (version)
                    {
                    case Version::_1_0: memcpy(header.file.version, "V1.0", 4); break;
                    case Version::_2_0: memcpy(header.file.version, "V2.0", 4); break;
                    }

                    fromString(info.fileName, header.file.name, 100, false);
                    header.file.imageOffset         = 2048;
                    header.file.headerSize          = 2048 - 384;
                    header.file.industryHeaderSize  = 384;
                    header.file.userHeaderSize      = 0;
                    header.file.size                = 0;
                    header.file.dittoKey            = 0;
                    header.file.encryptionKey       = 0;

                    header.image.elemSize = 1;
                    header.image.size[0]  = info.video[0].info.size.w;
                    header.image.size[1]  = info.video[0].info.size.w;
                    header.image.orient   = static_cast<uint16_t>(Orient::LeftRightTopBottom);

                    switch (info.video[0].info.type)
                    {
                    case Image::Type::L_U8:
                    case Image::Type::L_U16:
                    case Image::Type::L_F16:
                    case Image::Type::L_F32:
                        header.image.elem[0].descriptor = static_cast<uint8_t>(Descriptor::L);
                        break;
                    case Image::Type::RGB_U8:
                    case Image::Type::RGB_U10:
                    case Image::Type::RGB_U16:
                    case Image::Type::RGB_F16:
                    case Image::Type::RGB_F32:
                        header.image.elem[0].descriptor = static_cast<uint8_t>(Descriptor::RGB);
                        break;
                    case Image::Type::RGBA_U8:
                    case Image::Type::RGBA_U16:
                    case Image::Type::RGBA_F16:
                    case Image::Type::RGBA_F32:
                        header.image.elem[0].descriptor = static_cast<uint8_t>(Descriptor::RGBA);
                        break;
                    default: break;
                    }

                    switch (info.video[0].info.type)
                    {
                    case Image::Type::RGB_U10:
                        header.image.elem[0].packing = static_cast<uint16_t>(Components::TypeA);
                        break;
                    default: break;
                    }

                    const int bitDepth = Image::getBitDepth(info.video[0].info.type);
                    header.image.elem[0].bitDepth = bitDepth;
                    header.image.elem[0].dataSign = 0;
                    header.image.elem[0].lowData  = 0;
                    switch (bitDepth)
                    {
                    case 8:  header.image.elem[0].highData =   255; break;
                    case 10: header.image.elem[0].highData =  1023; break;
                    case 12: header.image.elem[0].highData =  4095; break;
                    case 16: header.image.elem[0].highData = 65535; break;
                    }

                    switch (colorProfile)
                    {
                    case ColorProfile::FilmPrint:
                        header.image.elem[0].transfer = static_cast<uint8_t>(Transfer::FilmPrint);
                        switch (version)
                        {
                        case Version::_1_0:
                            header.image.elem[0].colorimetric = static_cast<uint8_t>(Colorimetric_1_0::FilmPrint);
                            break;
                        default:
                            header.image.elem[0].colorimetric = static_cast<uint8_t>(Colorimetric_1_0::User);
                            break;
                        }
                        break;
                    default:
                        header.image.elem[0].transfer = static_cast<uint8_t>(Transfer::Linear);
                        switch (version)
                        {
                        case Version::_2_0:
                            header.image.elem[0].colorimetric = static_cast<uint8_t>(Colorimetric_2_0::FilmPrint);
                            break;
                        default:
                            header.image.elem[0].colorimetric = static_cast<uint8_t>(Colorimetric_2_0::User);
                            break;
                        }
                        break;
                    }

                    header.image.elem[0].encoding    = 0;
                    header.image.elem[0].dataOffset  = 2048;
                    header.image.elem[0].linePadding = 0;
                    header.image.elem[0].elemPadding = 0;

                    if (info.tags.hasTag("Time"))
                    {
                        fromString(info.tags.getTag("Time"), header.file.time, 24, false);
                    }
                    if (info.tags.hasTag("Creator"))
                    {
                        fromString(info.tags.getTag("Creator"), header.file.time, 100, false);
                    }
                    if (info.tags.hasTag("Project"))
                    {
                        fromString(info.tags.getTag("Project"), header.file.time, 200, false);
                    }
                    if (info.tags.hasTag("Copyright"))
                    {
                        fromString(info.tags.getTag("Copyright"), header.file.time, 200, false);
                    }
                    if (info.tags.hasTag("Creator"))
                    {
                        fromString(info.tags.getTag("Creator"), header.file.time, 100, false);
                    }

                    if (info.tags.hasTag("Source Offset"))
                    {
                        std::stringstream ss(info.tags.getTag("Source Offset"));
                        ss >> header.source.offset[0];
                        ss >> header.source.offset[1];
                    }
                    if (info.tags.hasTag("Source Center"))
                    {
                        std::stringstream ss(info.tags.getTag("Source Center"));
                        ss >> header.source.center[0];
                        ss >> header.source.center[1];
                    }
                    if (info.tags.hasTag("Source Size"))
                    {
                        std::stringstream ss(info.tags.getTag("Source Size"));
                        ss >> header.source.size[0];
                        ss >> header.source.size[1];
                    }
                    if (info.tags.hasTag("Source File"))
                    {
                        fromString(info.tags.getTag("Source File"), header.source.file, 100, false);
                    }
                    if (info.tags.hasTag("Source Time"))
                    {
                        fromString(info.tags.getTag("Source Time"), header.source.time, 24, false);
                    }
                    if (info.tags.hasTag("Source Input Device"))
                    {
                        fromString(info.tags.getTag("Source Input Device"), header.source.inputDevice, 32, false);
                    }
                    if (info.tags.hasTag("Source Input Serial"))
                    {
                        fromString(info.tags.getTag("Source Input Serial"), header.source.inputSerial, 32, false);
                    }
                    if (info.tags.hasTag("Source Border"))
                    {
                        std::stringstream ss(info.tags.getTag("Source Border"));
                        ss >> header.source.border[0];
                        ss >> header.source.border[1];
                        ss >> header.source.border[2];
                        ss >> header.source.border[3];
                    }
                    if (info.tags.hasTag("Source Pixel Aspect"))
                    {
                        std::stringstream ss(info.tags.getTag("Source Pixel Aspect"));
                        ss >> header.source.pixelAspect[0];
                        ss >> header.source.pixelAspect[1];
                    }
                    if (info.tags.hasTag("Source Scan Size"))
                    {
                        std::stringstream ss(info.tags.getTag("Source Scan Size"));
                        ss >> header.source.scanSize[0];
                        ss >> header.source.scanSize[1];
                    }

                    if (info.tags.hasTag("Keycode"))
                    {
                        int id = 0, type = 0, prefix = 0, count = 0, offset = 0;
                        Time::stringToKeycode(info.tags.getTag("Keycode"), id, type, prefix, count, offset);
                        {
                            std::stringstream ss;
                            ss << id;
                            fromString(ss.str(), header.film.id, 2, false);
                        }
                        {
                            std::stringstream ss;
                            ss << type;
                            fromString(ss.str(), header.film.type, 2, false);
                        }
                        {
                            std::stringstream ss;
                            ss << offset;
                            fromString(ss.str(), header.film.offset, 2, false);
                        }
                        {
                            std::stringstream ss;
                            ss << prefix;
                            fromString(ss.str(), header.film.prefix, 6, false);
                        }
                        {
                            std::stringstream ss;
                            ss << count;
                            fromString(ss.str(), header.film.count, 4, false);
                        }
                    }
                    if (info.tags.hasTag("Film Format"))
                    {
                        fromString(info.tags.getTag("Film Format"), header.film.format, 32, false);
                    }
                    if (info.tags.hasTag("Film Frame"))
                    {
                        header.film.frame = std::stoi(info.tags.getTag("Film Frame"));
                    }
                    if (info.tags.hasTag("Film Sequence"))
                    {
                        header.film.sequence = std::stoi(info.tags.getTag("Film Sequence"));
                    }
                    if (info.tags.hasTag("Film Hold"))
                    {
                        header.film.hold = std::stoi(info.tags.getTag("Film Hold"));
                    }
                    if (info.tags.hasTag("Film Frame Rate"))
                    {
                        header.film.frameRate = std::stof(info.tags.getTag("Film Frame Rate"));
                    }
                    if (info.tags.hasTag("Film Shutter"))
                    {
                        header.film.shutter = std::stof(info.tags.getTag("Film Shutter"));
                    }
                    if (info.tags.hasTag("Film Frame ID"))
                    {
                        fromString(info.tags.getTag("Film Frame ID"), header.film.frameId, 32, false);
                    }
                    if (info.tags.hasTag("Film Slate"))
                    {
                        fromString(info.tags.getTag("Film Slate"), header.film.slate, 100, false);
                    }

                    if (info.tags.hasTag("Timecode"))
                    {
                        Time::stringToTimecode(info.tags.getTag("Timecode"), header.tv.timecode);
                    }
                    if (info.tags.hasTag("TV Interlace"))
                    {
                        header.tv.interlace = std::stoi(info.tags.getTag("TV Interlace"));
                    }
                    if (info.tags.hasTag("TV Field"))
                    {
                        header.tv.field = std::stoi(info.tags.getTag("TV Field"));
                    }
                    if (info.tags.hasTag("TV Video Signal"))
                    {
                        header.tv.videoSignal = std::stoi(info.tags.getTag("TV Video Signal"));
                    }
                    if (info.tags.hasTag("TV Sample Rate"))
                    {
                        std::stringstream ss(info.tags.getTag("TV Sample Rate"));
                        ss >> header.tv.sampleRate[0];
                        ss >> header.tv.sampleRate[1];
                    }
                    if (info.tags.hasTag("TV Frame Rate"))
                    {
                        header.tv.videoSignal = std::stof(info.tags.getTag("TV Frame Rate"));
                    }
                    if (info.tags.hasTag("TV Time Offset"))
                    {
                        header.tv.videoSignal = std::stof(info.tags.getTag("TV Time Offset"));
                    }
                    if (info.tags.hasTag("TV Gamma"))
                    {
                        header.tv.gamma = std::stof(info.tags.getTag("TV Gamma"));
                    }
                    if (info.tags.hasTag("TV Black Level"))
                    {
                        header.tv.blackLevel = std::stof(info.tags.getTag("TV Black Level"));
                    }
                    if (info.tags.hasTag("TV Black Gain"))
                    {
                        header.tv.blackGain = std::stof(info.tags.getTag("TV Black Gain"));
                    }
                    if (info.tags.hasTag("TV Break Point"))
                    {
                        header.tv.breakpoint = std::stof(info.tags.getTag("TV Break Point"));
                    }
                    if (info.tags.hasTag("TV White Level"))
                    {
                        header.tv.whiteLevel = std::stof(info.tags.getTag("TV White Level"));
                    }
                    if (info.tags.hasTag("TV Integration Times"))
                    {
                        header.tv.integrationTimes = std::stof(info.tags.getTag("TV Integration Times"));
                    }

                    Memory::Endian fileEndian = Memory::getEndian();
                    switch (endian)
                    {
                    case Endian::MSB: fileEndian = Memory::Endian::MSB; break;
                    case Endian::LSB: fileEndian = Memory::Endian::LSB; break;
                    default: break;
                    }
                    if (fileEndian != Memory::getEndian())
                    {
                        io.setEndian(true);
                        convertEndian(header);
                    }
                    memcpy(
                        &header.file.magic,
                        Memory::Endian::MSB == fileEndian ? magic[0] : magic[1],
                        4);
                    io.write(&header.file, sizeof(Header::File));
                    io.write(&header.image, sizeof(Header::Image));
                    io.write(&header.source, sizeof(Header::Source));
                    io.write(&header.film, sizeof(Header::Film));
                    io.write(&header.tv, sizeof(Header::TV));
                }

                void writeFinish(FileSystem::FileIO& io)
                {
                    const uint32_t size = static_cast<uint32_t>(io.getPos());
                    io.setPos(12);
                    io.writeU32(size);
                }

                struct Plugin::Private
                {
                    Settings settings;
                };

                Plugin::Plugin() :
                    _p(new Private)
                {}

                std::shared_ptr<Plugin> Plugin::create(
                    const std::shared_ptr<ResourceSystem>& resourceSystem,
                    const std::shared_ptr<LogSystem>& logSystem)
                {
                    auto out = std::shared_ptr<Plugin>(new Plugin);
                    out->_init(
                        pluginName,
                        DJV_TEXT("This plugin provides DPX image I/O."),
                        fileExtensions,
                        resourceSystem,
                        logSystem);
                    return out;
                }

                picojson::value Plugin::getOptions() const
                {
                    return toJSON(_p->settings);
                }

                void Plugin::setOptions(const picojson::value & value)
                {
                    fromJSON(value, _p->settings);
                }

                std::shared_ptr<IRead> Plugin::read(const std::string & fileName) const
                {
                    return Read::create(fileName, _resourceSystem, _logSystem);
                }

                std::shared_ptr<IWrite> Plugin::write(const std::string & fileName, const Info & info) const
                {
                    return Write::create(fileName, _p->settings, info, _resourceSystem, _logSystem);
                }

            } // namespace DPX
        } // namespace IO
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        AV::IO::DPX,
        Version,
        DJV_TEXT("1.0"),
        DJV_TEXT("2.0"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        AV::IO::DPX,
        Endian,
        DJV_TEXT("Auto"),
        DJV_TEXT("MSB"),
        DJV_TEXT("LSB"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        AV::IO::DPX,
        ColorProfile,
        DJV_TEXT("Raw"),
        DJV_TEXT("FilmPrint"));

    picojson::value toJSON(const AV::IO::DPX::Settings& value)
    {
        picojson::value out(picojson::object_type, true);
        {
            {
                std::stringstream ss;
                ss << value.colorProfile;
                out.get<picojson::object>()["Version"] = picojson::value(ss.str());
                out.get<picojson::object>()["Endian"] = picojson::value(ss.str());
                out.get<picojson::object>()["ColorProfile"] = picojson::value(ss.str());
            }
        }
        return out;
    }

    void fromJSON(const picojson::value& value, AV::IO::DPX::Settings& out)
    {
        if (value.is<picojson::object>())
        {
            for (const auto& i : value.get<picojson::object>())
            {
                if ("Version" == i.first)
                {
                    std::stringstream ss(i.second.get<std::string>());
                    ss >> out.version;
                }
                else if ("Endian" == i.first)
                {
                    std::stringstream ss(i.second.get<std::string>());
                    ss >> out.endian;
                }
                else if ("ColorProfile" == i.first)
                {
                    std::stringstream ss(i.second.get<std::string>());
                    ss >> out.colorProfile;
                }
            }
        }
        else
        {
            throw std::invalid_argument(DJV_TEXT("Cannot parse the value."));
        }
    }

} // namespace djv

