// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvSystemTest/DirectoryWatcherTest.h>

#include <djvSystem/Context.h>
#include <djvSystem/DirectoryWatcher.h>
#include <djvSystem/FileIO.h>
#include <djvSystem/Path.h>

#include <sstream>

using namespace djv::Core;
using namespace djv::System;

namespace djv
{
    namespace SystemTest
    {
        DirectoryWatcherTest::DirectoryWatcherTest(
            const File::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITickTest(
                "djv::SystemTest::DirectoryWatcherTest",
                File::Path(tempPath, "DirectoryWatcherTest"),
                context)
        {}
        
        void DirectoryWatcherTest::run()
        {
            if (auto context = getContext().lock())
            {
                auto watcher = File::DirectoryWatcher::create(context);
                const File::Path path(getTempPath());
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
                    auto io = File::IO::create();
                    io->open(
                        File::Path(path, ss.str()).get(),
                        File::Mode::Write);
                }
                
                _tickFor(std::chrono::milliseconds(1000));
                
                std::stringstream ss;
                ss << "changed: " << changed;
                _print(ss.str());
            }
        }
        
    } // namespace SystemTest
} // namespace djv

