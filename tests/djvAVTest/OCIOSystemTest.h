// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvTestLib/Test.h>

namespace djv
{
    namespace AVTest
    {
        class OCIOSystemTest : public Test::ITest
        {
        public:
            OCIOSystemTest(
                const Core::FileSystem::Path& tempPath,
                const std::shared_ptr<Core::Context>&);
            
            void run() override;
            
        private:
            void _config();
            void _system();
            void _operators();
            void _serialize();
        };
        
    } // namespace AVTest
} // namespace djv

