// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvTestLib/Test.h>

namespace djv
{
    namespace AudioTest
    {
        class DataTest : public Test::ITest
        {
        public:
            DataTest(
                const System::File::Path& tempPath,
                const std::shared_ptr<System::Context>&);
            
            void run() override;
            
        private:
            void _info();
            void _data();
            void _operators();
        };
        
    } // namespace AudioTest
} // namespace djv

