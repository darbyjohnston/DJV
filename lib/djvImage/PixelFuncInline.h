// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvMath/MathFunc.h>

#include <djvCore/MemoryFunc.h>

namespace djv
{
    namespace Image
    {
        inline void convert_U8_U8(U8_T in, U8_T& out)
        {
            out = in;
        }

        inline void convert_U8_U10(U8_T in, U10_T& out)
        {
            out = in << 2;
        }

        inline void convert_U8_U16(U8_T in, U16_T& out)
        {
            out = in << 8;
        }

        inline void convert_U8_U32(U8_T in, U32_T& out)
        {
            out = in << 24;
        }

        inline void convert_U8_F16(U8_T in, F16_T& out)
        {
            out = in / static_cast<float>(U8Range.getMax());
        }

        inline void convert_U8_F32(U8_T in, F32_T& out)
        {
            out = in / static_cast<float>(U8Range.getMax());
        }

        inline void convert_U10_U8(U10_T in, U8_T& out)
        {
            out = in >> 2;
        }

        inline void convert_U10_U8(U10_T in, U10_T& out)
        {
            out = in;
        }

        inline void convert_U10_U16(U10_T in, U16_T& out)
        {
            out = in << 6;
        }

        inline void convert_U10_U32(U10_T in, U32_T& out)
        {
            out = in << 22;
        }

        inline void convert_U10_F16(U10_T in, F16_T& out)
        {
            out = in / static_cast<float>(U10Range.getMax());
        }

        inline void convert_U10_F32(U10_T in, F32_T& out)
        {
            out = in / static_cast<float>(U10Range.getMax());
        }

        inline void convert_U16_U8(U16_T in, U8_T& out)
        {
            out = in >> 8;
        }

        inline void convert_U16_U10(U16_T in, U10_T& out)
        {
            out = in >> 6;
        }

        inline void convert_U16_U16(U16_T in, U16_T& out)
        {
            out = in;
        }

        inline void convert_U16_U32(U16_T in, U32_T& out)
        {
            out = in << 16;
        }

        inline void convert_U16_F16(U16_T in, F16_T& out)
        {
            out = in / static_cast<float>(U16Range.getMax());
        }

        inline void convert_U16_F32(U16_T in, F32_T& out)
        {
            out = in / static_cast<float>(U16Range.getMax());
        }

        inline void convert_U32_U8(U32_T in, U8_T& out)
        {
            out = in >> 24;
        }

        inline void convert_U32_U10(U32_T in, U10_T& out)
        {
            out = in >> 22;
        }

        inline void convert_U32_U16(U32_T in, U16_T& out)
        {
            out = in >> 16;
        }

        inline void convert_U32_U32(U32_T in, U32_T& out)
        {
            out = in;
        }

        inline void convert_U32_F16(U32_T in, F16_T& out)
        {
            out = in / static_cast<float>(U32Range.getMax());
        }

        inline void convert_U32_F32(U32_T in, F32_T& out)
        {
            out = in / static_cast<float>(U32Range.getMax());
        }

        inline void convert_F16_U8(F16_T in, U8_T& out)
        {
            out = static_cast<U8_T>(Math::clamp(
                static_cast<uint16_t>(in * U8Range.getMax()),
                static_cast<uint16_t>(U8Range.getMin()),
                static_cast<uint16_t>(U8Range.getMax())));
        }

        inline void convert_F16_U10(F16_T in, U10_T& out)
        {
            out = static_cast<U10_T>(Math::clamp(
                static_cast<uint16_t>(in * U10Range.getMax()),
                static_cast<uint16_t>(U10Range.getMin()),
                static_cast<uint16_t>(U10Range.getMax())));
        }

        inline void convert_F16_U16(F16_T in, U16_T& out)
        {
            out = static_cast<U16_T>(Math::clamp(
                static_cast<uint32_t>(in * U16Range.getMax()),
                static_cast<uint32_t>(U16Range.getMin()),
                static_cast<uint32_t>(U16Range.getMax())));
        }

        inline void convert_F16_U32(F16_T in, U32_T& out)
        {
            out = static_cast<U32_T>(Math::clamp(
                static_cast<uint64_t>(static_cast<double>(in) * static_cast<uint64_t>(U32Range.getMax())),
                static_cast<uint64_t>(U32Range.getMin()),
                static_cast<uint64_t>(U32Range.getMax())));
        }

        inline void convert_F16_F16(F16_T in, F16_T& out)
        {
            out = in;
        }

        inline void convert_F16_F32(F16_T in, F32_T& out)
        {
            out = in;
        }

        inline void convert_F32_U8(F32_T in, U8_T& out)
        {
            out = static_cast<U8_T>(Math::clamp(
                static_cast<uint16_t>(in * U8Range.getMax()),
                static_cast<uint16_t>(U8Range.getMin()),
                static_cast<uint16_t>(U8Range.getMax())));
        }

        inline void convert_F32_U10(F32_T in, U10_T& out)
        {
            out = static_cast<U10_T>(Math::clamp(
                static_cast<uint16_t>(in * U10Range.getMax()),
                static_cast<uint16_t>(U10Range.getMin()),
                static_cast<uint16_t>(U10Range.getMax())));
        }

        inline void convert_F32_U16(F32_T in, U16_T& out)
        {
            out = static_cast<U16_T>(Math::clamp(
                static_cast<uint32_t>(in * U16Range.getMax()),
                static_cast<uint32_t>(U16Range.getMin()),
                static_cast<uint32_t>(U16Range.getMax())));
        }

        inline void convert_F32_U32(F32_T in, U32_T& out)
        {
            out = static_cast<U32_T>(Math::clamp(
                static_cast<uint64_t>(static_cast<double>(in) * static_cast<uint64_t>(U32Range.getMax())),
                static_cast<uint64_t>(U32Range.getMin()),
                static_cast<uint64_t>(U32Range.getMax())));
        }

        inline void convert_F32_F16(F32_T in, F16_T& out)
        {
            out = in;
        }

        inline void convert_F32_F32(F32_T in, F32_T& out)
        {
            out = in;
        }

    } // namespace Image
} // namespace djv

namespace std
{
    inline std::size_t hash<djv::Image::Type>::operator() (djv::Image::Type value) const noexcept
    {
        size_t hash = 0;
        djv::Core::Memory::hashCombine<int>(hash, static_cast<int>(value));
        return hash;
    }

} // namespace std
