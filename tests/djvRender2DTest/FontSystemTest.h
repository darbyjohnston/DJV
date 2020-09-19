// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvTestLib/TickTest.h>

namespace djv
{
    namespace Render2DTest
    {
        class FontSystemTest : public Test::ITickTest
        {
        public:
            FontSystemTest(
                const System::File::Path& tempPath,
                const std::shared_ptr<System::Context>&);
            
            void run() override;
        
        private:
            void _info();
            void _metrics();
            void _textLine();
            void _glyphInfo();
            void _glyph();
            void _system();
            void _operators();
        };
        
    } // namespace Render2DTest
} // namespace djv

