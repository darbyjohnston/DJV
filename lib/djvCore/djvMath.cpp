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

//! \file djvMath.cpp

#include <djvMath.h>

#if defined(DJV_WINDOWS)
#include <windows.h>
#else
#include <sys/time.h>
#endif
#include <float.h>
#include <math.h>
#include <stdlib.h>

//------------------------------------------------------------------------------
// djvMath
//------------------------------------------------------------------------------

djvMath::~djvMath()
{}

const double djvMath::pi     = 3.1415927;
const double djvMath::piX2   = 6.2831853;
const double djvMath::piDiv2 = 1.5707963;
const double djvMath::piDiv4 = 0.7853981;

int djvMath::pow(int value, int power)
{
    return static_cast<int>(::floor(
        ::pow(static_cast<double>(value), static_cast<double>(power))));
}

double djvMath::pow(double value, double power)
{
    return ::pow(value, power);
}

double djvMath::sqrt(double value)
{
    return ::sqrt(value);
}

double djvMath::exp(double value)
{
    return ::exp(value);
}

double djvMath::log(double value)
{
    return ::log(value);
}

double djvMath::log10(double value)
{
    return ::log10(value);
}

double djvMath::antiLog10(double value)
{
    return exp(value * log(10.0));
}

double djvMath::bias(double value, double bias)
{
    return pow(value, log(bias) / log(0.5));
}

namespace
{

const double almostZero = FLT_MIN * 2.0;
const double almostOne  = 1.0 - almostZero;

} // namespace

double djvMath::gain(double value, double gain)
{
    if (value < almostZero)
    {
        return 0.0;
    }
    else if (value > almostOne)
    {
        return 1.0;
    }

    const double tmp = log(1.0 - gain) / log(0.5);

    if (value < 0.5)
    {
        return pow(2.0 * value, tmp) / 2.0;
    }
    else
    {
        return 1.0 - pow(2.0 * (1.0 - value), tmp) / 2.0;
    }
}

double djvMath::softClip(double value, double softClip)
{
    const double tmp = 1.0 - softClip;

    if (value > tmp)
    {
        value = tmp + (1.0 - exp(-(value - tmp) / softClip)) * softClip;
    }

    return value;
}

double djvMath::sin(double value)
{
    return ::sin(value);
}
double djvMath::cos(double value)
{
    return ::cos(value);
}
double djvMath::tan(double value)
{
    return ::tan(value);
}

double djvMath::arcSin(double value)
{
    return ::asin(value);
}
double djvMath::arcCos(double value)
{
    return ::acos(value);
}
double djvMath::arcTan(double value)
{
    return ::atan(value);
}
double djvMath::arcTan(double a, double b)
{
    return ::atan2(a, b);
}

//! \todo What is the correct way to handle zero values for the trigonometry
//! functions?

double djvMath::sec(double value)
{
    const double tmp = cos(value);
    
    return tmp != 0.0 ? (1.0 / tmp) : 0.0;
}

double djvMath::coSec(double value)
{
    const double tmp = sin(value);
    
    return tmp != 0.0 ? (1.0 / tmp) : 0.0;
}

double djvMath::coTan(double value)
{
    const double tmp = tan(value);
    
    return tmp != 0.0 ? (1.0 / tmp) : 0.0;
}

namespace
{

double _rand()
{
    return ::rand() / static_cast<double>(RAND_MAX);
}

} // namespace

double djvMath::rand()
{
    return _rand();
}

double djvMath::rand(double value)
{
    return value * _rand();
}

double djvMath::rand(double min, double max)
{
    return min + (max - min) * _rand();
}

void djvMath::randSeed(unsigned int value)
{
    if (! value)
    {
        //! Seed the random number generator with the current time.

#if defined(DJV_WINDOWS)

        value = ::GetTickCount();

#else // DJV_WINDOWS

        struct ::timeval tmp = { 0, 0 };
        ::gettimeofday(&tmp, 0);
        value = tmp.tv_usec;

#endif // DJV_WINDOWS
    }

    ::srand(value);
}

int djvMath::floor(double value)
{
    return static_cast<int>(::floor(value));
}

int djvMath::ceil(double value)
{
    return static_cast<int>(::ceil(value));
}

int djvMath::round(double value)
{
    return static_cast<int>(::round(value));
}

bool djvMath::fuzzyCompare(float a, float b)
{
    return fuzzyCompare(a, b, FLT_EPSILON);
}

bool djvMath::fuzzyCompare(float a, float b, float precision)
{
    return ::fabsf(a - b) < precision;
}

bool djvMath::fuzzyCompare(double a, double b)
{
    return fuzzyCompare(a, b, DBL_EPSILON);
}

bool djvMath::fuzzyCompare(double a, double b, double precision)
{
    return ::fabs(a - b) < precision;
}

