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

#include <djvCoreTest/FileIOTest.h>

#include <djvCore/FileIO.h>

#include <sstream>

namespace djv
{
    using namespace Core;

    namespace CoreTest
    {
        FileIOTest::FileIOTest(const std::shared_ptr<Context>& context) :
            ITest("djv::CoreTest::FileIOTest", context)
        {}
        
        void FileIOTest::run(const std::vector<std::string>& args)
        {
            {
                const std::string fileName("FileIOTest");
                const std::string text("Hello world!");
                try
                {
                    FileSystem::FileIO io;
                    io.open(fileName, FileSystem::FileIO::Mode::Write);
                    for (auto i : text)
                    {
                        io.write8(i);
                    }
                }
                catch (const std::exception & e)
                {
                    _print(e.what());
                    DJV_ASSERT(0);
                }
                try
                {
                    FileSystem::FileIO io;
                    io.open(fileName, FileSystem::FileIO::Mode::Read);
                    std::string buf;
                    while (!io.isEOF())
                    {
                        int8_t c = 0;
                        io.read8(&c);
                        buf.push_back(c);
                    }
                    _print(buf);
                    DJV_ASSERT(text == buf);
                }
                catch (const std::exception & e)
                {
                    _print(e.what());
                }
            }
            {
                const std::string text("Hello world!");
                try
                {
                    FileSystem::FileIO io;
                    io.openTemp();
                    for (auto i : text)
                    {
                        io.writeU8(i);
                    }
                }
                catch (const std::exception & e)
                {
                    _print(e.what());
                    DJV_ASSERT(0);
                }
            }
        }
        
    } // namespace CoreTest
} // namespace djv

