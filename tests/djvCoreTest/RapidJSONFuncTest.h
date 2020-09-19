// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvTestLib/Test.h>

namespace djv
{
    namespace CoreTest
    {
        class RapidJSONFuncTest : public Test::ITest
        {
        public:
            RapidJSONFuncTest(
                const System::File::Path& tempPath,
                const std::shared_ptr<System::Context>&);
            
            void run() override;
        
        private:
            void _io();
            void _conversion();
        };
        
    } // namespace CoreTest
} // namespace djv

