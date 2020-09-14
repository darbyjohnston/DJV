// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvTestLib/Test.h>

namespace djv
{
    namespace CoreTest
    {
        class MathFuncTest : public Test::ITest
        {
        public:
            MathFuncTest(
                const Core::FileSystem::Path& tempPath,
                const std::shared_ptr<Core::Context>&);
            
            void run() override;
            
        private:
            void _misc();
            void _rational();
            void _random();
            void _conversion();
            void _comparison();
        };
        
    } // namespace CoreTest
} // namespace djv

