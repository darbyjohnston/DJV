// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvMath/Rational.h>

#include <math.h>

namespace djv
{
    namespace Math
    {
        Rational::Rational() noexcept
        {}

        Rational::Rational(int num, int den) noexcept :
            _num(num),
            _den(den)
        {}

        int Rational::getNum() const noexcept
        {
            return _num;
        }

        int Rational::getDen() const noexcept
        {
            return _den;
        }

        bool Rational::isValid() const noexcept
        {
            return _num != 0 && _den != 0;
        }

        float Rational::toFloat() const noexcept
        {
            return static_cast<float>(_num) / static_cast<float>(_den);
        }
        
        Rational Rational::fromFloat(float value)
        {
            //! \bug Implement a proper floating-point to rational number conversion.
            //! Check-out: OpenEXR\IlmImf\ImfRational.h
            return Rational(static_cast<int>(round(value)), 1);
        }

        Rational Rational::swap() const noexcept
        {
            return Rational(_den, _num);
        }

        bool Rational::operator == (const Rational& other) const noexcept
        {
            return _num == other._num && _den == other._den;
        }

        bool Rational::operator != (const Rational& other) const noexcept
        {
            return !(*this == other);
        }

    } // namespace Math
} // namespace djv

