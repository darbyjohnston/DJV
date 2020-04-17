// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvTestLib/Test.h>

namespace djv
{
    namespace CoreTest
    {
        class StringFormatTest : public Test::ITest
        {
        public:
            StringFormatTest(const std::shared_ptr<Core::Context>&);
            
            void run() override;
        };
        
    } // namespace CoreTest
} // namespace djv

