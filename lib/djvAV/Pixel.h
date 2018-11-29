//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#pragma once

#include <djvCore/Util.h>

#include <QMetaType>
#include <QOpenGLFunctions_3_3_Core>

#include <OpenEXR/half.h>

namespace djv
{
    namespace AV
    {
        class Pixel
        {
            Q_GADGET

        public:
            typedef uint8_t   U8_T;
            typedef uint16_t U10_T;
            typedef uint16_t U16_T;
            typedef uint32_t U32_T;
            typedef half     F16_T;
            typedef float    F32_T;

            static const U8_T   u8Min;
            static const U8_T   u8Max;
            static const U10_T u10Min;
            static const U10_T u10Max;
            static const U16_T u16Min;
            static const U16_T u16Max;
            static const U32_T u32Min;
            static const U32_T u32Max;
            static const F16_T f16Min;
            static const F16_T f16Max;
            static const F32_T f32Min;
            static const F32_T f32Max;

            struct U10_S_MSB
            {
                uint32_t r : 10, g : 10, b : 10, pad : 2;

                inline bool operator == (const U10_S_MSB &) const;
                inline bool operator != (const U10_S_MSB &) const;
            };
            struct U10_S_LSB
            {
                uint32_t pad : 2, b : 10, g : 10, r : 10;

                inline bool operator == (const U10_S_LSB &) const;
                inline bool operator != (const U10_S_LSB &) const;
            };
#if defined(DJV_ENDIAN_MSB)
            typedef U10_S_MSB U10_S;
#else
            typedef U10_S_LSB U10_S;
#endif

            inline Pixel();
            inline Pixel(GLenum format, GLenum type);

            inline GLenum getFormat() const;
            inline GLenum getType() const;
            inline bool isValid() const;

            inline size_t getChannelCount() const;
            inline size_t getBitDepth() const;
            inline size_t getByteCount() const;

            static inline Pixel getIntPixel(int channels, int bitDepth);
            static inline Pixel getFloatPixel(int channels, int bitDepth);

            static QString getPixelLabel(const Pixel &);

            static inline void U8ToU10(U8_T, U10_T &);
            static inline void U8ToU16(U8_T, U16_T &);
            static inline void U8ToU32(U8_T, U32_T &);
            static inline void U8ToF16(U8_T, F16_T &);
            static inline void U8ToF32(U8_T, F32_T &);

            static inline void U16ToU8(U16_T, U8_T &);
            static inline void U16ToU10(U16_T, U10_T &);
            static inline void U16ToU32(U16_T, U32_T &);
            static inline void U16ToF16(U16_T, F16_T &);
            static inline void U16ToF32(U16_T, F32_T &);

            static inline void U10ToU8(U16_T, U8_T &);
            static inline void U10ToU16(U16_T, U10_T &);
            static inline void U10ToU32(U16_T, U32_T &);
            static inline void U10ToF16(U16_T, F16_T &);
            static inline void U10ToF32(U16_T, F32_T &);

            static inline void U32ToU8(U32_T, U8_T &);
            static inline void U32ToU10(U32_T, U10_T &);
            static inline void U32ToU16(U32_T, U16_T &);
            static inline void U32ToF16(U32_T, F16_T &);
            static inline void U32ToF32(U32_T, F32_T &);

            static inline void F32ToU8(F32_T, U8_T &);
            static inline void F32ToU10(F32_T, U10_T &);
            static inline void F32ToU16(F32_T, U16_T &);
            static inline void F32ToU32(F32_T, U32_T &);
            static inline void F32ToF16(F32_T, F16_T &);

            static std::vector<uint8_t> convert(const std::vector<uint8_t> &, const Pixel &, const Pixel &);

            inline bool operator == (const Pixel &) const;
            inline bool operator != (const Pixel &) const;
            inline bool operator < (const Pixel &) const;

        private:
            GLenum _format = 0;
            GLenum _type   = 0;
        };

    } // namespace AV
} // namespace djv

#include <djvAV/PixelInline.h>
