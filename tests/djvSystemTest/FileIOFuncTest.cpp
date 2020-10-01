// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvSystemTest/FileIOFuncTest.h>

#include <djvSystem/FileIO.h>
#include <djvSystem/FileIOFunc.h>
#include <djvSystem/Path.h>

#include <limits>
#include <sstream>

using namespace djv::Core;
using namespace djv::System;

namespace djv
{
    namespace SystemTest
    {
        FileIOFuncTest::FileIOFuncTest(
            const File::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest(
                "djv::SystemTest::FileIOFuncTest",
                File::Path(tempPath, "FileIOFuncTest"),
                context),
            _fileName("file.txt"),
            _text("Hello"),
            _text2("world!")
        {}
        
        void FileIOFuncTest::run()
        {
            {
                auto io = File::IO::create();
                const std::string fileName = File::Path(getTempPath(), _fileName).get();
                io->open(
                    fileName,
                    File::Mode::Write);
                io->write(_text + " ");
                io->open(
                    fileName,
                    File::Mode::Append);
                io->write(_text2);

                io->open(
                    fileName,
                    File::Mode::Read);
                std::string buf = File::readContents(io);
                _print(buf);
                DJV_ASSERT((_text + " " + _text2) == buf);
                io->setPos(0);
                DJV_ASSERT(0 == io->getPos());
            }

            {
                const std::string fileName = File::Path(getTempPath(), _fileName).get();
                File::writeLines(
                    fileName,
                    {
                        "# This is a comment",
                        _text + " " + _text2
                    });

                auto io = File::IO::create();
                io->open(
                    fileName,
                    File::Mode::ReadWrite);
                char buf[String::cStringLength];
                File::readWord(io, buf);
                _print(buf);
                DJV_ASSERT(_text == buf);
                File::readWord(io, buf);
                _print(buf);
                DJV_ASSERT(_text2 == buf);
            }

            {
                const std::string fileName = File::Path(getTempPath(), _fileName).get();
                auto io = File::IO::create();
                io->open(
                    fileName,
                    File::Mode::Write);
                io->write(_text + "\n" + _text2);

                io->open(
                    fileName,
                    File::Mode::Read);
                char buf[String::cStringLength];
                File::readLine(io, buf);
                _print(buf);
                DJV_ASSERT(_text == buf);
                File::readLine(io, buf);
                _print(buf);
                DJV_ASSERT(_text2 == buf);
            }

            {
                const std::string fileName = File::Path(getTempPath(), _fileName).get();
                File::writeLines(
                    fileName,
                    {
                        _text,
                        "# This is a comment",
                        _text2
                    });

                const auto lines = File::readLines(fileName);
                for (const auto& i : lines)
                {
                    _print(i);
                }
                DJV_ASSERT(_text == lines[0]);
                DJV_ASSERT(_text2 == lines[2]);
            }
        }
        
    } // namespace SystemTest
} // namespace djv

