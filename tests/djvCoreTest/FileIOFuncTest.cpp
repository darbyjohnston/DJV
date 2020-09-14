// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/FileIOFuncTest.h>

#include <djvCore/FileIO.h>
#include <djvCore/FileIOFunc.h>
#include <djvCore/Path.h>

#include <limits>
#include <sstream>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        FileIOFuncTest::FileIOFuncTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Core::Context>& context) :
            ITest(
                "djv::CoreTest::FileIOFuncTest",
                FileSystem::Path(tempPath, "FileIOFuncTest"),
                context),
            _fileName("file.txt"),
            _text("Hello"),
            _text2("world!")
        {}
        
        void FileIOFuncTest::run()
        {
            {
                auto io = FileSystem::FileIO::create();
                const std::string fileName = FileSystem::Path(getTempPath(), _fileName).get();
                io->open(
                    fileName,
                    FileSystem::FileIO::Mode::Write);
                io->write(_text + " ");
                io->open(
                    fileName,
                    FileSystem::FileIO::Mode::Append);
                io->write(_text2);

                io->open(
                    fileName,
                    FileSystem::FileIO::Mode::Read);
                std::string buf = FileSystem::readContents(io);
                _print(buf);
                DJV_ASSERT((_text + " " + _text2) == buf);
                io->setPos(0);
                DJV_ASSERT(0 == io->getPos());
            }

            {
                const std::string fileName = FileSystem::Path(getTempPath(), _fileName).get();
                FileSystem::writeLines(
                    fileName,
                    {
                        "# This is a comment",
                        _text + " " + _text2
                    });

                auto io = FileSystem::FileIO::create();
                io->open(
                    fileName,
                    FileSystem::FileIO::Mode::ReadWrite);
                char buf[String::cStringLength];
                FileSystem::readWord(io, buf);
                _print(buf);
                DJV_ASSERT(_text == buf);
                FileSystem::readWord(io, buf);
                _print(buf);
                DJV_ASSERT(_text2 == buf);
            }

            {
                const std::string fileName = FileSystem::Path(getTempPath(), _fileName).get();
                auto io = FileSystem::FileIO::create();
                io->open(
                    fileName,
                    FileSystem::FileIO::Mode::Write);
                io->write(_text + "\n" + _text2);

                io->open(
                    fileName,
                    FileSystem::FileIO::Mode::Read);
                char buf[String::cStringLength];
                FileSystem::readLine(io, buf);
                _print(buf);
                DJV_ASSERT(_text == buf);
                FileSystem::readLine(io, buf);
                _print(buf);
                DJV_ASSERT(_text2 == buf);
            }

            {
                const std::string fileName = FileSystem::Path(getTempPath(), _fileName).get();
                FileSystem::writeLines(
                    fileName,
                    {
                        _text,
                        "# This is a comment",
                        _text2
                    });

                const auto lines = FileSystem::readLines(fileName);
                for (const auto& i : lines)
                {
                    _print(i);
                }
                DJV_ASSERT(_text == lines[0]);
                DJV_ASSERT(_text2 == lines[2]);
            }
        }
        
    } // namespace CoreTest
} // namespace djv

