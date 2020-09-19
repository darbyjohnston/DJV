// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvTestLib/Test.h>

namespace djv
{
    namespace MathTest
    {
        class RationalFuncTest : public Test::ITest
        {
        public:
            RationalFuncTest(
                const System::File::Path& tempPath,
                const std::shared_ptr<System::Context>&);
            
            void run() override;
        
        private:
            void _serialize();
        };
        
    } // namespace MathTest
} // namespace djv

