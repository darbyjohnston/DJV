// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvTestLib/Test.h>

namespace djv
{
    namespace OCIOTest
    {
        class OCIOSystemTest : public Test::ITest
        {
        public:
            OCIOSystemTest(
                const System::File::Path& tempPath,
                const std::shared_ptr<System::Context>&);
            
            void run() override;
            
        private:
            void _config();
            void _system();
            void _operators();
        };
        
    } // namespace OCIOTest
} // namespace djv

