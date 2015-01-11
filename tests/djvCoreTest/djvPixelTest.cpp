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

//! \file djvPixelTest.cpp

#include <djvPixelTest.h>

#include <djvAssert.h>
#include <djvColor.h>
#include <djvColorUtil.h>
#include <djvDebug.h>
#include <djvPixel.h>

#include <QStringList>

void djvPixelTest::run(int &, char **)
{
    DJV_DEBUG("djvPixelTest::run");
    
    mask();
    members();
    convert();
    operators();
}

void djvPixelTest::mask()
{
    DJV_DEBUG("djvPixelTest::mask");
    
    {
        const djvPixel::Mask mask;
        
        DJV_ASSERT(mask.mask[0]);
        DJV_ASSERT(mask.mask[1]);
        DJV_ASSERT(mask.mask[2]);
        DJV_ASSERT(mask.mask[3]);
    }
    
    {
        const djvPixel::Mask mask(false);
        
        DJV_ASSERT(! mask.mask[0]);
        DJV_ASSERT(! mask.mask[1]);
        DJV_ASSERT(! mask.mask[2]);
        DJV_ASSERT(! mask.mask[3]);
    }
    
    {
        const djvPixel::Mask mask(false, false, false, false);
        
        DJV_ASSERT(! mask.mask[0]);
        DJV_ASSERT(! mask.mask[1]);
        DJV_ASSERT(! mask.mask[2]);
        DJV_ASSERT(! mask.mask[3]);
    }
    
    {
        const djvPixel::Mask tmp(false, false, false, false);

        const djvPixel::Mask mask(tmp);
        
        DJV_ASSERT(tmp.mask[0] == mask.mask[0]);
        DJV_ASSERT(tmp.mask[1] == mask.mask[1]);
        DJV_ASSERT(tmp.mask[2] == mask.mask[2]);
        DJV_ASSERT(tmp.mask[3] == mask.mask[3]);
    }
    
    {
        const djvPixel::Mask tmp(false, false, false, false);

        djvPixel::Mask mask;
        
        mask = tmp;
        
        DJV_ASSERT(tmp.mask[0] == mask.mask[0]);
        DJV_ASSERT(tmp.mask[1] == mask.mask[1]);
        DJV_ASSERT(tmp.mask[2] == mask.mask[2]);
        DJV_ASSERT(tmp.mask[3] == mask.mask[3]);
    }
    
    {
        djvPixel::Mask mask;
        
        mask[1] = false;
        
        DJV_ASSERT(! mask[1]);
    }
    
    {
        const djvPixel::Mask
            a(false, false, false, false),
            b(false, false, false, false);
        
        DJV_ASSERT(a == b);
        DJV_ASSERT(a != djvPixel::Mask());
    }
    
    {
        const djvPixel::Mask tmp(true, false, true, false);
        
        djvPixel::Mask mask(tmp);
        
        QStringList data;
        
        data << mask;
        
        data >> mask;
        
        DJV_ASSERT(tmp == mask);
    }
}

void djvPixelTest::members()
{
    DJV_DEBUG("djvPixelTest::members");
    
    {
        DJV_ASSERT(djvPixel::RGB == djvPixel::format(djvPixel::RGB_U10));
        DJV_ASSERT(djvPixel::U10 == djvPixel::type(djvPixel::RGB_U10));
    }
    
    {
        djvPixel::FORMAT format = static_cast<djvPixel::FORMAT>(0);
        
        DJV_ASSERT(! djvPixel::format(0, format));
        
        for (int i = 0; i < djvPixel::channelsMax; ++i)
        {
            djvPixel::format(i + 1, format);
            
            DJV_ASSERT(static_cast<djvPixel::FORMAT>(i) == format);
        }
    }
    
    {
        djvPixel::TYPE type = static_cast<djvPixel::TYPE>(0);
        
        DJV_ASSERT(djvPixel::type(10, djvPixel::INTEGER, type));
        DJV_ASSERT(djvPixel::U10 == type);
        DJV_ASSERT(!djvPixel::type(10, djvPixel::FLOAT, type));
    }
    
    {
        djvPixel::TYPE type = static_cast<djvPixel::TYPE>(0);
        
        DJV_ASSERT(! djvPixel::type(0, djvPixel::INTEGER, type));
        
        djvPixel::type(8, djvPixel::INTEGER, type);
        
        DJV_ASSERT(djvPixel::U8 == type);
        
        djvPixel::type(10, djvPixel::INTEGER, type);
        
        DJV_ASSERT(djvPixel::U10 == type);
        
        djvPixel::type(16, djvPixel::INTEGER, type);
        
        DJV_ASSERT(djvPixel::U16 == type);
        
        DJV_ASSERT(! djvPixel::type(0, djvPixel::FLOAT, type));

        djvPixel::type(16, djvPixel::FLOAT, type);
        
        DJV_ASSERT(djvPixel::F16 == type);

        djvPixel::type(32, djvPixel::FLOAT, type);
        
        DJV_ASSERT(djvPixel::F32 == type);
    }
    
    {
        DJV_ASSERT(3 == djvPixel::channels(djvPixel::RGB));
        DJV_ASSERT(3 == djvPixel::channels(djvPixel::RGB_U10));
    }
    
    {
        DJV_ASSERT(2 == djvPixel::channelByteCount(djvPixel::RGBA_U16));
        DJV_ASSERT(8 == djvPixel::byteCount(djvPixel::RGBA_U16));
    }
    
    {
        DJV_ASSERT(16 == djvPixel::bitDepth(djvPixel::U16));
        DJV_ASSERT(16 == djvPixel::bitDepth(djvPixel::RGBA_U16));
    }
    
    {
        DJV_ASSERT(djvPixel::u16Max == djvPixel::max(djvPixel::RGBA_U16));
        DJV_ASSERT(1 == djvPixel::max(djvPixel::RGBA_F16));
    }
    
    {
        const djvPixel::FORMAT formatUndef = djvPixel::FORMAT_COUNT;
        const djvPixel::TYPE   typeUndef   = djvPixel::TYPE_COUNT;
        const djvPixel::PIXEL  pixelUndef  = static_cast<djvPixel::PIXEL>(0);
    
        const struct Data
        {
            djvPixel::FORMAT format;
            djvPixel::TYPE   type;
            djvPixel::PIXEL  result;
        }
            data [] =
        {
            { djvPixel::L,    djvPixel::U8,  djvPixel::L_U8     },
            { djvPixel::L,    djvPixel::U10, djvPixel::L_U16    },
            { djvPixel::L,    djvPixel::U16, djvPixel::L_U16    },
            { djvPixel::L,    djvPixel::F16, djvPixel::L_F16    },
            { djvPixel::L,    djvPixel::F32, djvPixel::L_F32    },
            { djvPixel::L,    typeUndef,     pixelUndef         },

            { djvPixel::LA,   djvPixel::U8,  djvPixel::LA_U8    },
            { djvPixel::LA,   djvPixel::U10, djvPixel::LA_U16   },
            { djvPixel::LA,   djvPixel::U16, djvPixel::LA_U16   },
            { djvPixel::LA,   djvPixel::F16, djvPixel::LA_F16   },
            { djvPixel::LA,   djvPixel::F32, djvPixel::LA_F32   },
            { djvPixel::LA,   typeUndef,     pixelUndef         },

            { djvPixel::RGB,  djvPixel::U8,  djvPixel::RGB_U8   },
            { djvPixel::RGB,  djvPixel::U10, djvPixel::RGB_U10  },
            { djvPixel::RGB,  djvPixel::U16, djvPixel::RGB_U16  },
            { djvPixel::RGB,  djvPixel::F16, djvPixel::RGB_F16  },
            { djvPixel::RGB,  djvPixel::F32, djvPixel::RGB_F32  },
            { djvPixel::RGB,  typeUndef,     pixelUndef         },

            { djvPixel::RGBA, djvPixel::U8,  djvPixel::RGBA_U8  },
            { djvPixel::RGBA, djvPixel::U10, djvPixel::RGBA_U16 },
            { djvPixel::RGBA, djvPixel::U16, djvPixel::RGBA_U16 },
            { djvPixel::RGBA, djvPixel::F16, djvPixel::RGBA_F16 },
            { djvPixel::RGBA, djvPixel::F32, djvPixel::RGBA_F32 },
            { djvPixel::RGBA, typeUndef,     pixelUndef         },
            
            { formatUndef,    djvPixel::U8,  pixelUndef         },
            { formatUndef,    djvPixel::U10, pixelUndef         },
            { formatUndef,    djvPixel::U16, pixelUndef         },
            { formatUndef,    djvPixel::F16, pixelUndef         },
            { formatUndef,    djvPixel::F32, pixelUndef         },
            
            { formatUndef,    typeUndef,     pixelUndef         }
        };
        
        const int dataCount = sizeof(data) / sizeof(data[0]);
        
        for (int i = 0; i < dataCount; ++i)
        {
            DJV_ASSERT(
                data[i].result ==
                djvPixel::pixel(data[i].format, data[i].type));
        }
    }
    
    {
        const djvPixel::FORMAT formatUndef = djvPixel::FORMAT_COUNT;
        const djvPixel::TYPE   typeUndef   = djvPixel::TYPE_COUNT;
        const djvPixel::PIXEL  pixelUndef  = static_cast<djvPixel::PIXEL>(0);
    
        const struct Data
        {
            djvPixel::FORMAT format;
            djvPixel::TYPE   type;
            djvPixel::PIXEL  result;
            bool             ok;
        }
            data [] =
        {
            { djvPixel::L,    djvPixel::U8,  djvPixel::L_U8,     true  },
            { djvPixel::L,    djvPixel::U10, pixelUndef,         false },
            { djvPixel::L,    djvPixel::U16, djvPixel::L_U16,    true  },
            { djvPixel::L,    djvPixel::F16, djvPixel::L_F16,    true  },
            { djvPixel::L,    djvPixel::F32, djvPixel::L_F32,    true  },
            { djvPixel::L,    typeUndef,     pixelUndef,         false },

            { djvPixel::LA,   djvPixel::U8,  djvPixel::LA_U8,    true  },
            { djvPixel::LA,   djvPixel::U10, pixelUndef,         false },
            { djvPixel::LA,   djvPixel::U16, djvPixel::LA_U16,   true  },
            { djvPixel::LA,   djvPixel::F16, djvPixel::LA_F16,   true  },
            { djvPixel::LA,   djvPixel::F32, djvPixel::LA_F32,   true  },
            { djvPixel::LA,   typeUndef,     pixelUndef,         false },

            { djvPixel::RGB,  djvPixel::U8,  djvPixel::RGB_U8,   true  },
            { djvPixel::RGB,  djvPixel::U10, djvPixel::RGB_U10,  true  },
            { djvPixel::RGB,  djvPixel::U16, djvPixel::RGB_U16,  true  },
            { djvPixel::RGB,  djvPixel::F16, djvPixel::RGB_F16,  true  },
            { djvPixel::RGB,  djvPixel::F32, djvPixel::RGB_F32,  true  },
            { djvPixel::RGB,  typeUndef,     pixelUndef,         false },

            { djvPixel::RGBA, djvPixel::U8,  djvPixel::RGBA_U8,  true  },
            { djvPixel::RGBA, djvPixel::U10, pixelUndef,         false },
            { djvPixel::RGBA, djvPixel::U16, djvPixel::RGBA_U16, true  },
            { djvPixel::RGBA, djvPixel::F16, djvPixel::RGBA_F16, true  },
            { djvPixel::RGBA, djvPixel::F32, djvPixel::RGBA_F32, true  },
            { djvPixel::RGBA, typeUndef,     pixelUndef,         false },
            
            { formatUndef,    djvPixel::U8,  pixelUndef,         false },
            { formatUndef,    djvPixel::U10, pixelUndef,         false },
            { formatUndef,    djvPixel::U16, pixelUndef,         false },
            { formatUndef,    djvPixel::F16, pixelUndef,         false },
            { formatUndef,    djvPixel::F32, pixelUndef,         false },

            { formatUndef,    typeUndef,     pixelUndef,         false }
        };
        
        const int dataCount = sizeof(data) / sizeof(data[0]);
        
        for (int i = 0; i < dataCount; ++i)
        {
            djvPixel::PIXEL result = pixelUndef;
            
            DJV_ASSERT(
                data[i].ok ==
                djvPixel::pixel(data[i].format, data[i].type, result));
            DJV_ASSERT(data[i].result == result);
        }
    }
    
    {
        djvPixel::PIXEL pixel = static_cast<djvPixel::PIXEL>(0);
        
        DJV_ASSERT(djvPixel::pixel(djvPixel::RGB, djvPixel::U10, pixel));
        DJV_ASSERT(djvPixel::RGB_U10 == pixel);

        DJV_ASSERT(! djvPixel::pixel(djvPixel::RGBA, djvPixel::U10, pixel));
    }
    
    {
        djvPixel::PIXEL pixel = static_cast<djvPixel::PIXEL>(0);
        
        DJV_ASSERT(djvPixel::pixel(3, 10, djvPixel::INTEGER, pixel));
        DJV_ASSERT(djvPixel::RGB_U10 == pixel);

        DJV_ASSERT(! djvPixel::pixel(3, 10, djvPixel::FLOAT, pixel));
        DJV_ASSERT(! djvPixel::pixel(4, 10, djvPixel::INTEGER, pixel));
    }
}
    
void djvPixelTest::convert()
{
    DJV_DEBUG("djvPixelTest::convert");
    
    {
        DJV_ASSERT(0                == djvPixel::u8ToU10(0));
        DJV_ASSERT(djvPixel::u10Max == djvPixel::u8ToU10(djvPixel::u8Max));
        DJV_ASSERT(0                == djvPixel::u8ToU16(0));
        DJV_ASSERT(djvPixel::u16Max == djvPixel::u8ToU16(djvPixel::u8Max));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0f, djvPixel::u8ToF16(0)));
        DJV_ASSERT(djvMath::fuzzyCompare(1.0f, djvPixel::u8ToF16(djvPixel::u8Max)));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0f, djvPixel::u8ToF32(0)));
        DJV_ASSERT(djvMath::fuzzyCompare(1.0f, djvPixel::u8ToF32(djvPixel::u8Max)));
    }
    
    {
        DJV_ASSERT(0                == djvPixel::u10ToU8(0));
        DJV_ASSERT(djvPixel::u8Max  == djvPixel::u10ToU8(djvPixel::u10Max));
        DJV_ASSERT(0                == djvPixel::u10ToU16(0));
        DJV_ASSERT(djvPixel::u16Max == djvPixel::u10ToU16(djvPixel::u10Max));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0f, djvPixel::u10ToF16(0)));
        DJV_ASSERT(djvMath::fuzzyCompare(1.0f, djvPixel::u10ToF16(djvPixel::u10Max)));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0f, djvPixel::u10ToF32(0)));
        DJV_ASSERT(djvMath::fuzzyCompare(1.0f, djvPixel::u10ToF32(djvPixel::u10Max)));
    }
    
    {
        DJV_ASSERT(0                == djvPixel::u16ToU8(0));
        DJV_ASSERT(djvPixel::u8Max  == djvPixel::u16ToU8(djvPixel::u16Max));
        DJV_ASSERT(0                == djvPixel::u16ToU10(0));
        DJV_ASSERT(djvPixel::u10Max == djvPixel::u16ToU10(djvPixel::u16Max));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0f, djvPixel::u16ToF16(0)));
        DJV_ASSERT(djvMath::fuzzyCompare(1.0f, djvPixel::u16ToF16(djvPixel::u16Max)));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0f, djvPixel::u16ToF32(0)));
        DJV_ASSERT(djvMath::fuzzyCompare(1.0f, djvPixel::u16ToF32(djvPixel::u16Max)));
    }
    
    {
        DJV_ASSERT(0                == djvPixel::f16ToU8(0.0f));
        DJV_ASSERT(djvPixel::u8Max  == djvPixel::f16ToU8(1.0f));
        DJV_ASSERT(0                == djvPixel::f16ToU10(0.0f));
        DJV_ASSERT(djvPixel::u10Max == djvPixel::f16ToU10(1.0f));
        DJV_ASSERT(0                == djvPixel::f16ToU16(0.0f));
        DJV_ASSERT(djvPixel::u16Max == djvPixel::f16ToU16(1.0f));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0f, djvPixel::f16ToF32(0.0f)));
        DJV_ASSERT(djvMath::fuzzyCompare(1.0f, djvPixel::f16ToF32(1.0f)));
    }
    
    {
        DJV_ASSERT(0                == djvPixel::f32ToU8(0.0f));
        DJV_ASSERT(djvPixel::u8Max  == djvPixel::f32ToU8(1.0f));
        DJV_ASSERT(0                == djvPixel::f32ToU10(0.0f));
        DJV_ASSERT(djvPixel::u10Max == djvPixel::f32ToU10(1.0f));
        DJV_ASSERT(0                == djvPixel::f32ToU16(0.0f));
        DJV_ASSERT(djvPixel::u16Max == djvPixel::f32ToU16(1.0f));
        DJV_ASSERT(djvMath::fuzzyCompare(0.0f, djvPixel::f32ToF16(0.0f)));
        DJV_ASSERT(djvMath::fuzzyCompare(1.0f, djvPixel::f32ToF16(1.0f)));
    }
    
    {
        for (int i = 0; i < djvPixel::PIXEL_COUNT; ++i)
        {
            for (int j = 0; j < djvPixel::PIXEL_COUNT; ++j)
            {
                djvColor
                    a(static_cast<djvPixel::PIXEL>(i)),
                    b(static_cast<djvPixel::PIXEL>(j));
                
                djvColorUtil::convert(djvColor(0.5), a);
                
                djvPixel::convert(
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

void djvPixelTest::operators()
{
    DJV_DEBUG("djvPixelTest::operators");

    {
        djvPixel::PIXEL pixel = djvPixel::L_F32;

        QStringList s;
        s << pixel;

        DJV_ASSERT(s.count() && s[0] == "L F32");

        s >> pixel;

        DJV_ASSERT(djvPixel::L_F32 == pixel);
    }
    
    {
        DJV_DEBUG_PRINT(djvPixel::RGBA);
        DJV_DEBUG_PRINT(djvPixel::F32);
        DJV_DEBUG_PRINT(djvPixel::FLOAT);
        DJV_DEBUG_PRINT(djvPixel::RGBA_F32);
    }
}


