// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvTestLib/Test.h>

namespace djv
{
    namespace CoreTest
    {
        class EventTest : public Test::ITest
        {
        public:
            EventTest(const std::shared_ptr<Core::Context>&);
            
            void run() override;

        private:
            void _enum();
            void _event();
            void _familyEvent();
            void _coreEvent();
            void _renderEvent();
            void _inputEvent();
        };
        
    } // namespace CoreTest
} // namespace djv

