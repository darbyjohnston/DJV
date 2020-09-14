// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvTestLib/Test.h>

namespace djv
{
    namespace CoreTest
    {
        class VectorFuncTest : public Test::ITest
        {
        public:
            VectorFuncTest(
                const Core::FileSystem::Path& tempPath,
                const std::shared_ptr<Core::Context>&);
            
            void run() override;
            
        private:
            void _compare();
            void _serialize();
            void _hash();
        };
        
    } // namespace CoreTest
} // namespace djv

