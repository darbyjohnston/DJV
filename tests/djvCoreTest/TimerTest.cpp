// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/TimerTest.h>

#include <djvCore/Timer.h>
#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        TimerTest::TimerTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Core::Context>& context) :
            ITickTest("djv::CoreTest::TimerTest", tempPath, context)
        {}
        
        void TimerTest::run()
        {
            for (auto i : Time::getTimerValueEnums())
            {
                std::stringstream ss;
                ss << i;
                _print(_getText(ss.str()));
            }

            if (auto context = getContext().lock())
            {
                auto timer = Time::Timer::create(context);
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
                auto timer = Time::Timer::create(context);
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
        
    } // namespace CoreTest
} // namespace djv

