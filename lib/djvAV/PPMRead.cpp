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
                Read::Read()
                {}

                Read::~Read()
                {
                    _finish();
                }

                std::shared_ptr<Read> Read::create(
                    const std::string & fileName,
                    size_t layer,
                    const std::shared_ptr<ResourceSystem>& resourceSystem,
                    const std::shared_ptr<LogSystem>& logSystem)
                {
                    auto out = std::shared_ptr<Read>(new Read);
                    out->_init(fileName, layer, resourceSystem, logSystem);
                    return out;
                }

                Info Read::_readInfo(const std::string & fileName)
                {
                    FileSystem::FileIO io;
                    Data data = Data::First;
                    return _open(fileName, io, data);
                }

                std::shared_ptr<Image::Image> Read::_readImage(const std::string & fileName)
                {
                    FileSystem::FileIO io;
                    Data data = Data::First;
                    const auto info = _open(fileName, io, data);
                    auto out = Image::Image::create(info.video[0].info);
                    switch (data)
                    {
                    case Data::ASCII:
                    {
                        const size_t channelCount = Image::getChannelCount(info.video[0].info.type);
                        const size_t bitDepth = Image::getBitDepth(info.video[0].info.type);
                        for (uint16_t y = 0; y < info.video[0].info.size.h; ++y)
                        {
                            readASCII(io, out->getData(y), info.video[0].info.size.w * channelCount, bitDepth);
                        }
                        break;
                    }
                    case Data::Binary:
                        io.read(out->getData(), info.video[0].info.getDataByteCount());
                        break;
                    default: break;
                    }
                    return out;
                }

                Info Read::_open(const std::string & fileName, FileSystem::FileIO & io, Data & data)
                {
                    io.open(fileName, FileSystem::FileIO::Mode::Read);

                    char magic[] = { 0, 0, 0 };
                    io.read(magic, 2);
                    if (magic[0] != 'P')
                    {
                        std::stringstream s;
                        s << DJV_TEXT("The PPM file") <<
                            " '" << fileName << "' " << DJV_TEXT("cannot be opened") << ".";
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
                        s << DJV_TEXT("The PPM file") <<
                            " '" << fileName << "' " << DJV_TEXT("cannot be opened") << ".";
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
                    char tmp[String::cStringLength] = "";
                    FileSystem::FileIO::readWord(io, tmp, String::cStringLength);
                    const int w = std::stoi(tmp);
                    FileSystem::FileIO::readWord(io, tmp, String::cStringLength);
                    const int h = std::stoi(tmp);
                    FileSystem::FileIO::readWord(io, tmp, String::cStringLength);
                    const int maxValue = std::stoi(tmp);
                    const size_t bitDepth = maxValue < 256 ? 8 : 16;
                    const auto imageType = Image::getIntType(channelCount, bitDepth);
                    if (Image::Type::None == imageType)
                    {
                        std::stringstream s;
                        s << DJV_TEXT("The PPM file") <<
                            " '" << fileName << "' " << DJV_TEXT("cannot be opened") << ".";
                        throw std::runtime_error(s.str());
                    }
                    Image::Layout layout;
                    layout.endian = data != Data::ASCII ? Memory::Endian::MSB : Memory::getEndian();
                    auto info = Image::Info(w, h, imageType, layout);
                    return Info(fileName, VideoInfo(info, _speed, _duration));
                }

            } // namespace PPM
        } // namespace IO
    } // namespace AV
} // namespace djv

