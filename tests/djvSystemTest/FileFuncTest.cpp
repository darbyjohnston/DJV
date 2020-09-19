// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvSystemTest/FileFuncTest.h>

#include <djvSystem/FileFunc.h>
#include <djvSystem/Path.h>

using namespace djv::Core;
using namespace djv::System;

namespace djv
{
    namespace SystemTest
    {
        FileFuncTest::FileFuncTest(
            const File::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest(
                "djv::SystemTest::FileFuncTest",
                File::Path(tempPath, "FileFuncTest"),
                context)
        {}
        
        void FileFuncTest::run()
        {
            FILE* f = File::fopen(File::Path(getTempPath(), "file.txt").get().c_str(), "w");
            DJV_ASSERT(f);
            fclose(f);
        }
        
    } // namespace SystemTest
} // namespace djv

