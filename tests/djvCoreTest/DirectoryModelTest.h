// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvTestLib/TickTest.h>

namespace djv
{
    namespace CoreTest
    {
        class DirectoryModelTest : public Test::ITickTest
        {
        public:
            DirectoryModelTest(const std::shared_ptr<Core::Context>&);
            
            void run() override;
        };
        
    } // namespace CoreTest
} // namespace djv

