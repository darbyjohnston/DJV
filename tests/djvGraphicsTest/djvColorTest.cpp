//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
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

//! \file djvColorTest.cpp

#include <djvColorTest.h>

#include <djvGraphics/Color.h>
#include <djvCore/Debug.h>

#include <QStringList>

using namespace djv::Core;
using namespace djv::Graphics;

namespace djv
{
    namespace GraphicsTest
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
                const Graphics::Color color;
                DJV_ASSERT(static_cast<Graphics::Pixel::PIXEL>(0) == color.pixel());
                DJV_ASSERT(0 == color.u8(0));
            }
            {
                const Graphics::Color color(Graphics::Color(1.f));
                DJV_ASSERT(Graphics::Pixel::L_F32 == color.pixel());
                DJV_ASSERT(Math::fuzzyCompare(1.f, color.f32(0)));
            }
            {
                const Graphics::Color color(Graphics::Pixel::RGB_U10);
                DJV_ASSERT(Graphics::Pixel::RGB_U10 == color.pixel());
                DJV_ASSERT(0 == color.u10(0));
                DJV_ASSERT(0 == color.u10(1));
                DJV_ASSERT(0 == color.u10(2));
            }
            {
                const Graphics::Color color(1.f);
                DJV_ASSERT(Graphics::Pixel::L_F32 == color.pixel());
                DJV_ASSERT(Math::fuzzyCompare(1.f, color.f32(0)));
            }
            {
                const Graphics::Color color(1.f, .9f);
                DJV_ASSERT(Graphics::Pixel::LA_F32 == color.pixel());
                DJV_ASSERT(Math::fuzzyCompare(1.f, color.f32(0)));
                DJV_ASSERT(Math::fuzzyCompare(.9f, color.f32(1)));
            }
            {
                const Graphics::Color color(1.f, .9f, .8f);
                DJV_ASSERT(Graphics::Pixel::RGB_F32 == color.pixel());
                DJV_ASSERT(Math::fuzzyCompare(1.f, color.f32(0)));
                DJV_ASSERT(Math::fuzzyCompare(.9f, color.f32(1)));
                DJV_ASSERT(Math::fuzzyCompare(.8f, color.f32(2)));
            }
            {
                const Graphics::Color color(1.f, .9f, .8f, .7f);
                DJV_ASSERT(Graphics::Pixel::RGBA_F32 == color.pixel());
                DJV_ASSERT(Math::fuzzyCompare(1.f, color.f32(0)));
                DJV_ASSERT(Math::fuzzyCompare(.9f, color.f32(1)));
                DJV_ASSERT(Math::fuzzyCompare(.8f, color.f32(2)));
                DJV_ASSERT(Math::fuzzyCompare(.7f, color.f32(3)));
            }
            {
                const quint8 data[] = { 255, 127, 0 };
                const Graphics::Color color(data, Graphics::Pixel::RGB_U8);
                DJV_ASSERT(Graphics::Pixel::RGB_U8 == color.pixel());
                DJV_ASSERT(data[0] == color.u8(0));
                DJV_ASSERT(data[1] == color.u8(1));
                DJV_ASSERT(data[2] == color.u8(2));
            }
        }

        void ColorTest::members()
        {
            DJV_DEBUG("ColorTest::members");
            {
                Graphics::Color color(Graphics::Pixel::RGB_U10);
                DJV_ASSERT(Graphics::Pixel::RGB_U10 == color.pixel());
                DJV_ASSERT(3 == color.channels());
                color.setPixel(Graphics::Pixel::RGBA_U8);
                DJV_ASSERT(Graphics::Pixel::RGBA_U8 == color.pixel());
                DJV_ASSERT(4 == color.channels());
            }
            {
                const Graphics::Color color(1.f, 0.f, 1.f, 0.f);
                DJV_ASSERT(Graphics::Pixel::u8Max == color.u8(0));
                DJV_ASSERT(0 == color.u8(1));
                DJV_ASSERT(Graphics::Pixel::u8Max == color.u8(2));
                DJV_ASSERT(0 == color.u8(3));
            }
            {
                const Graphics::Color color(1.f, 0.f, 1.f);
                DJV_ASSERT(Graphics::Pixel::u10Max == color.u10(0));
                DJV_ASSERT(0 == color.u10(1));
                DJV_ASSERT(Graphics::Pixel::u10Max == color.u10(2));
            }
            {
                const Graphics::Color color(1.f, 0.f, 1.f, 0.f);
                DJV_ASSERT(Graphics::Pixel::u16Max == color.u16(0));
                DJV_ASSERT(0 == color.u16(1));
                DJV_ASSERT(Graphics::Pixel::u16Max == color.u16(2));
                DJV_ASSERT(0 == color.u16(3));
            }
            {
                const Graphics::Color color(1.f, 0.f, 1.f, 0.f);
                DJV_ASSERT(Math::fuzzyCompare(1.f, color.f32(0)));
                DJV_ASSERT(Math::fuzzyCompare(0.f, color.f32(1)));
                DJV_ASSERT(Math::fuzzyCompare(1.f, color.f32(2)));
                DJV_ASSERT(Math::fuzzyCompare(0.f, color.f32(3)));
            }
            {
                Graphics::Color color(Graphics::Pixel::RGBA_F32);
                color.setU8(Graphics::Pixel::u8Max, 0);
                color.setU8(0, 1);
                color.setU8(Graphics::Pixel::u8Max, 2);
                color.setU8(0, 3);
                DJV_ASSERT(Math::fuzzyCompare(1.f, color.f32(0)));
                DJV_ASSERT(Math::fuzzyCompare(0.f, color.f32(1)));
                DJV_ASSERT(Math::fuzzyCompare(1.f, color.f32(2)));
                DJV_ASSERT(Math::fuzzyCompare(0.f, color.f32(3)));
            }
            {
                Graphics::Color color(Graphics::Pixel::RGB_F32);
                color.setU10(Graphics::Pixel::u10Max, 0);
                color.setU10(0, 1);
                color.setU10(Graphics::Pixel::u10Max, 2);
                DJV_ASSERT(Math::fuzzyCompare(1.f, color.f32(0)));
                DJV_ASSERT(Math::fuzzyCompare(0.f, color.f32(1)));
                DJV_ASSERT(Math::fuzzyCompare(1.f, color.f32(2)));
            }
            {
                Graphics::Color color(Graphics::Pixel::RGBA_F32);
                color.setU16(Graphics::Pixel::u16Max, 0);
                color.setU16(0, 1);
                color.setU16(Graphics::Pixel::u16Max, 2);
                color.setU16(0, 3);
                DJV_ASSERT(Math::fuzzyCompare(1.f, color.f32(0)));
                DJV_ASSERT(Math::fuzzyCompare(0.f, color.f32(1)));
                DJV_ASSERT(Math::fuzzyCompare(1.f, color.f32(2)));
                DJV_ASSERT(Math::fuzzyCompare(0.f, color.f32(3)));
            }
            {
                Graphics::Color color(Graphics::Pixel::RGBA_F32);
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
                Graphics::Color color(1.f, 1.f, 1.f, 1.f);
                color.zero();
                DJV_ASSERT(Math::fuzzyCompare(0.f, color.f32(0)));
                DJV_ASSERT(Math::fuzzyCompare(0.f, color.f32(1)));
                DJV_ASSERT(Math::fuzzyCompare(0.f, color.f32(2)));
                DJV_ASSERT(Math::fuzzyCompare(0.f, color.f32(3)));
            }
            {
                Graphics::Color color(Graphics::Pixel::L_U8);
                color.setU8(Graphics::Pixel::u8Max, 0);
                DJV_ASSERT(color.data());
                DJV_ASSERT(Graphics::Pixel::u8Max == *color.data());
                *color.data() = 0;
                DJV_ASSERT(0 == *color.data());
            }
        }

        void ColorTest::operators()
        {
            DJV_DEBUG("ColorTest::operators");
            {
                const Graphics::Color color = Graphics::Color(1.f);
                DJV_ASSERT(Math::fuzzyCompare(1.f, color.f32(0)));
            }
            {
                Graphics::Color a, b;
                DJV_ASSERT(a == b);
                a.setPixel(Graphics::Pixel::L_U8);
                b.setPixel(Graphics::Pixel::RGB_U8);
                DJV_ASSERT(a != b);
            }
            {
                Graphics::Color a(Graphics::Pixel::RGB_U8), b(Graphics::Pixel::RGB_U8);
                DJV_ASSERT(a == b);
                a.setU8(Graphics::Pixel::u8Max, 0);
                DJV_ASSERT(a != b);
            }
            {
                Graphics::Color a(Graphics::Pixel::RGB_U10), b(Graphics::Pixel::RGB_U10);
                DJV_ASSERT(a == b);
                a.setU10(Graphics::Pixel::u10Max, 0);
                DJV_ASSERT(a != b);
            }
            {
                Graphics::Color a(Graphics::Pixel::RGB_U16), b(Graphics::Pixel::RGB_U16);
                DJV_ASSERT(a == b);
                a.setU16(Graphics::Pixel::u16Max, 0);
                DJV_ASSERT(a != b);
            }
            {
                Graphics::Color a(Graphics::Pixel::RGB_F16), b(Graphics::Pixel::RGB_F16);
                DJV_ASSERT(a == b);
                a.setF16(1.f, 0);
                DJV_ASSERT(a != b);
            }
            {
                DJV_ASSERT(Graphics::Color(1.f) == Graphics::Color(1.f));
                DJV_ASSERT(Graphics::Color(1.f) != Graphics::Color(0.f));
            }
            {
                Graphics::Color color;
                QStringList s = QStringList() << "RGB U8" << "255" << "127" << "0";
                s >> color;
                DJV_ASSERT(255 == color.u8(0));
                DJV_ASSERT(127 == color.u8(1));
                DJV_ASSERT(0 == color.u8(2));
            }
            {
                Graphics::Color color;
                QStringList s = QStringList() << "RGB U10" << "1023" << "511" << "0";
                s >> color;
                DJV_ASSERT(1023 == color.u10(0));
                DJV_ASSERT(511 == color.u10(1));
                DJV_ASSERT(0 == color.u10(2));
            }
            {
                Graphics::Color color;
                QStringList s = QStringList() << "RGB F16" << "1.0" << "0.5" << "0.0";
                s >> color;
                DJV_ASSERT(Math::fuzzyCompare(1.f, color.f16(0)));
                DJV_ASSERT(Math::fuzzyCompare(.5f, color.f16(1)));
                DJV_ASSERT(Math::fuzzyCompare(0.f, color.f16(2)));
            }
            {
                Graphics::Color color;
                QStringList s = QStringList() << "RGB F32" << "1.0" << "0.5" << "0.0";
                s >> color;
                DJV_ASSERT(Math::fuzzyCompare(1.f, color.f32(0)));
                DJV_ASSERT(Math::fuzzyCompare(.5f, color.f32(1)));
                DJV_ASSERT(Math::fuzzyCompare(0.f, color.f32(2)));
            }
            {
                Graphics::Color color(1.f, 0.5f, 0.f);
                QStringList s;
                s << color;
                DJV_ASSERT((QStringList() << "RGB F32" << "1" << "0.5" << "0") == s);
            }
            {
                DJV_DEBUG_PRINT(Graphics::Color());
            }
        }

    } // namespace GraphicsTest
} // namespace djv
