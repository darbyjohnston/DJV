// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvMath/Range.h>

#include <OpenEXR/half.h>

#include <glad.h>

#include <limits>

namespace djv
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

        typedef uint8_t   U8_T;
        typedef uint16_t U10_T;
        typedef uint16_t U12_T;
        typedef uint16_t U16_T;
        typedef uint32_t U32_T;
        typedef half     F16_T;
        typedef float    F32_T;

        const Math::Range<U8_T> U8Range(
            std::numeric_limits<U8_T>::min(),
            std::numeric_limits<U8_T>::max());
        
        const Math::Range<U10_T> U10Range(0, 1023);
        
        const Math::Range<U12_T> U12Range(0, 4095);
        
        const Math::Range<U16_T> U16Range(
            std::numeric_limits<U16_T>::min(),
            std::numeric_limits<U16_T>::max());
        
        const Math::Range<U32_T> U32Range(
            std::numeric_limits<U32_T>::min(),
            std::numeric_limits<U32_T>::max());
        
        const Math::Range<F16_T> F16Range(0.F, 1.F);
        
        const Math::Range<F32_T> F32Range(0.F, 1.F);

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

    } // namespace Image
} // namespace djv

#include <djvImage/PixelInline.h>

