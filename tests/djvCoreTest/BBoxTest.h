// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvTestLib/Test.h>

namespace djv
{
    namespace CoreTest
    {
        class BBoxTest : public Test::ITest
        {
        public:
            BBoxTest(
                const Core::FileSystem::Path& tempPath,
                const std::shared_ptr<Core::Context>&);
            
            void run() override;

        private:
            void _ctor();
            void _components();
            void _util();
            void _comparison();
        };
        
    } // namespace CoreTest
} // namespace djv

