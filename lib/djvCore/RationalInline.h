// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Core
    {
        namespace Math
        {
            inline Rational::Rational() noexcept
            {}

            inline Rational::Rational(int num, int den) noexcept :
                _num(num),
                _den(den)
            {}

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
    } // namespace Core
} // namespace djv
