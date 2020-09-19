// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <algorithm>

namespace djv
{
    namespace Math
    {
        template<typename T>
        constexpr Range<T>::Range() noexcept
        {}

        template<typename T>
        constexpr Range<T>::Range(T minMax) noexcept :
            _min(minMax),
            _max(minMax)
        {}

        template<typename T>
        constexpr Range<T>::Range(T min, T max) :
            _min(std::min(min, max)),
            _max(std::max(min, max))
        {}

        template<typename T>
        inline Range<T>::~Range()
        {}

        template<typename T>
        inline T Range<T>::getMin() const noexcept
        {
            return _min;
        }

        template<typename T>
        inline T Range<T>::getMax() const noexcept
        {
            return _max;
        }

        template<>
        inline void Range<int>::zero() noexcept
        {
            _min = _max = 0;
        }

        template<>
        inline void Range<size_t>::zero() noexcept
        {
            _min = _max = size_t(0);
        }

        template<>
        inline void Range<float>::zero() noexcept
        {
            _min = _max = 0.F;
        }

        template<typename T>
        constexpr bool Range<T>::contains(T value) const noexcept
        {
            return value >= _min && value <= _max;
        }

        template<typename T>
        constexpr bool Range<T>::intersects(const Range<T>& value) const noexcept
        {
            return !(
                value._max < _min ||
                value._min > _max);
        }

        template<typename T>
        inline void Range<T>::expand(T value)
        {
            _min = std::min(_min, value);
            _max = std::max(_max, value);
        }

        template<typename T>
        inline void Range<T>::expand(const Range<T>& value)
        {
            _min = std::min(_min, value._min);
            _max = std::max(_max, value._max);
        }

        template<typename T>
        constexpr bool Range<T>::operator == (const Range<T>& value) const
        {
            return _min == value._min && _max == value._max;
        }

        template<typename T>
        constexpr bool Range<T>::operator != (const Range<T>& value) const
        {
            return !(*this == value);
        }

        template<typename T>
        constexpr bool Range<T>::operator < (const Range<T>& value) const
        {
            return _min < value._min;
        }

    } // namespace Math
} // namespace djv

