// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvSystemTest/TimerTest.h>

#include <djvSystem/Timer.h>
#include <djvSystem/Context.h>

#include <sstream>

using namespace djv::Core;
using namespace djv::System;

namespace djv
{
    namespace SystemTest
    {
        TimerTest::TimerTest(
            const File::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITickTest("djv::SystemTest::TimerTest", tempPath, context)
        {}
        
        void TimerTest::run()
        {
            if (auto context = getContext().lock())
            {
                auto timer = Timer::create(context);
                DJV_ASSERT(!timer->isActive());
                DJV_ASSERT(!timer->isRepeating());
                
                timer->start(
                    std::chrono::milliseconds(250),
                    [this](const std::chrono::steady_clock::time_point&, const Time::Duration& duration)
                    {
                        std::stringstream ss;
                        ss << duration.count();
                        _print(ss.str());
                    });
                DJV_ASSERT(timer->isActive());
                
                _tickFor(std::chrono::milliseconds(500));
            }

            if (auto context = getContext().lock())
            {
                auto timer = Timer::create(context);
                timer->setRepeating(true);
                DJV_ASSERT(timer->isRepeating());
                
                timer->start(
                    std::chrono::milliseconds(250),
                    [this](const std::chrono::steady_clock::time_point&, const Time::Duration& duration)
                    {
                        std::stringstream ss;
                        ss << duration.count();
                        _print(ss.str());
                    });
                DJV_ASSERT(timer->isActive());
                
                _tickFor(std::chrono::milliseconds(1000));
                timer->stop();
                DJV_ASSERT(!timer->isActive());
            }
        }
        
    } // namespace SystemTest
} // namespace djv

