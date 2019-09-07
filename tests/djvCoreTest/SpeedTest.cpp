//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <sstream>

namespace djv
{
    using namespace Core;

    namespace CoreTest
    {
        SpeedTest::SpeedTest(const std::shared_ptr<Context>& context) :
            ITest("djv::CoreTest::SpeedTest", context)
        {}
        
        void SpeedTest::run(const std::vector<std::string>& args)
        {
            {
                DJV_ASSERT(Time::toRational(Time::FPS::_24) == Math::Rational(24, 1));
                DJV_ASSERT(Time::FPS::_24 == Time::fromRational(Math::Rational(1, 24)));
            }
            {
                DJV_ASSERT(Time::FPS::_24 == Time::getDefaultSpeed());
                Time::setDefaultSpeed(Time::FPS::_25);
                DJV_ASSERT(Time::FPS::_25 == Time::getDefaultSpeed());
            }
            {
                const Math::Rational r = Time::toRational(Time::FPS::_29_97);
                DJV_ASSERT(r == Math::Rational(30000, 1001));
            }
        }
        
    } // namespace CoreTest
} // namespace djv

