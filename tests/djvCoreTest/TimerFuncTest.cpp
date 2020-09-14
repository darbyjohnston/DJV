// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/TimerFuncTest.h>

#include <djvCore/TimerFunc.h>

#include <sstream>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        TimerFuncTest::TimerFuncTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Core::Context>& context) :
            ITickTest("djv::CoreTest::TimerFuncTest", tempPath, context)
        {}
        
        void TimerFuncTest::run()
        {
            for (auto i : Time::getTimerValueEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Timer value: " + _getText(ss.str()));
            }
        }

    } // namespace CoreTest
} // namespace djv

