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

#include <djvAVTest/OpenGLImageTest.h>

#include <djvAV/AVContext.h>
#include <djvAV/OpenGL.h>
#include <djvAV/OpenGLImage.h>
#include <djvAV/OpenGLOffscreenBuffer.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>

#include <QPixmap>
#include <QScopedPointer>
#include <QStringList>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        void OpenGLImageTest::run(int & argc, char ** argv)
        {
            DJV_DEBUG("OpenGLImageTest::run");
            ctors();
            members(argc, argv);
            convert(argc, argv);
            operators();
        }

        void OpenGLImageTest::ctors()
        {
            DJV_DEBUG("OpenGLImageTest::ctors");
            {
                DJV_DEBUG_PRINT(AV::OpenGLImageXform());
            }
            {
                DJV_DEBUG_PRINT(AV::OpenGLImageColor());
            }
            {
                DJV_DEBUG_PRINT(AV::OpenGLImageLevels());
            }
            {
                DJV_DEBUG_PRINT(AV::OpenGLImageDisplayProfile());
            }
            {
                DJV_DEBUG_PRINT(AV::OpenGLImageFilter());
                DJV_DEBUG_PRINT(AV::OpenGLImageFilter(
                    AV::OpenGLImageFilter::BOX, AV::OpenGLImageFilter::TRIANGLE));
            }
            {
                DJV_DEBUG_PRINT(AV::OpenGLImageOptions());
            }
        }

        void OpenGLImageTest::members(int & argc, char ** argv)
        {
            DJV_DEBUG("OpenGLImageTest::members");
            {
                DJV_DEBUG_PRINT(AV::OpenGLImageXform::xformMatrix(AV::OpenGLImageXform()));
            }
            {
                DJV_DEBUG_PRINT(AV::OpenGLImageColor::brightnessMatrix(1.f, 0.f, 0.f));
                DJV_DEBUG_PRINT(AV::OpenGLImageColor::contrastMatrix(1.f, 0.f, 0.f));
                DJV_DEBUG_PRINT(AV::OpenGLImageColor::saturationMatrix(1.f, 0.f, 0.f));
                DJV_DEBUG_PRINT(AV::OpenGLImageColor::colorMatrix(AV::OpenGLImageColor()));
            }
            {
                DJV_DEBUG_PRINT(AV::OpenGLImageLevels::colorLut(AV::OpenGLImageLevels(), .5f));
            }
            {
                DJV_ASSERT(AV::OpenGLImageFilter::toGl(AV::OpenGLImageFilter::NEAREST) != GL_NONE);
                DJV_ASSERT(AV::OpenGLImageFilter::toGl(AV::OpenGLImageFilter::LINEAR) != GL_NONE);
                DJV_ASSERT(AV::OpenGLImageFilter::toGl(AV::OpenGLImageFilter::BOX) == GL_NONE);
            }
            {
                AV::AVContext context(argc, argv);
                for (int i = 0; i < AV::Pixel::PIXEL_COUNT; ++i)
                {
                    const AV::Pixel::PIXEL pixel = static_cast<AV::Pixel::PIXEL>(i);
                    DJV_DEBUG_PRINT("pixel = " << pixel);
                    AV::PixelData
                        a(AV::PixelDataInfo(1, 1, pixel)),
                        b(AV::PixelDataInfo(1, 1, pixel));
                    AV::Color color(pixel);

                    AV::OpenGLImage().copy(a, b);
                    DJV_DEBUG_PRINT("compare = " << (a == b));
                    AV::OpenGLImage().average(a, color);

                    DJV_DEBUG_PRINT("average = " << color);
                    AV::Color min, max;
                    AV::OpenGLImage().histogram(a, b, 1, min, max);

                    DJV_DEBUG_PRINT("histogram = " << b);
                    DJV_DEBUG_PRINT("min = " << min);
                    DJV_DEBUG_PRINT("max = " << max);
                    DJV_DEBUG_PRINT("");
                }
            }
        }

        void OpenGLImageTest::convert(int & argc, char ** argv)
        {
            DJV_DEBUG("OpenGLImageTest::convert");
            AV::AVContext context(argc, argv);
            AV::PixelData data(AV::PixelDataInfo(32, 32, AV::Pixel::L_F32));
            AV::OpenGLImage().toQt(data);
        }

        void OpenGLImageTest::operators()
        {
            DJV_DEBUG("OpenGLImageTest::operators");
            {
                AV::OpenGLImageXform a, b;
                a.mirror = b.mirror = AV::PixelDataInfo::Mirror(true, true);
                DJV_ASSERT(a == b);
                DJV_ASSERT(a != AV::OpenGLImageXform());
            }
            {
                AV::OpenGLImageColor a, b;
                a.brightness = b.brightness = 2.f;
                DJV_ASSERT(a == b);
                DJV_ASSERT(a != AV::OpenGLImageColor());
            }
            {
                AV::OpenGLImageLevels a, b;
                a.gamma = b.gamma = 2.2f;
                DJV_ASSERT(a == b);
                DJV_ASSERT(a != AV::OpenGLImageLevels());
            }
            {
                AV::OpenGLImageDisplayProfile a, b;
                a.softClip = b.softClip = .5f;
                DJV_ASSERT(a == b);
                DJV_ASSERT(a != AV::OpenGLImageDisplayProfile());
            }
            {
                AV::OpenGLImageFilter a, b;
                a.min = b.min = AV::OpenGLImageFilter::BOX;
                DJV_ASSERT(a == b);
                DJV_ASSERT(a != AV::OpenGLImageFilter());
            }
            {
                AV::OpenGLImageOptions a, b;
                a.channel = b.channel = AV::OpenGLImageOptions::CHANNEL_RED;
                DJV_ASSERT(a == b);
                DJV_ASSERT(a != AV::OpenGLImageOptions());
            }
            {
                AV::OpenGLImageXform a;
                a.mirror = AV::PixelDataInfo::Mirror(true, true);
                a.position = glm::vec2(1.f, 2.f);
                a.scale = glm::vec2(3.f, 4.f);
                a.rotate = 5.f;
                QStringList tmp;
                tmp << a;
                AV::OpenGLImageXform b;
                tmp >> b;
                DJV_ASSERT(a == b);
            }
            {
                AV::OpenGLImageColor a;
                a.brightness = 1.f;
                a.contrast = 2.f;
                a.saturation = 3.f;
                QStringList tmp;
                tmp << a;
                AV::OpenGLImageColor b;
                tmp >> b;
                DJV_ASSERT(a == b);
            }
            {
                AV::OpenGLImageLevels a;
                a.inLow = 1.f;
                a.inHigh = 2.f;
                a.gamma = 3.f;
                a.outLow = 4.f;
                a.outHigh = 5.f;
                QStringList tmp;
                tmp << a;
                AV::OpenGLImageLevels b;
                tmp >> b;
                DJV_ASSERT(a == b);
            }
            {
                DJV_DEBUG_PRINT(AV::OpenGLImageFilter::BELL);
                DJV_DEBUG_PRINT(AV::OpenGLImageOptions::CHANNEL_RED);
            }
        }

    } // namespace AVTest
} // namespace djv
