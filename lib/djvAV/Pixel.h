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

#include <djvAV/OpenGL.h>

#include <OpenEXR/half.h>

#include <limits>

namespace djv
{
    namespace AV
    {
        namespace Image
        {
            typedef uint8_t   U8_T;
            typedef uint16_t U10_T;
            typedef uint16_t U16_T;
            typedef uint32_t U32_T;
            typedef half     F16_T;
            typedef float    F32_T;

            const U8_T   U8Min = std::numeric_limits<uint8_t>::min();
            const U8_T   U8Max = std::numeric_limits<uint8_t>::max();
            const U10_T U10Min = 0;
            const U10_T U10Max = 1023;
            const U16_T U16Min = std::numeric_limits<uint16_t>::min();
            const U16_T U16Max = std::numeric_limits<uint16_t>::max();
            const U32_T U32Min = std::numeric_limits<uint32_t>::min();
            const U32_T U32Max = std::numeric_limits<uint32_t>::max();
            const F16_T F16Min = 0.f;
            const F16_T F16Max = 1.f;
            const F32_T F32Min = 0.f;
            const F32_T F32Max = 1.f;

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

            class Pixel
            {
            public:
                inline Pixel();
                inline Pixel(GLenum format, GLenum type);

                inline GLenum getFormat() const;
                inline GLenum getType() const;
                inline bool isValid() const;

                inline size_t getChannelCount() const;
                inline size_t getBitDepth() const;
                inline size_t getByteCount() const;

                inline bool operator == (const Pixel &) const;
                inline bool operator != (const Pixel &) const;
                inline bool operator < (const Pixel &) const;

            private:
                GLenum _format = GL_NONE;
                GLenum _type = GL_NONE;
            };

            inline Pixel getIntPixel(int channels, int bitDepth);
            inline Pixel getFloatPixel(int channels, int bitDepth);

            inline void U8ToU10(U8_T, U10_T &);
            inline void U8ToU16(U8_T, U16_T &);
            inline void U8ToU32(U8_T, U32_T &);
            inline void U8ToF16(U8_T, F16_T &);
            inline void U8ToF32(U8_T, F32_T &);

            inline void U16ToU8(U16_T, U8_T &);
            inline void U16ToU10(U16_T, U10_T &);
            inline void U16ToU32(U16_T, U32_T &);
            inline void U16ToF16(U16_T, F16_T &);
            inline void U16ToF32(U16_T, F32_T &);

            inline void U10ToU8(U16_T, U8_T &);
            inline void U10ToU16(U16_T, U10_T &);
            inline void U10ToU32(U16_T, U32_T &);
            inline void U10ToF16(U16_T, F16_T &);
            inline void U10ToF32(U16_T, F32_T &);

            inline void U32ToU8(U32_T, U8_T &);
            inline void U32ToU10(U32_T, U10_T &);
            inline void U32ToU16(U32_T, U16_T &);
            inline void U32ToF16(U32_T, F16_T &);
            inline void U32ToF32(U32_T, F32_T &);

            inline void F16ToU8(F16_T, U8_T &);
            inline void F16ToU10(F16_T, U10_T &);
            inline void F16ToU16(F16_T, U16_T &);
            inline void F16ToU32(F16_T, U32_T &);
            inline void F16ToF32(F16_T, F32_T &);

            inline void F32ToU8(F32_T, U8_T &);
            inline void F32ToU10(F32_T, U10_T &);
            inline void F32ToU16(F32_T, U16_T &);
            inline void F32ToU32(F32_T, U32_T &);
            inline void F32ToF16(F32_T, F16_T &);

            void convert(const Pixel &, const void *, const Pixel &, void *);

        } // namespace Image
    } // namespace AV

    std::ostream & operator << (std::ostream &, AV::Image::Pixel);
    std::istream & operator >> (std::istream &, AV::Image::Pixel &);

} // namespace djv

#include <djvAV/PixelInline.h>
