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

#include <djvCore/Core.h>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

namespace djv
{
    namespace Core
    {
        //! This namespace provides math functionality.
        namespace Math
        {
            static const float pi = 3.14159265359f;
            static const float pi2 = pi * 2.f;

            //! Get the absolute value.
            template<typename T>
            constexpr T abs(T);

            //! Get the anti-logarithm.
            template<typename T>
            inline T antiLog10(T);

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
            constexpr T clamp(const T &, const T & _min, const T & _max);

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
            inline T softClip(T value, T softClip);

            template<typename T>
            inline size_t getNumDigits(T);

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
            inline const T & getRandom(const std::vector<T> &);

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
            inline int toPow2(int);

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

    inline bool fuzzyCompare(double a, double b, double e = .1e-9);
    inline bool fuzzyCompare(float a, float b, float e = .1e-6f);

} // namespace djv

#include <djvCore/MathInline.h>
