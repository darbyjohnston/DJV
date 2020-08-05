// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

#include <vector>

#include <stddef.h>

namespace djv
{
    namespace Core
    {
        //! This namespace provides math functionality.
        namespace Math
        {
            static const float pi = 3.14159265359F;
            static const float pi2 = pi * 2.F;

            //! Get the absolute value.
            template<typename T>
            constexpr T abs(T);

            //! Get the anti-logarithm.
            template<typename T>
            T antiLog10(T);

            //! Step function.
            template<typename T>
            constexpr T step(T value, T step);

            //! Pulse function.
            template<typename T>
            constexpr T pulse(T value, T min, T max);

            //! Bias function.
            template<typename T>
            T bias(T value, T bias);

            //! Gain function.
            template<typename T>
            T gain(T value, T gain);

            //! Clamp a number between two values.
            template<typename T>
            constexpr T clamp(const T&, const T& _min, const T& _max);

            //! Linear interpolation.
            template<typename T, typename U>
            constexpr T lerp(U value, T min, T max);

            //! Smooth step function.
            template<typename T>
            constexpr T smoothStep(T value, T min, T max);

            //! Soft clip function.
            //!
            //! References:
            //! - Ken McGaugh, "Re: [Openexr-devel] Cineon -> half conversion?"
            //!   OpenEXR Mailing List 3/2/05
            template<typename T>
            T softClip(T value, T softClip);

            //! Get the number of digits.
            template<typename T>
            size_t getNumDigits(T);

            //! Find the closest value. The index of the result is returned.
            template<typename T>
            size_t closest(T, const std::vector<T>&);

            //! \name Random Numbers
            ///@{

            //! Get a random number between zero and one.
            float getRandom();

            //! Get a random number between zero and the given value.
            float getRandom(float);

            //! Get a random number between zero and the given value.
            int getRandom(int);

            //! Get a random number between two values.
            float getRandom(float min, float max);

            //! Get a random number between two values.
            int getRandom(int min, int max);

            //! Get a random item from a list.
            template<typename T>
            const T& getRandom(const std::vector<T>&);

            //! Seed the random value generator.
            void setRandomSeed(unsigned int);

            //! Seed the random value generator with the current time.
            void setRandomSeed();

            ///@}

            //! \name Conversion
            ///@{

            //! Get the value after the decimal.
            template<typename T>
            constexpr T getFraction(T);

            //! Find the closest power of two.
            int toPow2(int);

            //! Convert readians to degrees.
            constexpr float rad2deg(float);

            //! Convert degrees to radians.
            constexpr float deg2rad(float);

            ///@{

            //! \name Comparison
            ///@{

            //! Are the signs of both numbers the same?
            constexpr bool haveSameSign(float, float);

            ///@}

        } // namespace Math
    } // namespace Core

    bool fuzzyCompare(double a, double b, double e = .1e-9);
    bool fuzzyCompare(float a, float b, float e = .1e-6F);

} // namespace djv

#include <djvCore/MathInline.h>
