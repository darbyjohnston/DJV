// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/FileSystemTest.h>

#include <djvCore/FileSystem.h>
#include <djvCore/Path.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        FileSystemTest::FileSystemTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Core::Context>& context) :
            ITest(
                "djv::CoreTest::FileSystemTest",
                FileSystem::Path(tempPath, "FileSystemTest"),
                context)
        {}
        
        void FileSystemTest::run()
        {
            FILE* f = FileSystem::fopen(FileSystem::Path(getTempPath(), "file.txt").get().c_str(), "w");
            DJV_ASSERT(f);
            fclose(f);
        }
        
    } // namespace CoreTest
} // namespace djv

