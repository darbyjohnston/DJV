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

#include <djvGraphicsTest/ColorUtilTest.h>

#include <djvGraphics/Color.h>
#include <djvGraphics/ColorUtil.h>

#include <djvCore/Debug.h>

#include <QColor>

using namespace djv::Core;
using namespace djv::Graphics;

namespace djv
{
    namespace GraphicsTest
    {
        void ColorUtilTest::run(int &, char **)
        {
            DJV_DEBUG("ColorUtilTest::run");
            scale();
            lerp();
            convert();
            qt();
        }

        void ColorUtilTest::scale()
        {
            DJV_DEBUG("ColorUtilTest::scale");
            {
                Graphics::Color color(Graphics::Pixel::L_F32);
                Graphics::ColorUtil::scale(.5f, Graphics::Color(1.f), color);
                DJV_ASSERT(Math::fuzzyCompare(.5f, color.f32(0)));
            }
            {
                const Graphics::Color color = Graphics::ColorUtil::scale(.5f, Graphics::Color(1.f));
                DJV_ASSERT(Math::fuzzyCompare(.5f, color.f32(0)));
            }
        }

        void ColorUtilTest::lerp()
        {
            DJV_DEBUG("ColorUtilTest::lerp");
            {
                Graphics::Color color(Graphics::Pixel::L_F32);
                Graphics::ColorUtil::lerp(.5f, Graphics::Color(0.f), Graphics::Color(1.f), color);
                DJV_ASSERT(Math::fuzzyCompare(.5f, color.f32(0)));
            }
            {
                const Graphics::Color color = Graphics::ColorUtil::lerp(.5f, Graphics::Color(0.f), Graphics::Color(1.f));
                DJV_ASSERT(Math::fuzzyCompare(.5f, color.f32(0)));
            }
        }

        void ColorUtilTest::convert()
        {
            DJV_DEBUG("ColorUtilTest::convert");
            const struct Data
            {
                QStringList  in;
                Graphics::Pixel::PIXEL pixel;
                QStringList  out;
            }
            data[] =
            {
                { QStringList() << "L U8" << "0",      Graphics::Pixel::L_U8,    QStringList() << "L U8" << "0" },
                { QStringList() << "L U8" << "255",    Graphics::Pixel::L_U8,    QStringList() << "L U8" << "255" },
                { QStringList() << "L U8" << "0",      Graphics::Pixel::RGB_U10, QStringList() << "RGB U10" << "0" << "0" << "0" },
                { QStringList() << "L U8" << "255",    Graphics::Pixel::RGB_U10, QStringList() << "RGB U10" << "1023" << "1023" << "1023" },
                { QStringList() << "L U8" << "0",      Graphics::Pixel::L_U16,   QStringList() << "L U16" << "0" },
                { QStringList() << "L U8" << "255",    Graphics::Pixel::L_U16,   QStringList() << "L U16" << "65535" },
                { QStringList() << "L U16" << "0",     Graphics::Pixel::L_U8,    QStringList() << "L U8" << "0" },
                { QStringList() << "L U16" << "65535", Graphics::Pixel::L_U8,    QStringList() << "L U8" << "255" },
                { QStringList() << "L U16" << "0",     Graphics::Pixel::RGB_U10, QStringList() << "RGB U10" << "0" << "0" << "0" },
                { QStringList() << "L U16" << "65535", Graphics::Pixel::RGB_U10, QStringList() << "RGB U10" << "1023" << "1023" << "1023" },
                { QStringList() << "L U16" << "0",     Graphics::Pixel::L_U16,   QStringList() << "L U16" << "0" },
                { QStringList() << "L U16" << "65535", Graphics::Pixel::L_U16,   QStringList() << "L U16" << "65535" },
                { QStringList() << "L F32" << "-1.0",  Graphics::Pixel::L_U8,    QStringList() << "L U8" << "0" },
                { QStringList() << "L F32" << "0.0",   Graphics::Pixel::L_U8,    QStringList() << "L U8" << "0" },
                { QStringList() << "L F32" << "0.5",   Graphics::Pixel::L_U8,    QStringList() << "L U8" << "128" },
                { QStringList() << "L F32" << "1.0",   Graphics::Pixel::L_U8,    QStringList() << "L U8" << "255" },
                { QStringList() << "L F32" << "2.0",   Graphics::Pixel::L_U8,    QStringList() << "L U8" << "255" },
                { QStringList() << "L F32" << "-1.0",  Graphics::Pixel::L_U16,   QStringList() << "L U16" << "0" },
                { QStringList() << "L F32" << "0.0",   Graphics::Pixel::L_U16,   QStringList() << "L U16" << "0" },
                { QStringList() << "L F32" << "0.5",   Graphics::Pixel::L_U16,   QStringList() << "L U16" << "32768" },
                { QStringList() << "L F32" << "1.0",   Graphics::Pixel::L_U16,   QStringList() << "L U16" << "65535" },
                { QStringList() << "L F32" << "2.0",   Graphics::Pixel::L_U16,   QStringList() << "L U16" << "65535" },
                { QStringList() << "L F32" << "1.0",   Graphics::Pixel::L_F16,   QStringList() << "L F16" << "1" }
            };
            const int dataCount = sizeof(data) / sizeof(Data);
            for (int i = 0; i < dataCount; ++i)
            {
                Graphics::Color in;
                QStringList tmp = data[i].in;
                tmp >> in;
                Graphics::Color out(data[i].pixel);
                Graphics::ColorUtil::convert(in, out);
                QStringList label;
                label << out;
                DJV_ASSERT(label == data[i].out);
            }
        }

        void ColorUtilTest::qt()
        {
            DJV_DEBUG("ColorUtilTest::qt");
            {
                Graphics::Color color(Graphics::Pixel::RGBA_U8);
                color.setU8(255, 0);
                color.setU8(127, 1);
                color.setU8(0, 2);
                color.setU8(255, 3);
                const QColor qColor = Graphics::ColorUtil::toQt(color);
                DJV_ASSERT(qColor.red() == color.u8(0));
                DJV_ASSERT(qColor.green() == color.u8(1));
                DJV_ASSERT(qColor.blue() == color.u8(2));
                DJV_ASSERT(qColor.alpha() == color.u8(3));
            }
            {
                QColor qColor(255, 127, 0, 255);
                const Graphics::Color color = Graphics::ColorUtil::fromQt(qColor);
                DJV_ASSERT(color.u8(0) == qColor.red());
                DJV_ASSERT(color.u8(1) == qColor.green());
                DJV_ASSERT(color.u8(2) == qColor.blue());
                DJV_ASSERT(color.u8(3) == qColor.alpha());
            }
        }

    } // namespace GraphicsTest
} // namespace djv
