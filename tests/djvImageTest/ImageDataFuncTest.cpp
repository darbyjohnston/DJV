// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvImageTest/ImageDataFuncTest.h>

#include <djvImage/ImageData.h>
#include <djvImage/ImageDataFunc.h>

using namespace djv::Core;
using namespace djv::Image;

namespace djv
{
    namespace ImageTest
    {
        ImageDataFuncTest::ImageDataFuncTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::ImageTest::ImageDataFuncTest", tempPath, context)
        {}
        
        void ImageDataFuncTest::run()
        {
            _serialize();
        }
        
        void ImageDataFuncTest::_serialize()
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
            {}
            
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
            {}
        }
        
    } // namespace ImageTest
} // namespace djv

