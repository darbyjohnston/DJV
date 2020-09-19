// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvSystemTest/EventFuncTest.h>

#include <djvSystem/EventFunc.h>

#include <sstream>

using namespace djv::Core;
using namespace djv::System;

namespace djv
{
    namespace SystemTest
    {
        EventFuncTest::EventFuncTest(
            const File::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest("djv::SystemTest::EventFuncTest", tempPath, context)
        {}
        
        void EventFuncTest::run()
        {
            _enum();
            _serialize();
        }

        void EventFuncTest::_enum()
        {
            for (auto i : Event::getTypeEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Event type: " + _getText(ss.str()));
            }
        }
        
        void EventFuncTest::_serialize()
        {
            for (auto i : Event::getTypeEnums())
            {
                std::stringstream ss;
                ss << i;
                Event::Type j = Event::Type::First;
                ss >> j;
                DJV_ASSERT(i == j);
            }
        }
        
    } // namespace SystemTest
} // namespace djv

