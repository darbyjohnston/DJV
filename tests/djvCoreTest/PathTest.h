// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvTestLib/Test.h>

namespace djv
{
    namespace CoreTest
    {
        class PathTest : public Test::ITest
        {
        public:
            PathTest(const std::shared_ptr<Core::Context>&);
            
            void run() override;
            
        private:
            void _enum();
            void _path();
            void _split();
            void _util();
            void _operators();
            void _serialize();
        };
        
    } // namespace CoreTest
} // namespace djv

