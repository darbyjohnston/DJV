// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/EventFuncTest.h>

#include <djvCore/EventFunc.h>

#include <sstream>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        EventFuncTest::EventFuncTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::EventFuncTest", tempPath, context)
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
        
    } // namespace CoreTest
} // namespace djv

