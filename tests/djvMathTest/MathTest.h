// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvTestLib/Test.h>

namespace djv
{
    namespace MathTest
    {
        class MathTest : public Test::ITest
        {
        public:
            MathTest(
                const System::File::Path& tempPath,
                const std::shared_ptr<System::Context>&);
            
            void run() override;
            
        private:
            void _misc();
            void _conversion();
            void _comparison();
        };
        
    } // namespace MathTest
} // namespace djv

