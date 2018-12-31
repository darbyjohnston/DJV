//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <algorithm>

#include <float.h>
#include <math.h>

namespace djv
{
    namespace Core
    {
        namespace Math
        {
            template<class T>
            inline T abs(T value)
            {
                return value < T(0) ? -value : value;
            }

            template<>
            inline float antiLog10(float value)
            {
                return exp(value * log(10.f));
            }

            template<>
            inline double antiLog10(double value)
            {
                return exp(value * log(10.));
            }

            template<class T>
            inline T step(T value, T step)
            {
                return value <= step ? value : step;
            }

            template<class T>
            inline T pulse(T value, T min, T max)
            {
                return step(value, max) - step(value, min);
            }

            template<>
            inline float bias(float value, float bias)
            {
                return pow(value, log(bias) / log(.5f));
            }

            template<>
            inline double bias(double value, double bias)
            {
                return pow(value, log(bias) / log(.5));
            }

            template<>
            inline float gain(float value, float gain)
            {
                static const float almostZero = FLT_MIN * 2.f;
                static const float almostOne = 1.f - almostZero;
                if (value < almostZero)
                {
                    return 0.f;
                }
                else if (value > almostOne)
                {
                    return 1.f;
                }
                const float tmp = log(1.f - gain) / log(.5f);
                if (value < .5f)
                {
                    return pow(2.f * value, tmp) / 2.f;
                }
                else
                {
                    return 1.f - pow(2.f * (1.f - value), tmp) / 2.f;
                }
            }

            template<>
            inline double gain(double value, double gain)
            {
                static const double almostZero = DBL_MIN * 2.;
                static const double almostOne = 1. - almostZero;
                if (value < almostZero)
                {
                    return 0.;
                }
                else if (value > almostOne)
                {
                    return 1.;
                }
                const double tmp = log(1 - gain) / log(5.);
                if (value < 5.)
                {
                    return pow(2. * value, tmp) / 2.;
                }
                else
                {
                    return 1. - pow(2 * (1. - value), tmp) / 2.;
                }
            }

            template<typename T>
            inline T clamp(const T& v, const T& _min, const T& _max)
            {
                return std::max(std::min(v, _max), _min);
            }

            template<class T, class U>
            inline T lerp(U value, T min, T max)
            {
                return min + T(value * (max - min));
            }

            inline float smoothStep(float value, float min, float max)
            {
                return lerp(value * value * (3.f - (2.f * value)), min, max);
            }

            inline double smoothStep(double value, double min, double max)
            {
                return lerp(value * value * (3. - (2. * value)), min, max);
            }

            template<typename T>
            inline T softClip(T value, T softClip)
            {
                const T tmp = T(1) - softClip;
                if (value > tmp)
                {
                    value = tmp + (T(1) - exp(-(value - tmp) / softClip)) * softClip;
                }
                return value;
            }

            template<typename T>
            inline size_t getNumDigits(T value)
            {
                value = abs(value);
                return
                    (value < 10 ? 1 :
                    (value < 100 ? 2 :
                    (value < 1000 ? 3 :
                    (value < 10000 ? 4 :
                    (value < 100000 ? 5 :
                    (value < 1000000 ? 6 :
                    (value < 10000000 ? 7 :
                    (value < 100000000 ? 8 :
                    (value < 1000000000 ? 9 : 10)))))))));
            }

            template<typename T>
            inline const T& getRandom(const std::vector<T>& value)
            {
                return value[getRandom(0, static_cast<int>(value.size()) - 1)];
            }

            inline float getFraction(float value)
            {
                return value - static_cast<int>(value);
            }

            inline double getFraction(double value)
            {
                return value - static_cast<int>(value);
            }

            inline int toPow2(int value)
            {
                int i = 0;
                for (; value > (1 << i); ++i)
                    ;
                return 1 << i;
            }

            inline float rad2deg(float value)
            {
                return value / pi2 * 360.f;
            }

            inline float deg2rad(float value)
            {
                return value / 360.f * pi2;
            }

            inline bool haveSameSign(float a, float b)
            {
                return ((a > 0.f && b > 0.f) || (a < 0.f && b < 0.f));
            }

        } // namespace Math
    } // namespace Core

    inline bool fuzzyCompare(double a, double b, double e)
    {
        return fabs(a - b) < e;
    }

    inline bool fuzzyCompare(float a, float b, float e)
    {
        return fabsf(a - b) < e;
    }

} // namespace djv
