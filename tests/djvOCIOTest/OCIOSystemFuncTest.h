// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvTestLib/Test.h>

namespace djv
{
    namespace OCIOTest
    {
        class OCIOSystemFuncTest : public Test::ITest
        {
        public:
            OCIOSystemFuncTest(
                const System::File::Path& tempPath,
                const std::shared_ptr<System::Context>&);
            
            void run() override;
            
        private:
            void _enum();
            void _serialize();
        };
        
    } // namespace OCIOTest
} // namespace djv

