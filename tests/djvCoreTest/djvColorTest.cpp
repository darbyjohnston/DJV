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

#include <djvColor.h>
#include <djvDebug.h>

#include <QStringList>

void djvColorTest::run(int &, char **)
{
    DJV_DEBUG("djvColorTest::run");
    
    ctors();
    members();
    operators();
}

void djvColorTest::ctors()
{
    DJV_DEBUG("djvColorTest::ctors");
    
    {
        const djvColor color;
        
        DJV_ASSERT(static_cast<djvPixel::PIXEL>(0) == color.pixel());
        DJV_ASSERT(0 == color.u8(0));
    }
    
    {
        const djvColor color(djvColor(1.0f));

        DJV_ASSERT(djvPixel::L_F32 == color.pixel());
        DJV_ASSERT(djvMath::fuzzyCompare(1.0f, color.f32(0)));
    }
    
    {
        const djvColor color(djvPixel::RGB_U10);

        DJV_ASSERT(djvPixel::RGB_U10 == color.pixel());
        DJV_ASSERT(0 == color.u10(0));
        DJV_ASSERT(0 == color.u10(1));
        DJV_ASSERT(0 == color.u10(2));
    }
    
    {
        const djvColor color(1.0f);

        DJV_ASSERT(djvPixel::L_F32 == color.pixel());
        DJV_ASSERT(djvMath::fuzzyCompare(1.0f, color.f32(0)));
    }
    
    {
        const djvColor color(1.0f, 0.9f);

        DJV_ASSERT(djvPixel::LA_F32 == color.pixel());
        DJV_ASSERT(djvMath::fuzzyCompare(1.0f, color.f32(0)));
        DJV_ASSERT(djvMath::fuzzyCompare(0.9f, color.f32(1)));
    }
    
    {
        const djvColor color(1.0f, 0.9f, 0.8f);

        DJV_ASSERT(djvPixel::RGB_F32 == color.pixel());
        DJV_ASSERT(djvMath::fuzzyCompare(1.0f, color.f32(0)));
        DJV_ASSERT(djvMath::fuzzyCompare(0.9f, color.f32(1)));
        DJV_ASSERT(djvMath::fuzzyCompare(0.8f, color.f32(2)));
    }
    
    {
        const djvColor color(1.0f, 0.9f, 0.8f, 0.7f);

        DJV_ASSERT(djvPixel::RGBA_F32 == color.pixel());
        DJV_ASSERT(djvMath::fuzzyCompare(1.0f, color.f32(0)));
        DJV_ASSERT(djvMath::fuzzyCompare(0.9f, color.f32(1)));
        DJV_ASSERT(djvMath::fuzzyCompare(0.8f, color.f32(2)));
        DJV_ASSERT(djvMath::fuzzyCompare(0.7f, color.f32(3)));
    }
    
    {
        const quint8 data [] = { 255, 127, 0 };
        
        const djvColor color(data, djvPixel::RGB_U8);

        DJV_ASSERT(djvPixel::RGB_U8 == color.pixel());
        DJV_ASSERT(data[0] == color.u8(0));
        DJV_ASSERT(data[1] == color.u8(1));
        DJV_ASSERT(data[2] == color.u8(2));
    }
}

void djvColorTest::members()
{
    DJV_DEBUG("djvColorTest::members");
    
    {
        djvColor color(djvPixel::RGB_U10);
        
        DJV_ASSERT(djvPixel::RGB_U10 == color.pixel());
        DJV_ASSERT(3 == color.channels());
        
        color.setPixel(djvPixel::RGBA_U8);

        DJV_ASSERT(djvPixel::RGBA_U8 == color.pixel());
        DJV_ASSERT(4 == color.channels());
    }
    
    {
        const djvColor color(1.0f, 0.0f, 1.0f, 0.0f);
        
        DJV_ASSERT(djvPixel::u8Max == color.u8(0));
        DJV_ASSERT(0 == color.u8(1));
        DJV_ASSERT(djvPixel::u8Max == color.u8(2));
        DJV_ASSERT(0 == color.u8(3));
    }
    
    {
        const djvColor color(1.0f, 0.0f, 1.0f);
        
        DJV_ASSERT(djvPixel::u10Max == color.u10(0));
        DJV_ASSERT(0 == color.u10(1));
        DJV_ASSERT(djvPixel::u10Max == color.u10(2));
    }
    
    {
        const djvColor color(1.0f, 0.0f, 1.0f, 0.0f);
        
        DJV_ASSERT(djvPixel::u16Max == color.u16(0));
        DJV_ASSERT(0 == color.u16(1));
        DJV_ASSERT(djvPixel::u16Max == color.u16(2));
        DJV_ASSERT(0 == color.u16(3));
    }
    
    {
        const djvColor color(1.0f, 0.0f, 1.0f, 0.0f);
        
        DJV_ASSERT(djvMath::fuzzyCompare(1.0f, color.f32(0)));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0f, color.f32(1)));
        DJV_ASSERT(djvMath::fuzzyCompare(1.0f, color.f32(2)));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0f, color.f32(3)));
    }
    
    {
        djvColor color(djvPixel::RGBA_F32);
        
        color.setU8(djvPixel::u8Max, 0);
        color.setU8(0, 1);
        color.setU8(djvPixel::u8Max, 2);
        color.setU8(0, 3);
        
        DJV_ASSERT(djvMath::fuzzyCompare(1.0f, color.f32(0)));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0f, color.f32(1)));
        DJV_ASSERT(djvMath::fuzzyCompare(1.0f, color.f32(2)));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0f, color.f32(3)));
    }
    
    {
        djvColor color(djvPixel::RGB_F32);
        
        color.setU10(djvPixel::u10Max, 0);
        color.setU10(0, 1);
        color.setU10(djvPixel::u10Max, 2);
        
        DJV_ASSERT(djvMath::fuzzyCompare(1.0f, color.f32(0)));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0f, color.f32(1)));
        DJV_ASSERT(djvMath::fuzzyCompare(1.0f, color.f32(2)));
    }
    
    {
        djvColor color(djvPixel::RGBA_F32);
        
        color.setU16(djvPixel::u16Max, 0);
        color.setU16(0, 1);
        color.setU16(djvPixel::u16Max, 2);
        color.setU16(0, 3);
        
        DJV_ASSERT(djvMath::fuzzyCompare(1.0f, color.f32(0)));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0f, color.f32(1)));
        DJV_ASSERT(djvMath::fuzzyCompare(1.0f, color.f32(2)));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0f, color.f32(3)));
    }
    
    {
        djvColor color(djvPixel::RGBA_F32);
        
        color.setF32(1.0f, 0);
        color.setF32(0.0f, 1);
        color.setF32(1.0f, 2);
        color.setF32(0.0f, 3);
        
        DJV_ASSERT(djvMath::fuzzyCompare(1.0f, color.f32(0)));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0f, color.f32(1)));
        DJV_ASSERT(djvMath::fuzzyCompare(1.0f, color.f32(2)));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0f, color.f32(3)));
    }
    
    {
        djvColor color(1.0f, 1.0f, 1.0f, 1.0f);
        color.zero();
        
        DJV_ASSERT(djvMath::fuzzyCompare(0.0f, color.f32(0)));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0f, color.f32(1)));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0f, color.f32(2)));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0f, color.f32(3)));
    }
    
    {
        djvColor color(djvPixel::L_U8);
        color.setU8(djvPixel::u8Max, 0);
        
        DJV_ASSERT(color.data());
        DJV_ASSERT(djvPixel::u8Max == *color.data());
        
        *color.data() = 0;

        DJV_ASSERT(0 == *color.data());
    }
}

void djvColorTest::operators()
{
    DJV_DEBUG("djvColorTest::operators");
    
    {
        const djvColor color = djvColor(1.0f);
        
        DJV_ASSERT(djvMath::fuzzyCompare(1.0f, color.f32(0)));
    }
    
    {
        djvColor a, b;
        
        DJV_ASSERT(a == b);

        a.setPixel(djvPixel::L_U8);
        b.setPixel(djvPixel::RGB_U8);
        
        DJV_ASSERT(a != b);
    }
    
    {
        djvColor a(djvPixel::RGB_U8), b(djvPixel::RGB_U8);
        
        DJV_ASSERT(a == b);
        
        a.setU8(djvPixel::u8Max, 0);
        
        DJV_ASSERT(a != b);
    }
    
    {
        djvColor a(djvPixel::RGB_U10), b(djvPixel::RGB_U10);
        
        DJV_ASSERT(a == b);
        
        a.setU10(djvPixel::u10Max, 0);
        
        DJV_ASSERT(a != b);
    }
    
    {
        djvColor a(djvPixel::RGB_U16), b(djvPixel::RGB_U16);
        
        DJV_ASSERT(a == b);
        
        a.setU16(djvPixel::u16Max, 0);
        
        DJV_ASSERT(a != b);
    }
    
    {
        djvColor a(djvPixel::RGB_F16), b(djvPixel::RGB_F16);
        
        DJV_ASSERT(a == b);
        
        a.setF16(1.0, 0);
        
        DJV_ASSERT(a != b);
    }
    
    {
        DJV_ASSERT(djvColor(1.0f) == djvColor(1.0f));
        DJV_ASSERT(djvColor(1.0f) != djvColor(0.0f));
    }
    
    {
        djvColor color;
        
        QStringList s = QStringList() << "RGB U8" << "255" << "127" << "0";

        s >> color;
        
        DJV_ASSERT(255 == color.u8(0));
        DJV_ASSERT(127 == color.u8(1));
        DJV_ASSERT(  0 == color.u8(2));
    }
    
    {
        djvColor color;
        
        QStringList s = QStringList() << "RGB U10" << "1023" << "511" << "0";

        s >> color;
        
        DJV_ASSERT(1023 == color.u10(0));
        DJV_ASSERT( 511 == color.u10(1));
        DJV_ASSERT(   0 == color.u10(2));
    }
        
    {
        djvColor color;
        
        QStringList s = QStringList() << "RGB F16" << "1.0" << "0.5" << "0.0";

        s >> color;
        
        DJV_ASSERT(djvMath::fuzzyCompare(1.0f, color.f16(0)));
        DJV_ASSERT(djvMath::fuzzyCompare(0.5f, color.f16(1)));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0f, color.f16(2)));
    }
        
    {
        djvColor color;
        
        QStringList s = QStringList() << "RGB F32" << "1.0" << "0.5" << "0.0";

        s >> color;
        
        DJV_ASSERT(djvMath::fuzzyCompare(1.0f, color.f32(0)));
        DJV_ASSERT(djvMath::fuzzyCompare(0.5f, color.f32(1)));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0f, color.f32(2)));
    }
        
    {
        djvColor color(1.0f, 0.5f, 0.0f);
        
        QStringList s;
        s << color;
        
        DJV_ASSERT((QStringList() << "RGB F32" << "1" << "0.5" << "0") == s);
    }
    
    {
        DJV_DEBUG_PRINT(djvColor());
    }
}

