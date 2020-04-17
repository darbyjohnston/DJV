// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvTestLib/TickTest.h>

namespace djv
{
    namespace AVTest
    {
        class ThumbnailSystemTest : public Test::ITickTest
        {
        public:
            ThumbnailSystemTest(const std::shared_ptr<Core::Context>&);
            
            void run() override;
        };
        
    } // namespace AVTest
} // namespace djv

