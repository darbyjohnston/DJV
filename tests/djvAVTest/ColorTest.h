// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvTestLib/Test.h>

namespace djv
{
    namespace AVTest
    {
        class ColorTest : public Test::ITest
        {
        public:
            ColorTest(
                const Core::FileSystem::Path& tempPath,
                const std::shared_ptr<Core::Context>&);
            
            void run() override;
            
        private:
            void _ctor();
            void _getSet();
            void _rgbToHSV(float, float, float);
            void _util();
            void _operators();
        };
        
    } // namespace AVTest
} // namespace djv

