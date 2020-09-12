// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvTestLib/Test.h>

namespace djv
{
    namespace AVTest
    {
        class JPEGTest : public Test::ITest
        {
        public:
            JPEGTest(
                const Core::FileSystem::Path& tempPath,
                const std::shared_ptr<Core::Context>&);
            
            void run() override;
            
        private:
            void _serialize();
        };
        
    } // namespace AVTest
} // namespace djv

