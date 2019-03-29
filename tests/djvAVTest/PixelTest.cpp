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

#include <djvAVTest/PixelTest.h>

#include <djvAV/Color.h>
#include <djvAV/ColorUtil.h>
#include <djvAV/Pixel.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>

#include <QStringList>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
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
                const AV::Pixel::Mask mask;
                DJV_ASSERT(mask.mask[0]);
                DJV_ASSERT(mask.mask[1]);
                DJV_ASSERT(mask.mask[2]);
                DJV_ASSERT(mask.mask[3]);
            }
            {
                const AV::Pixel::Mask mask(false);
                DJV_ASSERT(!mask.mask[0]);
                DJV_ASSERT(!mask.mask[1]);
                DJV_ASSERT(!mask.mask[2]);
                DJV_ASSERT(!mask.mask[3]);
            }
            {
                const AV::Pixel::Mask mask(false, false, false, false);
                DJV_ASSERT(!mask.mask[0]);
                DJV_ASSERT(!mask.mask[1]);
                DJV_ASSERT(!mask.mask[2]);
                DJV_ASSERT(!mask.mask[3]);
            }
            {
                const AV::Pixel::Mask tmp(false, false, false, false);
                const AV::Pixel::Mask mask(tmp);
                DJV_ASSERT(tmp.mask[0] == mask.mask[0]);
                DJV_ASSERT(tmp.mask[1] == mask.mask[1]);
                DJV_ASSERT(tmp.mask[2] == mask.mask[2]);
                DJV_ASSERT(tmp.mask[3] == mask.mask[3]);
            }
            {
                const AV::Pixel::Mask tmp(false, false, false, false);
                AV::Pixel::Mask mask;
                mask = tmp;
                DJV_ASSERT(tmp.mask[0] == mask.mask[0]);
                DJV_ASSERT(tmp.mask[1] == mask.mask[1]);
                DJV_ASSERT(tmp.mask[2] == mask.mask[2]);
                DJV_ASSERT(tmp.mask[3] == mask.mask[3]);
            }
            {
                AV::Pixel::Mask mask;
                mask[1] = false;
                DJV_ASSERT(!mask[1]);
            }
            {
                const AV::Pixel::Mask
                    a(false, false, false, false),
                    b(false, false, false, false);
                DJV_ASSERT(a == b);
                DJV_ASSERT(a != AV::Pixel::Mask());
            }
            {
                const AV::Pixel::Mask tmp(true, false, true, false);
                AV::Pixel::Mask mask(tmp);
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
                DJV_ASSERT(AV::Pixel::RGB == AV::Pixel::format(AV::Pixel::RGB_U10));
                DJV_ASSERT(AV::Pixel::U10 == AV::Pixel::type(AV::Pixel::RGB_U10));
            }
            {
                AV::Pixel::FORMAT format = static_cast<AV::Pixel::FORMAT>(0);
                DJV_ASSERT(!AV::Pixel::format(0, format));
                for (int i = 0; i < AV::Pixel::channelsMax; ++i)
                {
                    AV::Pixel::format(i + 1, format);
                    DJV_ASSERT(static_cast<AV::Pixel::FORMAT>(i) == format);
                }
            }
            {
                AV::Pixel::TYPE type = static_cast<AV::Pixel::TYPE>(0);
                DJV_ASSERT(AV::Pixel::intType(10, type));
                DJV_ASSERT(AV::Pixel::U10 == type);
            }
            {
                AV::Pixel::TYPE type = static_cast<AV::Pixel::TYPE>(0);
                DJV_ASSERT(AV::Pixel::intType(10, type));
                DJV_ASSERT(AV::Pixel::U10 == type);
            }
            {
                AV::Pixel::TYPE type = static_cast<AV::Pixel::TYPE>(0);
                DJV_ASSERT(!AV::Pixel::intType(0, type));
                AV::Pixel::intType(8, type);
                DJV_ASSERT(AV::Pixel::U8 == type);
                AV::Pixel::intType(10, type);
                DJV_ASSERT(AV::Pixel::U10 == type);
                AV::Pixel::intType(16, type);
                DJV_ASSERT(AV::Pixel::U16 == type);
                DJV_ASSERT(!AV::Pixel::floatType(0, type));
                AV::Pixel::floatType(16, type);
                DJV_ASSERT(AV::Pixel::F16 == type);
                AV::Pixel::floatType(32, type);
                DJV_ASSERT(AV::Pixel::F32 == type);
            }
            {
                DJV_ASSERT(3 == AV::Pixel::channels(AV::Pixel::RGB));
                DJV_ASSERT(3 == AV::Pixel::channels(AV::Pixel::RGB_U10));
            }
            {
                DJV_ASSERT(2 == AV::Pixel::channelByteCount(AV::Pixel::RGBA_U16));
                DJV_ASSERT(8 == AV::Pixel::byteCount(AV::Pixel::RGBA_U16));
            }
            {
                DJV_ASSERT(16 == AV::Pixel::bitDepth(AV::Pixel::U16));
                DJV_ASSERT(16 == AV::Pixel::bitDepth(AV::Pixel::RGBA_U16));
            }
            {
                DJV_ASSERT(AV::Pixel::u16Max == AV::Pixel::max(AV::Pixel::RGBA_U16));
                DJV_ASSERT(1 == AV::Pixel::max(AV::Pixel::RGBA_F16));
            }
            {
                const AV::Pixel::FORMAT formatUndef = AV::Pixel::FORMAT_COUNT;
                const AV::Pixel::TYPE   typeUndef = AV::Pixel::TYPE_COUNT;
                const AV::Pixel::PIXEL  pixelUndef = static_cast<AV::Pixel::PIXEL>(0);
                const struct Data
                {
                    AV::Pixel::FORMAT format;
                    AV::Pixel::TYPE   type;
                    AV::Pixel::PIXEL  result;
                }
                data[] =
                {
                    { AV::Pixel::L,    AV::Pixel::U8,  AV::Pixel::L_U8     },
                    { AV::Pixel::L,    AV::Pixel::U10, AV::Pixel::L_U16    },
                    { AV::Pixel::L,    AV::Pixel::U16, AV::Pixel::L_U16    },
                    { AV::Pixel::L,    AV::Pixel::F16, AV::Pixel::L_F16    },
                    { AV::Pixel::L,    AV::Pixel::F32, AV::Pixel::L_F32    },
                    { AV::Pixel::L,    typeUndef,            pixelUndef                },

                    { AV::Pixel::LA,   AV::Pixel::U8,  AV::Pixel::LA_U8    },
                    { AV::Pixel::LA,   AV::Pixel::U10, AV::Pixel::LA_U16   },
                    { AV::Pixel::LA,   AV::Pixel::U16, AV::Pixel::LA_U16   },
                    { AV::Pixel::LA,   AV::Pixel::F16, AV::Pixel::LA_F16   },
                    { AV::Pixel::LA,   AV::Pixel::F32, AV::Pixel::LA_F32   },
                    { AV::Pixel::LA,   typeUndef,            pixelUndef                },

                    { AV::Pixel::RGB,  AV::Pixel::U8,  AV::Pixel::RGB_U8   },
                    { AV::Pixel::RGB,  AV::Pixel::U10, AV::Pixel::RGB_U10  },
                    { AV::Pixel::RGB,  AV::Pixel::U16, AV::Pixel::RGB_U16  },
                    { AV::Pixel::RGB,  AV::Pixel::F16, AV::Pixel::RGB_F16  },
                    { AV::Pixel::RGB,  AV::Pixel::F32, AV::Pixel::RGB_F32  },
                    { AV::Pixel::RGB,  typeUndef,     pixelUndef         },

                    { AV::Pixel::RGBA, AV::Pixel::U8,  AV::Pixel::RGBA_U8  },
                    { AV::Pixel::RGBA, AV::Pixel::U10, AV::Pixel::RGBA_U16 },
                    { AV::Pixel::RGBA, AV::Pixel::U16, AV::Pixel::RGBA_U16 },
                    { AV::Pixel::RGBA, AV::Pixel::F16, AV::Pixel::RGBA_F16 },
                    { AV::Pixel::RGBA, AV::Pixel::F32, AV::Pixel::RGBA_F32 },
                    { AV::Pixel::RGBA, typeUndef,     pixelUndef         },

                    { formatUndef,    AV::Pixel::U8,  pixelUndef         },
                    { formatUndef,    AV::Pixel::U10, pixelUndef         },
                    { formatUndef,    AV::Pixel::U16, pixelUndef         },
                    { formatUndef,    AV::Pixel::F16, pixelUndef         },
                    { formatUndef,    AV::Pixel::F32, pixelUndef         },
                    { formatUndef,    typeUndef,     pixelUndef         }
                };
                const int dataCount = sizeof(data) / sizeof(data[0]);
                for (int i = 0; i < dataCount; ++i)
                {
                    DJV_ASSERT(
                        data[i].result ==
                        AV::Pixel::pixel(data[i].format, data[i].type));
                }
            }
            {
                const AV::Pixel::FORMAT formatUndef = AV::Pixel::FORMAT_COUNT;
                const AV::Pixel::TYPE   typeUndef = AV::Pixel::TYPE_COUNT;
                const AV::Pixel::PIXEL  pixelUndef = static_cast<AV::Pixel::PIXEL>(0);
                const struct Data
                {
                    AV::Pixel::FORMAT format;
                    AV::Pixel::TYPE   type;
                    AV::Pixel::PIXEL  result;
                    bool             ok;
                }
                data[] =
                {
                    { AV::Pixel::L,    AV::Pixel::U8,  AV::Pixel::L_U8,     true  },
                    { AV::Pixel::L,    AV::Pixel::U10, pixelUndef,         false },
                    { AV::Pixel::L,    AV::Pixel::U16, AV::Pixel::L_U16,    true  },
                    { AV::Pixel::L,    AV::Pixel::F16, AV::Pixel::L_F16,    true  },
                    { AV::Pixel::L,    AV::Pixel::F32, AV::Pixel::L_F32,    true  },
                    { AV::Pixel::L,    typeUndef,     pixelUndef,         false },

                    { AV::Pixel::LA,   AV::Pixel::U8,  AV::Pixel::LA_U8,    true  },
                    { AV::Pixel::LA,   AV::Pixel::U10, pixelUndef,         false },
                    { AV::Pixel::LA,   AV::Pixel::U16, AV::Pixel::LA_U16,   true  },
                    { AV::Pixel::LA,   AV::Pixel::F16, AV::Pixel::LA_F16,   true  },
                    { AV::Pixel::LA,   AV::Pixel::F32, AV::Pixel::LA_F32,   true  },
                    { AV::Pixel::LA,   typeUndef,     pixelUndef,         false },

                    { AV::Pixel::RGB,  AV::Pixel::U8,  AV::Pixel::RGB_U8,   true  },
                    { AV::Pixel::RGB,  AV::Pixel::U10, AV::Pixel::RGB_U10,  true  },
                    { AV::Pixel::RGB,  AV::Pixel::U16, AV::Pixel::RGB_U16,  true  },
                    { AV::Pixel::RGB,  AV::Pixel::F16, AV::Pixel::RGB_F16,  true  },
                    { AV::Pixel::RGB,  AV::Pixel::F32, AV::Pixel::RGB_F32,  true  },
                    { AV::Pixel::RGB,  typeUndef,     pixelUndef,         false },

                    { AV::Pixel::RGBA, AV::Pixel::U8,  AV::Pixel::RGBA_U8,  true  },
                    { AV::Pixel::RGBA, AV::Pixel::U10, pixelUndef,         false },
                    { AV::Pixel::RGBA, AV::Pixel::U16, AV::Pixel::RGBA_U16, true  },
                    { AV::Pixel::RGBA, AV::Pixel::F16, AV::Pixel::RGBA_F16, true  },
                    { AV::Pixel::RGBA, AV::Pixel::F32, AV::Pixel::RGBA_F32, true  },
                    { AV::Pixel::RGBA, typeUndef,     pixelUndef,         false },
                    { formatUndef,    AV::Pixel::U8,  pixelUndef,         false },
                    { formatUndef,    AV::Pixel::U10, pixelUndef,         false },
                    { formatUndef,    AV::Pixel::U16, pixelUndef,         false },
                    { formatUndef,    AV::Pixel::F16, pixelUndef,         false },
                    { formatUndef,    AV::Pixel::F32, pixelUndef,         false },

                    { formatUndef,    typeUndef,     pixelUndef,         false }
                };
                const int dataCount = sizeof(data) / sizeof(data[0]);
                for (int i = 0; i < dataCount; ++i)
                {
                    AV::Pixel::PIXEL result = pixelUndef;
                    DJV_ASSERT(
                        data[i].ok ==
                        AV::Pixel::pixel(data[i].format, data[i].type, result));
                    DJV_ASSERT(data[i].result == result);
                }
            }
            {
                AV::Pixel::PIXEL pixel = static_cast<AV::Pixel::PIXEL>(0);
                DJV_ASSERT(AV::Pixel::pixel(AV::Pixel::RGB, AV::Pixel::U10, pixel));
                DJV_ASSERT(AV::Pixel::RGB_U10 == pixel);
                DJV_ASSERT(!AV::Pixel::pixel(AV::Pixel::RGBA, AV::Pixel::U10, pixel));
            }
            {
                AV::Pixel::PIXEL pixel = static_cast<AV::Pixel::PIXEL>(0);
                DJV_ASSERT(AV::Pixel::intPixel(3, 10, pixel));
                DJV_ASSERT(AV::Pixel::RGB_U10 == pixel);
                DJV_ASSERT(!AV::Pixel::floatPixel(3, 10, pixel));
                DJV_ASSERT(!AV::Pixel::intPixel(4, 10, pixel));
            }
        }

        void PixelTest::convert()
        {
            DJV_DEBUG("PixelTest::convert");
            {
                DJV_ASSERT(0 == AV::Pixel::u8ToU10(0));
                DJV_ASSERT(AV::Pixel::u10Max == AV::Pixel::u8ToU10(AV::Pixel::u8Max));
                DJV_ASSERT(0 == AV::Pixel::u8ToU16(0));
                DJV_ASSERT(AV::Pixel::u16Max == AV::Pixel::u8ToU16(AV::Pixel::u8Max));
                DJV_ASSERT(Math::fuzzyCompare(0.f, AV::Pixel::u8ToF16(0)));
                DJV_ASSERT(Math::fuzzyCompare(1.f, AV::Pixel::u8ToF16(AV::Pixel::u8Max)));
                DJV_ASSERT(Math::fuzzyCompare(0.f, AV::Pixel::u8ToF32(0)));
                DJV_ASSERT(Math::fuzzyCompare(1.f, AV::Pixel::u8ToF32(AV::Pixel::u8Max)));
            }
            {
                DJV_ASSERT(0 == AV::Pixel::u10ToU8(0));
                DJV_ASSERT(AV::Pixel::u8Max == AV::Pixel::u10ToU8(AV::Pixel::u10Max));
                DJV_ASSERT(0 == AV::Pixel::u10ToU16(0));
                DJV_ASSERT(AV::Pixel::u16Max == AV::Pixel::u10ToU16(AV::Pixel::u10Max));
                DJV_ASSERT(Math::fuzzyCompare(0.f, AV::Pixel::u10ToF16(0)));
                DJV_ASSERT(Math::fuzzyCompare(1.f, AV::Pixel::u10ToF16(AV::Pixel::u10Max)));
                DJV_ASSERT(Math::fuzzyCompare(0.f, AV::Pixel::u10ToF32(0)));
                DJV_ASSERT(Math::fuzzyCompare(1.f, AV::Pixel::u10ToF32(AV::Pixel::u10Max)));
            }
            {
                DJV_ASSERT(0 == AV::Pixel::u16ToU8(0));
                DJV_ASSERT(AV::Pixel::u8Max == AV::Pixel::u16ToU8(AV::Pixel::u16Max));
                DJV_ASSERT(0 == AV::Pixel::u16ToU10(0));
                DJV_ASSERT(AV::Pixel::u10Max == AV::Pixel::u16ToU10(AV::Pixel::u16Max));
                DJV_ASSERT(Math::fuzzyCompare(0.f, AV::Pixel::u16ToF16(0)));
                DJV_ASSERT(Math::fuzzyCompare(1.f, AV::Pixel::u16ToF16(AV::Pixel::u16Max)));
                DJV_ASSERT(Math::fuzzyCompare(0.f, AV::Pixel::u16ToF32(0)));
                DJV_ASSERT(Math::fuzzyCompare(1.f, AV::Pixel::u16ToF32(AV::Pixel::u16Max)));
            }
            {
                DJV_ASSERT(0 == AV::Pixel::f16ToU8(0.f));
                DJV_ASSERT(AV::Pixel::u8Max == AV::Pixel::f16ToU8(1.f));
                DJV_ASSERT(0 == AV::Pixel::f16ToU10(0.f));
                DJV_ASSERT(AV::Pixel::u10Max == AV::Pixel::f16ToU10(1.f));
                DJV_ASSERT(0 == AV::Pixel::f16ToU16(0.f));
                DJV_ASSERT(AV::Pixel::u16Max == AV::Pixel::f16ToU16(1.f));
                DJV_ASSERT(Math::fuzzyCompare(0.f, AV::Pixel::f16ToF32(0.f)));
                DJV_ASSERT(Math::fuzzyCompare(1.f, AV::Pixel::f16ToF32(1.f)));
            }
            {
                DJV_ASSERT(0 == AV::Pixel::f32ToU8(0.f));
                DJV_ASSERT(AV::Pixel::u8Max == AV::Pixel::f32ToU8(1.f));
                DJV_ASSERT(0 == AV::Pixel::f32ToU10(0.f));
                DJV_ASSERT(AV::Pixel::u10Max == AV::Pixel::f32ToU10(1.f));
                DJV_ASSERT(0 == AV::Pixel::f32ToU16(0.f));
                DJV_ASSERT(AV::Pixel::u16Max == AV::Pixel::f32ToU16(1.f));
                DJV_ASSERT(Math::fuzzyCompare(0.f, AV::Pixel::f32ToF16(0.f)));
                DJV_ASSERT(Math::fuzzyCompare(1.f, AV::Pixel::f32ToF16(1.f)));
            }
            {
                for (int i = 0; i < AV::Pixel::PIXEL_COUNT; ++i)
                {
                    for (int j = 0; j < AV::Pixel::PIXEL_COUNT; ++j)
                    {
                        AV::Color
                            a(static_cast<AV::Pixel::PIXEL>(i)),
                            b(static_cast<AV::Pixel::PIXEL>(j));
                        AV::ColorUtil::convert(AV::Color(.5f), a);
                        AV::Pixel::convert(
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
                AV::Pixel::PIXEL pixel = AV::Pixel::L_F32;
                QStringList s;
                s << pixel;
                DJV_ASSERT(s.count() && s[0] == "L F32");
                s >> pixel;
                DJV_ASSERT(AV::Pixel::L_F32 == pixel);
            }
            {
                DJV_DEBUG_PRINT(AV::Pixel::RGBA);
                DJV_DEBUG_PRINT(AV::Pixel::F32);
                DJV_DEBUG_PRINT(AV::Pixel::RGBA_F32);
            }
        }

    } // namespace AVTest
} // namespace djv
