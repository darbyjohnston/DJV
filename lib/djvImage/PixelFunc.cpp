// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvImage/PixelFunc.h>

#include <algorithm>
#include <array>
#include <functional>
#include <map>

#define CONVERT_L_L(A, B) \
    void convert_L_##A##_L_##B(const void * in, void * out, size_t size) \
    { \
        const A##_T * inP = reinterpret_cast<const A##_T *>(in); \
        B##_T * outP = reinterpret_cast<B##_T *>(out); \
        for (size_t i = 0; i < size; ++i, ++inP, ++outP) \
        { \
            convert_##A##_##B(inP[0], outP[0]); \
        } \
    }
#define CONVERT_L_LA(A, B) \
    void convert_L_##A##_LA_##B(const void * in, void * out, size_t size) \
    { \
        const A##_T * inP = reinterpret_cast<const A##_T *>(in); \
        B##_T * outP = reinterpret_cast<B##_T *>(out); \
        for (size_t i = 0; i < size; ++i, ++inP, outP += 2) \
        { \
            convert_##A##_##B(inP[0], outP[0]); \
            outP[1] = B##Range.getMax(); \
        } \
    }
#define CONVERT_L_RGB(A, B) \
    void convert_L_##A##_RGB_##B(const void * in, void * out, size_t size) \
    { \
        const A##_T * inP = reinterpret_cast<const A##_T *>(in); \
        B##_T * outP = reinterpret_cast<B##_T *>(out); \
        for (size_t i = 0; i < size; ++i, ++inP, outP += 3) \
        { \
            convert_##A##_##B(inP[0], outP[0]); \
            outP[1] = outP[0]; \
            outP[2] = outP[0]; \
        } \
    }
#define CONVERT_L_RGBA(A, B) \
    void convert_L_##A##_RGBA_##B(const void * in, void * out, size_t size) \
    { \
        const A##_T * inP = reinterpret_cast<const A##_T *>(in); \
        B##_T * outP = reinterpret_cast<B##_T *>(out); \
        for (size_t i = 0; i < size; ++i, ++inP, outP += 4) \
        { \
            convert_##A##_##B(inP[0], outP[0]); \
            outP[1] = outP[0]; \
            outP[2] = outP[0]; \
            outP[3] = B##Range.getMax(); \
        } \
    }

#define CONVERT_LA_L(A, B) \
    void convert_LA_##A##_L_##B(const void * in, void * out, size_t size) \
    { \
        const A##_T * inP = reinterpret_cast<const A##_T *>(in); \
        B##_T * outP = reinterpret_cast<B##_T *>(out); \
        for (size_t i = 0; i < size; ++i, inP += 2, ++outP) \
        { \
            convert_##A##_##B(inP[0], outP[0]); \
        } \
    }
#define CONVERT_LA_LA(A, B) \
    void convert_LA_##A##_LA_##B(const void * in, void * out, size_t size) \
    { \
        const A##_T * inP = reinterpret_cast<const A##_T *>(in); \
        B##_T * outP = reinterpret_cast<B##_T *>(out); \
        for (size_t i = 0; i < size; ++i, inP += 2, outP += 2) \
        { \
            convert_##A##_##B(inP[0], outP[0]); \
            convert_##A##_##B(inP[1], outP[1]); \
        } \
    }
#define CONVERT_LA_RGB(A, B) \
    void convert_LA_##A##_RGB_##B(const void * in, void * out, size_t size) \
    { \
        const A##_T * inP = reinterpret_cast<const A##_T *>(in); \
        B##_T * outP = reinterpret_cast<B##_T *>(out); \
        for (size_t i = 0; i < size; ++i, inP += 2, outP += 3) \
        { \
            convert_##A##_##B(inP[0], outP[0]); \
            outP[1] = outP[0]; \
            outP[2] = outP[0]; \
        } \
    }
#define CONVERT_LA_RGBA(A, B) \
    void convert_LA_##A##_RGBA_##B(const void * in, void * out, size_t size) \
    { \
        const A##_T * inP = reinterpret_cast<const A##_T *>(in); \
        B##_T * outP = reinterpret_cast<B##_T *>(out); \
        for (size_t i = 0; i < size; ++i, inP += 2, outP += 4) \
        { \
            convert_##A##_##B(inP[0], outP[0]); \
            outP[1] = outP[0]; \
            outP[2] = outP[0]; \
            convert_##A##_##B(inP[1], outP[3]); \
        } \
    }

#define CONVERT_RGB_L(A, B) \
    void convert_RGB_##A##_L_##B(const void * in, void * out, size_t size) \
    { \
        const A##_T * inP = reinterpret_cast<const A##_T *>(in); \
        B##_T * outP = reinterpret_cast<B##_T *>(out); \
        for (size_t i = 0; i < size; ++i, inP += 3, ++outP) \
        { \
            const A##_T tmp = static_cast<A##_T>((inP[0] + inP[1] + inP[2]) / 3.F); \
            convert_##A##_##B(tmp, outP[0]); \
        } \
    }
#define CONVERT_RGB_LA(A, B) \
    void convert_RGB_##A##_LA_##B(const void * in, void * out, size_t size) \
    { \
        const A##_T * inP = reinterpret_cast<const A##_T *>(in); \
        B##_T * outP = reinterpret_cast<B##_T *>(out); \
        for (size_t i = 0; i < size; ++i, inP += 3, outP += 2) \
        { \
            const A##_T tmp = static_cast<A##_T>((inP[0] + inP[1] + inP[2]) / 3.F); \
            convert_##A##_##B(tmp, outP[0]); \
            convert_##A##_##B(inP[1], outP[1]); \
        } \
    }
#define CONVERT_RGB_RGB(A, B) \
    void convert_RGB_##A##_RGB_##B(const void * in, void * out, size_t size) \
    { \
        const A##_T * inP = reinterpret_cast<const A##_T *>(in); \
        B##_T * outP = reinterpret_cast<B##_T *>(out); \
        for (size_t i = 0; i < size; ++i, inP += 3, outP += 3) \
        { \
            convert_##A##_##B(inP[0], outP[0]); \
            convert_##A##_##B(inP[1], outP[1]); \
            convert_##A##_##B(inP[2], outP[2]); \
        } \
    }
#define CONVERT_RGB_RGBA(A, B) \
    void convert_RGB_##A##_RGBA_##B(const void * in, void * out, size_t size) \
    { \
        const A##_T * inP = reinterpret_cast<const A##_T *>(in); \
        B##_T * outP = reinterpret_cast<B##_T *>(out); \
        for (size_t i = 0; i < size; ++i, inP += 3, outP += 4) \
        { \
            convert_##A##_##B(inP[0], outP[0]); \
            convert_##A##_##B(inP[1], outP[1]); \
            convert_##A##_##B(inP[2], outP[2]); \
            outP[3] = B##Range.getMax(); \
        } \
    }

#define CONVERT_RGBA_L(A, B) \
    void convert_RGBA_##A##_L_##B(const void * in, void * out, size_t size) \
    { \
        const A##_T * inP = reinterpret_cast<const A##_T *>(in); \
        B##_T * outP = reinterpret_cast<B##_T *>(out); \
        for (size_t i = 0; i < size; ++i, inP += 4, ++outP) \
        { \
            const A##_T tmp = static_cast<A##_T>((inP[0] + inP[1] + inP[2]) / 3.F); \
            convert_##A##_##B(tmp, outP[0]); \
        } \
    }
#define CONVERT_RGBA_LA(A, B) \
    void convert_RGBA_##A##_LA_##B(const void * in, void * out, size_t size) \
    { \
        const A##_T * inP = reinterpret_cast<const A##_T *>(in); \
        B##_T * outP = reinterpret_cast<B##_T *>(out); \
        for (size_t i = 0; i < size; ++i, inP += 4, outP += 2) \
        { \
            const A##_T tmp = static_cast<A##_T>((inP[0] + inP[1] + inP[2]) / 3.F); \
            convert_##A##_##B(tmp, outP[0]); \
            convert_##A##_##B(inP[1], outP[1]); \
        } \
    }
#define CONVERT_RGBA_RGB(A, B) \
    void convert_RGBA_##A##_RGB_##B(const void * in, void * out, size_t size) \
    { \
        const A##_T * inP = reinterpret_cast<const A##_T *>(in); \
        B##_T * outP = reinterpret_cast<B##_T *>(out); \
        for (size_t i = 0; i < size; ++i, inP += 4, outP += 3) \
        { \
            convert_##A##_##B(inP[0], outP[0]); \
            convert_##A##_##B(inP[1], outP[1]); \
            convert_##A##_##B(inP[2], outP[2]); \
        } \
    }
#define CONVERT_RGBA_RGBA(A, B) \
    void convert_RGBA_##A##_RGBA_##B(const void * in, void * out, size_t size) \
    { \
        const A##_T * inP = reinterpret_cast<const A##_T *>(in); \
        B##_T * outP = reinterpret_cast<B##_T *>(out); \
        for (size_t i = 0; i < size; ++i, inP += 4, outP += 4) \
        { \
            convert_##A##_##B(inP[0], outP[0]); \
            convert_##A##_##B(inP[1], outP[1]); \
            convert_##A##_##B(inP[2], outP[2]); \
            convert_##A##_##B(inP[3], outP[3]); \
        } \
    }

#define CONVERT_L_RGB_U10(A) \
    void convert_L_##A##_RGB_U10(const void * in, void * out, size_t size) \
    { \
        const A##_T * inP = reinterpret_cast<const A##_T *>(in); \
        U10_S * outP = reinterpret_cast<U10_S *>(out); \
        for (size_t i = 0; i < size; ++i, ++inP, ++outP) \
        { \
            U10_T tmp = 0; \
            convert_##A##_U10(inP[0], tmp); \
            outP->r = tmp; \
            outP->g = tmp; \
            outP->b = tmp; \
        } \
    }
#define CONVERT_LA_RGB_U10(A) \
    void convert_LA_##A##_RGB_U10(const void * in, void * out, size_t size) \
    { \
        const A##_T * inP = reinterpret_cast<const A##_T *>(in); \
        U10_S * outP = reinterpret_cast<U10_S *>(out); \
        for (size_t i = 0; i < size; ++i, inP += 2, ++outP) \
        { \
            U10_T tmp = 0; \
            convert_##A##_U10(inP[0], tmp); \
            outP->r = tmp; \
            outP->g = tmp; \
            outP->b = tmp; \
        } \
    }
#define CONVERT_RGB_RGB_U10(A) \
    void convert_RGB_##A##_RGB_U10(const void * in, void * out, size_t size) \
    { \
        const A##_T * inP = reinterpret_cast<const A##_T *>(in); \
        U10_S * outP = reinterpret_cast<U10_S *>(out); \
        for (size_t i = 0; i < size; ++i, inP += 3, ++outP) \
        { \
            U10_T tmp = 0; \
            convert_##A##_U10(inP[0], tmp); \
            outP->r = tmp; \
            convert_##A##_U10(inP[1], tmp); \
            outP->g = tmp; \
            convert_##A##_U10(inP[2], tmp); \
            outP->b = tmp; \
        } \
    }
#define CONVERT_RGBA_RGB_U10(A) \
    void convert_RGBA_##A##_RGB_U10(const void * in, void * out, size_t size) \
    { \
        const A##_T * inP = reinterpret_cast<const A##_T *>(in); \
        U10_S * outP = reinterpret_cast<U10_S *>(out); \
        for (size_t i = 0; i < size; ++i, inP += 4, ++outP) \
        { \
            U10_T tmp = 0; \
            convert_##A##_U10(inP[0], tmp); \
            outP->r = tmp; \
            convert_##A##_U10(inP[1], tmp); \
            outP->g = tmp; \
            convert_##A##_U10(inP[2], tmp); \
            outP->b = tmp; \
        } \
    }
#define CONVERT_RGB_U10_L(B) \
    void convert_RGB_U10_L_##B(const void * in, void * out, size_t size) \
    { \
        const U10_S * inP = reinterpret_cast<const U10_S *>(in); \
        B##_T * outP = reinterpret_cast<B##_T *>(out); \
        for (size_t i = 0; i < size; ++i, ++inP, ++outP) \
        { \
            const U10_T tmp = static_cast<U10_T>((inP->r + inP->g + inP->b) / 3.F); \
            convert_U10_##B(tmp, outP[0]); \
        } \
    }
#define CONVERT_RGB_U10_LA(B) \
    void convert_RGB_U10_LA_##B(const void * in, void * out, size_t size) \
    { \
        const U10_S * inP = reinterpret_cast<const U10_S *>(in); \
        B##_T * outP = reinterpret_cast<B##_T *>(out); \
        for (size_t i = 0; i < size; ++i, ++inP, outP += 2) \
        { \
            const U10_T tmp = static_cast<U10_T>((inP->r + inP->g + inP->b) / 3.F); \
            convert_U10_##B(tmp, outP[0]); \
            outP[1] = B##Range.getMax(); \
        } \
    }
#define CONVERT_RGB_U10_RGB(B) \
    void convert_RGB_U10_RGB_##B(const void * in, void * out, size_t size) \
    { \
        const U10_S * inP = reinterpret_cast<const U10_S *>(in); \
        B##_T * outP = reinterpret_cast<B##_T *>(out); \
        for (size_t i = 0; i < size; ++i, ++inP, outP += 3) \
        { \
            convert_U10_##B(inP->r, outP[0]); \
            convert_U10_##B(inP->g, outP[1]); \
            convert_U10_##B(inP->b, outP[2]); \
        } \
    }
#define CONVERT_RGB_U10_RGBA(B) \
    void convert_RGB_U10_RGBA_##B(const void * in, void * out, size_t size) \
    { \
        const U10_S * inP = reinterpret_cast<const U10_S *>(in); \
        B##_T * outP = reinterpret_cast<B##_T *>(out); \
        for (size_t i = 0; i < size; ++i, ++inP, outP += 3) \
        { \
            convert_U10_##B(inP->r, outP[0]); \
            convert_U10_##B(inP->g, outP[1]); \
            convert_U10_##B(inP->b, outP[2]); \
            outP[3] = B##Range.getMax(); \
        } \
    }
#define CONVERT_RGB_U10_RGB_U10() \
    void convert_RGB_U10_RGB_U10(const void * in, void * out, size_t size) \
    { \
        const U10_S * inP = reinterpret_cast<const U10_S *>(in); \
        U10_S * outP = reinterpret_cast<U10_S *>(out); \
        for (size_t i = 0; i < size; ++i, ++inP, ++outP) \
        { \
            outP->r = inP->r; \
            outP->g = inP->g; \
            outP->b = inP->b; \
        } \
    }

#define CONVERT_L(A) \
    CONVERT_L_L(A, U8); \
    CONVERT_L_L(A, U16); \
    CONVERT_L_L(A, U32); \
    CONVERT_L_L(A, F16); \
    CONVERT_L_L(A, F32); \
    CONVERT_L_LA(A, U8); \
    CONVERT_L_LA(A, U16); \
    CONVERT_L_LA(A, U32); \
    CONVERT_L_LA(A, F16); \
    CONVERT_L_LA(A, F32); \
    CONVERT_L_RGB(A, U8); \
    CONVERT_L_RGB_U10(A); \
    CONVERT_L_RGB(A, U16); \
    CONVERT_L_RGB(A, U32); \
    CONVERT_L_RGB(A, F16); \
    CONVERT_L_RGB(A, F32); \
    CONVERT_L_RGBA(A, U8); \
    CONVERT_L_RGBA(A, U16); \
    CONVERT_L_RGBA(A, U32); \
    CONVERT_L_RGBA(A, F16); \
    CONVERT_L_RGBA(A, F32);

#define CONVERT_LA(A) \
    CONVERT_LA_L(A, U8); \
    CONVERT_LA_L(A, U16); \
    CONVERT_LA_L(A, U32); \
    CONVERT_LA_L(A, F16); \
    CONVERT_LA_L(A, F32); \
    CONVERT_LA_LA(A, U8); \
    CONVERT_LA_LA(A, U16); \
    CONVERT_LA_LA(A, U32); \
    CONVERT_LA_LA(A, F16); \
    CONVERT_LA_LA(A, F32); \
    CONVERT_LA_RGB(A, U8); \
    CONVERT_LA_RGB_U10(A); \
    CONVERT_LA_RGB(A, U16); \
    CONVERT_LA_RGB(A, U32); \
    CONVERT_LA_RGB(A, F16); \
    CONVERT_LA_RGB(A, F32); \
    CONVERT_LA_RGBA(A, U8); \
    CONVERT_LA_RGBA(A, U16); \
    CONVERT_LA_RGBA(A, U32); \
    CONVERT_LA_RGBA(A, F16); \
    CONVERT_LA_RGBA(A, F32);

#define CONVERT_RGB(A) \
    CONVERT_RGB_L(A, U8); \
    CONVERT_RGB_L(A, U16); \
    CONVERT_RGB_L(A, U32); \
    CONVERT_RGB_L(A, F16); \
    CONVERT_RGB_L(A, F32); \
    CONVERT_RGB_LA(A, U8); \
    CONVERT_RGB_LA(A, U16); \
    CONVERT_RGB_LA(A, U32); \
    CONVERT_RGB_LA(A, F16); \
    CONVERT_RGB_LA(A, F32); \
    CONVERT_RGB_RGB(A, U8); \
    CONVERT_RGB_RGB_U10(A); \
    CONVERT_RGB_RGB(A, U16); \
    CONVERT_RGB_RGB(A, U32); \
    CONVERT_RGB_RGB(A, F16); \
    CONVERT_RGB_RGB(A, F32); \
    CONVERT_RGB_RGBA(A, U8); \
    CONVERT_RGB_RGBA(A, U16); \
    CONVERT_RGB_RGBA(A, U32); \
    CONVERT_RGB_RGBA(A, F16); \
    CONVERT_RGB_RGBA(A, F32);

#define CONVERT_RGB_U10() \
    CONVERT_RGB_U10_L(U8); \
    CONVERT_RGB_U10_L(U16); \
    CONVERT_RGB_U10_L(U32); \
    CONVERT_RGB_U10_L(F16); \
    CONVERT_RGB_U10_L(F32); \
    CONVERT_RGB_U10_LA(U8); \
    CONVERT_RGB_U10_LA(U16); \
    CONVERT_RGB_U10_LA(U32); \
    CONVERT_RGB_U10_LA(F16); \
    CONVERT_RGB_U10_LA(F32); \
    CONVERT_RGB_U10_RGB(U8); \
    CONVERT_RGB_U10_RGB(U16); \
    CONVERT_RGB_U10_RGB(U32); \
    CONVERT_RGB_U10_RGB(F16); \
    CONVERT_RGB_U10_RGB(F32); \
    CONVERT_RGB_U10_RGBA(U8); \
    CONVERT_RGB_U10_RGBA(U16); \
    CONVERT_RGB_U10_RGBA(U32); \
    CONVERT_RGB_U10_RGBA(F16); \
    CONVERT_RGB_U10_RGBA(F32);

#define CONVERT_RGBA(A) \
    CONVERT_RGBA_L(A, U8); \
    CONVERT_RGBA_L(A, U16); \
    CONVERT_RGBA_L(A, U32); \
    CONVERT_RGBA_L(A, F16); \
    CONVERT_RGBA_L(A, F32); \
    CONVERT_RGBA_LA(A, U8); \
    CONVERT_RGBA_LA(A, U16); \
    CONVERT_RGBA_LA(A, U32); \
    CONVERT_RGBA_LA(A, F16); \
    CONVERT_RGBA_LA(A, F32); \
    CONVERT_RGBA_RGB(A, U8); \
    CONVERT_RGBA_RGB_U10(A); \
    CONVERT_RGBA_RGB(A, U16); \
    CONVERT_RGBA_RGB(A, U32); \
    CONVERT_RGBA_RGB(A, F16); \
    CONVERT_RGBA_RGB(A, F32); \
    CONVERT_RGBA_RGBA(A, U8); \
    CONVERT_RGBA_RGBA(A, U16); \
    CONVERT_RGBA_RGBA(A, U32); \
    CONVERT_RGBA_RGBA(A, F16); \
    CONVERT_RGBA_RGBA(A, F32);

#define CONVERT_MAP(A) \
    { \
        Type::A, \
        { \
            { Type::L_U8,     convert_##A##_L_U8 },     \
            { Type::L_U16,    convert_##A##_L_U16 },    \
            { Type::L_U32,    convert_##A##_L_U32 },    \
            { Type::L_F16,    convert_##A##_L_F16 },    \
            { Type::L_F32,    convert_##A##_L_F32 },    \
            { Type::LA_U8,    convert_##A##_LA_U8 },    \
            { Type::LA_U16,   convert_##A##_LA_U16 },   \
            { Type::LA_U32,   convert_##A##_LA_U32 },   \
            { Type::LA_F16,   convert_##A##_LA_F16 },   \
            { Type::LA_F32,   convert_##A##_LA_F32 },   \
            { Type::RGB_U8,   convert_##A##_RGB_U8 },   \
            { Type::RGB_U10,  convert_##A##_RGB_U10 },  \
            { Type::RGB_U16,  convert_##A##_RGB_U16 },  \
            { Type::RGB_U32,  convert_##A##_RGB_U32 },  \
            { Type::RGB_F16,  convert_##A##_RGB_F16 },  \
            { Type::RGB_F32,  convert_##A##_RGB_F32 },  \
            { Type::RGBA_U8,  convert_##A##_RGBA_U8 },  \
            { Type::RGBA_U16, convert_##A##_RGBA_U16 }, \
            { Type::RGBA_U32, convert_##A##_RGBA_U32 }, \
            { Type::RGBA_F16, convert_##A##_RGBA_F16 }, \
            { Type::RGBA_F32, convert_##A##_RGBA_F32 }  \
        } \
    }

namespace djv
{
    namespace Image
    {
        Channels getChannels(Type value) noexcept
        {
            const std::array<Channels, static_cast<size_t>(Type::Count)> data =
            {
                Channels::None,

                Channels::L,
                Channels::L,
                Channels::L,
                Channels::L,
                Channels::L,

                Channels::LA,
                Channels::LA,
                Channels::LA,
                Channels::LA,
                Channels::LA,

                Channels::RGB,
                Channels::RGB,
                Channels::RGB,
                Channels::RGB,
                Channels::RGB,
                Channels::RGB,

                Channels::RGBA,
                Channels::RGBA,
                Channels::RGBA,
                Channels::RGBA,
                Channels::RGBA
            };
            return data[static_cast<size_t>(value)];
        }

        uint8_t getChannelCount(Type value) noexcept
        {
            const std::array<uint8_t, static_cast<size_t>(Type::Count)> data =
            {
                0,
                1, 1, 1, 1, 1,
                2, 2, 2, 2, 2,
                3, 3, 3, 3, 3, 3,
                4, 4, 4, 4, 4
            };
            return data[static_cast<size_t>(value)];
        }

        DataType getDataType(Type value) noexcept
        {
            const std::array<DataType, static_cast<size_t>(Type::Count)> data =
            {
                DataType::None,

                DataType::U8,
                DataType::U16,
                DataType::U32,
                DataType::F16,
                DataType::F32,

                DataType::U8,
                DataType::U16,
                DataType::U32,
                DataType::F16,
                DataType::F32,

                DataType::U8,
                DataType::U10,
                DataType::U16,
                DataType::U32,
                DataType::F16,
                DataType::F32,

                DataType::U8,
                DataType::U16,
                DataType::U32,
                DataType::F16,
                DataType::F32
            };
            return data[static_cast<size_t>(value)];
        }

        uint8_t getBitDepth(Type value) noexcept
        {
            const std::array<uint8_t, static_cast<size_t>(Type::Count)> data =
            {
                0,
                8, 16, 32, 16, 32,
                8, 16, 32, 16, 32,
                8, 10, 16, 32, 16, 32,
                8, 16, 32, 16, 32
            };
            return data[static_cast<size_t>(value)];
        }

        uint8_t getBitDepth(DataType value) noexcept
        {
            const std::array<uint8_t, static_cast<size_t>(DataType::Count)> data =
            {
                0,
                8,
                10,
                16,
                32,
                16,
                32
            };
            return data[static_cast<size_t>(value)];
        }

        size_t getByteCount(Type value) noexcept
        {
            const std::array<size_t, static_cast<size_t>(Type::Count)> data =
            {
                0,
                1, 2, 4, 2, 4,
                2, 4, 8, 4, 8,
                3, 4, 6, 12, 6, 12,
                4, 8, 16, 8, 16
            };
            return data[static_cast<size_t>(value)];
        }

        size_t getByteCount(DataType value) noexcept
        {
            const std::array<size_t, static_cast<size_t>(DataType::Count)> data =
            {
                0,
                1,
                2,
                2,
                4,
                2,
                4
            };
            return data[static_cast<size_t>(value)];
        }

        bool isIntType(Type value) noexcept
        {
            const std::array<bool, static_cast<size_t>(Type::Count)> data =
            {
                false,
                true, true, true, false, false,
                true, true, true, false, false,
                true, true, true, true, false, false,
                true, true, true, false, false,
            };
            return data[static_cast<size_t>(value)];
        }

        bool isFloatType(Type value) noexcept
        {
            const std::array<bool, static_cast<size_t>(Type::Count)> data =
            {
                false,
                false, false, false, true, true,
                false, false, false, true, true,
                false, false, false, false, true, true,
                false, false, false, true, true
            };
            return data[static_cast<size_t>(value)];
        }

        Math::IntRange getIntRange(Type value)
        {
            const std::array<Math::IntRange, static_cast<size_t>(Type::Count)> data =
            {
                Math::IntRange(0, 0),

                Math::IntRange(U8Range.getMin(), U8Range.getMax()),
                Math::IntRange(U16Range.getMin(), U16Range.getMax()),
                Math::IntRange(U32Range.getMin(), U32Range.getMax()),
                Math::IntRange(0, 0),
                Math::IntRange(0, 0),

                Math::IntRange(U8Range.getMin(), U8Range.getMax()),
                Math::IntRange(U16Range.getMin(), U16Range.getMax()),
                Math::IntRange(U32Range.getMin(), U32Range.getMax()),
                Math::IntRange(0, 0),
                Math::IntRange(0, 0),

                Math::IntRange(U8Range.getMin(), U8Range.getMax()),
                Math::IntRange(U10Range.getMin(), U10Range.getMax()),
                Math::IntRange(U16Range.getMin(), U16Range.getMax()),
                Math::IntRange(U32Range.getMin(), U32Range.getMax()),
                Math::IntRange(0, 0),
                Math::IntRange(0, 0),

                Math::IntRange(U8Range.getMin(), U8Range.getMax()),
                Math::IntRange(U16Range.getMin(), U16Range.getMax()),
                Math::IntRange(U32Range.getMin(), U32Range.getMax()),
                Math::IntRange(0, 0),
                Math::IntRange(0, 0),
            };
            return data[static_cast<size_t>(value)];
        }

        Math::FloatRange getFloatRange(Type value)
        {
            const std::array<Math::FloatRange, static_cast<size_t>(Type::Count)> data =
            {
                Math::FloatRange(0.F, 0.F),

                Math::FloatRange(0.F, 0.F),
                Math::FloatRange(0.F, 0.F),
                Math::FloatRange(0.F, 0.F),
                Math::FloatRange(F16Range.getMin(), F16Range.getMax()),
                Math::FloatRange(F32Range.getMin(), F32Range.getMax()),

                Math::FloatRange(0.F, 0.F),
                Math::FloatRange(0.F, 0.F),
                Math::FloatRange(0.F, 0.F),
                Math::FloatRange(F16Range.getMin(), F16Range.getMax()),
                Math::FloatRange(F32Range.getMin(), F32Range.getMax()),

                Math::FloatRange(0.F, 0.F),
                Math::FloatRange(0.F, 0.F),
                Math::FloatRange(0.F, 0.F),
                Math::FloatRange(0.F, 0.F),
                Math::FloatRange(F16Range.getMin(), F16Range.getMax()),
                Math::FloatRange(F32Range.getMin(), F32Range.getMax()),

                Math::FloatRange(0.F, 0.F),
                Math::FloatRange(0.F, 0.F),
                Math::FloatRange(0.F, 0.F),
                Math::FloatRange(F16Range.getMin(), F16Range.getMax()),
                Math::FloatRange(F32Range.getMin(), F32Range.getMax()),
            };
            return data[static_cast<size_t>(value)];
        }

        Type getIntType(uint8_t channelCount, uint8_t bitDepth) noexcept
        {
            switch (channelCount)
            {
            case 1:
                switch (bitDepth)
                {
                case  8: return Type::L_U8;
                case 16: return Type::L_U16;
                case 32: return Type::L_U32;
                default: break;
                }
                break;
            case 2:
                switch (bitDepth)
                {
                case  8: return Type::LA_U8;
                case 16: return Type::LA_U16;
                case 32: return Type::LA_U32;
                default: break;
                }
                break;
            case 3:
                switch (bitDepth)
                {
                case  8: return Type::RGB_U8;
                case 10: return Type::RGB_U10;
                case 16: return Type::RGB_U16;
                case 32: return Type::RGB_U32;
                default: break;
                }
                break;
            case 4:
                switch (bitDepth)
                {
                case  8: return Type::RGBA_U8;
                case 16: return Type::RGBA_U16;
                case 32: return Type::RGBA_U32;
                default: break;
                }
                break;
            default: break;
            }
            return Type::None;
        }

        Type getFloatType(uint8_t channelCount, uint8_t bitDepth) noexcept
        {
            switch (channelCount)
            {
            case 1:
                switch (bitDepth)
                {
                case 16: return Type::L_F16;
                case 32: return Type::L_F32;
                default: break;
                }
                break;
            case 2:
                switch (bitDepth)
                {
                case 16: return Type::LA_F16;
                case 32: return Type::LA_F32;
                default: break;
                }
                break;
            case 3:
                switch (bitDepth)
                {
                case 16: return Type::RGB_F16;
                case 32: return Type::RGB_F32;
                default: break;
                }
                break;
            case 4:
                switch (bitDepth)
                {
                case 16: return Type::RGBA_F16;
                case 32: return Type::RGBA_F32;
                default: break;
                }
                break;
            default: break;
            }
            return Type::None;
        }

        GLenum getGLFormat(Type value) noexcept
        {
            const std::array<GLenum, static_cast<size_t>(Type::Count)> data =
            {
                GL_NONE,

#if defined(DJV_GL_ES2)
                GL_LUMINANCE,
                GL_NONE,
                GL_NONE,
                GL_NONE,
                GL_NONE,

                GL_LUMINANCE_ALPHA,
                GL_NONE,
                GL_NONE,
                GL_NONE,
                GL_NONE,

                GL_RGB,
                GL_NONE,
                GL_NONE,
                GL_NONE,
                GL_NONE,
                GL_NONE,

                GL_RGBA,
                GL_NONE,
                GL_NONE,
                GL_NONE,
                GL_NONE
#else // DJV_GL_ES2
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
#endif // DJV_GL_ES2
            };
            return data[static_cast<size_t>(value)];
        }

        GLenum getGLType(Type value) noexcept
        {
            const std::array<GLenum, static_cast<size_t>(Type::Count)> data =
            {
                GL_NONE,

#if defined(DJV_GL_ES2)
                GL_UNSIGNED_BYTE,
                GL_NONE,
                GL_NONE,
                GL_NONE,
                GL_NONE,

                GL_UNSIGNED_BYTE,
                GL_NONE,
                GL_NONE,
                GL_NONE,
                GL_NONE,

                GL_UNSIGNED_BYTE,
                GL_NONE,
                GL_NONE,
                GL_NONE,
                GL_NONE,
                GL_NONE,

                GL_UNSIGNED_BYTE,
                GL_NONE,
                GL_NONE,
                GL_NONE,
                GL_NONE,
#else // DJV_GL_ES2
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
#endif // DJV_GL_ES2
            };
            return data[static_cast<size_t>(value)];
        }

        namespace
        {
            CONVERT_L(U8);
            CONVERT_L(U16);
            CONVERT_L(U32);
            CONVERT_L(F16);
            CONVERT_L(F32);
            CONVERT_LA(U8);
            CONVERT_LA(U16);
            CONVERT_LA(U32);
            CONVERT_LA(F16);
            CONVERT_LA(F32);
            CONVERT_RGB(U8);
            CONVERT_RGB_U10();
            CONVERT_RGB_U10_RGB_U10();
            CONVERT_RGB(U16);
            CONVERT_RGB(U32);
            CONVERT_RGB(F16);
            CONVERT_RGB(F32);
            CONVERT_RGBA(U8);
            CONVERT_RGBA(U16);
            CONVERT_RGBA(U32);
            CONVERT_RGBA(F16);
            CONVERT_RGBA(F32);

        } // namespace

        void convert(const void * in, Type inType, void * out, Type outType, size_t size)
        {
            typedef std::function<void(const void *, void *, size_t)> Function;
            static const std::map<Type, std::map<Type, Function> > functions =
            {
                CONVERT_MAP(L_U8),
                CONVERT_MAP(L_U16),
                CONVERT_MAP(L_U32),
                CONVERT_MAP(L_F16),
                CONVERT_MAP(L_F32),
                CONVERT_MAP(LA_U8),
                CONVERT_MAP(LA_U16),
                CONVERT_MAP(LA_U32),
                CONVERT_MAP(LA_F16),
                CONVERT_MAP(LA_F32),
                CONVERT_MAP(RGB_U8),
                CONVERT_MAP(RGB_U10),
                CONVERT_MAP(RGB_U16),
                CONVERT_MAP(RGB_U32),
                CONVERT_MAP(RGB_F16),
                CONVERT_MAP(RGB_F32),
                CONVERT_MAP(RGBA_U8),
                CONVERT_MAP(RGBA_U16),
                CONVERT_MAP(RGBA_U32),
                CONVERT_MAP(RGBA_F16),
                CONVERT_MAP(RGBA_F32)
            };
            Function function;
            const auto i = functions.find(inType);
            if (i != functions.end())
            {
                const auto j = i->second.find(outType);
                if (j != i->second.end())
                {
                    function = j->second;
                }
            }
            if (function)
            {
                function(in, out, size);
            }
        }

        DJV_ENUM_HELPERS_IMPLEMENTATION(Type);
        DJV_ENUM_HELPERS_IMPLEMENTATION(Channels);
        DJV_ENUM_HELPERS_IMPLEMENTATION(DataType);

    } // namespace Image

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Image,
        Type,
        DJV_TEXT("image_type_none"),
        DJV_TEXT("image_type_l_u8"),
        DJV_TEXT("image_type_l_u16"),
        DJV_TEXT("image_type_l_u32"),
        DJV_TEXT("image_type_l_f16"),
        DJV_TEXT("image_type_l_f32"),
        DJV_TEXT("image_type_la_u8"),
        DJV_TEXT("image_type_la_u16"),
        DJV_TEXT("image_type_la_u32"),
        DJV_TEXT("image_type_la_f16"),
        DJV_TEXT("image_type_la_f32"),
        DJV_TEXT("image_type_rgb_u8"),
        DJV_TEXT("image_type_rgb_u10"),
        DJV_TEXT("image_type_rgb_u16"),
        DJV_TEXT("image_type_rgb_u32"),
        DJV_TEXT("image_type_rgb_f16"),
        DJV_TEXT("image_type_rgb_f32"),
        DJV_TEXT("image_type_rgba_u8"),
        DJV_TEXT("image_type_rgba_u16"),
        DJV_TEXT("image_type_rgba_u32"),
        DJV_TEXT("image_type_rgba_f16"),
        DJV_TEXT("image_type_rgba_f32"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Image,
        Channels,
        DJV_TEXT("image_channel_type_none"),
        DJV_TEXT("image_channel_type_l"),
        DJV_TEXT("image_channel_type_la"),
        DJV_TEXT("image_channel_type_rgb"),
        DJV_TEXT("image_channel_type_rgba"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Image,
        DataType,
        DJV_TEXT("image_data_type_none"),
        DJV_TEXT("image_data_type_u8"),
        DJV_TEXT("image_data_type_u10"),
        DJV_TEXT("image_data_type_u16"),
        DJV_TEXT("image_data_type_u32"),
        DJV_TEXT("image_data_type_f16"),
        DJV_TEXT("image_data_type_f32"));

    rapidjson::Value toJSON(Image::Type value, rapidjson::Document::AllocatorType& allocator)
    {
        std::stringstream ss;
        ss << value;
        const std::string& s = ss.str();
        return rapidjson::Value(s.c_str(), s.size(), allocator);
    }

    void fromJSON(const rapidjson::Value& value, Image::Type& out)
    {
        if (value.IsString())
        {
            std::stringstream ss(value.GetString());
            ss >> out;
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

} // namespace djv

