// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvRender2DTest/DataTest.h>

#include <djvRender2D/Data.h>

using namespace djv::Core;
using namespace djv::Render2D;

namespace djv
{
    namespace Render2DTest
    {
        DataTest::DataTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::Render2DTest::DataTest", tempPath, context)
        {}
        
        void DataTest::run()
        {
            {
                ImageOptions options;
                options.channelsDisplay = ImageChannelsDisplay::Red;
                DJV_ASSERT(options == options);
                DJV_ASSERT(options != ImageOptions());
            }
            
            _enum();
            _serialize();
        }

        void DataTest::_enum()
        {
            for (const auto& i : getImageChannelsDisplayEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Image channel display: " + _getText(ss.str()));
            }

            for (const auto& i : getImageCacheEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Image channel display: " + _getText(ss.str()));
            }

            for (const auto& i : getImageFilterEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Image channel display: " + _getText(ss.str()));
            }
        }
        
        void DataTest::_serialize()
        {
            {
                const ImageFilter value = ImageFilter::Linear;
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(value, allocator);
                ImageFilter value2 = ImageFilter::First;
                fromJSON(json, value2);
                DJV_ASSERT(value == value2);
            }

            try
            {
                ImageFilter value = ImageFilter::First;
                auto json = rapidjson::Value(rapidjson::kObjectType);
                fromJSON(json, value);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
            
            {
                const ImageFilterOptions value;
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(value, allocator);
                ImageFilterOptions value2;
                fromJSON(json, value2);
                DJV_ASSERT(value == value2);
            }

            try
            {
                ImageFilterOptions value;
                auto json = rapidjson::Value(rapidjson::kArrayType);
                fromJSON(json, value);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
        }

    } // namespace Render2DTest
} // namespace djv

