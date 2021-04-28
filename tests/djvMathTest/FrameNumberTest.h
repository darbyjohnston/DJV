// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvTestLib/Test.h>

namespace djv
{
    namespace MathTest
    {
        class FrameNumberTest : public Test::ITest
        {
        public:
            FrameNumberTest(
                const System::File::Path& tempPath,
                const std::shared_ptr<System::Context>&);
            
            void run() override;
            
        private:
            void _sequence();
            void _operators();
            void _conversion();
            void _serialize();
        };
        
    } // namespace MathTest
} // namespace djv

