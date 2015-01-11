//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
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

//! \file djvMath.h

#ifndef DJV_MATH_H
#define DJV_MATH_H

#include <djvConfig.h>
#include <djvCoreExport.h>

//! \addtogroup djvCoreMath
//@{

//------------------------------------------------------------------------------
//! \class djvMath
//!
//! This class provides math utilities.
//!
//! References:
//!
//! - Andrew Glassner, "Graphics Gems"
//! - David S. Ebert, F. Kenton Musgrave, Darwyn Peachey, Ken Perlin,
//!   Steven Worley, "Texturing & Modeling, A Procedural Approach"
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvMath
{
public:

    //! Destructor.
    
    virtual ~djvMath() = 0;
    
    static const double pi;     //!< Pi
    static const double piX2;   //!< Pi times two
    static const double piDiv2; //!< Pi divided by two
    static const double piDiv4; //!< Pi divided by four

    //! Get the absolute value.

    template<typename T>
    static inline T abs(T);

    //! Raise a value to a power.

    static int pow(int value, int power);

    //! Raise a value to a power.

    static double pow(double value, double power);

    //! Get the square root of a value.

    static double sqrt(double);

    //! Exponential.

    static double exp(double);

    //! Logarithm.

    static double log(double);

    //! Logarithm.

    static double log10(double);

    //! Logarithm.

    static double antiLog10(double);

    //! Modulus.

    template<typename T>
    static inline T mod(T value, T mod);

    //! Wrap a number between two values.

    template<typename T>
    static inline T wrap(T value, T min, T max);

    //! Step function.

    template<typename T>
    static inline T step(T value, T step);

    //! Pulse function.

    template<typename T>
    static inline T pulse(T value, T min, T max);

    //! Bias function.

    static double bias(double value, double bias);

    //! Gain function.

    static double gain(double value, double gain);

    //! Return the minimum of two values.

    template<typename T>
    static inline T min(const T &, const T &);

    //! Return the maximum of two values.

    template<typename T>
    static inline T max(const T &, const T &);

    //! Clamp a number between two values.

    template<typename T>
    static inline T clamp(T, T min, T max);

    //! Linear interpolation.

    template<typename T, typename U>
    static inline T lerp(U value, T min, T max);

    //! Smooth step function.

    static inline double smoothStep(double value, double min, double max);

    //! Soft clip function.
    //!
    //! References:
    //!
    //! - Ken McGaugh, "Re: [Openexr-devel] Cineon -> half conversion?"
    //!   OpenEXR Mailing List 3/2/05

    static double softClip(double value, double softClip);

    //! Sine function.

    static double sin(double);

    //! Cosine function.

    static double cos(double);

    //! Secant function.

    static double sec(double);

    //! Tangent function.

    static double tan(double);

    //! Inverse sine function.

    static double arcSin(double);

    //! Inverse cosine function.

    static double arcCos(double);

    //! Inverse tangent function.

    static double arcTan(double);

    //! Inverse tangent function.

    static double arcTan(double, double);

    //! Cosecant function.

    static double coSec(double);

    //! Cotangent function.

    static double coTan(double);

    //! Get a random number between zero and one.

    static double rand();

    //! Get a random number between zero and the given value.

    static double rand(double);

    //! Get a random number between two values.

    static double rand(double min, double max);

    //! Seed the random value generator. A value of zero uses a random seed.

    static void randSeed(unsigned int = 0);

    //! Round to the smallest integer value.

    static int floor(double);

    //! Round to the largest integer value.

    static int ceil(double);

    //! Round to an integer value.

    static int round(double);

    //! Get the value after the decimal.

    static inline double fraction(double);

    //! Find the closest power of two.

    static inline int toPow2(int);

    //! Convert degrees to radians.

    static inline double degreesToRadians(double);

    //! Convert radians to degrees.

    static inline double radiansToDegrees(double);
    
    //! Floating point comparison.
    
    static bool fuzzyCompare(float, float);
    
    //! Floating point comparison.
    
    static bool fuzzyCompare(float, float, float precision);
    
    //! Floating point comparison.
    
    static bool fuzzyCompare(double, double);
    
    //! Floating point comparison.
    
    static bool fuzzyCompare(double, double, double precision);
};

//@} // djvCoreMath

#include <djvMathInline.h>

#endif // DJV_MATH_H

