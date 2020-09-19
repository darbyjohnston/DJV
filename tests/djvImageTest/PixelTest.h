// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvTestLib/Test.h>

namespace djv
{
    namespace ImageTest
    {
        class PixelTest : public Test::ITest
        {
        public:
            PixelTest(
                const System::File::Path& tempPath,
                const std::shared_ptr<System::Context>&);
            
            void run() override;
            
        private:
            void _enum();
            void _constants();
        };
        
    } // namespace ImageTest
} // namespace djv

