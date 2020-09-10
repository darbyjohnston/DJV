// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/ImageUtilTest.h>

#include <djvAV/Color.h>
#include <djvAV/ImageData.h>
#include <djvAV/ImageUtil.h>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        ImageUtilTest::ImageUtilTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest("djv::AVTest::ImageUtilTest", tempPath, context)
        {}
        
        void ImageUtilTest::run()
        {
            {
                const Image::U8_T min = Image::U8Range.getMin();
                const Image::U8_T max = Image::U8Range.getMax();
                auto data = Image::Data::create(Image::Info(2, 1, Image::Type::L_U8));
                Image::U8_T* p = reinterpret_cast<Image::U8_T*>(data->getData());
                p[0] = min;
                p[1] = max;
                const auto average = Image::getAverageColor(data);
                {
                    std::stringstream ss;
                    ss << average;
                    _print("Average: " + ss.str());
                }
            }
            
            {
                const Image::U10_T min = Image::U10Range.getMin();
                const Image::U10_T max = Image::U10Range.getMax();
                auto data = Image::Data::create(Image::Info(2, 1, Image::Type::RGB_U10));
                Image::U10_S* p = reinterpret_cast<Image::U10_S*>(data->getData());
                p[0].r = min;
                p[0].g = min;
                p[0].b = min;
                p[1].r = max;
                p[1].g = max;
                p[1].b = max;
                const auto average = Image::getAverageColor(data);
                {
                    std::stringstream ss;
                    ss << average;
                    _print("Average: " + ss.str());
                }
            }
            
            {
                const Image::U16_T min = Image::U16Range.getMin();
                const Image::U16_T max = Image::U16Range.getMax();
                auto data = Image::Data::create(Image::Info(2, 1, Image::Type::L_U16));
                Image::U16_T* p = reinterpret_cast<Image::U16_T*>(data->getData());
                p[0] = min;
                p[1] = max;
                const auto average = Image::getAverageColor(data);
                {
                    std::stringstream ss;
                    ss << average;
                    _print("Average: " + ss.str());
                }
            }
            
            {
                const Image::U32_T min = Image::U32Range.getMin();
                const Image::U32_T max = Image::U32Range.getMax();
                auto data = Image::Data::create(Image::Info(2, 1, Image::Type::L_U32));
                Image::U32_T* p = reinterpret_cast<Image::U32_T*>(data->getData());
                p[0] = min;
                p[1] = max;
                const auto average = Image::getAverageColor(data);
                {
                    std::stringstream ss;
                    ss << average;
                    _print("Average: " + ss.str());
                }
            }
            
            {
                const Image::F16_T min = Image::F16Range.getMin();
                const Image::F16_T max = Image::F16Range.getMax();
                auto data = Image::Data::create(Image::Info(2, 1, Image::Type::L_F16));
                Image::F16_T* p = reinterpret_cast<Image::F16_T*>(data->getData());
                p[0] = min;
                p[1] = max;
                const auto average = Image::getAverageColor(data);
                {
                    std::stringstream ss;
                    ss << average;
                    _print("Average: " + ss.str());
                }
            }
            
            {
                const Image::F32_T min = Image::F32Range.getMin();
                const Image::F32_T max = Image::F32Range.getMax();
                auto data = Image::Data::create(Image::Info(2, 1, Image::Type::L_F32));
                Image::F32_T* p = reinterpret_cast<Image::F32_T*>(data->getData());
                p[0] = min;
                p[1] = max;
                const auto average = Image::getAverageColor(data);
                {
                    std::stringstream ss;
                    ss << average;
                    _print("Average: " + ss.str());
                }
            }
        }
                
    } // namespace AVTest
} // namespace djv

