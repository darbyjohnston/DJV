// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/Cineon.h>

#include <djvAV/CineonFunc.h>

#include <djvCore/FileIO.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace Cineon
            {
                struct Read::Private
                {
                    ColorProfile colorProfile = ColorProfile::FilmPrint;
                };

                Read::Read() :
                    _p(new Private)
                {}

                Read::~Read()
                {
                    _finish();
                }

                std::shared_ptr<Read> Read::create(
                    const FileSystem::FileInfo& fileInfo,
                    const ReadOptions& readOptions,
                    const std::shared_ptr<TextSystem>& textSystem,
                    const std::shared_ptr<ResourceSystem>& resourceSystem,
                    const std::shared_ptr<LogSystem>& logSystem)
                {
                    auto out = std::shared_ptr<Read>(new Read);
                    out->_init(fileInfo, readOptions, textSystem, resourceSystem, logSystem);
                    return out;
                }
                
                std::shared_ptr<Image::Image> Read::readImage(
                    const Info& info,
                    const std::shared_ptr<FileSystem::FileIO>& io)
                {
#if defined(DJV_MMAP)
                    auto out = Image::Image::create(info.video[0].info, io);
#else // DJV_MMAP
                    auto infoTmp = info;
                    bool convertEndian = false;
                    if (infoTmp.video[0].layout.endian != Memory::getEndian())
                    {
                        convertEndian = true;
                        infoTmp.video[0].layout.endian = Memory::getEndian();
                    }
                    auto out = Image::Image::create(infoTmp.video[0]);
                    io->read(out->getData(), out->getDataByteCount());
                    if (convertEndian)
                    {
                        const size_t dataByteCount = out->getDataByteCount();
                        switch (Image::getDataType(infoTmp.video[0].type))
                        {
                            case Image::DataType::U10:
                                Memory::endian(out->getData(), dataByteCount / 4, 4);
                                break;
                            default: break;                            
                        }
                    }
#endif // DJV_MMAP
                    out->setTags(infoTmp.tags);
                    return out;
                }

                Info Read::_readInfo(const std::string& fileName)
                {
                    auto io = FileSystem::FileIO::create();
                    return _open(fileName, io);
                }

                std::shared_ptr<Image::Image> Read::_readImage(const std::string& fileName)
                {
                    auto io = FileSystem::FileIO::create();
                    const auto info = _open(fileName, io);
                    auto out = readImage(info, io);
                    out->setPluginName(pluginName);
                    return out;
                }

                Info Read::_open(const std::string& fileName, const std::shared_ptr<FileSystem::FileIO>& io)
                {
                    DJV_PRIVATE_PTR();
                    io->open(fileName, FileSystem::FileIO::Mode::Read);
                    Info info;
                    info.videoSpeed = _speed;
                    info.videoSequence = _sequence;
                    info.video.push_back(Image::Info());
                    read(io, info, p.colorProfile, _textSystem);
                    return info;
                }

            } // namespace Cineon
        } // namespace IO
    } // namespace AV
} // namespace djv

