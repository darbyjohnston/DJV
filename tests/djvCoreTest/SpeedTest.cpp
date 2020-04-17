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
                DJV_ASSERT(Time::toRational(Time::FPS::_24) == Math::Rational(24, 1));
                DJV_ASSERT(Time::FPS::_24 == Time::fromRational(Math::Rational(24, 1)));
            }
            
            {
                const Math::Rational r = Time::toRational(Time::FPS::_29_97);
                DJV_ASSERT(r == Math::Rational(30000, 1001));
            }
            
            {
                Time::setDefaultSpeed(Time::FPS::_25);
                DJV_ASSERT(Time::FPS::_25 == Time::getDefaultSpeed());
                Time::setDefaultSpeed(Time::FPS::_24);
            }
            
            {
                const Time::Speed speed;
                DJV_ASSERT(Time::Speed(Time::FPS::_24) == speed);
            }
            
            {
                const Time::Speed speed(30000, 1001);
                DJV_ASSERT(Time::Speed(Time::FPS::_29_97) == speed);
            }
            
            {
                const Math::Rational r = Math::Rational(24, 1);
                const Time::Speed speed(r);
                DJV_ASSERT(r == speed);
            }
        }
        
    } // namespace CoreTest
} // namespace djv

