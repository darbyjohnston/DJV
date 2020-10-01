// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvSystemTest/FileInfoTest.h>

#include <djvSystem/FileIO.h>
#include <djvSystem/FileInfo.h>

#include <djvMath/FrameNumberFunc.h>

#include <iomanip>
#include <sstream>

using namespace djv::Core;
using namespace djv::System;

namespace djv
{
    namespace SystemTest
    {
        FileInfoTest::FileInfoTest(
            const File::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest(
                "djv::SystemTest::FileInfoTest",
                File::Path(tempPath, "FileInfoTest"),
                context),
            _fileName("file.txt"),
            _sequenceName("render.#.exr"),
            _sequence(1, 100)
        {}
        
        void FileInfoTest::run()
        {
            auto io = File::IO::create();
            io->open(
                File::Path(getTempPath(), _fileName).get(),
                File::Mode::Write);
            
            const File::Info info(File::Path(_sequenceName), File::Type::Sequence, _sequence);
            for (const auto& i : Math::Frame::toFrames(info.getSequence()))
            {
                io->open(
                    File::Path(getTempPath(), info.getFileName(i)).get(),
                    File::Mode::Write);
            }
            
            _ctor();
            _path();
            _sequences();
            _operators();
        }

        void FileInfoTest::_ctor()
        {
            {
                const File::Info info;
                DJV_ASSERT(info.isEmpty());
            }
            
            {
                const File::Path path("/");
                const File::Info info(path, false);
                DJV_ASSERT(path == info.getPath());
                DJV_ASSERT(!info.isEmpty());
            }
            
            {
                const File::Path path("/");
                const File::Type type = File::Type::File;
                const Math::Frame::Sequence sequence(1, 10);
                const File::Info info(path, type, sequence, false);
                DJV_ASSERT(type == info.getType());
                DJV_ASSERT(sequence == info.getSequence());
            }
            
            {
                const std::string path = "/";
                const File::Info info(path, false);
                DJV_ASSERT(path == info.getPath().get());
            }
        }

        void FileInfoTest::_path()
        {
            {
                File::Info info;
                info.setPath(File::Path(getTempPath(), _fileName));
                DJV_ASSERT(info.doesExist());
                DJV_ASSERT(File::Type::File == info.getType());
                {
                    std::stringstream ss;
                    ss << "Size: " << info.getSize();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "User: " << info.getUser();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "Permissions: " << info.getPermissions();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "Time: " << info.getTime();
                    _print(ss.str());
                }
            }
            
            {
                File::Info info;
                info.setPath(File::Path(getTempPath(), _fileName), false);
                DJV_ASSERT(info.stat());
                DJV_ASSERT(File::Type::File == info.getType());
                {
                    std::stringstream ss;
                    ss << "Size: " << info.getSize();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "User: " << info.getUser();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "Permissions: " << info.getPermissions();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "Time: " << info.getTime();
                    _print(ss.str());
                }
            }
            
            {
                File::Info info(
                    File::Path(getTempPath(), _sequenceName),
                    File::Type::Sequence,
                    _sequence,
                    false);
                DJV_ASSERT(info.stat());
                DJV_ASSERT(File::Type::Sequence == info.getType());
                {
                    std::stringstream ss;
                    ss << "Size: " << info.getSize();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "User: " << info.getUser();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "Permissions: " << info.getPermissions();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "Time: " << info.getTime();
                    _print(ss.str());
                }
            }
            
            {
                File::Info info("/tmp/render.1.exr");
                info.addToSequence(File::Info("/tmp/render.3.exr"));
                info.addToSequence(File::Info("/tmp/render.2.exr"));
                info.addToSequence(File::Info("/tmp/render.4.exr"));
                DJV_ASSERT("/tmp/render.1-4.exr" == info.getFileName());
                DJV_ASSERT("/tmp/render.1.exr" == info.getFileName(1));
                DJV_ASSERT("render.1.exr" == info.getFileName(1, false));
            }
            
            {
                const std::string root(1, File::Path::getCurrentSeparator());
                File::Info info(root);
                DJV_ASSERT(root == info.getFileName());
            }
        }

        void FileInfoTest::_sequences()
        {
            {
                File::Info info(_sequenceName);
                DJV_ASSERT(Math::Frame::Sequence() == info.getSequence());
                info.setSequence(_sequence);
                DJV_ASSERT(_sequence == info.getSequence());
                File::Info info2(info);
                info2.setSequence(Math::Frame::Sequence(Math::Frame::Range(101, 110), 4));
                DJV_ASSERT(info.isCompatible(info2));
                info.addToSequence(info2);
                DJV_ASSERT(Math::Frame::Sequence(Math::Frame::Range(1, 110), 4) == info.getSequence());
            }
            
            {
                File::Info info("render.1.exr");
                DJV_ASSERT(!info.isCompatible(File::Info("render.1.png")));
            }
            
            {
                File::Info info("render.1.exr");
                DJV_ASSERT(!info.isCompatible(File::Info("snapshot.1.exr")));
            }

            {
                File::Info info("frame#.png");
                info.setSequence(Math::Frame::Sequence(10));
                info.addToSequence(File::Info("frame09.png"));
                DJV_ASSERT(2 == info.getSequence().getPad());
            }

            {
                File::Info info("render.1.exr");
                DJV_ASSERT(!info.isCompatible(File::Info("/tmp/render.1.exr")));
            }
        }

        void FileInfoTest::_operators()
        {
            {
                const File::Info info(_fileName);
                DJV_ASSERT(info == info);
                DJV_ASSERT(info != File::Info());
                DJV_ASSERT(info < File::Info());
                DJV_ASSERT(_fileName == std::string(info));
            }
        }
        
    } // namespace SystemTest
} // namespace djv

