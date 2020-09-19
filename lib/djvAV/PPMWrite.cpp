// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/PPM.h>

#include <djvSystem/FileIO.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace PPM
            {
                struct Write::Private
                {
                    Options options;
                };

                Write::Write() :
                    _p(new Private)
                {}

                Write::~Write()
                {
                    _finish();
                }

                std::shared_ptr<Write> Write::create(
                    const System::File::Info& fileInfo,
                    const Info& info,
                    const WriteOptions& writeOptions,
                    const Options& options,
                    const std::shared_ptr<System::TextSystem>& textSystem,
                    const std::shared_ptr<System::ResourceSystem>& resourceSystem,
                    const std::shared_ptr<System::LogSystem>& logSystem)
                {
                    auto out = std::shared_ptr<Write>(new Write);
                    out->_p->options = options;
                    out->_init(fileInfo, info, writeOptions, textSystem, resourceSystem, logSystem);
                    return out;
                }

                Image::Type Write::_getImageType(Image::Type value) const
                {
                    Image::Type out = Image::Type::None;
                    switch (value)
                    {
                    case Image::Type::L_U8:
                    case Image::Type::L_U16:
                    case Image::Type::RGB_U8:
                    case Image::Type::RGB_U16:  out = value; break;
                    case Image::Type::LA_U8:    out = Image::Type::L_U8; break;
                    case Image::Type::L_U32:
                    case Image::Type::L_F16:
                    case Image::Type::L_F32:
                    case Image::Type::LA_U16:
                    case Image::Type::LA_U32:
                    case Image::Type::LA_F16:
                    case Image::Type::LA_F32:   out = Image::Type::L_U16; break;
                    case Image::Type::RGB_U10:
                    case Image::Type::RGB_U32:
                    case Image::Type::RGB_F16:
                    case Image::Type::RGB_F32:  out = Image::Type::RGB_U16; break;
                    case Image::Type::RGBA_U8:  out = Image::Type::RGB_U8; break;
                    case Image::Type::RGBA_U16:
                    case Image::Type::RGBA_U32:
                    case Image::Type::RGBA_F16:
                    case Image::Type::RGBA_F32: out = Image::Type::RGB_U16; break;
                    default: break;
                    }
                    return out;
                }

                Image::Layout Write::_getImageLayout() const
                {
                    Image::Layout out;
                    out.endian = _p->options.data != Data::ASCII ? Memory::Endian::MSB : Memory::getEndian();
                    return out;
                }
                
                void Write::_write(const std::string& fileName, const std::shared_ptr<Image::Image>& image)
                {
                    DJV_PRIVATE_PTR();

                    const auto& info = image->getInfo();
                    int ppmType = Data::ASCII == p.options.data ? 2 : 5;
                    const uint8_t channelCount = Image::getChannelCount(info.type);
                    if (3 == channelCount)
                    {
                        ++ppmType;
                    }
                    char magic[] = "P \n";
                    magic[1] = '0' + ppmType;
                    auto io = System::File::IO::create();
                    io->open(fileName, System::File::IO::Mode::Write);
                    io->write(magic, 3);

                    std::stringstream ss;
                    ss << info.size.w << ' ' << info.size.h;
                    io->write(ss.str());
                    io->writeU8('\n');
                    const size_t bitDepth = Image::getBitDepth(info.type);
                    const int maxValue = 8 == bitDepth ? 255 : 65535;
                    ss = std::stringstream();
                    ss << maxValue;
                    io->write(ss.str());
                    io->writeU8('\n');

                    switch (p.options.data)
                    {
                    case Data::ASCII:
                    {
                        std::vector<uint8_t> scanline(getScanlineByteCount(
                            info.size.w,
                            Image::getChannelCount(info.type),
                            Image::getBitDepth(info.type)));
                        for (uint16_t y = 0; y < info.size.h; ++y)
                        {
                            const size_t size = writeASCII(
                                image->getData(y),
                                reinterpret_cast<char *>(scanline.data()),
                                static_cast<size_t>(info.size.w) * channelCount,
                                bitDepth);
                            io->write(scanline.data(), size);
                        }
                        break;
                    }
                    case Data::Binary:
                        io->write(image->getData(), info.getDataByteCount());
                        break;
                    default: break;
                    }
                }

            } // namespace PPM
        } // namespace IO
    } // namespace AV
} // namespace djv

