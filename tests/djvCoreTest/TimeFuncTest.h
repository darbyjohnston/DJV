// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvTestLib/Test.h>

namespace djv
{
    namespace CoreTest
    {
        class TimeFuncTest : public Test::ITest
        {
        public:
            TimeFuncTest(
                const Core::FileSystem::Path& tempPath,
                const std::shared_ptr<Core::Context>&);
            
            void run() override;
        
        private:
            void _conversion();
            void _keycode();
            void _timecode();
            void _serialize();
        };
        
    } // namespace CoreTest
} // namespace djv

