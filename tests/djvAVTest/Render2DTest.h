// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvTestLib/Test.h>

namespace djv
{
    namespace AVTest
    {
        class Render2DTest : public Test::ITest
        {
        public:
            Render2DTest(const std::shared_ptr<Core::Context>&);
            
            void run() override;
            
        private:
            void _system();
            void _operators();
        };
        
    } // namespace AVTest
} // namespace djv

