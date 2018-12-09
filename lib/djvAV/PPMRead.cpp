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
                Read::Read()
                {}

                std::shared_ptr<Read> Read::create(const std::string & fileName, const std::shared_ptr<Queue> & queue, const std::shared_ptr<Core::Context> & context)
                {
                    auto out = std::shared_ptr<Read>(new Read);
                    out->_init(fileName, queue, context);
                    return out;
                }

                Info Read::_readInfo(const std::string & fileName)
                {
                    Core::FileIO io;
                    Data data = Data::First;
                    return _open(fileName, io, data);
                }

                std::shared_ptr<Image> Read::_readImage(const std::string & fileName)
                {
                    std::shared_ptr<Image> out;
                    Core::FileIO io;
                    Data data = Data::First;
                    const auto info = _open(fileName, io, data);
                    if (info.video.size())
                    {
                        switch (data)
                        {
                        case Data::ASCII:
                        {
                            out = Image::create(info.video[0].info);
                            const size_t channelCount = Pixel::getChannelCount(info.video[0].info.type);
                            const size_t bitDepth = Pixel::getBitDepth(info.video[0].info.type);
                            for (int y = 0; y < info.video[0].info.size.y; ++y)
                            {
                                readASCII(io, out->getData(y), info.video[0].info.size.x * channelCount, bitDepth);
                            }
                            break;
                        }
                        case Data::Binary:
                            out = Image::create(info.video[0].info);
                            io.read(out->getData(), info.video[0].info.getDataByteCount());
                            break;
                        }
                    }
                    return out;
                }

                Info Read::_open(const std::string & fileName, Core::FileIO & io, Data & data)
                {
                    io.open(fileName, Core::FileIO::Mode::Read);

                    char magic[] = { 0, 0, 0 };
                    io.read(magic, 2);
                    if (magic[0] != 'P')
                    {
                        std::stringstream s;
                        s << pluginName << ": " << DJV_TEXT("cannot open") << ": " << fileName;
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
                        s << pluginName << ": " << DJV_TEXT("cannot open") << ": " << fileName;
                        throw std::runtime_error(s.str());
                    }
                    }
                    const int ppmType = magic[1] - '0';
                    data = (2 == ppmType || 3 == ppmType) ? Data::ASCII : Data::Binary;

                    size_t channelCount = 0;
                    switch (ppmType)
                    {
                    case 2:
                    case 5: channelCount = 1; break;
                    case 3:
                    case 6: channelCount = 3; break;
                    }
                    char tmp[Core::String::cStringLength] = "";
                    Core::FileIO::readWord(io, tmp, Core::String::cStringLength);
                    const int w = std::stoi(tmp);
                    Core::FileIO::readWord(io, tmp, Core::String::cStringLength);
                    const int h = std::stoi(tmp);
                    Core::FileIO::readWord(io, tmp, Core::String::cStringLength);
                    const int maxValue = std::stoi(tmp);
                    const size_t bitDepth = maxValue < 256 ? 8 : 16;
                    const auto pixelType = Pixel::getIntType(channelCount, bitDepth);
                    if (Pixel::Type::None == pixelType)
                    {
                        std::stringstream s;
                        s << pluginName << ": " << DJV_TEXT("cannot open") << ": " << fileName;
                        throw std::runtime_error(s.str());
                    }
                    Pixel::Layout layout;
                    layout.mirror.y = true;
                    layout.endian = data != Data::ASCII ? Core::Memory::Endian::MSB : Core::Memory::getEndian();
                    auto info = Pixel::Info(w, h, pixelType, layout);
                    return Info(fileName, VideoInfo(info, _speed, _duration), AudioInfo());
                }

            } // namespace PPM
        } // namespace IO
    } // namespace AV
} // namespace djv

