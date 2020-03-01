//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

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
        
        void SpeedTest::run(const std::vector<std::string>& args)
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

