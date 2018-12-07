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

#include <djvAV/Pixel.h>

#include <algorithm>
#include <functional>
#include <map>

namespace djv
{
    namespace AV
    {
        namespace Pixel
        {
            namespace
            {
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
                        outP[1] = B##Max; \
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
                        outP[3] = B##Max; \
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
                        const A##_T tmp = static_cast<A##_T>((inP[0] + inP[1] + inP[2]) / 3.f); \
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
                        const A##_T tmp = static_cast<A##_T>((inP[0] + inP[1] + inP[2]) / 3.f); \
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
                        outP[3] = B##Max; \
                    } \
                }

#define CONVERT_RGBA_L(A, B) \
                void convert_RGBA_##A##_L_##B(const void * in, void * out, size_t size) \
                { \
                    const A##_T * inP = reinterpret_cast<const A##_T *>(in); \
                    B##_T * outP = reinterpret_cast<B##_T *>(out); \
                    for (size_t i = 0; i < size; ++i, inP += 4, ++outP) \
                    { \
                        const A##_T tmp = static_cast<A##_T>((inP[0] + inP[1] + inP[2]) / 3.f); \
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
                        const A##_T tmp = static_cast<A##_T>((inP[0] + inP[1] + inP[2]) / 3.f); \
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
                        const U10_T tmp = static_cast<U10_T>((inP->r + inP->g + inP->b) / 3.f); \
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
                        const U10_T tmp = static_cast<U10_T>((inP->r + inP->g + inP->b) / 3.f); \
                        convert_U10_##B(tmp, outP[0]); \
                        outP[1] = B##Max; \
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
                        outP[3] = B##Max; \
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
                std::map<Type, std::map<Type, Function> > functions =
                {
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
                else
                {
                    DJV_ASSERT(0);
                }
            }

        } // namespace Image
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        AV::Pixel,
        Type,
        DJV_TEXT("None"),
        DJV_TEXT("L_U8"),
        DJV_TEXT("L_U16"),
        DJV_TEXT("L_U32"),
        DJV_TEXT("L_F16"),
        DJV_TEXT("L_F32"),
        DJV_TEXT("LA_U8"),
        DJV_TEXT("LA_U16"),
        DJV_TEXT("LA_U32"),
        DJV_TEXT("LA_F16"),
        DJV_TEXT("LA_F32"),
        DJV_TEXT("RGB_U8"),
        DJV_TEXT("RGB_U10"),
        DJV_TEXT("RGB_U16"),
        DJV_TEXT("RGB_U32"),
        DJV_TEXT("RGB_F16"),
        DJV_TEXT("RGB_F32"),
        DJV_TEXT("RGBA_U8"),
        DJV_TEXT("RGBA_U16"),
        DJV_TEXT("RGBA_U32"),
        DJV_TEXT("RGBA_F16"),
        DJV_TEXT("RGBA_F32"));

} // namespace djv

