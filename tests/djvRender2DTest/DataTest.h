// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvTestLib/Test.h>

namespace djv
{
    namespace Render2DTest
    {
        class DataTest : public Test::ITest
        {
        public:
            DataTest(
                const System::File::Path& tempPath,
                const std::shared_ptr<System::Context>&);
            
            void run() override;

        private:
            void _enum();
            void _serialize();
        };
        
    } // namespace Render2DTest
} // namespace djv

