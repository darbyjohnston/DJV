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

#include <djvCore/Math.h>

namespace djv
{
    namespace AV
    {
        namespace Image
        {
            inline bool U10_S_MSB::operator == (const U10_S_MSB& value) const
            {
                return
                    value.r == r &&
                    value.g == g &&
                    value.b == b;
            }

            inline bool U10_S_MSB::operator != (const U10_S_MSB& value) const
            {
                return !(*this == value);
            }

            inline bool U10_S_LSB::operator == (const U10_S_LSB& value) const
            {
                return
                    value.r == r &&
                    value.g == g &&
                    value.b == b;
            }

            inline bool U10_S_LSB::operator != (const U10_S_LSB& value) const
            {
                return !(*this == value);
            }

            inline GLenum getFormat(Pixel value)
            {
                static const std::vector<GLenum> data =
                {
                    GL_NONE,

                    GL_RED,
                    GL_RED,
                    GL_RED,
                    GL_RED,
                    GL_RED,

                    GL_RG,
                    GL_RG,
                    GL_RG,
                    GL_RG,
                    GL_RG,

                    GL_RGB,
                    GL_RGBA,
                    GL_RGB,
                    GL_RGB,
                    GL_RGB,
                    GL_RGB,

                    GL_RGBA,
                    GL_RGBA,
                    GL_RGBA,
                    GL_RGBA,
                    GL_RGBA
                };
                DJV_ASSERT(data.size() == static_cast<size_t>(Pixel::Count));
                return data[static_cast<size_t>(value)];
            }

            inline GLenum getType(Pixel value)
            {
                static const std::vector<GLenum> data =
                {
                    GL_NONE,

                    GL_UNSIGNED_BYTE,
                    GL_UNSIGNED_SHORT,
                    GL_UNSIGNED_INT,
                    GL_HALF_FLOAT,
                    GL_FLOAT,

                    GL_UNSIGNED_BYTE,
                    GL_UNSIGNED_SHORT,
                    GL_UNSIGNED_INT,
                    GL_HALF_FLOAT,
                    GL_FLOAT,

                    GL_UNSIGNED_BYTE,
                    GL_UNSIGNED_INT_10_10_10_2,
                    GL_UNSIGNED_SHORT,
                    GL_UNSIGNED_INT,
                    GL_HALF_FLOAT,
                    GL_FLOAT,

                    GL_UNSIGNED_BYTE,
                    GL_UNSIGNED_SHORT,
                    GL_UNSIGNED_INT,
                    GL_HALF_FLOAT,
                    GL_FLOAT
                };
                DJV_ASSERT(data.size() == static_cast<size_t>(Pixel::Count));
                return data[static_cast<size_t>(value)];
            }

            inline size_t getChannelCount(Pixel value)
            {
                static const std::vector<GLenum> data =
                {
                    0,
                    1, 1, 1, 1, 1,
                    2, 2, 2, 2, 2,
                    3, 3, 3, 3, 3, 3,
                    4, 4, 4, 4, 4
                };
                DJV_ASSERT(data.size() == static_cast<size_t>(Pixel::Count));
                return data[static_cast<size_t>(value)];
            }

            inline size_t getBitDepth(Pixel value)
            {
                static const std::vector<GLenum> data =
                {
                    0,
                    8, 16, 32, 16, 32,
                    8, 16, 32, 16, 32,
                    8, 10, 16, 32, 16, 32,
                    8, 16, 32, 16, 32
                };
                DJV_ASSERT(data.size() == static_cast<size_t>(Pixel::Count));
                return data[static_cast<size_t>(value)];
            }

            inline size_t getByteCount(Pixel value)
            {
                static const std::vector<GLenum> data =
                {
                    0,
                    1, 2, 4, 2, 4,
                    2, 4, 8, 4, 8,
                    3, 4, 6, 12, 6, 12,
                    4, 8, 16, 8, 16
                };
                DJV_ASSERT(data.size() == static_cast<size_t>(Pixel::Count));
                return data[static_cast<size_t>(value)];
            }

            inline Pixel getIntPixel(int channelCount, int bitDepth)
            {
                switch (channelCount)
                {
                case 1:
                    switch (bitDepth)
                    {
                    case  8: return Pixel::L_U8;
                    case 16: return Pixel::L_U16;
                    case 32: return Pixel::L_U32;
                    }
                    break;
                case 2:
                    switch (bitDepth)
                    {
                    case  8: return Pixel::LA_U8;
                    case 16: return Pixel::LA_U16;
                    case 32: return Pixel::LA_U32;
                    }
                    break;
                case 3:
                    switch (bitDepth)
                    {
                    case  8: return Pixel::RGB_U8;
                    case 10: return Pixel::RGB_U10;
                    case 16: return Pixel::RGB_U16;
                    case 32: return Pixel::RGB_U32;
                    }
                    break;
                case 4:
                    switch (bitDepth)
                    {
                    case  8: return Pixel::RGBA_U8;
                    case 16: return Pixel::RGBA_U16;
                    case 32: return Pixel::RGBA_U32;
                    }
                    break;
                }
                return Pixel();
            }

            inline Pixel getFloatPixel(int channelCount, int bitDepth)
            {
                switch (channelCount)
                {
                case 1:
                    switch (bitDepth)
                    {
                    case 16: return Pixel::L_F16;
                    case 32: return Pixel::L_F32;
                    }
                    break;
                case 2:
                    switch (bitDepth)
                    {
                    case 16: return Pixel::LA_F16;
                    case 32: return Pixel::LA_F32;
                    }
                    break;
                case 3:
                    switch (bitDepth)
                    {
                    case 16: return Pixel::RGB_F16;
                    case 32: return Pixel::RGB_F32;
                    }
                    break;
                case 4:
                    switch (bitDepth)
                    {
                    case 16: return Pixel::RGBA_F16;
                    case 32: return Pixel::RGBA_F32;
                    }
                    break;
                }
                return Pixel();
            }

            inline void U8ToU10(U8_T in, U10_T & out)
            {
                out = in << 2;
            }

            inline void U8ToU16(U8_T in, U16_T & out)
            {
                out = in << 8;
            }

            inline void U8ToU32(U8_T in, U32_T & out)
            {
                out = in << 24;
            }

            inline void U8ToF16(U8_T in, F16_T & out)
            {
                out = in / static_cast<float>(U8Max);
            }

            inline void U8ToF32(U8_T in, F32_T & out)
            {
                out = in / static_cast<float>(U8Max);
            }

            inline void U10ToU8(U10_T in, U8_T & out)
            {
                out = in >> 2;
            }

            inline void U10ToU16(U10_T in, U16_T & out)
            {
                out = in << 6;
            }

            inline void U10ToU32(U10_T in, U32_T & out)
            {
                out = in << 22;
            }

            inline void U10ToF16(U10_T in, F16_T & out)
            {
                out = in / static_cast<float>(U10Max);
            }

            inline void U10ToF32(U10_T in, F32_T & out)
            {
                out = in / static_cast<float>(U10Max);
            }

            inline void U16ToU8(U16_T in, U8_T & out)
            {
                out = in >> 8;
            }

            inline void U16ToU10(U16_T in, U10_T & out)
            {
                out = in >> 6;
            }

            inline void U16ToU32(U16_T in, U32_T & out)
            {
                out = in << 16;
            }

            inline void U16ToF16(U16_T in, F16_T & out)
            {
                out = in / static_cast<float>(U16Max);
            }

            inline void U16ToF32(U16_T in, F32_T & out)
            {
                out = in / static_cast<float>(U16Max);
            }

            inline void U32ToU8(U32_T in, U8_T & out)
            {
                out = in >> 24;
            }

            inline void U32ToU10(U32_T in, U10_T & out)
            {
                out = in >> 22;
            }

            inline void U32ToU16(U32_T in, U16_T & out)
            {
                out = in >> 16;
            }

            inline void U32ToF16(U32_T in, F16_T & out)
            {
                out = in / static_cast<float>(U32Max);
            }

            inline void U32ToF32(U32_T in, F32_T & out)
            {
                out = in / static_cast<float>(U32Max);
            }

            inline void F16ToU8(F16_T in, U8_T & out)
            {
                out = static_cast<U8_T>(Core::Math::clamp(
                    static_cast<uint16_t>(in * U8Max),
                    static_cast<uint16_t>(U8Min),
                    static_cast<uint16_t>(U8Max)));
            }

            inline void F16ToU10(F16_T in, U10_T & out)
            {
                out = static_cast<U10_T>(Core::Math::clamp(
                    static_cast<uint16_t>(in * U10Max),
                    static_cast<uint16_t>(U10Min),
                    static_cast<uint16_t>(U10Max)));
            }

            inline void F16ToU16(F16_T in, U16_T & out)
            {
                out = static_cast<U16_T>(Core::Math::clamp(
                    static_cast<uint32_t>(in * U16Max),
                    static_cast<uint32_t>(U16Min),
                    static_cast<uint32_t>(U16Max)));
            }

            inline void F16ToU32(F16_T in, U32_T & out)
            {
                out = static_cast<U32_T>(Core::Math::clamp(
                    static_cast<uint64_t>(in * U32Max),
                    static_cast<uint64_t>(U32Min),
                    static_cast<uint64_t>(U32Max)));
            }

            inline void F16ToF32(F16_T in, F32_T & out)
            {
                out = in;
            }

            inline void F32ToU8(F32_T in, U8_T & out)
            {
                out = static_cast<U8_T>(Core::Math::clamp(
                    static_cast<uint16_t>(in * U8Max),
                    static_cast<uint16_t>(U8Min),
                    static_cast<uint16_t>(U8Max)));
            }

            inline void F32ToU10(F32_T in, U10_T & out)
            {
                out = static_cast<U10_T>(Core::Math::clamp(
                    static_cast<uint16_t>(in * U10Max),
                    static_cast<uint16_t>(U10Min),
                    static_cast<uint16_t>(U10Max)));
            }

            inline void F32ToU16(F32_T in, U16_T & out)
            {
                out = static_cast<U16_T>(Core::Math::clamp(
                    static_cast<uint32_t>(in * U16Max),
                    static_cast<uint32_t>(U16Min),
                    static_cast<uint32_t>(U16Max)));
            }

            inline void F32ToU32(F32_T in, U32_T & out)
            {
                out = static_cast<U32_T>(Core::Math::clamp(
                    static_cast<uint64_t>(in * U32Max),
                    static_cast<uint64_t>(U32Min),
                    static_cast<uint64_t>(U32Max)));
            }

            inline void F32ToF16(F32_T in, F16_T & out)
            {
                out = in;
            }

        } // namespace Image
    } // namespace AV
} // namespace djv
