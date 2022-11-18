// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <algorithm>
#include <limits>

#include <float.h>
#include <math.h>

namespace djv
{
    namespace Math
    {
        template<class T>
        constexpr T abs(T value) noexcept
        {
            return value < T(0) ? -value : value;
        }

        template<>
        inline float antiLog10(float value) noexcept
        {
            return exp(value * log(10.F));
        }

        template<>
        inline double antiLog10(double value) noexcept
        {
            return exp(value * log(10.));
        }

        template<class T>
        constexpr T step(T value, T step) noexcept
        {
            return value <= step ? value : step;
        }

        template<class T>
        constexpr T pulse(T value, T min, T max) noexcept
        {
            return step(value, max) - step(value, min);
        }

        template<>
        inline float bias(float value, float bias) noexcept
        {
            return pow(value, log(bias) / log(.5F));
        }

        template<>
        inline double bias(double value, double bias) noexcept
        {
            return pow(value, log(bias) / log(.5));
        }

        template<>
        inline float gain(float value, float gain) noexcept
        {
            const float almostZero = FLT_MIN * 2.F;
            const float almostOne = 1.F - almostZero;
            if (value < almostZero)
            {
                return 0.F;
            }
            else if (value > almostOne)
            {
                return 1.F;
            }
            const float tmp = logf(1.F - gain) / logf(.5F);
            if (value < .5F)
            {
                return powf(2.F * value, tmp) / 2.F;
            }
            else
            {
                return 1.F - powf(2.F * (1.F - value), tmp) / 2.F;
            }
        }

        template<>
        inline double gain(double value, double gain) noexcept
        {
            const double almostZero = DBL_MIN * 2.;
            const double almostOne = 1. - almostZero;
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
        constexpr T clamp(const T& v, const T& _min, const T& _max)
        {
            return std::max(std::min(v, _max), _min);
        }

        template<class T, class U>
        constexpr T lerp(U value, T min, T max) noexcept
        {
            return min + T(value * (max - min));
        }

        constexpr float smoothStep(float value, float min, float max) noexcept
        {
            return lerp(value * value * (3.F - (2.F * value)), min, max);
        }

        constexpr double smoothStep(double value, double min, double max) noexcept
        {
            return lerp(value * value * (3. - (2. * value)), min, max);
        }

        template<typename T>
        inline T softClip(T value, T softClip) noexcept
        {
            const T tmp = T(1) - softClip;
            if (value > tmp)
            {
                value = tmp + (T(1) - exp(-(value - tmp) / softClip)) * softClip;
            }
            return value;
        }

        template<typename T>
        inline size_t getNumDigits(T value) noexcept
        {
            size_t out = 0;
            value = abs(value);
            if (value < T(10))
            {
                out = 1;
            }
            else if (value < T(100))
            {
                out = 2;
            }
            else if (value < T(1000))
            {
                out = 3;
            }
            else if (value < T(10000))
            {
                out = 4;
            }
            else if (value < T(100000))
            {
                out = 5;
            }
            else if (value < T(1000000))
            {
                out = 6;
            }
            else if (value < T(10000000))
            {
                out = 7;
            }
            else if (value < T(100000000))
            {
                out = 8;
            }
            else if (value < T(1000000000))
            {
                out = 9;
            }
            return out;
        }

        template<typename T>
        size_t closest(T value, const std::vector<T>& list) noexcept
        {
            size_t out = 0;
            T minAbs = std::numeric_limits<T>::max();
            const size_t size = list.size();
            for (size_t i = 0; i < size; ++i)
            {
                const T abs = Math::abs(list[i] - value);
                if (abs < minAbs)
                {
                    out = i;
                    minAbs = abs;
                }
            }
            return out;
        }

        template<>
        constexpr float getFraction(float value) noexcept
        {
            return value - static_cast<int>(value);
        }

        template<>
        constexpr double getFraction(double value) noexcept
        {
            return value - static_cast<int>(value);
        }

        inline int toPow2(int value) noexcept
        {
            int i = 0;
            for (; value > (1 << i); ++i)
                ;
            return 1 << i;
        }

        constexpr float rad2deg(float value) noexcept
        {
            return value / (3.14159265359F * 2.F) * 360.F;
        }

        constexpr float deg2rad(float value) noexcept
        {
            return value / 360.F * (3.14159265359F * 2.F);
        }

        constexpr bool haveSameSign(float a, float b) noexcept
        {
            return ((a > 0.F && b > 0.F) || (a < 0.F && b < 0.F));
        }

    } // namespace Math

    inline bool fuzzyCompare(double a, double b, double e) noexcept
    {
        return fabs(a - b) < e;
    }

    inline bool fuzzyCompare(float a, float b, float e) noexcept
    {
        return fabsf(a - b) < e;
    }

} // namespace djv
