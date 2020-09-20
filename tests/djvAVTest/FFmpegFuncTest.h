// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvTestLib/Test.h>

namespace djv
{
    namespace AVTest
    {
        class FFmpegFuncTest : public Test::ITest
        {
        public:
            FFmpegFuncTest(
                const System::File::Path& tempPath,
                const std::shared_ptr<System::Context>&);
            
            void run() override;
        
        private:
            void _convert();
            void _serialize();
        };
        
    } // namespace AVTest
} // namespace djv

