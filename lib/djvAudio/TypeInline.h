// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvMath/Math.h>

namespace djv
{
    namespace Audio
    {
        inline void S8ToS16(S8_T value, S16_T& out) noexcept
        {
            out = value * 256;
        }

        inline void S8ToS32(S8_T value, S32_T& out) noexcept
        {
            out = value * 256 * 256 * 256;
        }

        inline void S8ToF32(S8_T value, F32_T& out) noexcept
        {
            out = value / static_cast<float>(S8Range.getMax());
        }

        inline void S8ToF64(S8_T value, F64_T& out) noexcept
        {
            out = value / static_cast<double>(S8Range.getMax());
        }

        inline void S16ToS8(S16_T value, S8_T& out) noexcept
        {
            out = value / 256;
        }

        inline void S16ToS32(S16_T value, S32_T& out) noexcept
        {
            out = value * 256 * 256;
        }

        inline void S16ToF32(S16_T value, F32_T& out) noexcept
        {
            out = value / static_cast<float>(S16Range.getMax());
        }

        inline void S16ToF64(S16_T value, F64_T& out) noexcept
        {
            out = value / static_cast<double>(S16Range.getMax());
        }

        inline void S32ToS8(S32_T value, S8_T& out) noexcept
        {
            out = value / 256 / 256 / 256;
        }

        inline void S32ToS16(S32_T value, S16_T& out) noexcept
        {
            out = value / 256 / 256;
        }

        inline void S32ToF32(S32_T value, F32_T& out) noexcept
        {
            out = value / static_cast<float>(S32Range.getMax());
        }

        inline void S32ToF64(S32_T value, F64_T& out) noexcept
        {
            out = value / static_cast<double>(S32Range.getMax());
        }

        inline void F32ToS8(F32_T value, S8_T& out) noexcept
        {
            out = static_cast<S8_T>(Math::clamp(
                static_cast<int16_t>(value * S8Range.getMax()),
                static_cast<int16_t>(S8Range.getMin()),
                static_cast<int16_t>(S8Range.getMax())));
        }

        inline void F32ToS16(F32_T value, S16_T& out) noexcept
        {
            out = static_cast<S16_T>(Math::clamp(
                static_cast<int32_t>(value * S16Range.getMax()),
                static_cast<int32_t>(S16Range.getMin()),
                static_cast<int32_t>(S16Range.getMax())));
        }

        inline void F32ToS32(F32_T value, S32_T& out) noexcept
        {
            out = static_cast<S32_T>(Math::clamp(
                static_cast<int64_t>(static_cast<int64_t>(value) * S32Range.getMax()),
                static_cast<int64_t>(S32Range.getMin()),
                static_cast<int64_t>(S32Range.getMax())));
        }

        inline void F32ToF64(F32_T value, F64_T& out) noexcept
        {
            out = static_cast<double>(value);
        }

        inline void F64ToS8(F64_T value, S8_T& out) noexcept
        {
            out = static_cast<S8_T>(Math::clamp(
                static_cast<int16_t>(value * S8Range.getMax()),
                static_cast<int16_t>(S8Range.getMin()),
                static_cast<int16_t>(S8Range.getMax())));
        }

        inline void F64ToS16(F64_T value, S16_T& out) noexcept
        {
            out = static_cast<S16_T>(Math::clamp(
                static_cast<int32_t>(value * S16Range.getMax()),
                static_cast<int32_t>(S16Range.getMin()),
                static_cast<int32_t>(S16Range.getMax())));
        }

        inline void F64ToS32(F64_T value, S32_T& out) noexcept
        {
            out = static_cast<S32_T>(Math::clamp(
                static_cast<int64_t>(static_cast<int64_t>(value) * S32Range.getMax()),
                static_cast<int64_t>(S32Range.getMin()),
                static_cast<int64_t>(S32Range.getMax())));
        }

        inline void F64ToF32(F64_T value, F32_T& out) noexcept
        {
            out = static_cast<float>(value);
        }

    } // namespace Audio
} // namespace djv
