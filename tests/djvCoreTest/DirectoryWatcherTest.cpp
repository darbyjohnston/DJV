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
        DirectoryWatcherTest::DirectoryWatcherTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Core::Context>& context) :
            ITickTest(
                "djv::CoreTest::DirectoryWatcherTest",
                FileSystem::Path(tempPath, "DirectoryWatcherTest"),
                context)
        {}
        
        void DirectoryWatcherTest::run()
        {
            if (auto context = getContext().lock())
            {
                auto watcher = FileSystem::DirectoryWatcher::create(context);
                const FileSystem::Path path(getTempPath());
                watcher->setPath(path);
                DJV_ASSERT(path == watcher->getPath());
                bool changed = false;
                watcher->setCallback(
                    [&changed]
                    {
                        changed = true;
                    });
                
                _tickFor(std::chrono::milliseconds(1000));
                
                for (size_t i = 0; i < 10; ++i)
                {
                    std::stringstream ss;
                    ss << "file" << i << ".txt";
                    auto io = FileSystem::FileIO::create();
                    io->open(
                        FileSystem::Path(path, ss.str()).get(),
                        FileSystem::FileIO::Mode::Write);
                }
                
                _tickFor(std::chrono::milliseconds(1000));
                
                std::stringstream ss;
                ss << "changed: " << changed;
                _print(ss.str());
            }
        }
        
    } // namespace CoreTest
} // namespace djv

