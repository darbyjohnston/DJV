// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/SpeedTest.h>

#include <djvAV/Speed.h>

#include <sstream>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        SpeedTest::SpeedTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::AVTest::SpeedTest", tempPath, context)
        {}
        
        void SpeedTest::run()
        {
            _enum();
            _serialize();
        }
        
        void SpeedTest::_enum()
        {
            for (auto i : getFPSEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("FPS: " + _getText(ss.str()));
            }
        }
        
        void SpeedTest::_serialize()
        {        
            {
                DJV_ASSERT(fromSpeed(FPS::_24) == Math::IntRational(24, 1));
                FPS fps = FPS::First;
                DJV_ASSERT(toSpeed(Math::IntRational(24, 1), fps));
                DJV_ASSERT(FPS::_24 == fps);
                DJV_ASSERT(!toSpeed(Math::IntRational(2, 1), fps));
            }
            
            {
                const Math::IntRational r = fromSpeed(FPS::_29_97);
                DJV_ASSERT(r == Math::IntRational(30000, 1001));
            }

            {
                const Math::IntRational speed(30000, 1001);
                DJV_ASSERT(fromSpeed(FPS::_29_97) == speed);
            }

            {
                DJV_ASSERT(fromSpeed(FPS::_23_976) == fromSpeed(23.976F));
                DJV_ASSERT(fromSpeed(FPS::_23_976) == fromSpeed(23.98F));
                DJV_ASSERT(fromSpeed(FPS::_24) == fromSpeed(24.F));
                DJV_ASSERT(fromSpeed(FPS::_29_97) == fromSpeed(29.97F));
                DJV_ASSERT(fromSpeed(FPS::_30) == fromSpeed(30.F));
                DJV_ASSERT(fromSpeed(2.F) == fromSpeed(2.F));
            }
            
            {
                setDefaultSpeed(FPS::_25);
                DJV_ASSERT(FPS::_25 == getDefaultSpeed());
                setDefaultSpeed(FPS::_24);
            }
            
            {
                const FPS v = FPS::_24;
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(v, allocator);
                FPS v2 = FPS::First;
                fromJSON(json, v2);
                DJV_ASSERT(v == v2);
            }

            try
            {
                auto json = rapidjson::Value(rapidjson::kObjectType);
                FPS v = FPS::First;
                fromJSON(json, v);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
        }
        
    } // namespace AVTest
} // namespace djv

