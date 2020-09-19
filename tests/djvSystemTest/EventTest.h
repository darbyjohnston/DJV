// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvTestLib/Test.h>

namespace djv
{
    namespace SystemTest
    {
        class EventTest : public Test::ITest
        {
        public:
            EventTest(
                const System::File::Path& tempPath,
                const std::shared_ptr<System::Context>&);
            
            void run() override;

        private:
            void _event();
            void _familyEvent();
            void _coreEvent();
            void _renderEvent();
            void _inputEvent();
        };
        
    } // namespace SystemTest
} // namespace djv

