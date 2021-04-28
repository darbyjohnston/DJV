// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvTestLib/Test.h>

namespace djv
{
    namespace AVTest
    {
        class TimeTest : public Test::ITest
        {
        public:
            TimeTest(
                const System::File::Path& tempPath,
                const std::shared_ptr<System::Context>&);
            
            void run() override;
        
        private:
            void _conversion();
            void _keycode();
            void _timecode();
            void _serialize();
        };
        
    } // namespace AVTest
} // namespace djv

