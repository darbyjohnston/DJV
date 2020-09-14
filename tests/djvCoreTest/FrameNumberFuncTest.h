// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvTestLib/Test.h>

namespace djv
{
    namespace CoreTest
    {
        class FrameNumberFuncTest : public Test::ITest
        {
        public:
            FrameNumberFuncTest(
                const Core::FileSystem::Path& tempPath,
                const std::shared_ptr<Core::Context>&);
            
            void run() override;
            
        private:
            void _conversion();
            void _serialize();
        };
        
    } // namespace CoreTest
} // namespace djv

