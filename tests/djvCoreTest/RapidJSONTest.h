// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvTestLib/Test.h>

namespace djv
{
    namespace CoreTest
    {
        class RapidJSONTest : public Test::ITest
        {
        public:
            RapidJSONTest(
                const Core::FileSystem::Path& tempPath,
                const std::shared_ptr<Core::Context>&);
            
            void run() override;
        
        private:
            void _io();
            void _conversion();
        };
        
    } // namespace CoreTest
} // namespace djv

