// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/Cineon.h>

#include <djvSystem/FileIO.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
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
                const System::File::Info& fileInfo,
                const IO::ReadOptions& readOptions,
                const std::shared_ptr<System::TextSystem>& textSystem,
                const std::shared_ptr<System::ResourceSystem>& resourceSystem,
                const std::shared_ptr<System::LogSystem>& logSystem)
            {
                auto out = std::shared_ptr<Read>(new Read);
                out->_init(fileInfo, readOptions, textSystem, resourceSystem, logSystem);
                return out;
            }
                
            std::shared_ptr<Image::Data> Read::readImage(
                const IO::Info& info,
                const std::shared_ptr<System::File::IO>& io)
            {
#if defined(DJV_MMAP)
                auto out = Image::Data::create(info.video[0].info, io);
#else // DJV_MMAP
                auto infoTmp = info;
                bool convertEndian = false;
                if (infoTmp.video[0].layout.endian != Memory::getEndian())
                {
                    convertEndian = true;
                    infoTmp.video[0].layout.endian = Memory::getEndian();
                }
                auto out = Image::Data::create(infoTmp.video[0]);
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

            IO::Info Read::_readInfo(const std::string& fileName)
            {
                auto io = System::File::IO::create();
                return _open(fileName, io);
            }

            std::shared_ptr<Image::Data> Read::_readImage(const std::string& fileName)
            {
                auto io = System::File::IO::create();
                const auto info = _open(fileName, io);
                auto out = readImage(info, io);
                out->setPluginName(pluginName);
                return out;
            }

            IO::Info Read::_open(const std::string& fileName, const std::shared_ptr<System::File::IO>& io)
            {
                DJV_PRIVATE_PTR();
                io->open(fileName, System::File::Mode::Read);
                IO::Info info;
                info.videoSpeed = _speed;
                info.videoSequence = _sequence;
                info.video.push_back(Image::Info());
                read(io, info, p.colorProfile, _textSystem);
                return info;
            }

        } // namespace Cineon
    } // namespace AV
} // namespace djv

