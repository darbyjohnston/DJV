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

#pragma once

#include <djvAV/OpenGL.h>

#include <djvCore/Enum.h>
#include <djvCore/Range.h>

#include <OpenEXR/half.h>

#include <limits>

namespace djv
{
    namespace AV
    {
        //! This namespace provides image functionality.
        namespace Image
        {
            //! This enumeration provides the image types.
            enum class Type
            {
                None,

                L_U8,
                L_U16,
                L_U32,
                L_F16,
                L_F32,
                
                LA_U8,
                LA_U16,
                LA_U32,
                LA_F16,
                LA_F32,
                
                RGB_U8,
                RGB_U10,
                RGB_U16,
                RGB_U32,
                RGB_F16,
                RGB_F32,

                RGBA_U8,
                RGBA_U16,
                RGBA_U32,
                RGBA_F16,
                RGBA_F32,

                Count,
                First = None
            };
            DJV_ENUM_HELPERS(Type);

            //! This enumeration provides the image channel types.
            enum class ChannelType
            {
                None,
                L,
                LA,
                RGB,
                RGBA,

                Count,
                First = None
            };
            DJV_ENUM_HELPERS(ChannelType);

            //! This enumeration provides the image data types.
            enum class DataType
            {
                None,
                U8,
                U10,
                U16,
                U32,
                F16,
                F32,

                Count,
                First = None
            };
            DJV_ENUM_HELPERS(DataType);

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

            //! This struct provides 10-bit MSB pixel data.
            struct U10_S_MSB
            {
                uint32_t r : 10, g : 10, b : 10, pad : 2;

                constexpr bool operator == (const U10_S_MSB &) const;
                constexpr bool operator != (const U10_S_MSB &) const;
            };
            //! This struct provides 10-bit LSB pixel data.
            struct U10_S_LSB
            {
                uint32_t pad : 2, b : 10, g : 10, r : 10;

                constexpr bool operator == (const U10_S_LSB &) const;
                constexpr bool operator != (const U10_S_LSB &) const;
            };
#if defined(DJV_ENDIAN_MSB)
            typedef U10_S_MSB U10_S;
#else
            typedef U10_S_LSB U10_S;
#endif

            inline ChannelType getChannelType(Type);
            inline uint8_t getChannelCount(Type);
            inline DataType getDataType(Type);
            inline uint8_t getBitDepth(Type);
            inline uint8_t getByteCount(Type);

            inline bool isIntType(Type);
            inline bool isFloatType(Type);
            inline Core::IntRange getIntRange(Type);
            inline Core::FloatRange getFloatRange(Type);
            inline Type getIntType(uint8_t channelCount, uint8_t bitDepth);
            inline Type getFloatType(uint8_t channelCount, uint8_t bitDepth);

            inline GLenum getGLFormat(Type);
            inline GLenum getGLType(Type);

            inline void convert_U8_U8(U8_T, U8_T &);
            inline void convert_U8_U10(U8_T, U10_T &);
            inline void convert_U8_U16(U8_T, U16_T &);
            inline void convert_U8_U32(U8_T, U32_T &);
            inline void convert_U8_F16(U8_T, F16_T &);
            inline void convert_U8_F32(U8_T, F32_T &);

            inline void convert_U16_U8(U16_T, U8_T &);
            inline void convert_U16_U10(U16_T, U10_T &);
            inline void convert_U16_U16(U16_T, U16_T &);
            inline void convert_U16_U32(U16_T, U32_T &);
            inline void convert_U16_F16(U16_T, F16_T &);
            inline void convert_U16_F32(U16_T, F32_T &);

            inline void convert_U10_U8(U16_T, U8_T &);
            inline void convert_U10_U10(U10_T, U10_T &);
            inline void convert_U10_U16(U16_T, U10_T &);
            inline void convert_U10_U32(U16_T, U32_T &);
            inline void convert_U10_F16(U16_T, F16_T &);
            inline void convert_U10_F32(U16_T, F32_T &);

            inline void convert_U32_U8(U32_T, U8_T &);
            inline void convert_U32_U10(U32_T, U10_T &);
            inline void convert_U32_U16(U32_T, U16_T &);
            inline void convert_U32_U32(U32_T, U32_T &);
            inline void convert_U32_F16(U32_T, F16_T &);
            inline void convert_U32_F32(U32_T, F32_T &);

            inline void convert_F16_U8(F16_T, U8_T &);
            inline void convert_F16_U10(F16_T, U10_T &);
            inline void convert_F16_U16(F16_T, U16_T &);
            inline void convert_F16_U32(F16_T, U32_T &);
            inline void convert_F16_F16(F16_T, F16_T &);
            inline void convert_F16_F32(F16_T, F32_T &);

            inline void convert_F32_U8(F32_T, U8_T &);
            inline void convert_F32_U10(F32_T, U10_T &);
            inline void convert_F32_U16(F32_T, U16_T &);
            inline void convert_F32_U32(F32_T, U32_T &);
            inline void convert_F32_F16(F32_T, F16_T &);
            inline void convert_F32_F32(F32_T, F32_T &);

            void convert(const void *, Type, void *, Type, size_t);

        } // namespace Image
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS(AV::Image::Type);
    DJV_ENUM_SERIALIZE_HELPERS(AV::Image::ChannelType);
    DJV_ENUM_SERIALIZE_HELPERS(AV::Image::DataType);

} // namespace djv

namespace std
{
    template<>
    struct hash<djv::AV::Image::Type>
    {
        inline std::size_t operator() (djv::AV::Image::Type) const noexcept;
    };

} // namespace std

#include <djvAV/PixelInline.h>
