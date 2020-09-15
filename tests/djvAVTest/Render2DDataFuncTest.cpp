// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/Render2DDataFuncTest.h>

#include <djvAV/Render2DDataFunc.h>

using namespace djv::Core;
using namespace djv::AV;
using namespace djv::AV::Render2D;

namespace djv
{
    namespace AVTest
    {
        Render2DDataFuncTest::Render2DDataFuncTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest("djv::AVTest::Render2DDataFuncTest", tempPath, context)
        {}
        
        void Render2DDataFuncTest::run()
        {
            _enum();
            _serialize();
        }
        
        void Render2DDataFuncTest::_enum()
        {
            for (const auto& i : getImageChannelDisplayEnums())
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
        
        void Render2DDataFuncTest::_serialize()
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
        
    } // namespace AVTest
} // namespace djv

