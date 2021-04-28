// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/CineonTest.h>

#include <djvAV/Cineon.h>

#include <djvSystem/Context.h>
#include <djvSystem/FileIO.h>
#include <djvSystem/TextSystem.h>

#include <djvCore/Error.h>
#include <djvCore/Memory.h>

using namespace djv::Core;
using namespace djv::AV;
using namespace djv::AV::IO;

namespace djv
{
    namespace AVTest
    {
        CineonTest::CineonTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest(
                "djv::AVTest::CineonTest",
                System::File::Path(tempPath, "CineonTest"),
                context)
        {}
                
        void CineonTest::run()
        {
            _util();
            _header();
        }
        
        void CineonTest::_util()
        {
            {
                DJV_ASSERT(!Cineon::isValid("", 0));
                DJV_ASSERT(!Cineon::isValid("\n", 1));
                DJV_ASSERT(Cineon::isValid("test", 4));
            }
            
            {
                DJV_ASSERT(Cineon::toString("test", 4) == "test");
            }
            
            {
                char buf[256];
                DJV_ASSERT(Cineon::fromString("test", buf, 1, false) == 1);
                DJV_ASSERT('t' == buf[0]);
                DJV_ASSERT(Cineon::fromString("test", buf, 1, true) == 1);
                DJV_ASSERT('\0' == buf[0]);
                DJV_ASSERT(Cineon::fromString("test", buf, 256, false) == 4);
                DJV_ASSERT('t' == buf[0]);
                DJV_ASSERT('e' == buf[1]);
                DJV_ASSERT('s' == buf[2]);
                DJV_ASSERT('t' == buf[3]);
                DJV_ASSERT(Cineon::fromString("test", buf, 256, true) == 5);
                DJV_ASSERT('t' == buf[0]);
                DJV_ASSERT('e' == buf[1]);
                DJV_ASSERT('s' == buf[2]);
                DJV_ASSERT('t' == buf[3]);
                DJV_ASSERT('\0' == buf[4]);
            }
        }
        
        void CineonTest::_header()
        {
            if (auto context = getContext().lock())
            {
                auto textSystem = context->getSystemT<System::TextSystem>();
                
                try
                {
                    auto io = System::File::IO::create();
                    io->open(System::File::Path(getTempPath(), "empty.cin").get(), System::File::Mode::Write);
                    io->open(System::File::Path(getTempPath(), "empty.cin").get(), System::File::Mode::Read);
                    Info info;
                    Cineon::ColorProfile colorProfile = Cineon::ColorProfile::First;
                    Cineon::read(io, info, colorProfile, textSystem);
                    DJV_ASSERT(false);
                }
                catch (const std::exception& e)
                {
                    _print(Error::format(e));
                }
                
                try
                {
                    auto io = System::File::IO::create();
                    io->open(System::File::Path(getTempPath(), "test.cin").get(), System::File::Mode::Write);
                    Cineon::Header header;
                    Cineon::zero(header);
                    io->write(&header.file, sizeof(Cineon::Header::File));
                    io->write(&header.image, sizeof(Cineon::Header::Image));
                    io->write(&header.source, sizeof(Cineon::Header::Source));
                    io->write(&header.film, sizeof(Cineon::Header::Film));
                    io->open(System::File::Path(getTempPath(), "test.cin").get(), System::File::Mode::Read);
                    Info info;
                    Cineon::ColorProfile colorProfile = Cineon::ColorProfile::First;
                    Cineon::read(io, info, colorProfile, textSystem);
                    DJV_ASSERT(false);
                }
                catch (const std::exception& e)
                {
                    _print(Error::format(e));
                }
                
                try
                {
                    Cineon::Header header;
                    Cineon::zero(header);
                    header.image.channels = 0;
                    Info info;
                    info.video.push_back(Image::Info());
                    _headerIO(header, info);
                    DJV_ASSERT(false);
                }
                catch (const std::exception& e)
                {
                    _print(Error::format(e));
                }
                
                try
                {
                    Cineon::Header header;
                    Cineon::zero(header);
                    header.image.channels = 2;
                    header.image.channel[1].size[0] = 1;
                    Info info;
                    info.video.push_back(Image::Info());
                    _headerIO(header, info);
                    DJV_ASSERT(false);
                }
                catch (const std::exception& e)
                {
                    _print(Error::format(e));
                }
                
                try
                {
                    Cineon::Header header;
                    Cineon::zero(header);
                    header.image.channels = 2;
                    header.image.channel[1].bitDepth = 1;
                    Info info;
                    info.video.push_back(Image::Info());
                    _headerIO(header, info);
                    DJV_ASSERT(false);
                }
                catch (const std::exception& e)
                {
                    _print(Error::format(e));
                }

                try
                {
                    Cineon::Header header;
                    Cineon::zero(header);
                    header.image.channels = 1;
                    Info info;
                    info.video.push_back(Image::Info());
                    _headerIO(header, info);
                    DJV_ASSERT(false);
                }
                catch (const std::exception& e)
                {
                    _print(Error::format(e));
                }

                try
                {
                    Cineon::Header header;
                    Cineon::zero(header);
                    header.image.channels = 3;
                    Info info;
                    info.video.push_back(Image::Info());
                    _headerIO(header, info);
                    DJV_ASSERT(false);
                }
                catch (const std::exception& e)
                {
                    _print(Error::format(e));
                }

                try
                {
                    Cineon::Header header;
                    header.image.channels = 3;
                    header.image.linePadding = 1;
                    header.image.channel[0].bitDepth = 10;
                    header.image.channel[1].bitDepth = 10;
                    header.image.channel[2].bitDepth = 10;
                    Info info;
                    info.video.push_back(Image::Info());
                    _headerIO(header, info);
                    DJV_ASSERT(false);
                }
                catch (const std::exception& e)
                {
                    _print(Error::format(e));
                }

                try
                {
                    Cineon::Header header;
                    header.image.channels = 3;
                    header.image.linePadding = 0;
                    header.image.channelPadding = 1;
                    header.image.channel[0].bitDepth = 10;
                    header.image.channel[1].bitDepth = 10;
                    header.image.channel[2].bitDepth = 10;
                    Info info;
                    info.video.push_back(Image::Info());
                    _headerIO(header, info);
                    DJV_ASSERT(false);
                }
                catch (const std::exception& e)
                {
                    _print(Error::format(e));
                }

                try
                {
                    Cineon::Header header;
                    header.image.channels = 3;
                    header.image.linePadding = 0;
                    header.image.channelPadding = 0;
                    header.image.channel[0].bitDepth = 10;
                    header.image.channel[1].bitDepth = 10;
                    header.image.channel[2].bitDepth = 10;
                    Info info;
                    info.video.push_back(Image::Info());
                    _headerIO(header, info);
                    DJV_ASSERT(false);
                }
                catch (const std::exception& e)
                {
                    _print(Error::format(e));
                }

                for (const auto& i : Cineon::getOrientEnums())
                {
                    Cineon::Header header;
                    header.file.imageOffset = 2048;
                    header.image.channels = 3;
                    header.image.linePadding = 0;
                    header.image.channelPadding = 0;
                    header.image.orient = static_cast<uint8_t>(i);
                    header.image.channel[0].size[0] = 0;
                    header.image.channel[0].size[1] = 0;
                    header.image.channel[1].size[0] = 0;
                    header.image.channel[1].size[1] = 0;
                    header.image.channel[2].size[0] = 0;
                    header.image.channel[2].size[1] = 0;
                    header.image.channel[0].bitDepth = 10;
                    header.image.channel[1].bitDepth = 10;
                    header.image.channel[2].bitDepth = 10;
                    Info info;
                    info.video.push_back(Image::Info());
                    _headerIO(header, info);
                }

                for (const auto& i : Cineon::getDescriptorEnums())
                {
                    Cineon::Header header;
                    header.file.imageOffset = 2048;
                    header.image.channels = 3;
                    header.image.linePadding = 0;
                    header.image.channelPadding = 0;
                    header.image.channel[0].size[0] = 0;
                    header.image.channel[0].size[1] = 0;
                    header.image.channel[1].size[0] = 0;
                    header.image.channel[1].size[1] = 0;
                    header.image.channel[2].size[0] = 0;
                    header.image.channel[2].size[1] = 0;
                    header.image.channel[0].bitDepth = 10;
                    header.image.channel[1].bitDepth = 10;
                    header.image.channel[2].bitDepth = 10;
                    header.image.channel[0].descriptor[1] = static_cast<uint8_t>(i);
                    Info info;
                    info.video.push_back(Image::Info());
                    _headerIO(header, info);
                }

                {
                    Cineon::Header header;
                    header.file.imageOffset = 2048;
                    header.image.channels = 3;
                    header.image.linePadding = 0;
                    header.image.channelPadding = 0;
                    header.image.channel[0].size[0] = 0;
                    header.image.channel[0].size[1] = 0;
                    header.image.channel[1].size[0] = 0;
                    header.image.channel[1].size[1] = 0;
                    header.image.channel[2].size[0] = 0;
                    header.image.channel[2].size[1] = 0;
                    header.image.channel[0].bitDepth = 10;
                    header.image.channel[1].bitDepth = 10;
                    header.image.channel[2].bitDepth = 10;
                    header.file.time[0] = ' ';
                    header.source.offset[0] = 1;
                    header.source.offset[1] = 1;
                    header.source.file[0] = ' ';
                    header.source.time[0] = ' ';
                    header.source.inputDevice[0] = ' ';
                    header.source.inputModel[0] = ' ';
                    header.source.inputSerial[0] = ' ';
                    header.source.inputPitch[0] = 1;
                    header.source.inputPitch[1] = 1;
                    header.source.gamma = 2.2F;
                    header.film.id = 1;
                    header.film.type = 1;
                    header.film.offset = 1;
                    header.film.prefix = 1;
                    header.film.count = 1;
                    header.film.format[0] = ' ';
                    header.film.frame = 1;
                    header.film.frameRate = 24.F;
                    header.film.frameId[0] =  ' ';
                    header.film.slate[0] = ' ';
                    Info info;
                    info.video.push_back(Image::Info());
                    _headerIO(header, info);
                }
                
                try
                {
                    Cineon::Header header;
                    header.file.imageOffset = 2048;
                    header.image.channels = 3;
                    header.image.linePadding = 0;
                    header.image.channelPadding = 0;
                    header.image.channel[0].size[0] = 1;
                    header.image.channel[0].size[1] = 1;
                    header.image.channel[1].size[0] = 1;
                    header.image.channel[1].size[1] = 1;
                    header.image.channel[2].size[0] = 1;
                    header.image.channel[2].size[1] = 1;
                    header.image.channel[0].bitDepth = 10;
                    header.image.channel[1].bitDepth = 10;
                    header.image.channel[2].bitDepth = 10;
                    Info info;
                    info.video.push_back(Image::Info());
                    _headerIO(header, info);
                    DJV_ASSERT(false);
                }
                catch (const std::exception& e)
                {
                    _print(Error::format(e));
                }

                for (const auto& i : Cineon::getColorProfileEnums())
                {
                    auto io = System::File::IO::create();
                    io->open(System::File::Path(getTempPath(), "test.cin").get(), System::File::Mode::Write);
                    Info info;
                    info.video.push_back(Image::Info());
                    Cineon::write(io, info, i);
                }

                {
                    auto io = System::File::IO::create();
                    io->open(System::File::Path(getTempPath(), "test.cin").get(), System::File::Mode::Write);
                    Info info;
                    info.video.push_back(Image::Info());
                    info.tags.set("Time", " ");
                    info.tags.set("Source Offset", "1 1");
                    info.tags.set("Source File", " ");
                    info.tags.set("Source Time", " ");
                    info.tags.set("Source Input Device", " ");
                    info.tags.set("Source Input Model", " ");
                    info.tags.set("Source Input Serial", " ");
                    info.tags.set("Source Input Pitch", "1 1");
                    info.tags.set("Source Gamma", "2.2");
                    info.tags.set("Keycode", "1:1:1:1:1");
                    info.tags.set("Film Format", " ");
                    info.tags.set("Film Frame", "1");
                    info.tags.set("Film Frame Rate", "24");
                    info.tags.set("Film Frame ID", " ");
                    info.tags.set("Film Slate", " ");
                    Cineon::write(io, info, Cineon::ColorProfile::Raw);
                    io->open(System::File::Path(getTempPath(), "test.cin").get(), System::File::Mode::Read);
                    Info info2;
                    info2.video.push_back(Image::Info());
                    Cineon::ColorProfile colorProfile = Cineon::ColorProfile::First;
                    Cineon::read(io, info2, colorProfile, textSystem);
                    DJV_ASSERT(info.tags == info2.tags);
                }
            }
        }

        void CineonTest::_headerIO(
            Cineon::Header& header,
            AV::IO::Info& info,
            Cineon::ColorProfile colorProfile)
        {
            if (auto context = getContext().lock())
            {
                auto textSystem = context->getSystemT<System::TextSystem>();
                auto io = System::File::IO::create();
                io->open(System::File::Path(getTempPath(), "test.cin").get(), System::File::Mode::Write);
                const bool flipEndian = Memory::getEndian() != Memory::Endian::MSB;
                io->setEndianConversion(flipEndian);
                if (flipEndian)
                {
                    Cineon::convertEndian(header);
                    header.file.magic = Cineon::magic[1];
                }
                else
                {
                    header.file.magic = Cineon::magic[0];
                }
                io->write(&header.file, sizeof(Cineon::Header::File));
                io->write(&header.image, sizeof(Cineon::Header::Image));
                io->write(&header.source, sizeof(Cineon::Header::Source));
                io->write(&header.film, sizeof(Cineon::Header::Film));
                io->open(System::File::Path(getTempPath(), "test.cin").get(), System::File::Mode::Read);
                Cineon::read(io, info, colorProfile, textSystem);
            }
        }

    } // namespace AVTest
} // namespace djv


