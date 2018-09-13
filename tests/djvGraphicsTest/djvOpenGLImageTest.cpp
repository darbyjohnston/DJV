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

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>
#include <djvGraphics/GraphicsContext.h>
#include <djvGraphics/OpenGL.h>
#include <djvGraphics/OpenGLImage.h>
#include <djvGraphics/OpenGLOffscreenBuffer.h>

#include <QPixmap>
#include <QScopedPointer>
#include <QStringList>

using namespace djv;

void djvOpenGLImageTest::run(int &, char **)
{
    DJV_DEBUG("djvOpenGLImageTest::run");
    ctors();
    members();
    convert();
    operators();
}

void djvOpenGLImageTest::ctors()
{
    DJV_DEBUG("djvOpenGLImageTest::ctors");
    {
        DJV_DEBUG_PRINT(Graphics::OpenGLImageXform());
    }
    {
        DJV_DEBUG_PRINT(Graphics::OpenGLImageColor());
    }
    {
        DJV_DEBUG_PRINT(Graphics::OpenGLImageLevels());
    }
    {
        DJV_DEBUG_PRINT(Graphics::OpenGLImageDisplayProfile());
    }
    {
        DJV_DEBUG_PRINT(Graphics::OpenGLImageFilter());
        DJV_DEBUG_PRINT(Graphics::OpenGLImageFilter(
            Graphics::OpenGLImageFilter::BOX, Graphics::OpenGLImageFilter::TRIANGLE));
    }
    {
        DJV_DEBUG_PRINT(Graphics::OpenGLImageOptions());
    }
}

void djvOpenGLImageTest::members()
{
    DJV_DEBUG("djvOpenGLImageTest::members");
    {
        DJV_DEBUG_PRINT(Graphics::OpenGLImageXform::xformMatrix(Graphics::OpenGLImageXform()));
    }
    {
        DJV_DEBUG_PRINT(Graphics::OpenGLImageColor::brightnessMatrix(1.f, 0.f, 0.f));
        DJV_DEBUG_PRINT(Graphics::OpenGLImageColor::contrastMatrix(1.f, 0.f, 0.f));
        DJV_DEBUG_PRINT(Graphics::OpenGLImageColor::saturationMatrix(1.f, 0.f, 0.f));
        DJV_DEBUG_PRINT(Graphics::OpenGLImageColor::colorMatrix(Graphics::OpenGLImageColor()));
    }
    {
        DJV_DEBUG_PRINT(Graphics::OpenGLImageLevels::colorLut(Graphics::OpenGLImageLevels(), .5f));
    }
    {
        DJV_ASSERT(Graphics::OpenGLImageFilter::toGl(Graphics::OpenGLImageFilter::NEAREST) != GL_NONE);
        DJV_ASSERT(Graphics::OpenGLImageFilter::toGl(Graphics::OpenGLImageFilter::LINEAR) != GL_NONE);
        DJV_ASSERT(Graphics::OpenGLImageFilter::toGl(Graphics::OpenGLImageFilter::BOX) == GL_NONE);
    }
    {
        Graphics::GraphicsContext context;
        for (int i = 0; i < Graphics::Pixel::PIXEL_COUNT; ++i)
        {
            const Graphics::Pixel::PIXEL pixel = static_cast<Graphics::Pixel::PIXEL>(i);
            DJV_DEBUG_PRINT("pixel = " << pixel);
            Graphics::PixelData
                a(Graphics::PixelDataInfo(1, 1, pixel)),
                b(Graphics::PixelDataInfo(1, 1, pixel));
            Graphics::Color color(pixel);
            Graphics::ColorUtil::convert(Graphics::Color(0.5), color);
            DJV_DEBUG_PRINT("color = " << color);
            memcpy(a.data(), color.data(), a.dataByteCount());
            DJV_DEBUG_PRINT("pixel = " << Graphics::OpenGLImage().read(a, 0, 0));

            Graphics::OpenGLImage().copy(a, b);
            DJV_DEBUG_PRINT("compare = " << (a == b));
            Graphics::OpenGLImage().average(a, color);

            DJV_DEBUG_PRINT("average = " << color);
            Graphics::Color min, max;
            Graphics::OpenGLImage().histogram(a, b, 1, min, max);

            DJV_DEBUG_PRINT("histogram = " << b);
            DJV_DEBUG_PRINT("min = " << min);
            DJV_DEBUG_PRINT("max = " << max);
            DJV_DEBUG_PRINT("");
        }
    }
}

void djvOpenGLImageTest::convert()
{
    DJV_DEBUG("djvOpenGLImageTest::convert");
    Graphics::GraphicsContext context;
    Graphics::PixelData data(Graphics::PixelDataInfo(32, 32, Graphics::Pixel::L_F32));
    Graphics::OpenGLImage().toQt(data);
}

void djvOpenGLImageTest::operators()
{
    DJV_DEBUG("djvOpenGLImageTest::operators");
    {
        Graphics::OpenGLImageXform a, b;
        a.mirror = b.mirror = Graphics::PixelDataInfo::Mirror(true, true);
        DJV_ASSERT(a == b);
        DJV_ASSERT(a != Graphics::OpenGLImageXform());
    }
    {
        Graphics::OpenGLImageColor a, b;
        a.brightness = b.brightness = 2.f;
        DJV_ASSERT(a == b);
        DJV_ASSERT(a != Graphics::OpenGLImageColor());
    }
    {
        Graphics::OpenGLImageLevels a, b;
        a.gamma = b.gamma = 2.2f;
        DJV_ASSERT(a == b);
        DJV_ASSERT(a != Graphics::OpenGLImageLevels());
    }
    {
        Graphics::OpenGLImageDisplayProfile a, b;
        a.softClip = b.softClip = .5f;
        DJV_ASSERT(a == b);
        DJV_ASSERT(a != Graphics::OpenGLImageDisplayProfile());
    }
    {
        Graphics::OpenGLImageFilter a, b;
        a.min = b.min = Graphics::OpenGLImageFilter::BOX;
        DJV_ASSERT(a == b);
        DJV_ASSERT(a != Graphics::OpenGLImageFilter());
    }
    {
        Graphics::OpenGLImageOptions a, b;
        a.channel = b.channel = Graphics::OpenGLImageOptions::CHANNEL_RED;
        DJV_ASSERT(a == b);
        DJV_ASSERT(a != Graphics::OpenGLImageOptions());
    }
    {
        Graphics::OpenGLImageXform a;
        a.mirror   = Graphics::PixelDataInfo::Mirror(true, true);
        a.position = glm::vec2(1.f, 2.f);
        a.scale    = glm::vec2(3.f, 4.f);
        a.rotate   = 5.f;
        QStringList tmp;
        tmp << a;
        Graphics::OpenGLImageXform b;
        tmp >> b;
        DJV_ASSERT(a == b);
    }
    {
        Graphics::OpenGLImageColor a;
        a.brightness = 1.f;
        a.contrast   = 2.f;
        a.saturation = 3.f;
        QStringList tmp;
        tmp << a;
        Graphics::OpenGLImageColor b;
        tmp >> b;
        DJV_ASSERT(a == b);
    }
    {
        Graphics::OpenGLImageLevels a;
        a.inLow   = 1.f;
        a.inHigh  = 2.f;
        a.gamma   = 3.f;
        a.outLow  = 4.f;
        a.outHigh = 5.f;
        QStringList tmp;
        tmp << a;
        Graphics::OpenGLImageLevels b;
        tmp >> b;
        DJV_ASSERT(a == b);
    }
    {
        DJV_DEBUG_PRINT(Graphics::OpenGLImageFilter::BELL);
        DJV_DEBUG_PRINT(Graphics::OpenGLImageOptions::CHANNEL_RED);
    }
}

