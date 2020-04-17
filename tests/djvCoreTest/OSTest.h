// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvTestLib/Test.h>

namespace djv
{
    namespace CoreTest
    {
        class OSTest : public Test::ITest
        {
        public:
            OSTest(const std::shared_ptr<Core::Context>&);
            
            void run() override;
        };
        
    } // namespace CoreTest
} // namespace djv

