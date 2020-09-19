// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvTestLib/Test.h>

namespace djv
{
    namespace MathTest
    {
        class RangeTest : public Test::ITest
        {
        public:
            RangeTest(
                const System::File::Path& tempPath,
                const std::shared_ptr<System::Context>&);
            
            void run() override;
        
        private:
            void _ctor();
            void _util();
            void _operators();
        };
        
    } // namespace MathTest
} // namespace djv

