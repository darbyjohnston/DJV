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

#include <djvAV/PPM.h>

#include <djvCore/FileIO.h>

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
                    const FileSystem::FileInfo& fileInfo,
                    const Info & info,
                    const WriteOptions& writeOptions,
                    const Options& options,
                    const std::shared_ptr<ResourceSystem>& resourceSystem,
                    const std::shared_ptr<LogSystem>& logSystem)
                {
                    auto out = std::shared_ptr<Write>(new Write);
                    out->_p->options = options;
                    out->_init(fileInfo, info, writeOptions, resourceSystem, logSystem);
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
                
                void Write::_write(const std::string & fileName, const std::shared_ptr<Image::Image> & image)
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
                    FileSystem::FileIO io;
                    io.open(fileName, FileSystem::FileIO::Mode::Write);
                    io.write(magic, 3);

                    std::stringstream ss;
                    ss << info.size.w << ' ' << info.size.h;
                    io.write(ss.str());
                    io.writeU8('\n');
                    const size_t bitDepth = Image::getBitDepth(info.type);
                    const int maxValue = 8 == bitDepth ? 255 : 65535;
                    ss = std::stringstream();
                    ss << maxValue;
                    io.write(ss.str());
                    io.writeU8('\n');

                    switch (p.options.data)
                    {
                    case Data::ASCII:
                    {
                        std::vector<uint8_t> scanline(info.getScanlineByteCount());
                        for (uint16_t y = 0; y < info.size.h; ++y)
                        {
                            const size_t size = writeASCII(
                                image->getData(y),
                                reinterpret_cast<char *>(scanline.data()),
                                static_cast<size_t>(info.size.w) * channelCount,
                                bitDepth);
                            io.write(scanline.data(), size);
                        }
                        break;
                    }
                    case Data::Binary:
                        io.write(image->getData(), info.getDataByteCount());
                        break;
                    default: break;
                    }
                }

            } // namespace PPM
        } // namespace IO
    } // namespace AV
} // namespace djv

