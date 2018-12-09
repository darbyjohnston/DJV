//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvAV/FFmpeg.h>

#include <djvCore/FileIO.h>

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace PPM
            {
                struct Read::Private
                {
                    Pixel::Info info;
                    Data data = Data::First;
                    Core::FileIO io;
                };

                Read::Read() :
                    _p(new Private)
                {}

                Read::~Read()
                {}

                std::shared_ptr<Read> Read::create(const std::string & fileName, const std::shared_ptr<Queue> & queue, const std::shared_ptr<Core::Context> & context)
                {
                    auto out = std::shared_ptr<Read>(new Read);
                    out->_init(fileName, queue, context);
                    return out;
                }

                Info Read::_open(const std::string& fileName, const Speed & speed, Duration duration)
                {
                    DJV_PRIVATE_PTR();
                    p.io.open(fileName, Core::FileIO::Mode::Read);

                    char magic[] = { 0, 0, 0 };
                    p.io.read(magic, 2);
                    if (magic[0] != 'P')
                    {
                        std::stringstream s;
                        s << pluginName << " cannot open: " << fileName;
                        throw std::runtime_error(s.str());
                    }
                    switch (magic[1])
                    {
                    case '2':
                    case '3':
                    case '5':
                    case '6': break;
                    default:
                    {
                        std::stringstream s;
                        s << pluginName << " cannot open: " << fileName;
                        throw std::runtime_error(s.str());
                    }
                    }
                    const int ppmType = magic[1] - '0';
                    p.data = (2 == ppmType || 3 == ppmType) ? Data::ASCII : Data::Binary;

                    size_t channelCount = 0;
                    switch (ppmType)
                    {
                    case 2:
                    case 5: channelCount = 1; break;
                    case 3:
                    case 6: channelCount = 3; break;
                    }
                    char tmp[Core::String::cStringLength] = "";
                    Core::FileIO::readWord(p.io, tmp, Core::String::cStringLength);
                    const int w = std::stoi(tmp);
                    Core::FileIO::readWord(p.io, tmp, Core::String::cStringLength);
                    const int h = std::stoi(tmp);
                    Core::FileIO::readWord(p.io, tmp, Core::String::cStringLength);
                    const int maxValue = std::stoi(tmp);
                    const size_t bitDepth = maxValue < 256 ? 8 : 16;
                    const auto pixelType = Pixel::getIntType(channelCount, bitDepth);
                    if (Pixel::Type::None == pixelType)
                    {
                        std::stringstream s;
                        s << pluginName << " cannot open: " << fileName;
                        throw std::runtime_error(s.str());
                    }
                    Pixel::Layout layout;
                    layout.mirror.y = true;
                    layout.endian = p.data != Data::ASCII ? Core::Memory::Endian::MSB : Core::Memory::getEndian();
                    p.info = Pixel::Info(w, h, pixelType, layout);
                    return Info(fileName, VideoInfo(p.info, speed, duration), AudioInfo());
                }

                std::shared_ptr<Image> Read::_read()
                {
                    DJV_PRIVATE_PTR();
                    std::shared_ptr<Image> out;
                    switch (p.data)
                    {
                    case Data::ASCII:
                    {
                        out = Image::create(p.info);
                        const size_t channelCount = Pixel::getChannelCount(p.info.type);
                        const size_t bitDepth = Pixel::getBitDepth(p.info.type);
                        for (int y = 0; y < p.info.size.y; ++y)
                        {
                            readASCII(p.io, out->getData(y), p.info.size.x * channelCount, bitDepth);
                        }
                        break;
                    }
                    case Data::Binary:
                        out = Image::create(p.info);
                        p.io.read(out->getData(), p.info.getDataByteCount());
                        break;
                    }
                    return out;
                }

                void Read::_close()
                {
                    _p->io.close();
                }

            } // namespace PPM
        } // namespace IO
    } // namespace AV
} // namespace djv

