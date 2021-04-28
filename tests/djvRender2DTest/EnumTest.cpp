// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvRender2DTest/EnumTest.h>

#include <djvRender2D/Enum.h>

using namespace djv::Core;
using namespace djv::Render2D;

namespace djv
{
    namespace Render2DTest
    {
        EnumTest::EnumTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::Render2DTest::EnumTest", tempPath, context)
        {}
        
        void EnumTest::run()
        {
            for (auto i : getSideEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Side: " + _getText(ss.str()));
            }

            for (auto i : getAlphaBlendEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Alpha blend: " + _getText(ss.str()));
            }
            
            {
                const AlphaBlend value = AlphaBlend::First;
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(value, allocator);
                AlphaBlend value2 = AlphaBlend::Count;
                fromJSON(json, value2);
                DJV_ASSERT(value == value2);
            }

            try
            {
                auto json = rapidjson::Value(rapidjson::kObjectType);
                AlphaBlend value;
                fromJSON(json, value);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
        }

    } // namespace Render2DTest
} // namespace djv

