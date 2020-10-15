// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvImageTest/InfoFuncTest.h>

#include <djvImage/Info.h>
#include <djvImage/InfoFunc.h>

#include <djvCore/Memory.h>

using namespace djv::Core;
using namespace djv::Image;

namespace djv
{
    namespace ImageTest
    {
        InfoFuncTest::InfoFuncTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::ImageTest::InfoFuncTest", tempPath, context)
        {}
        
        void InfoFuncTest::run()
        {
            _serialize();
        }
        
        void InfoFuncTest::_serialize()
        {
            {
                const Image::Size value(1, 2);
                std::stringstream ss;
                ss << value;
                Image::Size value2;
                ss >> value2;
                DJV_ASSERT(value == value2);
            }

            try
            {
                Image::Size value;
                std::stringstream ss;
                ss >> value;
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {
            }

            {
                const Image::Size value(1, 2);
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(value, allocator);
                Image::Size value2;
                fromJSON(json, value2);
                DJV_ASSERT(value == value2);
            }

            try
            {
                auto json = rapidjson::Value(rapidjson::kObjectType);
                Image::Size value;
                fromJSON(json, value);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {
            }
        }

    } // namespace ImageTest
} // namespace djv

