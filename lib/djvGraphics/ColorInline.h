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

#include <djvCore/Assert.h>

namespace djv
{
    namespace Graphics
    {
        inline Pixel::PIXEL Color::pixel() const
        {
            return _pixel;
        }

        inline int Color::channels() const
        {
            return _channels;
        }

        inline quint8 * Color::data()
        {
            return _data;
        }

        inline const quint8 * Color::data() const
        {
            return _data;
        }

#define _COLOR_SET_WORK(TYPE) \
    switch (Pixel::type(_pixel)) \
    { \
        case Pixel::U8: \
            (reinterpret_cast<Pixel::U8_T *>(_data))[c] = PIXEL_##TYPE##_TO_U8(in); \
            break; \
        case Pixel::U10: \
        { \
            Pixel::U10_S * p = reinterpret_cast<Pixel::U10_S *>(_data); \
            switch (c) \
            { \
                case 0: p->r = PIXEL_##TYPE##_TO_U10(in); break; \
                case 1: p->g = PIXEL_##TYPE##_TO_U10(in); break; \
                case 2: p->b = PIXEL_##TYPE##_TO_U10(in); break; \
            } \
        } \
        break; \
        case Pixel::U16: \
            (reinterpret_cast<Pixel::U16_T *>(_data))[c] = PIXEL_##TYPE##_TO_U16(in); \
            break; \
        case Pixel::F16: \
            (reinterpret_cast<Pixel::F16_T *>(_data))[c] = PIXEL_##TYPE##_TO_F16(in); \
            break; \
        case Pixel::F32: \
            (reinterpret_cast<Pixel::F32_T *>(_data))[c] = PIXEL_##TYPE##_TO_F32(in); \
            break; \
        default: break; \
    }

#define _COLOR_SET(NAME, TYPE) \
    inline void Color::set##NAME(Pixel::TYPE##_T in, int c) \
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
            DJV_ASSERT(c >= 0 && c < Pixel::channels(_pixel)); \
            _COLOR_SET_WORK(TYPE) \
        } \
    }

        _COLOR_SET(U8, U8)
            _COLOR_SET(U10, U10)
            _COLOR_SET(U16, U16)
            _COLOR_SET(F16, F16)
            _COLOR_SET(F32, F32)

#define _COLOR_GET(NAME, TYPE) \
    inline Pixel::TYPE##_T Color::NAME(int c) const \
    { \
        DJV_ASSERT(c >= 0 && c < Pixel::channels(_pixel)); \
        switch (Pixel::type(_pixel)) \
        { \
            case Pixel::U8: \
                return PIXEL_U8_TO_##TYPE((reinterpret_cast<const Pixel::U8_T *>(_data))[c]); \
            case Pixel::U10: \
            { \
                const Pixel::U10_S * p = reinterpret_cast<const Pixel::U10_S *>(_data); \
                switch (c) \
                { \
                    case 0: return PIXEL_U10_TO_##TYPE(p->r); \
                    case 1: return PIXEL_U10_TO_##TYPE(p->g); \
                    case 2: return PIXEL_U10_TO_##TYPE(p->b); \
                } \
            } \
            break; \
            \
            case Pixel::U16: \
                return PIXEL_U16_TO_##TYPE((reinterpret_cast<const Pixel::U16_T *>(_data))[c]); \
            \
            case Pixel::F16: \
                return PIXEL_F16_TO_##TYPE((reinterpret_cast<const Pixel::F16_T *>(_data))[c]); \
            \
            case Pixel::F32: \
                return PIXEL_F32_TO_##TYPE((reinterpret_cast<const Pixel::F32_T *>(_data))[c]); \
            \
            default: break; \
        } \
        return 0; \
    }

            _COLOR_GET(u8, U8)
            _COLOR_GET(u10, U10)
            _COLOR_GET(u16, U16)
            _COLOR_GET(f16, F16)
            _COLOR_GET(f32, F32)

    } // namespace Graphics
} // namespace djv
