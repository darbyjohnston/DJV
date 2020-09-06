// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvTestLib/TickTest.h>

namespace djv
{
    namespace UITest
    {
        class WidgetTest : public Test::ITickTest
        {
        public:
            WidgetTest(
                const Core::FileSystem::Path& tempPath,
                const std::shared_ptr<Core::Context>&);
            
            void run() override;
        };
        
    } // namespace UITest
} // namespace djv

