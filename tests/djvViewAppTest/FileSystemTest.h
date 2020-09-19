// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvTestLib/Test.h>

namespace djv
{
    namespace System
    {
        namespace File
        {
            class Info;
        
        } // namespace File
    } // namespace System

    namespace ViewAppTest
    {
        class FileSystemTest : public Test::ITest
        {
        public:
            FileSystemTest(
                const System::File::Path& tempPath,
                const std::shared_ptr<System::Context>&);
            
            void run() override;

        private:
            void _writeImage(const System::File::Info&);
        };
        
    } // namespace ViewAppTest
} // namespace djv

