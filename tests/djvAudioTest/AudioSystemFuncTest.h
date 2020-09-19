// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvTestLib/Test.h>

namespace djv
{
    namespace AudioTest
    {
        class AudioSystemFuncTest : public Test::ITest
        {
        public:
            AudioSystemFuncTest(
                const System::File::Path& tempPath,
                const std::shared_ptr<System::Context>&);
            
            void run() override;
        };
        
    } // namespace AudioTest
} // namespace djv

