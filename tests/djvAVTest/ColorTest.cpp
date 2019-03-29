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

#include <djvCore/Debug.h>

#include <QStringList>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        void ColorTest::run(int &, char **)
        {
            DJV_DEBUG("ColorTest::run");
            ctors();
            members();
            operators();
        }

        void ColorTest::ctors()
        {
            DJV_DEBUG("ColorTest::ctors");
            {
                const AV::Color color;
                DJV_ASSERT(static_cast<AV::Pixel::PIXEL>(0) == color.pixel());
                DJV_ASSERT(0 == color.u8(0));
            }
            {
                const AV::Color color(AV::Color(1.f));
                DJV_ASSERT(AV::Pixel::L_F32 == color.pixel());
                DJV_ASSERT(Math::fuzzyCompare(1.f, color.f32(0)));
            }
            {
                const AV::Color color(AV::Pixel::RGB_U10);
                DJV_ASSERT(AV::Pixel::RGB_U10 == color.pixel());
                DJV_ASSERT(0 == color.u10(0));
                DJV_ASSERT(0 == color.u10(1));
                DJV_ASSERT(0 == color.u10(2));
            }
            {
                const AV::Color color(1.f);
                DJV_ASSERT(AV::Pixel::L_F32 == color.pixel());
                DJV_ASSERT(Math::fuzzyCompare(1.f, color.f32(0)));
            }
            {
                const AV::Color color(1.f, .9f);
                DJV_ASSERT(AV::Pixel::LA_F32 == color.pixel());
                DJV_ASSERT(Math::fuzzyCompare(1.f, color.f32(0)));
                DJV_ASSERT(Math::fuzzyCompare(.9f, color.f32(1)));
            }
            {
                const AV::Color color(1.f, .9f, .8f);
                DJV_ASSERT(AV::Pixel::RGB_F32 == color.pixel());
                DJV_ASSERT(Math::fuzzyCompare(1.f, color.f32(0)));
                DJV_ASSERT(Math::fuzzyCompare(.9f, color.f32(1)));
                DJV_ASSERT(Math::fuzzyCompare(.8f, color.f32(2)));
            }
            {
                const AV::Color color(1.f, .9f, .8f, .7f);
                DJV_ASSERT(AV::Pixel::RGBA_F32 == color.pixel());
                DJV_ASSERT(Math::fuzzyCompare(1.f, color.f32(0)));
                DJV_ASSERT(Math::fuzzyCompare(.9f, color.f32(1)));
                DJV_ASSERT(Math::fuzzyCompare(.8f, color.f32(2)));
                DJV_ASSERT(Math::fuzzyCompare(.7f, color.f32(3)));
            }
            {
                const quint8 data[] = { 255, 127, 0 };
                const AV::Color color(data, AV::Pixel::RGB_U8);
                DJV_ASSERT(AV::Pixel::RGB_U8 == color.pixel());
                DJV_ASSERT(data[0] == color.u8(0));
                DJV_ASSERT(data[1] == color.u8(1));
                DJV_ASSERT(data[2] == color.u8(2));
            }
        }

        void ColorTest::members()
        {
            DJV_DEBUG("ColorTest::members");
            {
                AV::Color color(AV::Pixel::RGB_U10);
                DJV_ASSERT(AV::Pixel::RGB_U10 == color.pixel());
                DJV_ASSERT(3 == color.channels());
                color.setPixel(AV::Pixel::RGBA_U8);
                DJV_ASSERT(AV::Pixel::RGBA_U8 == color.pixel());
                DJV_ASSERT(4 == color.channels());
            }
            {
                const AV::Color color(1.f, 0.f, 1.f, 0.f);
                DJV_ASSERT(AV::Pixel::u8Max == color.u8(0));
                DJV_ASSERT(0 == color.u8(1));
                DJV_ASSERT(AV::Pixel::u8Max == color.u8(2));
                DJV_ASSERT(0 == color.u8(3));
            }
            {
                const AV::Color color(1.f, 0.f, 1.f);
                DJV_ASSERT(AV::Pixel::u10Max == color.u10(0));
                DJV_ASSERT(0 == color.u10(1));
                DJV_ASSERT(AV::Pixel::u10Max == color.u10(2));
            }
            {
                const AV::Color color(1.f, 0.f, 1.f, 0.f);
                DJV_ASSERT(AV::Pixel::u16Max == color.u16(0));
                DJV_ASSERT(0 == color.u16(1));
                DJV_ASSERT(AV::Pixel::u16Max == color.u16(2));
                DJV_ASSERT(0 == color.u16(3));
            }
            {
                const AV::Color color(1.f, 0.f, 1.f, 0.f);
                DJV_ASSERT(Math::fuzzyCompare(1.f, color.f32(0)));
                DJV_ASSERT(Math::fuzzyCompare(0.f, color.f32(1)));
                DJV_ASSERT(Math::fuzzyCompare(1.f, color.f32(2)));
                DJV_ASSERT(Math::fuzzyCompare(0.f, color.f32(3)));
            }
            {
                AV::Color color(AV::Pixel::RGBA_F32);
                color.setU8(AV::Pixel::u8Max, 0);
                color.setU8(0, 1);
                color.setU8(AV::Pixel::u8Max, 2);
                color.setU8(0, 3);
                DJV_ASSERT(Math::fuzzyCompare(1.f, color.f32(0)));
                DJV_ASSERT(Math::fuzzyCompare(0.f, color.f32(1)));
                DJV_ASSERT(Math::fuzzyCompare(1.f, color.f32(2)));
                DJV_ASSERT(Math::fuzzyCompare(0.f, color.f32(3)));
            }
            {
                AV::Color color(AV::Pixel::RGB_F32);
                color.setU10(AV::Pixel::u10Max, 0);
                color.setU10(0, 1);
                color.setU10(AV::Pixel::u10Max, 2);
                DJV_ASSERT(Math::fuzzyCompare(1.f, color.f32(0)));
                DJV_ASSERT(Math::fuzzyCompare(0.f, color.f32(1)));
                DJV_ASSERT(Math::fuzzyCompare(1.f, color.f32(2)));
            }
            {
                AV::Color color(AV::Pixel::RGBA_F32);
                color.setU16(AV::Pixel::u16Max, 0);
                color.setU16(0, 1);
                color.setU16(AV::Pixel::u16Max, 2);
                color.setU16(0, 3);
                DJV_ASSERT(Math::fuzzyCompare(1.f, color.f32(0)));
                DJV_ASSERT(Math::fuzzyCompare(0.f, color.f32(1)));
                DJV_ASSERT(Math::fuzzyCompare(1.f, color.f32(2)));
                DJV_ASSERT(Math::fuzzyCompare(0.f, color.f32(3)));
            }
            {
                AV::Color color(AV::Pixel::RGBA_F32);
                color.setF32(1.f, 0);
                color.setF32(0.f, 1);
                color.setF32(1.f, 2);
                color.setF32(0.f, 3);
                DJV_ASSERT(Math::fuzzyCompare(1.f, color.f32(0)));
                DJV_ASSERT(Math::fuzzyCompare(0.f, color.f32(1)));
                DJV_ASSERT(Math::fuzzyCompare(1.f, color.f32(2)));
                DJV_ASSERT(Math::fuzzyCompare(0.f, color.f32(3)));
            }
            {
                AV::Color color(1.f, 1.f, 1.f, 1.f);
                color.zero();
                DJV_ASSERT(Math::fuzzyCompare(0.f, color.f32(0)));
                DJV_ASSERT(Math::fuzzyCompare(0.f, color.f32(1)));
                DJV_ASSERT(Math::fuzzyCompare(0.f, color.f32(2)));
                DJV_ASSERT(Math::fuzzyCompare(0.f, color.f32(3)));
            }
            {
                AV::Color color(AV::Pixel::L_U8);
                color.setU8(AV::Pixel::u8Max, 0);
                DJV_ASSERT(color.data());
                DJV_ASSERT(AV::Pixel::u8Max == *color.data());
                *color.data() = 0;
                DJV_ASSERT(0 == *color.data());
            }
        }

        void ColorTest::operators()
        {
            DJV_DEBUG("ColorTest::operators");
            {
                const AV::Color color = AV::Color(1.f);
                DJV_ASSERT(Math::fuzzyCompare(1.f, color.f32(0)));
            }
            {
                AV::Color a, b;
                DJV_ASSERT(a == b);
                a.setPixel(AV::Pixel::L_U8);
                b.setPixel(AV::Pixel::RGB_U8);
                DJV_ASSERT(a != b);
            }
            {
                AV::Color a(AV::Pixel::RGB_U8), b(AV::Pixel::RGB_U8);
                DJV_ASSERT(a == b);
                a.setU8(AV::Pixel::u8Max, 0);
                DJV_ASSERT(a != b);
            }
            {
                AV::Color a(AV::Pixel::RGB_U10), b(AV::Pixel::RGB_U10);
                DJV_ASSERT(a == b);
                a.setU10(AV::Pixel::u10Max, 0);
                DJV_ASSERT(a != b);
            }
            {
                AV::Color a(AV::Pixel::RGB_U16), b(AV::Pixel::RGB_U16);
                DJV_ASSERT(a == b);
                a.setU16(AV::Pixel::u16Max, 0);
                DJV_ASSERT(a != b);
            }
            {
                AV::Color a(AV::Pixel::RGB_F16), b(AV::Pixel::RGB_F16);
                DJV_ASSERT(a == b);
                a.setF16(1.f, 0);
                DJV_ASSERT(a != b);
            }
            {
                DJV_ASSERT(AV::Color(1.f) == AV::Color(1.f));
                DJV_ASSERT(AV::Color(1.f) != AV::Color(0.f));
            }
            {
                AV::Color color;
                QStringList s = QStringList() << "RGB U8" << "255" << "127" << "0";
                s >> color;
                DJV_ASSERT(255 == color.u8(0));
                DJV_ASSERT(127 == color.u8(1));
                DJV_ASSERT(0 == color.u8(2));
            }
            {
                AV::Color color;
                QStringList s = QStringList() << "RGB U10" << "1023" << "511" << "0";
                s >> color;
                DJV_ASSERT(1023 == color.u10(0));
                DJV_ASSERT(511 == color.u10(1));
                DJV_ASSERT(0 == color.u10(2));
            }
            {
                AV::Color color;
                QStringList s = QStringList() << "RGB F16" << "1.0" << "0.5" << "0.0";
                s >> color;
                DJV_ASSERT(Math::fuzzyCompare(1.f, color.f16(0)));
                DJV_ASSERT(Math::fuzzyCompare(.5f, color.f16(1)));
                DJV_ASSERT(Math::fuzzyCompare(0.f, color.f16(2)));
            }
            {
                AV::Color color;
                QStringList s = QStringList() << "RGB F32" << "1.0" << "0.5" << "0.0";
                s >> color;
                DJV_ASSERT(Math::fuzzyCompare(1.f, color.f32(0)));
                DJV_ASSERT(Math::fuzzyCompare(.5f, color.f32(1)));
                DJV_ASSERT(Math::fuzzyCompare(0.f, color.f32(2)));
            }
            {
                AV::Color color(1.f, 0.5f, 0.f);
                QStringList s;
                s << color;
                DJV_ASSERT((QStringList() << "RGB F32" << "1" << "0.5" << "0") == s);
            }
            {
                DJV_DEBUG_PRINT(AV::Color());
            }
        }

    } // namespace AVTest
} // namespace djv
