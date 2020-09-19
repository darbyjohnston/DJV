// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvSystemTest/TimerFuncTest.h>

#include <djvSystem/TimerFunc.h>

#include <sstream>

using namespace djv::Core;
using namespace djv::System;

namespace djv
{
    namespace SystemTest
    {
        TimerFuncTest::TimerFuncTest(
            const File::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITickTest("djv::SystemTest::TimerFuncTest", tempPath, context)
        {}
        
        void TimerFuncTest::run()
        {
            for (auto i : getTimerValueEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Timer value: " + _getText(ss.str()));
            }
        }

    } // namespace SystemTest
} // namespace djv

