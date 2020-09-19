// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvTestLib/Test.h>

namespace djv
{
    namespace MathTest
    {
        class VectorFuncTest : public Test::ITest
        {
        public:
            VectorFuncTest(
                const System::File::Path& tempPath,
                const std::shared_ptr<System::Context>&);
            
            void run() override;
            
        private:
            void _compare();
            void _serialize();
            void _hash();
        };
        
    } // namespace MathTest
} // namespace djv

