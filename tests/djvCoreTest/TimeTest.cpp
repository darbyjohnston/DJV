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

#include <djvCoreTest/TimeTest.h>

#include <djvCore/Speed.h>
#include <djvCore/Time.h>

#include <iostream>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        TimeTest::TimeTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::TimeTest", context)
        {}
        
        void TimeTest::run(const std::vector<std::string>& args)
        {
            {
                std::stringstream ss;
                ss << "timebase: " << Time::getTimebaseRational();
                _print(ss.str());
            }
            
            {
                int64_t t = 1;
                int64_t t2 = Time::scale(t, Math::Rational(2, 1), Math::Rational(1, 1));
                DJV_ASSERT(2 == t2);
            }
            
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
                ss << "label: " << i << " = " << Time::getLabel(i);
                _print(ss.str());
            }
            
            for (time_t i : { 0, 1000, 10000 })
            {
                std::stringstream ss;
                ss << "label: " << i << " = " << Time::getLabel(i);
                _print(ss.str());
            }
            
            for (time_t i : { 0, 1000, 10000 })
            {
                tm result;
                Time::localtime(&i, &result);
                std::stringstream ss;
                ss << "localtime: " << i << " = " << result.tm_hour << ":" << result.tm_min << ":" << result.tm_sec;
                _print(ss.str());
            }
            
            {
                const int id = 0;
                const int type = 1;
                const int prefix = 2;
                const int count = 3;
                const int offset = 4;
                std::string s = Time::keycodeToString(id, type, prefix, count, offset);
                int _id = 0;
                int _type = 0;
                int _prefix = 0;
                int _count = 0;
                int _offset = 0;
                Time::stringToKeycode(s, _id, _type, _prefix, _count, _offset);
                DJV_ASSERT(id == _id);
                DJV_ASSERT(type == _type);
                DJV_ASSERT(prefix == _prefix);
                DJV_ASSERT(count == _count);
                DJV_ASSERT(offset == _offset);
            }
            
            try
            {
                int id = 0;
                int type = 0;
                int prefix = 0;
                int count = 0;
                int offset = 0;
                Time::stringToKeycode(std::string(), id, type, prefix, count, offset);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
            
            {
                uint32_t t = Time::timeToTimecode(1, 2, 3, 4);
                int64_t f = Time::timecodeToFrame(t, Time::Speed());
                DJV_ASSERT(Time::frameToTimecode(f, Time::Speed()) == t);
            }
            
            {
                uint32_t f = Time::frameToTimecode(100, Time::Speed(0, 0));
                DJV_ASSERT(0 == f);
            }
            
            {
                uint32_t t = Time::timeToTimecode(1, 2, 3, 4);
                std::string s = Time::timecodeToString(t);
                uint32_t t2 = 0;
                Time::stringToTimecode(s, t2);
                DJV_ASSERT(t == t2);
            }

            try
            {
                uint32_t t2 = 0;
                Time::stringToTimecode("1:2:3", t2);
                Time::stringToTimecode("1:2", t2);
                Time::stringToTimecode("1", t2);
                Time::stringToTimecode("1:2:3:4:5", t2);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
            
            {
                int64_t f = Time::timecodeToFrame(0, Time::Speed(0, 0));
                DJV_ASSERT(0 == f);
            }
        }
        
    } // namespace CoreTest
} // namespace djv

