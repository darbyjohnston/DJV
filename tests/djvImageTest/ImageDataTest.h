// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvTestLib/Test.h>

namespace djv
{
    namespace ImageTest
    {
        class ImageDataTest : public Test::ITest
        {
        public:
            ImageDataTest(
                const System::File::Path& tempPath,
                const std::shared_ptr<System::Context>&);
            
            void run() override;
        
        private:
            void _mirror();
            void _layout();
            void _size();
            void _info();
            void _data();
            void _util();
            void _operators();
            void _serialize();
        };
        
    } // namespace ImageTest
} // namespace djv

