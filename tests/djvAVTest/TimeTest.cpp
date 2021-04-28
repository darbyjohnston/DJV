// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/TimeTest.h>

#include <djvAV/Speed.h>
#include <djvAV/Time.h>

#include <djvMath/Rational.h>

#include <sstream>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        TimeTest::TimeTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::AVTest::TimeTest", tempPath, context)
        {}
        
        void TimeTest::run()
        {
            _conversion();
            _keycode();
            _timecode();
            _serialize();
        }
        
        void TimeTest::_conversion()
        {
            {
                int64_t t = 1;
                int64_t t2 = Time::scale(t, Math::IntRational(2, 1), Math::IntRational(1, 1));
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
        
        void TimeTest::_keycode()
        {
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
        }
        
        void TimeTest::_timecode()
        {
            {
                uint32_t t = Time::timeToTimecode(1, 2, 3, 4);
                int64_t f = Time::timecodeToFrame(t, fromSpeed(getDefaultSpeed()));
                DJV_ASSERT(Time::frameToTimecode(f, fromSpeed(getDefaultSpeed())) == t);
            }
            
            {
                uint32_t f = Time::frameToTimecode(100, Math::IntRational(0, 0));
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
                int64_t f = Time::timecodeToFrame(0, Math::IntRational(0, 0));
                DJV_ASSERT(0 == f);
            }

            for (auto i : Time::getUnitsEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Time units: " + _getText(ss.str()));
            }
        }
        
        void TimeTest::_serialize()
        {
            {
                const Math::Frame::Number frame = 100;
                const std::string s = Time::toString(frame, fromSpeed(getDefaultSpeed()), Time::Units::Frames);
                _print("Frames: " + s);
                DJV_ASSERT(frame == Time::fromString(s, fromSpeed(getDefaultSpeed()), Time::Units::Frames));
            }

            {
                const Math::Frame::Number frame = 100;
                const std::string s = Time::toString(frame, fromSpeed(getDefaultSpeed()), Time::Units::Timecode);
                _print("Timecode: " + s);
                DJV_ASSERT(frame == Time::fromString(s, fromSpeed(getDefaultSpeed()), Time::Units::Timecode));
            }
            
            {
                const Time::Units v = Time::Units::Frames;
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(v, allocator);
                Time::Units v2 = Time::Units::First;
                fromJSON(json, v2);
                DJV_ASSERT(v == v2);
            }

            try
            {
                auto json = rapidjson::Value(rapidjson::kObjectType);
                Time::Units v = Time::Units::First;
                fromJSON(json, v);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
        }
        
    } // namespace AVTest
} // namespace djv

