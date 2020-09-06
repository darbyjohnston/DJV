// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvTestLib/Test.h>

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            class FileInfo;
        
        } // namespace FileSystem
    } // namespace Core

    namespace ViewAppTest
    {
        class FileSystemTest : public Test::ITest
        {
        public:
            FileSystemTest(
                const Core::FileSystem::Path& tempPath,
                const std::shared_ptr<Core::Context>&);
            
            void run() override;

        private:
            void _writeImage(const Core::FileSystem::FileInfo&);
        };
        
    } // namespace ViewAppTest
} // namespace djv

