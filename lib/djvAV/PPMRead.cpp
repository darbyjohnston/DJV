// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/PPM.h>

#include <djvAV/PPMFunc.h>

#include <djvSystem/File.h>
#include <djvSystem/FileIO.h>
#include <djvSystem/FileIOFunc.h>
#include <djvSystem/TextSystem.h>

#include <djvCore/StringFormat.h>

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
                    Data data = Data::First;
                    return _open(fileName, io, data);
                }

                std::shared_ptr<Image::Image> Read::_readImage(const std::string& fileName)
                {
                    auto io = System::File::IO::create();
                    Data data = Data::First;
                    const auto info = _open(fileName, io, data);
                    auto imageInfo = info.video[0];
                    std::shared_ptr<Image::Image> out;
                    switch (data)
                    {
                    case Data::ASCII:
                    {
                        out = Image::Image::create(imageInfo);
                        out->setPluginName(pluginName);
                        const size_t channelCount = Image::getChannelCount(imageInfo.type);
                        const size_t bitDepth = Image::getBitDepth(imageInfo.type);
                        for (uint16_t y = 0; y < imageInfo.size.h; ++y)
                        {
                            readASCII(io, out->getData(y), imageInfo.size.w * channelCount, bitDepth);
                        }
                        break;
                    }
                    case Data::Binary:
                    {
                        if (imageInfo.layout.endian != Memory::getEndian())
                        {
                            imageInfo.layout.endian = Memory::getEndian();
                        }
#if defined(DJV_MMAP)
                        out = Image::Image::create(imageInfo, io);
#else // DJV_MMAP
                        out = Image::Image::create(imageInfo);
                        io->read(out->getData(), out->getDataByteCount());
#endif // DJV_MMAP
                        out->setPluginName(pluginName);
                        break;
                    }
                    default: break;
                    }
                    return out;
                }

                Info Read::_open(const std::string& fileName, const std::shared_ptr<System::File::IO>& io, Data& data)
                {
                    io->open(fileName, System::File::Mode::Read);

                    char magic[] = { 0, 0, 0 };
                    io->read(magic, 2);
                    if (magic[0] != 'P')
                    {
                        throw System::File::Error(String::Format("{0}: {1}").
                            arg(fileName).
                            arg(_textSystem->getText(DJV_TEXT("error_bad_magic_number"))));
                    }
                    switch (magic[1])
                    {
                    case '2':
                    case '3':
                    case '5':
                    case '6': break;
                    default:
                    {
                        throw System::File::Error(String::Format("{0}: {1}").
                            arg(fileName).
                            arg(_textSystem->getText(DJV_TEXT("error_bad_magic_number"))));
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
                    default: break;
                    }
                    char tmp[String::cStringLength] = "";
                    System::File::readWord(io, tmp, String::cStringLength);
                    const int w = std::stoi(tmp);
                    System::File::readWord(io, tmp, String::cStringLength);
                    const int h = std::stoi(tmp);
                    System::File::readWord(io, tmp, String::cStringLength);
                    const int maxValue = std::stoi(tmp);
                    const size_t bitDepth = maxValue < 256 ? 8 : 16;
                    const auto imageType = Image::getIntType(channelCount, bitDepth);
                    if (Image::Type::None == imageType)
                    {
                        throw System::File::Error(String::Format("{0}: {1}").
                            arg(fileName).
                            arg(_textSystem->getText(DJV_TEXT("error_unsupported_image_type"))));
                    }
                    Image::Layout layout;
                    layout.endian = data != Data::ASCII ? Memory::Endian::MSB : Memory::getEndian();
                    auto imageInfo = Image::Info(w, h, imageType, layout);

                    const size_t ioSize = io->getSize();
                    const size_t ioPos = io->getPos();
                    const size_t fileDataByteCount = ioSize > 0 ? (ioSize - ioPos) : 0;
                    const size_t dataByteCount = imageInfo.getDataByteCount();
                    if (Data::Binary == data && dataByteCount > fileDataByteCount)
                    {
                        throw System::File::Error(String::Format("{0}: {1}").
                            arg(fileName).
                            arg(_textSystem->getText(DJV_TEXT("error_incomplete_file"))));
                    }

                    Info info;
                    info.fileName = fileName;
                    info.videoSpeed = _speed;
                    info.videoSequence = _sequence;
                    info.video.push_back(imageInfo);
                    return info;
                }

            } // namespace PPM
        } // namespace IO
    } // namespace AV
} // namespace djv

