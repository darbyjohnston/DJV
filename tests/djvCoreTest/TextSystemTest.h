// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvTestLib/TickTest.h>

namespace djv
{
    namespace CoreTest
    {
        class TextSystemTest : public Test::ITickTest
        {
        public:
            TextSystemTest(const std::shared_ptr<Core::Context>&);
            
            void run() override;
        };
        
    } // namespace CoreTest
} // namespace djv

