//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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

#include <djvCoreTest/FileIOTest.h>

#include <djvCore/FileIO.h>

#include <sstream>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        FileIOTest::FileIOTest(const std::shared_ptr<Context>& context) :
            ITest("djv::CoreTest::FileIOTest", context),
            _fileName("FileIOTest"),
            _text("Hello"),
            _text2("world!")
        {}
        
        void FileIOTest::run(const std::vector<std::string>& args)
        {
            _io();
            _error();
            _endian();
            _temp();
        }

        void FileIOTest::_io()
        {
            {
                FileSystem::FileIO io;
                DJV_ASSERT(!io.isOpen());
                DJV_ASSERT(io.getFileName().empty());
                DJV_ASSERT(0 == io.getSize());
                DJV_ASSERT(0 == io.getPos());
                DJV_ASSERT(io.isEOF());
                io.open(_fileName, FileSystem::FileIO::Mode::Write);
                DJV_ASSERT(io.isOpen());
                DJV_ASSERT(io.getFileName() == _fileName);
                FileSystem::FileIO io2(std::move(io));
                DJV_ASSERT(io2.isOpen());
                io2.write(_text + " ");
                DJV_ASSERT(io2.getSize() > 0);
                DJV_ASSERT(io2.getPos() > 0);
                io2.setPos(0);
                DJV_ASSERT(0 == io2.getPos());
                io2.seek(1);
                DJV_ASSERT(1 == io2.getPos());
                DJV_ASSERT(!io2.isEOF());
            }

            {
                FileSystem::FileIO io;
                io.open(_fileName, FileSystem::FileIO::Mode::Append);
                io.write(_text2);

                io.open(_fileName, FileSystem::FileIO::Mode::Read);
                FileSystem::FileIO io2;
                io2 = std::move(io);
                std::string buf = FileSystem::FileIO::readContents(io2);
                _print(buf);
                DJV_ASSERT((_text + " " + _text2) == buf);
                io2.setPos(0);
                DJV_ASSERT(0 == io2.getPos());
            }

            {
                FileSystem::FileIO::writeLines(
                    _fileName,
                    {
                        "# This is a comment",
                        _text + " " + _text2
                    });

                FileSystem::FileIO io;
                io.open(_fileName, FileSystem::FileIO::Mode::ReadWrite);
                char buf[String::cStringLength];
                FileSystem::FileIO::readWord(io, buf);
                _print(buf);
                DJV_ASSERT(_text == buf);
                FileSystem::FileIO::readWord(io, buf);
                _print(buf);
                DJV_ASSERT(_text2 == buf);
            }

            {
                FileSystem::FileIO io;
                io.open(_fileName, FileSystem::FileIO::Mode::Write);
                io.write(_text + "\n" + _text2);

                io.open(_fileName, FileSystem::FileIO::Mode::Read);
                char buf[String::cStringLength];
                FileSystem::FileIO::readLine(io, buf);
                _print(buf);
                DJV_ASSERT(_text == buf);
                FileSystem::FileIO::readLine(io, buf);
                _print(buf);
                DJV_ASSERT(_text2 == buf);
            }

            {
                FileSystem::FileIO::writeLines(
                    _fileName,
                    {
                        _text,
                        "# This is a comment",
                        _text2
                    });

                const auto lines = FileSystem::FileIO::readLines(_fileName);
                for (const auto& i : lines)
                {
                    _print(i);
                }
                DJV_ASSERT(_text == lines[0]);
                DJV_ASSERT(_text2 == lines[2]);
            }

            {
                const int8_t   i8  = std::numeric_limits<int8_t>::max();
                const uint8_t  u8  = std::numeric_limits<uint8_t>::max();
                const int16_t  i16 = std::numeric_limits<int16_t>::max();
                const uint16_t u16 = std::numeric_limits<uint16_t>::max();
                const int32_t  i32 = std::numeric_limits<int32_t>::max();
                const uint32_t u32 = std::numeric_limits<uint32_t>::max();
                const float    f   = std::numeric_limits<float>::max();
                FileSystem::FileIO io;
                io.open(_fileName, FileSystem::FileIO::Mode::Write);
                io.write8(i8);
                io.writeU8(u8);
                io.write16(i16);
                io.writeU16(u16);
                io.write32(i32);
                io.writeU32(u32);
                io.writeF32(f);

                io.open(_fileName, FileSystem::FileIO::Mode::Read);
                int8_t   _i8  = 0;
                uint8_t  _u8  = 0;
                int16_t  _i16 = 0;
                uint16_t _u16 = 0;
                int32_t  _i32 = 0;
                uint32_t _u32 = 0;
                float    _f   = 0.F;
                io.read8(&_i8);
                io.readU8(&_u8);
                io.read16(&_i16);
                io.readU16(&_u16);
                io.read32(&_i32);
                io.readU32(&_u32);
                io.readF32(&_f);
                DJV_ASSERT(i8  == _i8);
                DJV_ASSERT(u8  == _u8);
                DJV_ASSERT(i16 == _i16);
                DJV_ASSERT(u16 == _u16);
                DJV_ASSERT(i32 == _i32);
                DJV_ASSERT(u32 == _u32);
                DJV_ASSERT(f   == _f);
            }
        }
        
        void FileIOTest::_error()
        {
            try
            {
                FileSystem::FileIO io;
                io.open(std::string(), FileSystem::FileIO::Mode::Read);
                DJV_ASSERT(false);
            }
            catch (const std::exception& e)
            {
                _print(e.what());
            }
            
            try
            {
                FileSystem::FileIO io;
                io.open(std::string(), FileSystem::FileIO::Mode::Write);
                DJV_ASSERT(false);
            }
            catch (const std::exception& e)
            {
                _print(e.what());
            }
            
            try
            {
                FileSystem::FileIO io;
                io.open(std::string(), FileSystem::FileIO::Mode::ReadWrite);
                DJV_ASSERT(false);
            }
            catch (const std::exception& e)
            {
                _print(e.what());
            }
            
            try
            {
                FileSystem::FileIO io;
                io.open(std::string(), FileSystem::FileIO::Mode::Append);
                DJV_ASSERT(false);
            }
            catch (const std::exception& e)
            {
                _print(e.what());
            }
        }

        void FileIOTest::_endian()
        {
            uint32_t a = 0;
            uint32_t b = 0;
            uint8_t* aP = reinterpret_cast<uint8_t*>(&a);
            uint8_t* bP = reinterpret_cast<uint8_t*>(&b);
            aP[0] = bP[3] = 1;
            aP[1] = bP[2] = 2;
            aP[2] = bP[1] = 3;
            aP[3] = bP[0] = 4;
            
            FileSystem::FileIO io;
            DJV_ASSERT(!io.hasEndianConversion());
            io.open(_fileName, FileSystem::FileIO::Mode::Write);
            io.writeU32(a);
            io.setEndianConversion(true);
            DJV_ASSERT(io.hasEndianConversion());
            io.writeU32(a);

            io.open(_fileName, FileSystem::FileIO::Mode::Read);
            io.setEndianConversion(false);
            uint32_t _a = 0;
            uint32_t _b = 0;
            io.readU32(&_a);
            DJV_ASSERT(a == _a);
            io.setEndianConversion(true);
            io.readU32(&_b);
            DJV_ASSERT(a == _b);
        }

        void FileIOTest::_temp()
        {
            FileSystem::FileIO io;
            io.openTemp();
            for (auto i : _text)
            {
                io.writeU8(i);
            }
        }
        
    } // namespace CoreTest
} // namespace djv

