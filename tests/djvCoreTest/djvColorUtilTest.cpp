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

//! \file djvColorUtilTest.cpp

#include <djvColorUtilTest.h>

#include <djvColor.h>
#include <djvColorUtil.h>
#include <djvDebug.h>

#include <QColor>

void djvColorUtilTest::run(int &, char **)
{
    DJV_DEBUG("djvColorUtilTest::run");
    
    scale();
    lerp();
    convert();
    qt();
}

void djvColorUtilTest::scale()
{
    DJV_DEBUG("djvColorUtilTest::scale");
        
    {
        djvColor color(djvPixel::L_F32);
        djvColorUtil::scale(0.5, djvColor(1.0f), color);
        
        DJV_ASSERT(djvMath::fuzzyCompare(0.5f, color.f32(0)));
    }
    
    {
        const djvColor color = djvColorUtil::scale(0.5, djvColor(1.0f));

        DJV_ASSERT(djvMath::fuzzyCompare(0.5f, color.f32(0)));
    }
}
    
void djvColorUtilTest::lerp()
{
    DJV_DEBUG("djvColorUtilTest::lerp");
        
    {
        djvColor color(djvPixel::L_F32);
        djvColorUtil::lerp(0.5, djvColor(0.0f), djvColor(1.0f), color);
        
        DJV_ASSERT(djvMath::fuzzyCompare(0.5f, color.f32(0)));
    }
    
    {
        const djvColor color = djvColorUtil::lerp(0.5, djvColor(0.0f), djvColor(1.0f));

        DJV_ASSERT(djvMath::fuzzyCompare(0.5f, color.f32(0)));
    }
}

void djvColorUtilTest::convert()
{
    DJV_DEBUG("djvColorUtilTest::convert");
    
    const struct Data
    {
        QStringList  in;
        djvPixel::PIXEL pixel;
        QStringList  out;
    }
        data [] =
    {
        { QStringList() << "L U8" << "0",      djvPixel::L_U8,    QStringList() << "L U8" << "0" },
        { QStringList() << "L U8" << "255",    djvPixel::L_U8,    QStringList() << "L U8" << "255" },
        { QStringList() << "L U8" << "0",      djvPixel::RGB_U10, QStringList() << "RGB U10" << "0" << "0" << "0" },
        { QStringList() << "L U8" << "255",    djvPixel::RGB_U10, QStringList() << "RGB U10" << "1023" << "1023" << "1023" },
        { QStringList() << "L U8" << "0",      djvPixel::L_U16,   QStringList() << "L U16" << "0" },
        { QStringList() << "L U8" << "255",    djvPixel::L_U16,   QStringList() << "L U16" << "65535" },
        { QStringList() << "L U16" << "0",     djvPixel::L_U8,    QStringList() << "L U8" << "0" },
        { QStringList() << "L U16" << "65535", djvPixel::L_U8,    QStringList() << "L U8" << "255" },
        { QStringList() << "L U16" << "0",     djvPixel::RGB_U10, QStringList() << "RGB U10" << "0" << "0" << "0" },
        { QStringList() << "L U16" << "65535", djvPixel::RGB_U10, QStringList() << "RGB U10" << "1023" << "1023" << "1023" },
        { QStringList() << "L U16" << "0",     djvPixel::L_U16,   QStringList() << "L U16" << "0" },
        { QStringList() << "L U16" << "65535", djvPixel::L_U16,   QStringList() << "L U16" << "65535" },
        { QStringList() << "L F32" << "-1.0",  djvPixel::L_U8,    QStringList() << "L U8" << "0" },
        { QStringList() << "L F32" << "0.0",   djvPixel::L_U8,    QStringList() << "L U8" << "0" },
        { QStringList() << "L F32" << "0.5",   djvPixel::L_U8,    QStringList() << "L U8" << "128" },
        { QStringList() << "L F32" << "1.0",   djvPixel::L_U8,    QStringList() << "L U8" << "255" },
        { QStringList() << "L F32" << "2.0",   djvPixel::L_U8,    QStringList() << "L U8" << "255" },
        { QStringList() << "L F32" << "-1.0",  djvPixel::L_U16,   QStringList() << "L U16" << "0" },
        { QStringList() << "L F32" << "0.0",   djvPixel::L_U16,   QStringList() << "L U16" << "0" },
        { QStringList() << "L F32" << "0.5",   djvPixel::L_U16,   QStringList() << "L U16" << "32768" },
        { QStringList() << "L F32" << "1.0",   djvPixel::L_U16,   QStringList() << "L U16" << "65535" },
        { QStringList() << "L F32" << "2.0",   djvPixel::L_U16,   QStringList() << "L U16" << "65535" },
        { QStringList() << "L F32" << "1.0",   djvPixel::L_F16,   QStringList() << "L F16" << "1" }
    };
    
    const int dataCount = sizeof(data) / sizeof(Data);

    for (int i = 0; i < dataCount; ++i)
    {
        djvColor in;
        QStringList tmp = data[i].in;
        tmp >> in;

        djvColor out(data[i].pixel);
        djvColorUtil::convert(in, out);

        DJV_ASSERT(djvStringUtil::label(out) == data[i].out);
    }
}

void djvColorUtilTest::qt()
{
    DJV_DEBUG("djvColorUtilTest::qt");
    
    {
        djvColor color(djvPixel::RGBA_U8);
        color.setU8(255, 0);
        color.setU8(127, 1);
        color.setU8(  0, 2);
        color.setU8(255, 3);
        
        const QColor qColor = djvColorUtil::toQt(color);
        
        DJV_ASSERT(qColor.red()   == color.u8(0));
        DJV_ASSERT(qColor.green() == color.u8(1));
        DJV_ASSERT(qColor.blue()  == color.u8(2));
        DJV_ASSERT(qColor.alpha() == color.u8(3));
    }
    
    {
        QColor qColor(255, 127, 0, 255);
        
        const djvColor color = djvColorUtil::fromQt(qColor);

        DJV_ASSERT(color.u8(0) == qColor.red());
        DJV_ASSERT(color.u8(1) == qColor.green());
        DJV_ASSERT(color.u8(2) == qColor.blue());
        DJV_ASSERT(color.u8(3) == qColor.alpha());
    }
}

