// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvGLTest/EnumTest.h>

#include <djvGL/Enum.h>

using namespace djv::Core;
using namespace djv::GL;

namespace djv
{
    namespace GLTest
    {
        EnumTest::EnumTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::GLTest::EnumTest", tempPath, context)
        {}
        
        void EnumTest::run()
        {
            for (auto i : getSwapIntervalEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Swap interval: " + _getText(ss.str()));
            }

            {
                const SwapInterval value = SwapInterval::First;
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(value, allocator);
                SwapInterval value2 = SwapInterval::Count;
                fromJSON(json, value2);
                DJV_ASSERT(value == value2);
            }

            try
            {
                auto json = rapidjson::Value(rapidjson::kObjectType);
                SwapInterval value;
                fromJSON(json, value);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
        }

    } // namespace GLTest
} // namespace djv

