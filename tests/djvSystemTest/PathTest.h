// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvTestLib/Test.h>

namespace djv
{
    namespace SystemTest
    {
        class PathTest : public Test::ITest
        {
        public:
            PathTest(
                const System::File::Path& tempPath,
                const std::shared_ptr<System::Context>&);
            
            void run() override;
            
        private:
            void _path();
            void _split();
            void _operators();
            void _enum();
            void _util();
            void _serialize();
        };
        
    } // namespace SystemTest
} // namespace djv

