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

#include <djvAV/SGI.h>

#include <djvCore/FileIO.h>
#include <djvCore/FileSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace SGI
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
                    template<typename T>
                    bool read(
                        const void* in,
                        const void* end,
                        void*       out,
                        size_t      size,
                        bool        endian)
                    {
                        const size_t bytes = sizeof(T);
                        const T* inP = reinterpret_cast<const T*>(in);
                        T* outP = reinterpret_cast<T*>(out);
                        const T* const outEnd = outP + size;
                        while (outP < outEnd)
                        {
                            // Information.
                            if (inP > end)
                            {
                                return false;
                            }
                            const size_t count = *inP & 0x7f;
                            const bool   run = !(*inP & 0x80);
                            const size_t length = run ? 1 : count;
                            ++inP;

                            // Unpack.
                            if (inP + length > end)
                            {
                                return false;
                            }
                            if (run)
                            {
                                if (!endian)
                                {
                                    for (size_t j = 0; j < count; ++j, ++outP)
                                    {
                                        *outP = *inP;
                                    }
                                }
                                else
                                {
                                    Memory::endian(inP, outP, 1, bytes);
                                    Memory::endian(inP, outP, 1, bytes);
                                    if (count > 1)
                                    {
                                        for (size_t j = 0; j < count - 1; ++j)
                                        {
                                            outP[1 + j] = outP[0];
                                        }
                                    }
                                    outP += count;
                                }
                                ++inP;
                            }
                            else
                            {
                                if (!endian)
                                {
                                    for (size_t j = 0; j < length; ++j, ++inP, ++outP)
                                    {
                                        *outP = *inP;
                                    }
                                }
                                else
                                {
                                    Memory::endian(inP, outP, length, bytes);
                                    inP += length;
                                    outP += length;
                                }
                            }
                        }
                        return true;
                    }

                    bool readRle(
                        const void* in,
                        const void* end,
                        void*       out,
                        size_t      size,
                        size_t      bytes,
                        bool        endian)
                    {
                        switch (bytes)
                        {
                        case 1: return read<uint8_t>(in, end, out, size, false);
                        case 2: return read<uint16_t>(in, end, out, size, endian);
                        default: break;
                        }
                        return false;
                    }

                    void planarInterleave(
                        const std::shared_ptr<Image::Data>& in,
                        std::shared_ptr<Image::Image>& out)
                    {
                        const size_t w = out->getWidth();
                        const size_t h = out->getHeight();
                        const size_t channels = Image::getChannelCount(out->getType());
                        const size_t pixelByteCount = out->getPixelByteCount();
                        const size_t channelByteCount = Image::getByteCount(Image::getDataType(out->getType()));
                        for (size_t c = 0; c < channels; ++c)
                        {
                            for (size_t y = 0; y < h; ++y)
                            {
                                const uint8_t* inP = in->getData() + (c * in->getHeight() + y) * in->getWidth() * channelByteCount;
                                uint8_t* outP = out->getData(0, y) + c * channelByteCount;
                                for (
                                    size_t x = 0;
                                    x < w;
                                    ++x, inP += channelByteCount,
                                    outP += pixelByteCount)
                                {
                                    switch (channelByteCount)
                                    {
                                    case 4: outP[3] = inP[3];
                                    case 3: outP[2] = inP[2];
                                    case 2: outP[1] = inP[1];
                                    case 1: outP[0] = inP[0];
                                    default: break;
                                    }
                                }
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

                    const size_t pos = io.getPos();
                    const size_t size = io.getSize() - pos;
                    const Image::Info& imageInfo = info.video[0].info;
                    const size_t channels = Image::getChannelCount(imageInfo.type);
                    const size_t bytes = Image::getByteCount(Image::getDataType(imageInfo.type));
                    const size_t dataByteCount = out->getDataByteCount();
                    std::shared_ptr<Image::Data> tmp = Image::Data::create(imageInfo);
                    if (!_compression)
                    {
                        if (1 == bytes)
                        {
                            io.readU8(tmp->getData(), dataByteCount);
                        }
                        else
                        {
                            io.read(tmp->getData(), size / bytes, bytes);
                        }
                    }
                    else
                    {
                        std::vector<uint8_t> tmp(size);
                        io.read(tmp.data(), size / bytes, bytes);
                        const uint8_t* inP = tmp.data();
                        const uint8_t* end = inP + size;
                        uint8_t* outP = out->getData();
                        for (size_t c = 0; c < channels; ++c)
                        {
                            for (size_t y = 0; y < imageInfo.size.h; ++y, outP += imageInfo.size.w * bytes)
                            {
                                if (!readRle(
                                    inP + _rleOffset[y + imageInfo.size.h * c] - pos,
                                    end,
                                    outP,
                                    imageInfo.size.w,
                                    bytes,
                                    io.getEndian()))
                                {
                                    throw FileSystem::Error(DJV_TEXT("Read error."));
                                }
                            }
                        }
                    }

                    // Interleave the image channels.
                    planarInterleave(tmp, out);

                    return out;
                }

                namespace
                {
                    struct Header
                    {
                        Header();

                        void read(FileSystem::FileIO&, Image::Info&, bool& compression);

                    private:
                        struct Data
                        {
                            uint16_t magic;
                            uint8_t  storage;
                            uint8_t  bytes;
                            uint16_t dimension;
                            uint16_t width;
                            uint16_t height;
                            uint16_t channels;
                            uint32_t pixelMin;
                            uint32_t pixelMax;
                        }
                        _data;
                    };

                    Header::Header()
                    {
                        _data.magic = 474;
                        _data.storage = 0;
                        _data.bytes = 0;
                        _data.dimension = 0;
                        _data.width = 0;
                        _data.height = 0;
                        _data.channels = 0;
                        _data.pixelMin = 0;
                        _data.pixelMax = 0;
                    }

                    void Header::read(FileSystem::FileIO& io, Image::Info& info, bool& compression)
                    {
                        // Read.
                        io.readU16(&_data.magic);
                        if (_data.magic != 474)
                        {
                            throw FileSystem::Error(DJV_TEXT("File not supported."));
                        }
                        io.readU8(&_data.storage);
                        io.readU8(&_data.bytes);
                        io.readU16(&_data.dimension);
                        io.readU16(&_data.width);
                        io.readU16(&_data.height);
                        io.readU16(&_data.channels);
                        io.readU32(&_data.pixelMin);
                        io.readU32(&_data.pixelMax);
                        io.setPos(512);

                        // Information.
                        info.size.w = _data.width;
                        info.size.h = _data.height;
                        info.type = Image::getIntType(_data.channels, 1 == _data.bytes ? 8 : 16);
                        if (Image::Type::None == info.type)
                        {
                            throw FileSystem::Error(DJV_TEXT("File not supported."));
                        }
                        info.layout.mirror.y = true;
                        info.layout.endian = Memory::Endian::MSB;

                        compression = _data.storage ? true : false;
                    }
                
                } // namespace

                Info Read::_open(const std::string & fileName, FileSystem::FileIO& io)
                {
                    io.setEndian(Memory::getEndian() != Memory::Endian::MSB);
                    io.open(fileName, FileSystem::FileIO::Mode::Read);
                    Image::Info imageInfo;
                    Header().read(io, imageInfo, _compression);
                    auto info = Info(fileName, VideoInfo(imageInfo, _speed, _sequence));
                    return info;
                }

            } // namespace SGI
        } // namespace IO
    } // namespace AV
} // namespace djv

