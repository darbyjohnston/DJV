// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvTestLib/TickTest.h>

namespace djv
{
    namespace SystemTest
    {
        class TextSystemTest : public Test::ITickTest
        {
        public:
            TextSystemTest(
                const System::File::Path& tempPath,
                const std::shared_ptr<System::Context>&);
            
            void run() override;
        };
        
    } // namespace SystemTest
} // namespace djv

