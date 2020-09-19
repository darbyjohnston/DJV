// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvImageTest/ColorTest.h>

#include <djvImage/Color.h>

#include <djvCore/ErrorFunc.h>

using namespace djv::Core;
using namespace djv::Image;

namespace djv
{
    namespace ImageTest
    {
        ColorTest::ColorTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::ImageTest::ColorTest", tempPath, context)
        {}
        
        void ColorTest::run()
        {
            _ctor();
            _getSet();
            _util();
            _operators();
        }
                
        void ColorTest::_ctor()
        {
            {
                Image::Color c;
                DJV_ASSERT(!c.isValid());
            }
            
            {
                const Image::Type type = Image::Type::L_U8;
                Image::Color c(type);
                DJV_ASSERT(type == c.getType());
                DJV_ASSERT(c.isValid());
            }
            
            {
                Image::Color c(1);
                DJV_ASSERT(Image::Type::L_U8 == c.getType());
                DJV_ASSERT(c.isValid());
            }
            
            {
                Image::Color c(0, 1, 2, 3);
                DJV_ASSERT(Image::Type::RGBA_U8 == c.getType());
                DJV_ASSERT(c.isValid());
            }
            
            {
                Image::Color c(0.F, 1.F, 2.F, 3.F);
                DJV_ASSERT(Image::Type::RGBA_F32 == c.getType());
                DJV_ASSERT(c.isValid());
            }
            
            {
                Image::Color c(1.F);
                DJV_ASSERT(Image::Type::L_F32 == c.getType());
                DJV_ASSERT(c.isValid());
            }
        }
        
        void ColorTest::_getSet()
        {
            {
                Image::Color c(Image::Type::L_U8);
                c.setU8(1, 0);
                DJV_ASSERT(1 == c.getU8(0));
            }
            
            {
                Image::Color c(Image::Type::L_U16);
                c.setU16(1, 0);
                DJV_ASSERT(1 == c.getU16(0));
            }
            
            {
                Image::Color c(Image::Type::L_U32);
                c.setU32(1, 0);
                DJV_ASSERT(1 == c.getU32(0));
            }
            
            {
                Image::Color c(Image::Type::L_F16);
                c.setF16(1.F, 0);
                DJV_ASSERT(1.F == c.getF16(0));
            }
            
            {
                Image::Color c(Image::Type::L_F32);
                c.setF32(1.F, 0);
                DJV_ASSERT(1.f == c.getF32(0));
            }
            
            {
                Image::Color c(Image::Type::L_U8);
                uint8_t buf[1] = { 1 };
                c.setData(buf);
                DJV_ASSERT(1 == c.getU8(0));
            }
            
            {
                Image::Color c(Image::Type::L_U8);
                c.getData()[0] = 1;
                DJV_ASSERT(1 == c.getU8(0));
            }
        }
        
        void ColorTest::_rgbToHSV(float r, float g, float b)
        {
            float rgb[3] = { r, g, b };
            float hsv[3] = { 0.F, 0.F, 0.F };
            Image::Color::rgbToHSV(rgb, hsv);
            std::stringstream ss;
            ss << "rgbToHSV: " << rgb[0] << "," << rgb[1] << "," << rgb[2] << " = " <<
                hsv[0] << "," << hsv[1] << "," << hsv[2];
            _print(ss.str());
        }
        
        void ColorTest::_util()
        {
            {
                Image::Color c(1, 2, 3);
                c.zero();
                DJV_ASSERT(0 == c.getU8(0));
                DJV_ASSERT(0 == c.getU8(1));
                DJV_ASSERT(0 == c.getU8(2));
            }
            
            {
                Image::Color c = Image::Color::RGB_U8(1, 2, 3);
                DJV_ASSERT(Image::Type::RGB_U8 == c.getType());
                DJV_ASSERT(1 == c.getU8(0));
                DJV_ASSERT(2 == c.getU8(1));
                DJV_ASSERT(3 == c.getU8(2));
            }
            
            {
                Image::Color c = Image::Color::RGB_F32(1.F, 2.F, 3.F);
                DJV_ASSERT(Image::Type::RGB_F32 == c.getType());
                DJV_ASSERT(1.F == c.getF32(0));
                DJV_ASSERT(2.F == c.getF32(1));
                DJV_ASSERT(3.F == c.getF32(2));
            }
            
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
                Image::Color::hsvToRGB(hsv, rgb);
                std::stringstream ss;
                ss << "hsvToRGB: " << hsv[0] << "," << hsv[1] << "," << hsv[2] << " = " <<
                    rgb[0] << "," << rgb[1] << "," << rgb[2];
                _print(ss.str());
            }
            
            for (const auto& i : Image::getTypeEnums())
            {
                Image::Color c(i);
                _print("Color: " + Image::Color::getLabel(c));
            }
        }
        
        void ColorTest::_operators()
        {
            const Image::Color c(0, 1, 2, 3);
            DJV_ASSERT(c == c);
            DJV_ASSERT(c != Image::Color());
        }
                
    } // namespace ImageTest
} // namespace djv

