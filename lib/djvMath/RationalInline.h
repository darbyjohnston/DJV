// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvMath/MathFunc.h>

namespace djv
{
    namespace Math
    {
        template<typename T>
        inline Rational<T>::Rational() noexcept
        {}

        template<typename T>
        inline Rational<T>::Rational(T num, T den) noexcept :
            _num(num),
            _den(den)
        {}

        template<typename T>
        inline T Rational<T>::getNum() const noexcept
        {
            return _num;
        }

        template<typename T>
        inline T Rational<T>::getDen() const noexcept
        {
            return _den;
        }

        template<typename T>
        inline bool Rational<T>::isValid() const noexcept
        {
            return _num != 0 && _den != 0;
        }

        template<typename T>
        inline float Rational<T>::toFloat() const noexcept
        {
            return _den > 0 ? static_cast<float>(_num) / static_cast<float>(_den) : 0.F;
        }

        template<typename T>
        inline Rational<T> Rational<T>::fromFloat(float value)
        {
            //! \bug Implement a proper floating-point to rational number conversion.
            //! Check-out: OpenEXR\IlmImf\ImfRational.h
            return Rational<T>(static_cast<T>(roundf(value)), 1);
        }

        template<typename T>
        inline Rational<T> Rational<T>::swap() const noexcept
        {
            return Rational<T>(_den, _num);
        }

        template<typename T>
        inline bool Rational<T>::operator == (const Rational& other) const noexcept
        {
            return _num == other._num && _den == other._den;
        }

        template<typename T>
        inline bool Rational<T>::operator != (const Rational& other) const noexcept
        {
            return !(*this == other);
        }

    } // namespace Math
} // namespace djv
