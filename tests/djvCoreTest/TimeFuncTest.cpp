// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/TimeFuncTest.h>

#include <djvCore/TimeFunc.h>

#include <sstream>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        TimeFuncTest::TimeFuncTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::CoreTest::TimeFuncTest", tempPath, context)
        {}
        
        void TimeFuncTest::run()
        {
            {
                int hours = 0;
                int minutes = 0;
                double seconds = 0.0;
                Time::secondsToTime(3661.f, hours, minutes, seconds);
                DJV_ASSERT(1 == hours);
                DJV_ASSERT(1 == minutes);
                DJV_ASSERT(1.0 == seconds);
            }
            
            for (double i : { 0.0, 1000.0, 10000.0 })
            {
                std::stringstream ss;
                ss << "Label: " << i << " = " << Time::getLabel(i);
                _print(ss.str());
            }
            
            for (time_t i : { 0, 1000, 10000 })
            {
                std::stringstream ss;
                ss << "Label: " << i << " = " << Time::getLabel(i);
                _print(ss.str());
            }
            
            for (time_t i : { 0, 1000, 10000 })
            {
                tm result;
                Time::localtime(&i, &result);
                std::stringstream ss;
                ss << "Localtime: " << i << " = " << result.tm_hour << ":" << result.tm_min << ":" << result.tm_sec;
                _print(ss.str());
            }
        }
        
    } // namespace CoreTest
} // namespace djv

