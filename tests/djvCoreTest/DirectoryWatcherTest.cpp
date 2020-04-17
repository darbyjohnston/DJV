// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/DirectoryWatcherTest.h>

#include <djvCore/Context.h>
#include <djvCore/DirectoryWatcher.h>
#include <djvCore/FileIO.h>
#include <djvCore/Path.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        DirectoryWatcherTest::DirectoryWatcherTest(const std::shared_ptr<Core::Context>& context) :
            ITickTest("djv::CoreTest::DirectoryWatcherTest", context)
        {}
        
        void DirectoryWatcherTest::run()
        {
            if (auto context = getContext().lock())
            {
                auto watcher = FileSystem::DirectoryWatcher::create(context);
                const FileSystem::Path path(".");
                watcher->setPath(path);
                DJV_ASSERT(path == watcher->getPath());
                bool changed = false;
                watcher->setCallback(
                    [&changed]
                    {
                        changed = true;
                    });
                
                _tickFor(std::chrono::milliseconds(1000));
                
                auto io = FileSystem::FileIO::create();
                io->open(
                    std::string(FileSystem::Path(path, "DirectoryWatcherTest")),
                    FileSystem::FileIO::Mode::Write);
                io->close();
                
                _tickFor(std::chrono::milliseconds(1000));
                
                std::stringstream ss;
                ss << "changed: " << changed;
                _print(ss.str());
            }
        }
        
    } // namespace CoreTest
} // namespace djv

