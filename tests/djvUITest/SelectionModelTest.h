// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvTestLib/TickTest.h>

namespace djv
{
    namespace UITest
    {
        class SelectionModelTest : public Test::ITickTest
        {
        public:
            SelectionModelTest(
                const System::File::Path& tempPath,
                const std::shared_ptr<System::Context>&);
            
            void run() override;
        };
        
    } // namespace UITest
} // namespace djv

