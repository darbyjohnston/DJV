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
        ColorTest::ColorTest(Core::Context * context) :
            ITest("djv::AVTest::ColorTest", context)
        {}
        
        void ColorTest::run(int & argc, char ** argv)
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
                AV::Image::Color in(0, 127, 255);
                std::stringstream ss;
                ss << in;
                AV::Image::Color out;
                ss >> out;
                DJV_ASSERT(in == out);
                _print(ss.str());
            }
            {
                AV::Image::Color in(AV::Image::Type::RGB_U10);
                in.setU10(0, 0);
                in.setU10(511, 1);
                in.setU10(1023, 2);
                std::stringstream ss;
                ss << in;
                AV::Image::Color out;
                ss >> out;
                DJV_ASSERT(in == out);
                _print(ss.str());
            }
        }
        
    } // namespace AVTest
} // namespace djv

