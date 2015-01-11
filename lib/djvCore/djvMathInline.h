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

//! \file djvMathInline.h

//------------------------------------------------------------------------------
// djvMath
//------------------------------------------------------------------------------

template<class T>
inline T djvMath::abs(T value)
{
    return value < T(0) ? -value : value;
}

template<class T>
inline T djvMath::mod(T value, T mod)
{
    T tmp = value;

    if (mod != T(0))
    {
        tmp = value - (static_cast<int>(value / mod) * mod);

        if (tmp < T(0))
        {
            tmp += mod;
        }
    }

    return tmp;
}

template<class T>
inline T djvMath::wrap(T value, T min, T max)
{
    return max > min ?
           (min + mod(value - min, max - min + static_cast<T>(1))) :
           min;
}

template<class T>
inline T djvMath::step(T value, T step)
{
    return value <= step ? value : step;
}

template<class T>
inline T djvMath::pulse(T value, T min, T max)
{
    return step(value, max) - step(value, min);
}

template<class T>
inline T djvMath::min(const T & a, const T & b)
{
    return a < b ? a : b;
}

template<class T>
inline T djvMath::max(const T & a, const T & b)
{
    return a > b ? a : b;
}

template<class T>
inline T djvMath::clamp(T value, T min, T max)
{
    return value < min ? min : (value > max ? max : value);
}

template<class T, class U>
inline T djvMath::lerp(U value, T min, T max)
{
    return min + T(value * (max - min));
}

inline double djvMath::smoothStep(double value, double min, double max)
{
    return lerp(value * value * (3.0 - (2.0 * value)), min, max);
}

inline double djvMath::fraction(double value)
{
    return value - static_cast<int>(value);
}

inline int djvMath::toPow2(int value)
{
    int i = 0;

    for (; value > (1 << i); ++i)
        ;

    return 1 << i;
}

inline double djvMath::degreesToRadians(double value)
{
    static const double m = 1.0 / 360.0 * piX2;
    
    return value * m;
}

inline double djvMath::radiansToDegrees(double value)
{
    static const double m = 1.0 / piX2 * 360.0;
    
    return value * m;
}

