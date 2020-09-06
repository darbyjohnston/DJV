// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvTestLib/Test.h>

namespace djv
{
    namespace CoreTest
    {
        class UndoStackTest : public Test::ITest
        {
        public:
            UndoStackTest(
                const Core::FileSystem::Path& tempPath,
                const std::shared_ptr<Core::Context>&);
            
            void run() override;

        private:
            void _printList(const std::shared_ptr<std::vector<int> >&);
        };
        
    } // namespace CoreTest
} // namespace djv

