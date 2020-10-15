// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvTestLib/Test.h>

namespace djv
{
    namespace ImageTest
    {
        class InfoTest : public Test::ITest
        {
        public:
            InfoTest(
                const System::File::Path& tempPath,
                const std::shared_ptr<System::Context>&);
            
            void run() override;
        
        private:
            void _mirror();
            void _layout();
            void _size();
            void _info();
        };
        
    } // namespace ImageTest
} // namespace djv

