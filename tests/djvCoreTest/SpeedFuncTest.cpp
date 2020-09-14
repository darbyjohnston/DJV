// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/SpeedFuncTest.h>

#include <djvCore/SpeedFunc.h>

#include <sstream>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        SpeedFuncTest::SpeedFuncTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::SpeedFuncTest", tempPath, context)
        {}
        
        void SpeedFuncTest::run()
        {
            _enum();
            _serialize();
        }
        
        void SpeedFuncTest::_enum()
        {
            for (auto i : Time::getFPSEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("FPS: " + _getText(ss.str()));
            }
        }
        
        void SpeedFuncTest::_serialize()
        {        
            {
                DJV_ASSERT(Time::fromSpeed(Time::FPS::_24) == Math::Rational(24, 1));
                Time::FPS fps = Time::FPS::First;
                DJV_ASSERT(Time::toSpeed(Math::Rational(24, 1), fps));
                DJV_ASSERT(Time::FPS::_24 == fps);
                DJV_ASSERT(!Time::toSpeed(Math::Rational(2, 1), fps));
            }
            
            {
                const Math::Rational r = Time::fromSpeed(Time::FPS::_29_97);
                DJV_ASSERT(r == Math::Rational(30000, 1001));
            }

            {
                const Math::Rational speed(30000, 1001);
                DJV_ASSERT(Time::fromSpeed(Time::FPS::_29_97) == speed);
            }

            {
                DJV_ASSERT(Time::fromSpeed(Time::FPS::_23_976) == Time::fromSpeed(23.976F));
                DJV_ASSERT(Time::fromSpeed(Time::FPS::_23_976) == Time::fromSpeed(23.98F));
                DJV_ASSERT(Time::fromSpeed(Time::FPS::_24) == Time::fromSpeed(24.F));
                DJV_ASSERT(Time::fromSpeed(Time::FPS::_29_97) == Time::fromSpeed(29.97F));
                DJV_ASSERT(Time::fromSpeed(Time::FPS::_30) == Time::fromSpeed(30.F));
                DJV_ASSERT(Time::fromSpeed(2.F) == Time::fromSpeed(2.F));
            }
            
            {
                Time::setDefaultSpeed(Time::FPS::_25);
                DJV_ASSERT(Time::FPS::_25 == Time::getDefaultSpeed());
                Time::setDefaultSpeed(Time::FPS::_24);
            }
            
            {
                const Time::FPS v = Time::FPS::_24;
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(v, allocator);
                Time::FPS v2 = Time::FPS::First;
                fromJSON(json, v2);
                DJV_ASSERT(v == v2);
            }

            try
            {
                auto json = rapidjson::Value(rapidjson::kObjectType);
                Time::FPS v = Time::FPS::First;
                fromJSON(json, v);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
        }
        
    } // namespace CoreTest
} // namespace djv

