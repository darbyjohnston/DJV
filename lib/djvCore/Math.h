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

namespace djv
{
    namespace Core
    {
        //! This class provides math utilities.
        //!
        //! References:
        //!
        //! - Andrew Glassner, "Graphics Gems"
        //! - David S. Ebert, F. Kenton Musgrave, Darwyn Peachey, Ken Perlin,
        //!   Steven Worley, "Texturing & Modeling, A Procedural Approach"
        class Math
        {
        public:
            virtual ~Math() = 0;

            static const float pi;     //!< Pi
            static const float piX2;   //!< Pi times two
            static const float piDiv2; //!< Pi divided by two
            static const float piDiv4; //!< Pi divided by four

            //! Get the absolute value.
            template<typename T>
            static constexpr T abs(T);

            //! Raise a value to a power.
            static int pow(int value, int power);

            //! Raise a value to a power.
            static float pow(float value, float power);

            //! Get the square root of a value.
            static float sqrt(float);

            //! Exponential.
            static float exp(float);

            //! Logarithm.
            static float log(float);

            //! Logarithm.
            static float log10(float);

            //! Logarithm.
            static float antiLog10(float);

            //! Modulus.
            template<typename T>
            static inline T mod(T value, T mod);

            //! Wrap a number between two values.
            template<typename T>
            static constexpr T wrap(T value, T min, T max);

            //! Step function.
            template<typename T>
            static constexpr T step(T value, T step);

            //! Pulse function.
            template<typename T>
            static constexpr T pulse(T value, T min, T max);

            //! Bias function.
            static float bias(float value, float bias);

            //! Gain function.
            static float gain(float value, float gain);

            //! Return the minimum of two values.
            template<typename T>
            static constexpr T min(const T &, const T &);

            //! Return the maximum of two values.
            template<typename T>
            static constexpr T max(const T &, const T &);

            //! Clamp a number between two values.
            template<typename T>
            static constexpr T clamp(T, T min, T max);

            //! Linear interpolation.
            template<typename T, typename U>
            static constexpr T lerp(U value, T min, T max);

            //! Smooth step function.
            static constexpr float smoothStep(float value, float min, float max);

            //! Soft clip function.
            //!
            //! References:
            //!
            //! - Ken McGaugh, "Re: [Openexr-devel] Cineon -> half conversion?"
            //!   OpenEXR Mailing List 3/2/05
            static float softClip(float value, float softClip);

            //! Sine function.
            static float sin(float);

            //! Cosine function.
            static float cos(float);

            //! Secant function.
            static float sec(float);

            //! Tangent function.
            static float tan(float);

            //! Inverse sine function.
            static float arcSin(float);

            //! Inverse cosine function.
            static float arcCos(float);

            //! Inverse tangent function.
            static float arcTan(float);

            //! Inverse tangent function.
            static float arcTan(float, float);

            //! Cosecant function.
            static float coSec(float);

            //! Cotangent function.
            static float coTan(float);

            //! Get a random number between zero and one.
            static float rand();

            //! Get a random number between zero and the given value.
            static float rand(float);

            //! Get a random number between two values.
            static float rand(float min, float max);

            //! Seed the random value generator. A value of zero uses a random seed.
            static void randSeed(unsigned int = 0);

            //! Round to the smallest integer value.
            static int floor(float);

            //! Round to the largest integer value.
            static int ceil(float);

            //! Round to an integer value.
            static int round(float);

            //! Get the value after the decimal.
            static constexpr float fraction(float);

            //! Find the closest power of two.
            static inline int toPow2(int);

            //! Convert degrees to radians.
            static constexpr float degreesToRadians(float);

            //! Convert radians to degrees.
            static constexpr float radiansToDegrees(float);

            //! Floating point comparison.
            static bool fuzzyCompare(float, float);

            //! Floating point comparison.
            static bool fuzzyCompare(float, float, float precision);

            //! Floating point comparison.    
            static bool fuzzyCompare(double, double);

            //! Floating point comparison.
            static bool fuzzyCompare(double, double, double precision);
        };

    } // namespace Core
} // namespace djv

#include <djvCore/MathInline.h>

