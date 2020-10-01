// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/Cineon.h>

#include <djvAV/CineonFunc.h>

#include <djvSystem/FileIO.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace Cineon
            {
                struct Write::Private
                {};

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
                    const std::shared_ptr<System::TextSystem>& textSystem,
                    const std::shared_ptr<System::ResourceSystem>& resourceSystem,
                    const std::shared_ptr<System::LogSystem>& logSystem)
                {
                    auto out = std::shared_ptr<Write>(new Write);
                    out->_init(fileInfo, info, writeOptions, textSystem, resourceSystem, logSystem);
                    return out;
                }

                Image::Type Write::_getImageType(Image::Type) const
                {
                    return Image::Type::RGB_U10;
                }

                Image::Layout Write::_getImageLayout() const
                {
                    Image::Layout out;
                    out.endian = Memory::Endian::MSB;
                    out.alignment = 4;
                    return out;
                }
                
                void Write::_write(const std::string& fileName, const std::shared_ptr<Image::Image>& image)
                {
                    auto io = System::File::IO::create();
                    io->open(fileName, System::File::Mode::Write);
                    Info info;
                    info.video.push_back(image->getInfo());
                    info.tags = image->getTags();
                    write(io, info, _options.colorSpace.empty() ? ColorProfile::Raw : ColorProfile::FilmPrint);
                    io->write(image->getData(), image->getDataByteCount());
                    writeFinish(io);
                }

            } // namespace Cineon
        } // namespace IO
    } // namespace AV
} // namespace djv

