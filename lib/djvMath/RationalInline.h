// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvMath/MathFunc.h>

namespace djv
{
    namespace Math
    {
        inline int Rational::getNum() const noexcept
        {
            return _num;
        }

        inline int Rational::getDen() const noexcept
        {
            return _den;
        }

        inline bool Rational::isValid() const noexcept
        {
            return _num != 0 && _den != 0;
        }

        inline float Rational::toFloat() const noexcept
        {
            return static_cast<float>(_num) / static_cast<float>(_den);
        }
        
        inline Rational Rational::fromFloat(float value)
        {
            //! \bug Implement a proper floating-point to rational number conversion.
            //! Check-out: OpenEXR\IlmImf\ImfRational.h
            return Rational(static_cast<int>(round(value)), 1);
        }

        inline Rational Rational::swap() const noexcept
        {
            return Rational(_den, _num);
        }

        inline bool Rational::operator == (const Rational& other) const noexcept
        {
            return _num == other._num && _den == other._den;
        }

        inline bool Rational::operator != (const Rational& other) const noexcept
        {
            return !(*this == other);
        }

    } // namespace Math
} // namespace djv
