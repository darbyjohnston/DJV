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

#include <djvCore/Math.h>

#if defined(DJV_WINDOWS)
#include <windows.h>
#else
#include <sys/time.h>
#endif
#include <float.h>
#include <math.h>
#include <stdlib.h>

namespace djv
{
    namespace Core
    {
        Math::~Math()
        {}

        const float Math::pi = 3.1415927f;
        const float Math::piX2 = 6.2831853f;
        const float Math::piDiv2 = 1.5707963f;
        const float Math::piDiv4 = 0.7853981f;

        int Math::pow(int value, int power)
        {
            return static_cast<int>(::floor(::pow(static_cast<float>(value), static_cast<float>(power))));
        }

        float Math::pow(float value, float power)
        {
            return ::powf(value, power);
        }

        float Math::sqrt(float value)
        {
            return ::sqrtf(value);
        }

        float Math::exp(float value)
        {
            return ::expf(value);
        }

        float Math::log(float value)
        {
            return ::logf(value);
        }

        float Math::log10(float value)
        {
            return ::log10f(value);
        }

        float Math::antiLog10(float value)
        {
            return exp(value * log(10.f));
        }

        float Math::bias(float value, float bias)
        {
            return pow(value, log(bias) / log(0.5));
        }

        namespace
        {
            const float almostZero = FLT_MIN * 2.f;
            const float almostOne = 1.f - almostZero;

        } // namespace

        float Math::gain(float value, float gain)
        {
            if (value < almostZero)
            {
                return 0.f;
            }
            else if (value > almostOne)
            {
                return 1.f;
            }
            const float tmp = log(1.f - gain) / log(0.5);
            if (value < 0.5)
            {
                return pow(2.f * value, tmp) / 2.f;
            }
            else
            {
                return 1.f - pow(2.f * (1.f - value), tmp) / 2.f;
            }
        }

        float Math::softClip(float value, float softClip)
        {
            const float tmp = 1.f - softClip;
            if (value > tmp)
            {
                value = tmp + (1.f - exp(-(value - tmp) / softClip)) * softClip;
            }
            return value;
        }

        float Math::sin(float value)
        {
            return ::sinf(value);
        }

        float Math::cos(float value)
        {
            return ::cosf(value);
        }

        float Math::tan(float value)
        {
            return ::tanf(value);
        }

        float Math::arcSin(float value)
        {
            return ::asinf(value);
        }

        float Math::arcCos(float value)
        {
            return ::acosf(value);
        }

        float Math::arcTan(float value)
        {
            return ::atanf(value);
        }

        float Math::arcTan(float a, float b)
        {
            return ::atan2f(a, b);
        }

        //! \todo What is the correct way to handle zero values for the trigonometry
        //! functions?
        float Math::sec(float value)
        {
            const float tmp = cos(value);
            return tmp != 0.f ? (1.f / tmp) : 0.f;
        }

        float Math::coSec(float value)
        {
            const float tmp = sin(value);
            return tmp != 0.f ? (1.f / tmp) : 0.f;
        }

        float Math::coTan(float value)
        {
            const float tmp = tan(value);
            return tmp != 0.f ? (1.f / tmp) : 0.f;
        }

        namespace
        {
            float _rand()
            {
                return ::rand() / static_cast<float>(RAND_MAX);
            }

        } // namespace

        float Math::rand()
        {
            return _rand();
        }

        float Math::rand(float value)
        {
            return value * _rand();
        }

        float Math::rand(float min, float max)
        {
            return min + (max - min) * _rand();
        }

        void Math::randSeed(unsigned int value)
        {
            if (!value)
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

        int Math::floor(float value)
        {
            return static_cast<int>(::floor(value));
        }

        int Math::ceil(float value)
        {
            return static_cast<int>(::ceil(value));
        }

        int Math::round(float value)
        {
            return static_cast<int>(::round(value));
        }

        bool Math::fuzzyCompare(float a, float b)
        {
            return fuzzyCompare(a, b, FLT_EPSILON);
        }

        bool Math::fuzzyCompare(float a, float b, float precision)
        {
            return ::fabsf(a - b) < precision;
        }

        bool Math::fuzzyCompare(double a, double b)
        {
            return fuzzyCompare(a, b, DBL_EPSILON);
        }

        bool Math::fuzzyCompare(double a, double b, double precision)
        {
            return ::fabs(a - b) < precision;
        }

    } // namespace Core
} // namespace djv
