// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvTestLib/Test.h>

namespace djv
{
    namespace CoreTest
    {
        class MathTest : public Test::ITest
        {
        public:
            MathTest(const std::shared_ptr<Core::Context>&);
            
            void run() override;
            
        private:
            void _misc();
            void _rational();
            void _random();
        };
        
    } // namespace CoreTest
} // namespace djv

