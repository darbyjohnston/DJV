// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvImage/Pixel.h>

#include <djvCore/Enum.h>
#include <djvCore/RapidJSONFunc.h>

#include <sstream>
#include <vector>

namespace djv
{
    namespace Image
    {
        Channels getChannels(Type) noexcept;
        uint8_t getChannelCount(Type) noexcept;
        DataType getDataType(Type) noexcept;
        uint8_t getBitDepth(Type) noexcept;
        uint8_t getBitDepth(DataType) noexcept;
        size_t getByteCount(Type) noexcept;
        size_t getByteCount(DataType) noexcept;

        bool isIntType(Type) noexcept;
        bool isFloatType(Type) noexcept;
        Math::IntRange getIntRange(Type);
        Math::FloatRange getFloatRange(Type);
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

        DJV_ENUM_HELPERS(Type);
        DJV_ENUM_HELPERS(Channels);
        DJV_ENUM_HELPERS(DataType);

    } // namespace Image

    DJV_ENUM_SERIALIZE_HELPERS(Image::Type);
    DJV_ENUM_SERIALIZE_HELPERS(Image::Channels);
    DJV_ENUM_SERIALIZE_HELPERS(Image::DataType);

    rapidjson::Value toJSON(Image::Type, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Image::Type&);

} // namespace djv

namespace std
{
    template<>
    struct hash<djv::Image::Type>
    {
        std::size_t operator() (djv::Image::Type) const noexcept;
    };

} // namespace std

#include <djvImage/PixelFuncInline.h>

