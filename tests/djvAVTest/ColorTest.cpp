//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvAV/Context.h>
#include <djvAV/Color.h>

#include <sstream>

namespace djv
{
    namespace AVTest
    {
        ColorTest::ColorTest(const std::shared_ptr<Core::Context> & context) :
            ITest("djv::AVTest::ColorTest", context)
        {}
        
        void ColorTest::run(int & argc, char ** argv)
        {
            if (auto context = std::dynamic_pointer_cast<AV::Context>(getContext().lock()))
            {
                {
                    auto u8 = AV::Image::Color(AV::Image::Pixel::L_U8);
                    auto u8p = reinterpret_cast<AV::Image::U8_T *>(u8.getData());
                    u8p[0] = 255;
                    const auto u16 = u8.convert(AV::Image::Pixel::L_U16, context);
                    const auto u16p = reinterpret_cast<const AV::Image::U16_T *>(u16.getData());
                    std::stringstream ss;
                    ss << "U8 = " << static_cast<int>(u8p[0]) << ", " << "U16 = " << u16p[0];
                    _print(ss.str());
                }
                {
                    auto u8 = AV::Image::Color(AV::Image::Pixel::L_U8);
                    auto u8p = reinterpret_cast<AV::Image::U8_T *>(u8.getData());
                    u8p[0] = 255;
                    const auto u10 = u8.convert(AV::Image::Pixel::RGB_U10, context);
                    const auto u10p = reinterpret_cast<const AV::Image::U10_S *>(u10.getData());
                    std::stringstream ss;
                    ss << "U8 = " << static_cast<int>(u8p[0]) << ", " << "U10 = " << u10p->r << ", " << u10p->g << ", " << u10p->b;
                    _print(ss.str());
                }
                {
                    auto u8 = AV::Image::Color(AV::Image::Pixel::L_U8);
                    auto u8p = reinterpret_cast<AV::Image::U8_T *>(u8.getData());
                    u8p[0] = 255;
                    const auto f32 = u8.convert(AV::Image::Pixel::L_F32, context);
                    const auto f32p = reinterpret_cast<const AV::Image::F32_T *>(f32.getData());
                    std::stringstream ss;
                    ss << "U8 = " << static_cast<int>(u8p[0]) << ", " << "F32 = " << f32p[0];
                    _print(ss.str());
                }
            }
        }
        
    } // namespace AVTest
} // namespace djv

