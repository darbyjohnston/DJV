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

//! \file djvColorInline.h

#include <djvAssert.h>

//------------------------------------------------------------------------------
// djvColor
//------------------------------------------------------------------------------

inline djvPixel::PIXEL djvColor::pixel() const
{
    return _pixel;
}

inline int djvColor::channels() const
{
    return _channels;
}

inline quint8 * djvColor::data()
{
    return _data;
}

inline const quint8 * djvColor::data() const
{
    return _data;
}

#define _COLOR_SET_WORK(TYPE) \
    \
    switch (djvPixel::type(_pixel)) \
    { \
        \
        case djvPixel::U8: \
            (reinterpret_cast<djvPixel::U8_T *>(_data))[c] = \
                PIXEL_##TYPE##_TO_U8(in); \
            break; \
        \
        case djvPixel::U10: \
        { \
            djvPixel::U10_S * p = reinterpret_cast<djvPixel::U10_S *>(_data); \
            switch (c) \
            { \
                case 0: p->r = PIXEL_##TYPE##_TO_U10(in); break; \
                case 1: p->g = PIXEL_##TYPE##_TO_U10(in); break; \
                case 2: p->b = PIXEL_##TYPE##_TO_U10(in); break; \
            } \
        } \
        break; \
        \
        case djvPixel::U16: \
            (reinterpret_cast<djvPixel::U16_T *>(_data))[c] = \
                PIXEL_##TYPE##_TO_U16(in); \
            break; \
        \
        case djvPixel::F16: \
            (reinterpret_cast<djvPixel::F16_T *>(_data))[c] = \
                PIXEL_##TYPE##_TO_F16(in); \
            break; \
        \
        case djvPixel::F32: \
            (reinterpret_cast<djvPixel::F32_T *>(_data))[c] = \
                PIXEL_##TYPE##_TO_F32(in); \
            break; \
        \
        default: break; \
    }

#define _COLOR_SET(NAME, TYPE) \
    \
    inline void djvColor::set##NAME(djvPixel::TYPE##_T in, int c) \
    { \
        if (-1 == c) \
        { \
            for (c = 0; c < _channels; ++c) \
            { \
                _COLOR_SET_WORK(TYPE) \
            } \
        } \
        else \
        { \
            DJV_ASSERT(c >= 0 && c < djvPixel::channels(_pixel)); \
            \
            _COLOR_SET_WORK(TYPE) \
        } \
    }

_COLOR_SET( U8,  U8)
_COLOR_SET(U10, U10)
_COLOR_SET(U16, U16)
_COLOR_SET(F16, F16)
_COLOR_SET(F32, F32)

#define _COLOR_GET(NAME, TYPE) \
    \
    inline djvPixel::TYPE##_T djvColor::NAME(int c) const \
    { \
        DJV_ASSERT(c >= 0 && c < djvPixel::channels(_pixel)); \
        \
        switch (djvPixel::type(_pixel)) \
        { \
            \
            case djvPixel::U8: \
                return PIXEL_U8_TO_##TYPE( \
                    (reinterpret_cast<const djvPixel::U8_T *>(_data))[c]); \
            \
            case djvPixel::U10: \
            { \
                const djvPixel::U10_S * p = \
                    reinterpret_cast<const djvPixel::U10_S *>(_data); \
                switch (c) \
                { \
                    case 0: return PIXEL_U10_TO_##TYPE(p->r); \
                    case 1: return PIXEL_U10_TO_##TYPE(p->g); \
                    case 2: return PIXEL_U10_TO_##TYPE(p->b); \
                } \
            } \
            break; \
            \
            case djvPixel::U16: \
                return PIXEL_U16_TO_##TYPE( \
                    (reinterpret_cast<const djvPixel::U16_T *>(_data))[c]); \
            \
            case djvPixel::F16: \
                return PIXEL_F16_TO_##TYPE( \
                    (reinterpret_cast<const djvPixel::F16_T *>(_data))[c]); \
            \
            case djvPixel::F32: \
                return PIXEL_F32_TO_##TYPE( \
                    (reinterpret_cast<const djvPixel::F32_T *>(_data))[c]); \
            \
            default: break; \
        } \
        \
        return 0; \
    }

_COLOR_GET( u8,  U8)
_COLOR_GET(u10, U10)
_COLOR_GET(u16, U16)
_COLOR_GET(f16, F16)
_COLOR_GET(f32, F32)
