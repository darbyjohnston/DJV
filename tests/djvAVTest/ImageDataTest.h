// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvTestLib/Test.h>

namespace djv
{
    namespace AVTest
    {
        class ImageDataTest : public Test::ITest
        {
        public:
            ImageDataTest(const std::shared_ptr<Core::Context>&);
            
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
        
    } // namespace AVTest
} // namespace djv

