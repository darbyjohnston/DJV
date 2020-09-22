// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvImageTest/ColorFuncTest.h>

#include <djvImage/ColorFunc.h>

#include <djvCore/ErrorFunc.h>

using namespace djv::Core;
using namespace djv::Image;

namespace djv
{
    namespace ImageTest
    {
        ColorFuncTest::ColorFuncTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::ImageTest::ColorFuncTest", tempPath, context)
        {}
        
        void ColorFuncTest::run()
        {
            _util();
            _serialize();
        }

        void ColorFuncTest::_rgbToHSV(float r, float g, float b)
        {
            float rgb[3] = { r, g, b };
            float hsv[3] = { 0.F, 0.F, 0.F };
            Image::rgbToHSV(rgb, hsv);
            std::stringstream ss;
            ss << "rgbToHSV: " << rgb[0] << "," << rgb[1] << "," << rgb[2] << " = " <<
                hsv[0] << "," << hsv[1] << "," << hsv[2];
            _print(ss.str());
        }

        void ColorFuncTest::_util()
        {
            {
                float r = 0.F;
                float g = 0.F;
                float b = 0.F;
                for (float r = -.1F; r <= 1.1F; r += .1F)
                {
                    _rgbToHSV(r, g, b);
                }
                for (float g = -.1F; g <= 1.1F; g += .1F)
                {
                    _rgbToHSV(r, g, b);
                }
                for (float b = -.1F; b <= 1.1F; b += .1F)
                {
                    _rgbToHSV(r, g, b);
                }
            }

            for (float v = -1.F; v <= 1.1F; v += .1F)
            {
                float hsv[3] = { v, v, v };
                float rgb[3] = { 0.F, 0.F, 0.F };
                Image::hsvToRGB(hsv, rgb);
                std::stringstream ss;
                ss << "hsvToRGB: " << hsv[0] << "," << hsv[1] << "," << hsv[2] << " = " <<
                    rgb[0] << "," << rgb[1] << "," << rgb[2];
                _print(ss.str());
            }

            for (const auto& i : Image::getTypeEnums())
            {
                Image::Color c(i);
                _print("Color: " + Image::getLabel(c));
            }
        }

        void ColorFuncTest::_serialize()
        {
            {
                auto u8 = Image::Color(Image::Type::L_U8);
                u8.setU8(255, 0);
                const auto u16 = u8.convert(Image::Type::L_U16);
                std::stringstream ss;
                ss << "U8 = " << static_cast<int>(u8.getU8(0)) << ", " << "U16 = " << u16.getU16(0);
                _print(ss.str());
            }
            
            {
                auto u8 = Image::Color(Image::Type::L_U8);
                u8.setU8(255, 0);
                const auto u10 = u8.convert(Image::Type::RGB_U10);
                std::stringstream ss;
                ss << "U8 = " << static_cast<int>(u8.getU8(0)) << ", " << "U10 = " << u10.getU10(0) << ", " << u10.getU10(1) << ", " << u10.getU10(2);
                _print(ss.str());
            }
            
            {
                auto u8 = Image::Color(Image::Type::L_U8);
                u8.setU8(255, 0);
                const auto f32 = u8.convert(Image::Type::L_F32);
                std::stringstream ss;
                ss << "U8 = " << static_cast<int>(u8.getU8(0)) << ", " << "F32 = " << f32.getF32(0);
                _print(ss.str());
            }
            
            try
            {
                Image::Color c;
                std::stringstream ss;
                ss >> c;
                DJV_ASSERT(false);
            }
            catch (const std::exception& e)
            {
                _print(Error::format(e));
            }
            
            for (size_t i = 1; i < static_cast<size_t>(Image::Type::Count); ++i)
            {
                for (size_t j = 1; j < static_cast<size_t>(Image::Type::Count); ++j)
                {
                    Image::Color(static_cast<Image::Type>(i)).convert(static_cast<Image::Type>(j));
                }
            }
            
            {
                const Image::Color c(0, 1, 2, 3);
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(c, allocator);
                Image::Color c2;
                fromJSON(json, c2);
                DJV_ASSERT(c == c2);
            }
            
            {
                Image::Color c(Image::Type::RGB_U10);
                c.setU10(0, 0);
                c.setU10(1, 1);
                c.setU10(2, 2);
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(c, allocator);
                Image::Color c2;
                fromJSON(json, c2);
                DJV_ASSERT(c == c2);
            }
            
            {
                Image::Color c(Image::Type::RGBA_U16);
                c.setU16(0, 0);
                c.setU16(1, 1);
                c.setU16(3, 2);
                c.setU16(3, 3);
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(c, allocator);
                Image::Color c2;
                fromJSON(json, c2);
                DJV_ASSERT(c == c2);
            }
            
            {
                Image::Color c(Image::Type::RGBA_U32);
                c.setU32(0, 0);
                c.setU32(1, 1);
                c.setU32(2, 2);
                c.setU32(3, 3);
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(c, allocator);
                Image::Color c2;
                fromJSON(json, c2);
                DJV_ASSERT(c == c2);
            }
            
            {
                Image::Color c(Image::Type::RGBA_F16);
                c.setF16(0.F, 0);
                c.setF16(1.F, 1);
                c.setF16(2.F, 2);
                c.setF16(3.F, 3);
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(c, allocator);
                Image::Color c2;
                fromJSON(json, c2);
                DJV_ASSERT(c == c2);
            }
            
            {
                Image::Color c(Image::Type::RGBA_F32);
                c.setF32(0.F, 0);
                c.setF32(1.F, 1);
                c.setF32(2.F, 2);
                c.setF32(3.F, 3);
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(c, allocator);
                Image::Color c2;
                fromJSON(json, c2);
                DJV_ASSERT(c == c2);
            }

            try
            {
                auto json = rapidjson::Value(rapidjson::kObjectType);
                Image::Color c;
                fromJSON(json, c);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
        }
        
    } // namespace ImageTest
} // namespace djv

