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

#include <djvPixelTest.h>

#include <djvCore/Assert.h>
#include <djvGraphics/Color.h>
#include <djvGraphics/ColorUtil.h>
#include <djvCore/Debug.h>
#include <djvGraphics/Pixel.h>

#include <QStringList>

using namespace djv::Core;
using namespace djv::Graphics;

namespace djv
{
    namespace GraphicsTest
    {
        void PixelTest::run(int &, char **)
        {
            DJV_DEBUG("PixelTest::run");
            mask();
            members();
            convert();
            operators();
        }

        void PixelTest::mask()
        {
            DJV_DEBUG("PixelTest::mask");
            {
                const Graphics::Pixel::Mask mask;
                DJV_ASSERT(mask.mask[0]);
                DJV_ASSERT(mask.mask[1]);
                DJV_ASSERT(mask.mask[2]);
                DJV_ASSERT(mask.mask[3]);
            }
            {
                const Graphics::Pixel::Mask mask(false);
                DJV_ASSERT(!mask.mask[0]);
                DJV_ASSERT(!mask.mask[1]);
                DJV_ASSERT(!mask.mask[2]);
                DJV_ASSERT(!mask.mask[3]);
            }
            {
                const Graphics::Pixel::Mask mask(false, false, false, false);
                DJV_ASSERT(!mask.mask[0]);
                DJV_ASSERT(!mask.mask[1]);
                DJV_ASSERT(!mask.mask[2]);
                DJV_ASSERT(!mask.mask[3]);
            }
            {
                const Graphics::Pixel::Mask tmp(false, false, false, false);
                const Graphics::Pixel::Mask mask(tmp);
                DJV_ASSERT(tmp.mask[0] == mask.mask[0]);
                DJV_ASSERT(tmp.mask[1] == mask.mask[1]);
                DJV_ASSERT(tmp.mask[2] == mask.mask[2]);
                DJV_ASSERT(tmp.mask[3] == mask.mask[3]);
            }
            {
                const Graphics::Pixel::Mask tmp(false, false, false, false);
                Graphics::Pixel::Mask mask;
                mask = tmp;
                DJV_ASSERT(tmp.mask[0] == mask.mask[0]);
                DJV_ASSERT(tmp.mask[1] == mask.mask[1]);
                DJV_ASSERT(tmp.mask[2] == mask.mask[2]);
                DJV_ASSERT(tmp.mask[3] == mask.mask[3]);
            }
            {
                Graphics::Pixel::Mask mask;
                mask[1] = false;
                DJV_ASSERT(!mask[1]);
            }
            {
                const Graphics::Pixel::Mask
                    a(false, false, false, false),
                    b(false, false, false, false);
                DJV_ASSERT(a == b);
                DJV_ASSERT(a != Graphics::Pixel::Mask());
            }
            {
                const Graphics::Pixel::Mask tmp(true, false, true, false);
                Graphics::Pixel::Mask mask(tmp);
                QStringList data;
                data << mask;
                data >> mask;
                DJV_ASSERT(tmp == mask);
            }
        }

        void PixelTest::members()
        {
            DJV_DEBUG("PixelTest::members");
            {
                DJV_ASSERT(Graphics::Pixel::RGB == Graphics::Pixel::format(Graphics::Pixel::RGB_U10));
                DJV_ASSERT(Graphics::Pixel::U10 == Graphics::Pixel::type(Graphics::Pixel::RGB_U10));
            }
            {
                Graphics::Pixel::FORMAT format = static_cast<Graphics::Pixel::FORMAT>(0);
                DJV_ASSERT(!Graphics::Pixel::format(0, format));
                for (int i = 0; i < Graphics::Pixel::channelsMax; ++i)
                {
                    Graphics::Pixel::format(i + 1, format);
                    DJV_ASSERT(static_cast<Graphics::Pixel::FORMAT>(i) == format);
                }
            }
            {
                Graphics::Pixel::TYPE type = static_cast<Graphics::Pixel::TYPE>(0);
                DJV_ASSERT(Graphics::Pixel::type(10, Graphics::Pixel::INTEGER, type));
                DJV_ASSERT(Graphics::Pixel::U10 == type);
                DJV_ASSERT(!Graphics::Pixel::type(10, Graphics::Pixel::FLOAT, type));
            }
            {
                Graphics::Pixel::TYPE type = static_cast<Graphics::Pixel::TYPE>(0);
                DJV_ASSERT(!Graphics::Pixel::type(0, Graphics::Pixel::INTEGER, type));
                Graphics::Pixel::type(8, Graphics::Pixel::INTEGER, type);
                DJV_ASSERT(Graphics::Pixel::U8 == type);
                Graphics::Pixel::type(10, Graphics::Pixel::INTEGER, type);
                DJV_ASSERT(Graphics::Pixel::U10 == type);
                Graphics::Pixel::type(16, Graphics::Pixel::INTEGER, type);
                DJV_ASSERT(Graphics::Pixel::U16 == type);
                DJV_ASSERT(!Graphics::Pixel::type(0, Graphics::Pixel::FLOAT, type));
                Graphics::Pixel::type(16, Graphics::Pixel::FLOAT, type);
                DJV_ASSERT(Graphics::Pixel::F16 == type);
                Graphics::Pixel::type(32, Graphics::Pixel::FLOAT, type);
                DJV_ASSERT(Graphics::Pixel::F32 == type);
            }
            {
                DJV_ASSERT(3 == Graphics::Pixel::channels(Graphics::Pixel::RGB));
                DJV_ASSERT(3 == Graphics::Pixel::channels(Graphics::Pixel::RGB_U10));
            }
            {
                DJV_ASSERT(2 == Graphics::Pixel::channelByteCount(Graphics::Pixel::RGBA_U16));
                DJV_ASSERT(8 == Graphics::Pixel::byteCount(Graphics::Pixel::RGBA_U16));
            }
            {
                DJV_ASSERT(16 == Graphics::Pixel::bitDepth(Graphics::Pixel::U16));
                DJV_ASSERT(16 == Graphics::Pixel::bitDepth(Graphics::Pixel::RGBA_U16));
            }
            {
                DJV_ASSERT(Graphics::Pixel::u16Max == Graphics::Pixel::max(Graphics::Pixel::RGBA_U16));
                DJV_ASSERT(1 == Graphics::Pixel::max(Graphics::Pixel::RGBA_F16));
            }
            {
                const Graphics::Pixel::FORMAT formatUndef = Graphics::Pixel::FORMAT_COUNT;
                const Graphics::Pixel::TYPE   typeUndef = Graphics::Pixel::TYPE_COUNT;
                const Graphics::Pixel::PIXEL  pixelUndef = static_cast<Graphics::Pixel::PIXEL>(0);
                const struct Data
                {
                    Graphics::Pixel::FORMAT format;
                    Graphics::Pixel::TYPE   type;
                    Graphics::Pixel::PIXEL  result;
                }
                data[] =
                {
                    { Graphics::Pixel::L,    Graphics::Pixel::U8,  Graphics::Pixel::L_U8     },
                    { Graphics::Pixel::L,    Graphics::Pixel::U10, Graphics::Pixel::L_U16    },
                    { Graphics::Pixel::L,    Graphics::Pixel::U16, Graphics::Pixel::L_U16    },
                    { Graphics::Pixel::L,    Graphics::Pixel::F16, Graphics::Pixel::L_F16    },
                    { Graphics::Pixel::L,    Graphics::Pixel::F32, Graphics::Pixel::L_F32    },
                    { Graphics::Pixel::L,    typeUndef,            pixelUndef                },

                    { Graphics::Pixel::LA,   Graphics::Pixel::U8,  Graphics::Pixel::LA_U8    },
                    { Graphics::Pixel::LA,   Graphics::Pixel::U10, Graphics::Pixel::LA_U16   },
                    { Graphics::Pixel::LA,   Graphics::Pixel::U16, Graphics::Pixel::LA_U16   },
                    { Graphics::Pixel::LA,   Graphics::Pixel::F16, Graphics::Pixel::LA_F16   },
                    { Graphics::Pixel::LA,   Graphics::Pixel::F32, Graphics::Pixel::LA_F32   },
                    { Graphics::Pixel::LA,   typeUndef,            pixelUndef                },

                    { Graphics::Pixel::RGB,  Graphics::Pixel::U8,  Graphics::Pixel::RGB_U8   },
                    { Graphics::Pixel::RGB,  Graphics::Pixel::U10, Graphics::Pixel::RGB_U10  },
                    { Graphics::Pixel::RGB,  Graphics::Pixel::U16, Graphics::Pixel::RGB_U16  },
                    { Graphics::Pixel::RGB,  Graphics::Pixel::F16, Graphics::Pixel::RGB_F16  },
                    { Graphics::Pixel::RGB,  Graphics::Pixel::F32, Graphics::Pixel::RGB_F32  },
                    { Graphics::Pixel::RGB,  typeUndef,     pixelUndef         },

                    { Graphics::Pixel::RGBA, Graphics::Pixel::U8,  Graphics::Pixel::RGBA_U8  },
                    { Graphics::Pixel::RGBA, Graphics::Pixel::U10, Graphics::Pixel::RGBA_U16 },
                    { Graphics::Pixel::RGBA, Graphics::Pixel::U16, Graphics::Pixel::RGBA_U16 },
                    { Graphics::Pixel::RGBA, Graphics::Pixel::F16, Graphics::Pixel::RGBA_F16 },
                    { Graphics::Pixel::RGBA, Graphics::Pixel::F32, Graphics::Pixel::RGBA_F32 },
                    { Graphics::Pixel::RGBA, typeUndef,     pixelUndef         },

                    { formatUndef,    Graphics::Pixel::U8,  pixelUndef         },
                    { formatUndef,    Graphics::Pixel::U10, pixelUndef         },
                    { formatUndef,    Graphics::Pixel::U16, pixelUndef         },
                    { formatUndef,    Graphics::Pixel::F16, pixelUndef         },
                    { formatUndef,    Graphics::Pixel::F32, pixelUndef         },
                    { formatUndef,    typeUndef,     pixelUndef         }
                };
                const int dataCount = sizeof(data) / sizeof(data[0]);
                for (int i = 0; i < dataCount; ++i)
                {
                    DJV_ASSERT(
                        data[i].result ==
                        Graphics::Pixel::pixel(data[i].format, data[i].type));
                }
            }
            {
                const Graphics::Pixel::FORMAT formatUndef = Graphics::Pixel::FORMAT_COUNT;
                const Graphics::Pixel::TYPE   typeUndef = Graphics::Pixel::TYPE_COUNT;
                const Graphics::Pixel::PIXEL  pixelUndef = static_cast<Graphics::Pixel::PIXEL>(0);
                const struct Data
                {
                    Graphics::Pixel::FORMAT format;
                    Graphics::Pixel::TYPE   type;
                    Graphics::Pixel::PIXEL  result;
                    bool             ok;
                }
                data[] =
                {
                    { Graphics::Pixel::L,    Graphics::Pixel::U8,  Graphics::Pixel::L_U8,     true  },
                    { Graphics::Pixel::L,    Graphics::Pixel::U10, pixelUndef,         false },
                    { Graphics::Pixel::L,    Graphics::Pixel::U16, Graphics::Pixel::L_U16,    true  },
                    { Graphics::Pixel::L,    Graphics::Pixel::F16, Graphics::Pixel::L_F16,    true  },
                    { Graphics::Pixel::L,    Graphics::Pixel::F32, Graphics::Pixel::L_F32,    true  },
                    { Graphics::Pixel::L,    typeUndef,     pixelUndef,         false },

                    { Graphics::Pixel::LA,   Graphics::Pixel::U8,  Graphics::Pixel::LA_U8,    true  },
                    { Graphics::Pixel::LA,   Graphics::Pixel::U10, pixelUndef,         false },
                    { Graphics::Pixel::LA,   Graphics::Pixel::U16, Graphics::Pixel::LA_U16,   true  },
                    { Graphics::Pixel::LA,   Graphics::Pixel::F16, Graphics::Pixel::LA_F16,   true  },
                    { Graphics::Pixel::LA,   Graphics::Pixel::F32, Graphics::Pixel::LA_F32,   true  },
                    { Graphics::Pixel::LA,   typeUndef,     pixelUndef,         false },

                    { Graphics::Pixel::RGB,  Graphics::Pixel::U8,  Graphics::Pixel::RGB_U8,   true  },
                    { Graphics::Pixel::RGB,  Graphics::Pixel::U10, Graphics::Pixel::RGB_U10,  true  },
                    { Graphics::Pixel::RGB,  Graphics::Pixel::U16, Graphics::Pixel::RGB_U16,  true  },
                    { Graphics::Pixel::RGB,  Graphics::Pixel::F16, Graphics::Pixel::RGB_F16,  true  },
                    { Graphics::Pixel::RGB,  Graphics::Pixel::F32, Graphics::Pixel::RGB_F32,  true  },
                    { Graphics::Pixel::RGB,  typeUndef,     pixelUndef,         false },

                    { Graphics::Pixel::RGBA, Graphics::Pixel::U8,  Graphics::Pixel::RGBA_U8,  true  },
                    { Graphics::Pixel::RGBA, Graphics::Pixel::U10, pixelUndef,         false },
                    { Graphics::Pixel::RGBA, Graphics::Pixel::U16, Graphics::Pixel::RGBA_U16, true  },
                    { Graphics::Pixel::RGBA, Graphics::Pixel::F16, Graphics::Pixel::RGBA_F16, true  },
                    { Graphics::Pixel::RGBA, Graphics::Pixel::F32, Graphics::Pixel::RGBA_F32, true  },
                    { Graphics::Pixel::RGBA, typeUndef,     pixelUndef,         false },
                    { formatUndef,    Graphics::Pixel::U8,  pixelUndef,         false },
                    { formatUndef,    Graphics::Pixel::U10, pixelUndef,         false },
                    { formatUndef,    Graphics::Pixel::U16, pixelUndef,         false },
                    { formatUndef,    Graphics::Pixel::F16, pixelUndef,         false },
                    { formatUndef,    Graphics::Pixel::F32, pixelUndef,         false },

                    { formatUndef,    typeUndef,     pixelUndef,         false }
                };
                const int dataCount = sizeof(data) / sizeof(data[0]);
                for (int i = 0; i < dataCount; ++i)
                {
                    Graphics::Pixel::PIXEL result = pixelUndef;
                    DJV_ASSERT(
                        data[i].ok ==
                        Graphics::Pixel::pixel(data[i].format, data[i].type, result));
                    DJV_ASSERT(data[i].result == result);
                }
            }
            {
                Graphics::Pixel::PIXEL pixel = static_cast<Graphics::Pixel::PIXEL>(0);
                DJV_ASSERT(Graphics::Pixel::pixel(Graphics::Pixel::RGB, Graphics::Pixel::U10, pixel));
                DJV_ASSERT(Graphics::Pixel::RGB_U10 == pixel);
                DJV_ASSERT(!Graphics::Pixel::pixel(Graphics::Pixel::RGBA, Graphics::Pixel::U10, pixel));
            }
            {
                Graphics::Pixel::PIXEL pixel = static_cast<Graphics::Pixel::PIXEL>(0);
                DJV_ASSERT(Graphics::Pixel::pixel(3, 10, Graphics::Pixel::INTEGER, pixel));
                DJV_ASSERT(Graphics::Pixel::RGB_U10 == pixel);
                DJV_ASSERT(!Graphics::Pixel::pixel(3, 10, Graphics::Pixel::FLOAT, pixel));
                DJV_ASSERT(!Graphics::Pixel::pixel(4, 10, Graphics::Pixel::INTEGER, pixel));
            }
        }

        void PixelTest::convert()
        {
            DJV_DEBUG("PixelTest::convert");
            {
                DJV_ASSERT(0 == Graphics::Pixel::u8ToU10(0));
                DJV_ASSERT(Graphics::Pixel::u10Max == Graphics::Pixel::u8ToU10(Graphics::Pixel::u8Max));
                DJV_ASSERT(0 == Graphics::Pixel::u8ToU16(0));
                DJV_ASSERT(Graphics::Pixel::u16Max == Graphics::Pixel::u8ToU16(Graphics::Pixel::u8Max));
                DJV_ASSERT(Math::fuzzyCompare(0.f, Graphics::Pixel::u8ToF16(0)));
                DJV_ASSERT(Math::fuzzyCompare(1.f, Graphics::Pixel::u8ToF16(Graphics::Pixel::u8Max)));
                DJV_ASSERT(Math::fuzzyCompare(0.f, Graphics::Pixel::u8ToF32(0)));
                DJV_ASSERT(Math::fuzzyCompare(1.f, Graphics::Pixel::u8ToF32(Graphics::Pixel::u8Max)));
            }
            {
                DJV_ASSERT(0 == Graphics::Pixel::u10ToU8(0));
                DJV_ASSERT(Graphics::Pixel::u8Max == Graphics::Pixel::u10ToU8(Graphics::Pixel::u10Max));
                DJV_ASSERT(0 == Graphics::Pixel::u10ToU16(0));
                DJV_ASSERT(Graphics::Pixel::u16Max == Graphics::Pixel::u10ToU16(Graphics::Pixel::u10Max));
                DJV_ASSERT(Math::fuzzyCompare(0.f, Graphics::Pixel::u10ToF16(0)));
                DJV_ASSERT(Math::fuzzyCompare(1.f, Graphics::Pixel::u10ToF16(Graphics::Pixel::u10Max)));
                DJV_ASSERT(Math::fuzzyCompare(0.f, Graphics::Pixel::u10ToF32(0)));
                DJV_ASSERT(Math::fuzzyCompare(1.f, Graphics::Pixel::u10ToF32(Graphics::Pixel::u10Max)));
            }
            {
                DJV_ASSERT(0 == Graphics::Pixel::u16ToU8(0));
                DJV_ASSERT(Graphics::Pixel::u8Max == Graphics::Pixel::u16ToU8(Graphics::Pixel::u16Max));
                DJV_ASSERT(0 == Graphics::Pixel::u16ToU10(0));
                DJV_ASSERT(Graphics::Pixel::u10Max == Graphics::Pixel::u16ToU10(Graphics::Pixel::u16Max));
                DJV_ASSERT(Math::fuzzyCompare(0.f, Graphics::Pixel::u16ToF16(0)));
                DJV_ASSERT(Math::fuzzyCompare(1.f, Graphics::Pixel::u16ToF16(Graphics::Pixel::u16Max)));
                DJV_ASSERT(Math::fuzzyCompare(0.f, Graphics::Pixel::u16ToF32(0)));
                DJV_ASSERT(Math::fuzzyCompare(1.f, Graphics::Pixel::u16ToF32(Graphics::Pixel::u16Max)));
            }
            {
                DJV_ASSERT(0 == Graphics::Pixel::f16ToU8(0.f));
                DJV_ASSERT(Graphics::Pixel::u8Max == Graphics::Pixel::f16ToU8(1.f));
                DJV_ASSERT(0 == Graphics::Pixel::f16ToU10(0.f));
                DJV_ASSERT(Graphics::Pixel::u10Max == Graphics::Pixel::f16ToU10(1.f));
                DJV_ASSERT(0 == Graphics::Pixel::f16ToU16(0.f));
                DJV_ASSERT(Graphics::Pixel::u16Max == Graphics::Pixel::f16ToU16(1.f));
                DJV_ASSERT(Math::fuzzyCompare(0.f, Graphics::Pixel::f16ToF32(0.f)));
                DJV_ASSERT(Math::fuzzyCompare(1.f, Graphics::Pixel::f16ToF32(1.f)));
            }
            {
                DJV_ASSERT(0 == Graphics::Pixel::f32ToU8(0.f));
                DJV_ASSERT(Graphics::Pixel::u8Max == Graphics::Pixel::f32ToU8(1.f));
                DJV_ASSERT(0 == Graphics::Pixel::f32ToU10(0.f));
                DJV_ASSERT(Graphics::Pixel::u10Max == Graphics::Pixel::f32ToU10(1.f));
                DJV_ASSERT(0 == Graphics::Pixel::f32ToU16(0.f));
                DJV_ASSERT(Graphics::Pixel::u16Max == Graphics::Pixel::f32ToU16(1.f));
                DJV_ASSERT(Math::fuzzyCompare(0.f, Graphics::Pixel::f32ToF16(0.f)));
                DJV_ASSERT(Math::fuzzyCompare(1.f, Graphics::Pixel::f32ToF16(1.f)));
            }
            {
                for (int i = 0; i < Graphics::Pixel::PIXEL_COUNT; ++i)
                {
                    for (int j = 0; j < Graphics::Pixel::PIXEL_COUNT; ++j)
                    {
                        Graphics::Color
                            a(static_cast<Graphics::Pixel::PIXEL>(i)),
                            b(static_cast<Graphics::Pixel::PIXEL>(j));
                        Graphics::ColorUtil::convert(Graphics::Color(.5f), a);
                        Graphics::Pixel::convert(
                            a.data(),
                            a.pixel(),
                            b.data(),
                            b.pixel(),
                            1,
                            0,
                            false);
                        DJV_DEBUG_PRINT("a = " << a);
                        DJV_DEBUG_PRINT("b = " << b);
                    }
                }
            }
        }

        void PixelTest::operators()
        {
            DJV_DEBUG("PixelTest::operators");
            {
                Graphics::Pixel::PIXEL pixel = Graphics::Pixel::L_F32;
                QStringList s;
                s << pixel;
                DJV_ASSERT(s.count() && s[0] == "L F32");
                s >> pixel;
                DJV_ASSERT(Graphics::Pixel::L_F32 == pixel);
            }
            {
                DJV_DEBUG_PRINT(Graphics::Pixel::RGBA);
                DJV_DEBUG_PRINT(Graphics::Pixel::F32);
                DJV_DEBUG_PRINT(Graphics::Pixel::FLOAT);
                DJV_DEBUG_PRINT(Graphics::Pixel::RGBA_F32);
            }
        }

    } // namespace GraphicsTest
} // namespace djv
