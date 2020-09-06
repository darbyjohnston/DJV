// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvTestLib/Test.h>

namespace djv
{
    namespace CoreTest
    {
        class RationalTest : public Test::ITest
        {
        public:
            RationalTest(
                const Core::FileSystem::Path& tempPath,
                const std::shared_ptr<Core::Context>&);
            
            void run() override;
        
        private:
            void _ctor();
            void _conversion();
            void _util();
            void _operators();
            void _serialize();
        };
        
    } // namespace CoreTest
} // namespace djv

