// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvTestLib/Test.h>

namespace djv
{
    namespace CoreTest
    {
        class MemoryFuncTest : public Test::ITest
        {
        public:
            MemoryFuncTest(
                const Core::FileSystem::Path& tempPath,
                const std::shared_ptr<Core::Context>&);
            
            void run() override;
        
        private:
            void _label();
            void _endian();
            void _hash();
        };
        
    } // namespace CoreTest
} // namespace djv

