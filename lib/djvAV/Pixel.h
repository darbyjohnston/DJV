// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/OpenGL.h>

#include <djvCore/Enum.h>
#include <djvCore/RapidJSONFunc.h>
#include <djvCore/Range.h>

#include <OpenEXR/half.h>

#include <limits>
#include <sstream>
#include <vector>

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

            //! This enumeration provides the image channels.
            enum class Channels
            {
                None,
                L,
                LA,
                RGB,
                RGBA,

                Count,
                First = None
            };
            DJV_ENUM_HELPERS(Channels);

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
            typedef uint16_t U12_T;
            typedef uint16_t U16_T;
            typedef uint32_t U32_T;
            typedef half     F16_T;
            typedef float    F32_T;

            const Core::Math::Range<U8_T> U8Range(
                std::numeric_limits<U8_T>::min(),
                std::numeric_limits<U8_T>::max());
            
            const Core::Math::Range<U10_T> U10Range(0, 1023);
            
            const Core::Math::Range<U12_T> U12Range(0, 4095);
            
            const Core::Math::Range<U16_T> U16Range(
                std::numeric_limits<U16_T>::min(),
                std::numeric_limits<U16_T>::max());
            
            const Core::Math::Range<U32_T> U32Range(
                std::numeric_limits<U32_T>::min(),
                std::numeric_limits<U32_T>::max());
            
            const Core::Math::Range<F16_T> F16Range(0.F, 1.F);
            
            const Core::Math::Range<F32_T> F32Range(0.F, 1.F);

            //! This struct provides 10-bit MSB pixel data.
            struct U10_S_MSB
            {
                uint32_t r   : 10;
                uint32_t g   : 10;
                uint32_t b   : 10;
                uint32_t pad : 2;

                constexpr bool operator == (const U10_S_MSB&) const noexcept;
                constexpr bool operator != (const U10_S_MSB&) const noexcept;
            };

            //! This struct provides 10-bit LSB pixel data.
            struct U10_S_LSB
            {
                uint32_t pad : 2;
                uint32_t b   : 10;
                uint32_t g   : 10;
                uint32_t r   : 10;

                constexpr bool operator == (const U10_S_LSB&) const noexcept;
                constexpr bool operator != (const U10_S_LSB&) const noexcept;
            };
#if defined(DJV_ENDIAN_MSB)
            typedef U10_S_MSB U10_S;
#else
            typedef U10_S_LSB U10_S;
#endif

            Channels getChannels(Type) noexcept;
            uint8_t getChannelCount(Type) noexcept;
            DataType getDataType(Type) noexcept;
            uint8_t getBitDepth(Type) noexcept;
            uint8_t getBitDepth(DataType) noexcept;
            size_t getByteCount(Type) noexcept;
            size_t getByteCount(DataType) noexcept;

            bool isIntType(Type) noexcept;
            bool isFloatType(Type) noexcept;
            Core::IntRange getIntRange(Type);
            Core::FloatRange getFloatRange(Type);
            Type getIntType(uint8_t channelCount, uint8_t bitDepth) noexcept;
            Type getFloatType(uint8_t channelCount, uint8_t bitDepth) noexcept;

            GLenum getGLFormat(Type) noexcept;
            GLenum getGLType(Type) noexcept;

            void convert_U8_U8(U8_T, U8_T&);
            void convert_U8_U10(U8_T, U10_T&);
            void convert_U8_U16(U8_T, U16_T&);
            void convert_U8_U32(U8_T, U32_T&);
            void convert_U8_F16(U8_T, F16_T&);
            void convert_U8_F32(U8_T, F32_T&);

            void convert_U16_U8(U16_T, U8_T&);
            void convert_U16_U10(U16_T, U10_T&);
            void convert_U16_U16(U16_T, U16_T&);
            void convert_U16_U32(U16_T, U32_T&);
            void convert_U16_F16(U16_T, F16_T&);
            void convert_U16_F32(U16_T, F32_T&);

            void convert_U10_U8(U16_T, U8_T&);
            void convert_U10_U10(U10_T, U10_T&);
            void convert_U10_U16(U16_T, U10_T&);
            void convert_U10_U32(U16_T, U32_T&);
            void convert_U10_F16(U16_T, F16_T&);
            void convert_U10_F32(U16_T, F32_T&);

            void convert_U32_U8(U32_T, U8_T&);
            void convert_U32_U10(U32_T, U10_T&);
            void convert_U32_U16(U32_T, U16_T&);
            void convert_U32_U32(U32_T, U32_T&);
            void convert_U32_F16(U32_T, F16_T&);
            void convert_U32_F32(U32_T, F32_T&);

            void convert_F16_U8(F16_T, U8_T&);
            void convert_F16_U10(F16_T, U10_T&);
            void convert_F16_U16(F16_T, U16_T&);
            void convert_F16_U32(F16_T, U32_T&);
            void convert_F16_F16(F16_T, F16_T&);
            void convert_F16_F32(F16_T, F32_T&);

            void convert_F32_U8(F32_T, U8_T&);
            void convert_F32_U10(F32_T, U10_T&);
            void convert_F32_U16(F32_T, U16_T&);
            void convert_F32_U32(F32_T, U32_T&);
            void convert_F32_F16(F32_T, F16_T&);
            void convert_F32_F32(F32_T, F32_T&);

            void convert(const void *, Type, void *, Type, size_t);

        } // namespace Image
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS(AV::Image::Type);
    DJV_ENUM_SERIALIZE_HELPERS(AV::Image::Channels);
    DJV_ENUM_SERIALIZE_HELPERS(AV::Image::DataType);

    rapidjson::Value toJSON(AV::Image::Type, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, AV::Image::Type&);

} // namespace djv

namespace std
{
    template<>
    struct hash<djv::AV::Image::Type>
    {
        std::size_t operator() (djv::AV::Image::Type) const noexcept;
    };

} // namespace std

#include <djvAV/PixelInline.h>
