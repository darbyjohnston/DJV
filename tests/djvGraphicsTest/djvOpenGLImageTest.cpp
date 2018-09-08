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

#include <djvOpenGLImageTest.h>

#include <djvAssert.h>
#include <djvDebug.h>
#include <djvGraphicsContext.h>
#include <djvOpenGL.h>
#include <djvOpenGLImage.h>
#include <djvOpenGLOffscreenBuffer.h>

#include <QScopedPointer>
#include <QStringList>

using namespace gl;

void djvOpenGLImageTest::run(int &, char **)
{
    DJV_DEBUG("djvOpenGLImageTest::run");
    ctors();
    members();
    operators();
}

void djvOpenGLImageTest::ctors()
{
    DJV_DEBUG("djvOpenGLImageTest::ctors");
    {
        DJV_DEBUG_PRINT(djvOpenGLImageXform());
    }
    {
        DJV_DEBUG_PRINT(djvOpenGLImageColor());
    }
    {
        DJV_DEBUG_PRINT(djvOpenGLImageLevels());
    }
    {
        DJV_DEBUG_PRINT(djvOpenGLImageDisplayProfile());
    }
    {
        DJV_DEBUG_PRINT(djvOpenGLImageFilter());
        DJV_DEBUG_PRINT(djvOpenGLImageFilter(
            djvOpenGLImageFilter::BOX, djvOpenGLImageFilter::TRIANGLE));
    }
    {
        DJV_DEBUG_PRINT(djvOpenGLImageOptions());
    }
}

void djvOpenGLImageTest::members()
{
    DJV_DEBUG("djvOpenGLImageTest::members");
    {
        DJV_DEBUG_PRINT(djvOpenGLImageXform::xformMatrix(djvOpenGLImageXform()));
    }
    {
        DJV_DEBUG_PRINT(djvOpenGLImageColor::brightnessMatrix(1.f, 0.f, 0.f));
        DJV_DEBUG_PRINT(djvOpenGLImageColor::contrastMatrix(1.f, 0.f, 0.f));
        DJV_DEBUG_PRINT(djvOpenGLImageColor::saturationMatrix(1.f, 0.f, 0.f));
        DJV_DEBUG_PRINT(djvOpenGLImageColor::colorMatrix(djvOpenGLImageColor()));
    }
    {
        DJV_DEBUG_PRINT(djvOpenGLImageLevels::colorLut(djvOpenGLImageLevels(), .5f));
    }
    {
        DJV_ASSERT(djvOpenGLImageFilter::toGl(djvOpenGLImageFilter::NEAREST) != GL_NONE);
        DJV_ASSERT(djvOpenGLImageFilter::toGl(djvOpenGLImageFilter::LINEAR) != GL_NONE);
        DJV_ASSERT(djvOpenGLImageFilter::toGl(djvOpenGLImageFilter::BOX) == GL_NONE);
    }
    {
        djvGraphicsContext context;
        for (int i = 0; i < djvPixel::PIXEL_COUNT; ++i)
        {
            const djvPixel::PIXEL pixel = static_cast<djvPixel::PIXEL>(i);
            DJV_DEBUG_PRINT("pixel = " << pixel);
            djvPixelData
                a(djvPixelDataInfo(1, 1, pixel)),
                b(djvPixelDataInfo(1, 1, pixel));
            djvColor color(pixel);
            djvColorUtil::convert(djvColor(0.5), color);
            DJV_DEBUG_PRINT("color = " << color);
            memcpy(a.data(), color.data(), a.dataByteCount());
            DJV_DEBUG_PRINT("pixel = " << djvOpenGLImage::pixel(a, 0, 0));

            djvOpenGLImage::copy(a, b);
            DJV_DEBUG_PRINT("compare = " << (a == b));
            djvOpenGLImage::average(a, color);

            DJV_DEBUG_PRINT("average = " << color);
            djvColor min, max;
            djvOpenGLImage::histogram(a, b, 1, min, max);

            DJV_DEBUG_PRINT("histogram = " << b);
            DJV_DEBUG_PRINT("min = " << min);
            DJV_DEBUG_PRINT("max = " << max);
            DJV_DEBUG_PRINT("");
        }
    }
}

void djvOpenGLImageTest::operators()
{
    DJV_DEBUG("djvOpenGLImageTest::operators");
    {
        djvOpenGLImageXform a, b;
        a.mirror = b.mirror = djvPixelDataInfo::Mirror(true, true);
        DJV_ASSERT(a == b);
        DJV_ASSERT(a != djvOpenGLImageXform());
    }
    {
        djvOpenGLImageColor a, b;
        a.brightness = b.brightness = 2.f;
        DJV_ASSERT(a == b);
        DJV_ASSERT(a != djvOpenGLImageColor());
    }
    {
        djvOpenGLImageLevels a, b;
        a.gamma = b.gamma = 2.2f;
        DJV_ASSERT(a == b);
        DJV_ASSERT(a != djvOpenGLImageLevels());
    }
    {
        djvOpenGLImageDisplayProfile a, b;
        a.softClip = b.softClip = .5f;
        DJV_ASSERT(a == b);
        DJV_ASSERT(a != djvOpenGLImageDisplayProfile());
    }
    {
        djvOpenGLImageFilter a, b;
        a.min = b.min = djvOpenGLImageFilter::BOX;
        DJV_ASSERT(a == b);
        DJV_ASSERT(a != djvOpenGLImageFilter());
    }
    {
        djvOpenGLImageOptions a, b;
        a.channel = b.channel = djvOpenGLImageOptions::CHANNEL_RED;
        DJV_ASSERT(a == b);
        DJV_ASSERT(a != djvOpenGLImageOptions());
    }
    {
        djvOpenGLImageXform a;
        a.mirror   = djvPixelDataInfo::Mirror(true, true);
        a.position = glm::vec2(1.f, 2.f);
        a.scale    = glm::vec2(3.f, 4.f);
        a.rotate   = 5.f;
        QStringList tmp;
        tmp << a;
        djvOpenGLImageXform b;
        tmp >> b;
        DJV_ASSERT(a == b);
    }
    {
        djvOpenGLImageColor a;
        a.brightness = 1.f;
        a.contrast   = 2.f;
        a.saturation = 3.f;
        QStringList tmp;
        tmp << a;
        djvOpenGLImageColor b;
        tmp >> b;
        DJV_ASSERT(a == b);
    }
    {
        djvOpenGLImageLevels a;
        a.inLow   = 1.f;
        a.inHigh  = 2.f;
        a.gamma   = 3.f;
        a.outLow  = 4.f;
        a.outHigh = 5.f;
        QStringList tmp;
        tmp << a;
        djvOpenGLImageLevels b;
        tmp >> b;
        DJV_ASSERT(a == b);
    }
    {
        DJV_DEBUG_PRINT(djvOpenGLImageFilter::BELL);
        DJV_DEBUG_PRINT(djvOpenGLImageOptions::CHANNEL_RED);
    }
}

