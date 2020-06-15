// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/SpeedTest.h>

#include <djvCore/Speed.h>

#include <iostream>
#include <sstream>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        SpeedTest::SpeedTest(const std::shared_ptr<Context>& context) :
            ITest("djv::CoreTest::SpeedTest", context)
        {}
        
        void SpeedTest::run()
        {
            for (auto i : Time::getFPSEnums())
            {
                std::stringstream ss;
                ss << i;
                std::stringstream ss2;
                ss2 << "fps string: " << _getText(ss.str());
                _print(ss.str());
            }
        
            {
                DJV_ASSERT(Time::fromSpeed(Time::FPS::_24) == Math::Rational(24, 1));
                Time::FPS fps = Time::FPS::First;
                DJV_ASSERT(Time::toSpeed(Math::Rational(24, 1), fps));
                DJV_ASSERT(Time::FPS::_24 == fps);
            }
            
            {
                const Math::Rational r = Time::fromSpeed(Time::FPS::_29_97);
                DJV_ASSERT(r == Math::Rational(30000, 1001));
            }
            
            {
                Time::setDefaultSpeed(Time::FPS::_25);
                DJV_ASSERT(Time::FPS::_25 == Time::getDefaultSpeed());
                Time::setDefaultSpeed(Time::FPS::_24);
            }
            
            {
                const Math::Rational speed(30000, 1001);
                DJV_ASSERT(Time::fromSpeed(Time::FPS::_29_97) == speed);
            }
        }
        
    } // namespace CoreTest
} // namespace djv

