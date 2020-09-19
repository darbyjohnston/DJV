// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvTestLib/Test.h>

namespace djv
{
    namespace MathTest
    {
        class BBoxTest : public Test::ITest
        {
        public:
            BBoxTest(
                const System::File::Path& tempPath,
                const std::shared_ptr<System::Context>&);
            
            void run() override;

        private:
            void _ctor();
            void _components();
            void _util();
            void _comparison();
        };
        
    } // namespace MathTest
} // namespace djv

