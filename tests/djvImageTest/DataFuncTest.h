// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvTestLib/Test.h>

namespace djv
{
    namespace ImageTest
    {
        class DataFuncTest : public Test::ITest
        {
        public:
            DataFuncTest(
                const System::File::Path& tempPath,
                const std::shared_ptr<System::Context>&);
            
            void run() override;
        
        private:
            void _util();
        };
        
    } // namespace ImageTest
} // namespace djv

