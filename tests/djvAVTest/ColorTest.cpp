//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvAVTest/ColorTest.h>

#include <djvAV/Color.h>

namespace djv
{
    namespace AVTest
    {
        ColorTest::ColorTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::AVTest::ColorTest", context)
        {}
        
        void ColorTest::run(const std::vector<std::string>& args)
        {
            _ctor();
            _getSet();
            _util();
            _operators();
            _serialize();
        }
                
        void ColorTest::_ctor()
        {
            {
                AV::Image::Color c;
                DJV_ASSERT(!c.isValid());
            }
            
            {
                const AV::Image::Type type = AV::Image::Type::L_U8;
                AV::Image::Color c(type);
                DJV_ASSERT(type == c.getType());
                DJV_ASSERT(c.isValid());
            }
            
            {
                AV::Image::Color c(0, 1, 2, 3);
                DJV_ASSERT(AV::Image::Type::RGBA_U8 == c.getType());
                DJV_ASSERT(c.isValid());
            }
            
            {
                AV::Image::Color c(0.F, 1.F, 2.F, 3.F);
                DJV_ASSERT(AV::Image::Type::RGBA_F32 == c.getType());
                DJV_ASSERT(c.isValid());
            }
        }
        
        void ColorTest::_getSet()
        {
            {
                AV::Image::Color c(AV::Image::Type::L_U8);
                c.setU8(1, 0);
                DJV_ASSERT(1 == c.getU8(0));
            }
            {
                AV::Image::Color c(AV::Image::Type::L_U16);
                c.setU16(1, 0);
                DJV_ASSERT(1 == c.getU16(0));
            }
            {
                AV::Image::Color c(AV::Image::Type::L_U32);
                c.setU32(1, 0);
                DJV_ASSERT(1 == c.getU32(0));
            }
            {
                AV::Image::Color c(AV::Image::Type::L_F16);
                c.setF16(1.F, 0);
                DJV_ASSERT(1.F == c.getF16(0));
            }
            {
                AV::Image::Color c(AV::Image::Type::L_F32);
                c.setF32(1.F, 0);
                DJV_ASSERT(1.f == c.getF32(0));
            }
        }
        
        void ColorTest::_rgbToHSV(float r, float g, float b)
        {
            float rgb[3] = { r, g, b };
            float hsv[3] = { 0.F, 0.F, 0.F };
            AV::Image::Color::rgbToHSV(rgb, hsv);
            std::stringstream ss;
            ss << "rgbToHSV: " << rgb[0] << "," << rgb[1] << "," << rgb[2] << " = " <<
                hsv[0] << "," << hsv[1] << "," << hsv[2];
            _print(ss.str());
        }
        
        void ColorTest::_util()
        {
            {
                float r = 0.F;
                float g = 0.F;
                float b = 0.F;
                for (float r = 0.F; r <= 1.F; r += .1F)
                {
                    _rgbToHSV(r, g, b);
                }
                for (float g = 0.F; g <= 1.F; g += .1F)
                {
                    _rgbToHSV(r, g, b);
                }
                for (float b = 0.F; b <= 1.F; b += .1F)
                {
                    _rgbToHSV(r, g, b);
                }
            }

            for (float v = 0.F; v <= 1.F; v += .1F)
            {
                float hsv[3] = { v, v, v };
                float rgb[3] = { 0.F, 0.F, 0.F };
                AV::Image::Color::hsvToRGB(hsv, rgb);
                std::stringstream ss;
                ss << "hsvToRGB: " << hsv[0] << "," << hsv[1] << "," << hsv[2] << " = " <<
                    rgb[0] << "," << rgb[1] << "," << rgb[2];
                _print(ss.str());
            }
        }
        
        void ColorTest::_operators()
        {
            const AV::Image::Color c(0, 1, 2, 3);
            DJV_ASSERT(c == c);
            DJV_ASSERT(c != AV::Image::Color());
        }
        
        void ColorTest::_serialize()
        {
            {
                auto u8 = AV::Image::Color(AV::Image::Type::L_U8);
                u8.setU8(255, 0);
                const auto u16 = u8.convert(AV::Image::Type::L_U16);
                std::stringstream ss;
                ss << "U8 = " << static_cast<int>(u8.getU8(0)) << ", " << "U16 = " << u16.getU16(0);
                _print(ss.str());
            }
            
            {
                auto u8 = AV::Image::Color(AV::Image::Type::L_U8);
                u8.setU8(255, 0);
                const auto u10 = u8.convert(AV::Image::Type::RGB_U10);
                std::stringstream ss;
                ss << "U8 = " << static_cast<int>(u8.getU8(0)) << ", " << "U10 = " << u10.getU10(0) << ", " << u10.getU10(1) << ", " << u10.getU10(2);
                _print(ss.str());
            }
            
            {
                auto u8 = AV::Image::Color(AV::Image::Type::L_U8);
                u8.setU8(255, 0);
                const auto f32 = u8.convert(AV::Image::Type::L_F32);
                std::stringstream ss;
                ss << "U8 = " << static_cast<int>(u8.getU8(0)) << ", " << "F32 = " << f32.getF32(0);
                _print(ss.str());
            }
            
            {
                for (size_t i = 1; i < static_cast<size_t>(AV::Image::Type::Count); ++i)
                {
                    for (size_t j = 1; j < static_cast<size_t>(AV::Image::Type::Count); ++j)
                    {
                        AV::Image::Color(static_cast<AV::Image::Type>(i)).convert(static_cast<AV::Image::Type>(j));
                    }
                }
            }
            
            {
                const AV::Image::Color c(0, 1, 2, 3);
                auto json = toJSON(c);
                AV::Image::Color c2;
                fromJSON(json, c2);
                DJV_ASSERT(c == c2);
            }
            
            {
                AV::Image::Color c(AV::Image::Type::RGB_U10);
                c.setU10(0, 0);
                c.setU10(1, 1);
                c.setU10(2, 2);
                auto json = toJSON(c);
                AV::Image::Color c2;
                fromJSON(json, c2);
                DJV_ASSERT(c == c2);
            }
            
            {
                AV::Image::Color c(AV::Image::Type::RGBA_U16);
                c.setU16(0, 0);
                c.setU16(1, 1);
                c.setU16(3, 2);
                c.setU16(3, 3);
                auto json = toJSON(c);
                AV::Image::Color c2;
                fromJSON(json, c2);
                DJV_ASSERT(c == c2);
            }
            
            {
                AV::Image::Color c(AV::Image::Type::RGBA_U32);
                c.setU32(0, 0);
                c.setU32(1, 1);
                c.setU32(2, 2);
                c.setU32(3, 3);
                auto json = toJSON(c);
                AV::Image::Color c2;
                fromJSON(json, c2);
                DJV_ASSERT(c == c2);
            }
            
            {
                AV::Image::Color c(AV::Image::Type::RGBA_F16);
                c.setF16(0.F, 0);
                c.setF16(1.F, 1);
                c.setF16(2.F, 2);
                c.setF16(3.F, 3);
                auto json = toJSON(c);
                AV::Image::Color c2;
                fromJSON(json, c2);
                DJV_ASSERT(c == c2);
            }
            
            {
                AV::Image::Color c(AV::Image::Type::RGBA_F32);
                c.setF32(0.F, 0);
                c.setF32(1.F, 1);
                c.setF32(2.F, 2);
                c.setF32(3.F, 3);
                auto json = toJSON(c);
                AV::Image::Color c2;
                fromJSON(json, c2);
                DJV_ASSERT(c == c2);
            }

            try
            {
                auto json = picojson::value(picojson::object_type, true);
                AV::Image::Color c;
                fromJSON(json, c);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
        }
        
    } // namespace AVTest
} // namespace djv

