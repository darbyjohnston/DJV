// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvTestLib/Test.h>

namespace djv
{
    namespace ImageTest
    {
        class ImageFuncTest : public Test::ITest
        {
        public:
            ImageFuncTest(
                const System::File::Path& tempPath,
                const std::shared_ptr<System::Context>&);
            
            void run() override;
        };
        
    } // namespace ImageTest
} // namespace djv

