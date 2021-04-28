// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/PFM.h>

#include <djvSystem/File.h>
#include <djvSystem/FileIO.h>
#include <djvSystem/TextSystem.h>

#include <djvCore/StringFormat.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace PFM
        {
            Read::Read()
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

            IO::Info Read::_readInfo(const std::string& fileName)
            {
                auto io = System::File::IO::create();
                float scale;
                return _open(fileName, io, scale);
            }

            std::shared_ptr<Image::Data> Read::_readImage(const std::string& fileName)
            {
                auto io = System::File::IO::create();
                float scale;
                const auto info = _open(fileName, io, scale);
                auto imageInfo = info.video[0];
                std::shared_ptr<Image::Data> out;
                    
#if defined(DJV_MMAP)
                out = Image::Data::create(imageInfo, io);
#else // DJV_MMAP
                out = Image::Data::create(imageInfo);
                io->read(out->getData(), out->getDataByteCount());
#endif // DJV_MMAP

                if(scale - 1 > 1E-6)
                {
                    auto* data = reinterpret_cast<float*>(out->getData());
                    const size_t floats = out->getDataByteCount() / sizeof(float);
                    for (size_t i = 0; i < floats; ++i)
                    {
                        data[i] *= scale;
                    }
                }
                    
                out->setPluginName(pluginName);

                return out;
            }
                
            IO::Info Read::_open(const std::string& fileName, const std::shared_ptr<System::File::IO>& io, float& scale)
            {
                io->open(fileName, System::File::Mode::Read);

                char tmp[String::cStringLength] = "";
                System::File::readWord(io, tmp, String::cStringLength);
                std::string magic(tmp);
                    
                uint8_t channelCount = 0;
                    
                if(magic.length() == 2)
                {
                    if (magic == "PF")
                    {
                        channelCount = 3;
                    }
                    else if (magic == "Pf")
                    {
                        channelCount = 1;
                    }
                }
                else if(magic == "PF4")
                {
                    channelCount = 4;
                }
                    
                if(channelCount == 0)
                {
                    throw System::File::Error(String::Format("{0}: {1}").
                        arg(fileName).
                        arg(_textSystem->getText(DJV_TEXT("error_bad_magic_number"))));
                }
                    
                System::File::readWord(io, tmp, String::cStringLength);
                const int w = std::stoi(tmp);

                System::File::readWord(io, tmp, String::cStringLength);
                const int h = std::stoi(tmp);

                System::File::readWord(io, tmp, String::cStringLength);
                scale = std::stof(tmp);
                bool isPfmLittleEndian = scale < 0;
                scale = abs(scale);

                const auto imageType = Image::getFloatType(channelCount, 32);
                    
                Image::Layout layout;
                layout.endian = isPfmLittleEndian ? Memory::Endian::LSB : Memory::Endian::MSB;
                layout.mirror.y = true;
                    
                auto imageInfo = Image::Info(w, h, imageType, layout);
                    
                IO::Info info;
                info.fileName = fileName;
                info.videoSpeed = _speed;
                info.videoSequence = _sequence;
                info.video.push_back(imageInfo);
                return info;
            }

        } // namespace PFM
    } // namespace AV
} // namespace djv
