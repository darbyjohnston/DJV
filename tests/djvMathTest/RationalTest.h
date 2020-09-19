// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvTestLib/Test.h>

namespace djv
{
    namespace MathTest
    {
        class RationalTest : public Test::ITest
        {
        public:
            RationalTest(
                const System::File::Path& tempPath,
                const std::shared_ptr<System::Context>&);
            
            void run() override;
        
        private:
            void _ctor();
            void _conversion();
            void _util();
            void _operators();
        };
        
    } // namespace MathTest
} // namespace djv

