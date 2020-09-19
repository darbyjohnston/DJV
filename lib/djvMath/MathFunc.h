// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

#include <vector>

#include <stddef.h>

namespace djv
{
    namespace Math
    {
        //! Get the absolute value.
        template<typename T>
        constexpr T abs(T) noexcept;

        //! Get the anti-logarithm.
        template<typename T>
        T antiLog10(T) noexcept;

        //! Step function.
        template<typename T>
        constexpr T step(T value, T step) noexcept;

        //! Pulse function.
        template<typename T>
        constexpr T pulse(T value, T min, T max) noexcept;

        //! Bias function.
        template<typename T>
        T bias(T value, T bias) noexcept;

        //! Gain function.
        template<typename T>
        T gain(T value, T gain) noexcept;

        //! Clamp a number between two values.
        template<typename T>
        constexpr T clamp(const T&, const T& _min, const T& _max);

        //! Linear interpolation.
        template<typename T, typename U>
        constexpr T lerp(U value, T min, T max) noexcept;

        //! Smooth step function.
        template<typename T>
        constexpr T smoothStep(T value, T min, T max) noexcept;

        //! Soft clip function.
        //!
        //! References:
        //! - Ken McGaugh, "Re: [Openexr-devel] Cineon -> half conversion?"
        //!   OpenEXR Mailing List 3/2/05
        template<typename T>
        T softClip(T value, T softClip) noexcept;

        //! Get the number of digits.
        template<typename T>
        size_t getNumDigits(T) noexcept;

        //! Find the closest value. The index of the result is returned.
        template<typename T>
        size_t closest(T, const std::vector<T>&) noexcept;

        //! \name Conversion
        ///@{

        //! Get the value after the decimal.
        template<typename T>
        constexpr T getFraction(T) noexcept;

        //! Find the closest power of two.
        int toPow2(int) noexcept;

        //! Convert radians to degrees.
        constexpr float rad2deg(float) noexcept;

        //! Convert degrees to radians.
        constexpr float deg2rad(float) noexcept;

        ///@{

        //! \name Comparison
        ///@{

        //! Are the signs of both numbers the same?
        constexpr bool haveSameSign(float, float) noexcept;

        ///@}

    } // namespace Math

    bool fuzzyCompare(double a, double b, double e = .1e-9) noexcept;
    bool fuzzyCompare(float a, float b, float e = .1e-6F) noexcept;

} // namespace djv

#include <djvMath/MathFuncInline.h>

