// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Core
    {
        namespace Range
        {
            template<typename T>
            constexpr Range<T>::Range()
            {}

            template<typename T>
            constexpr Range<T>::Range(T minMax) :
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
            inline T Range<T>::getMin() const
            {
                return _min;
            }

            template<typename T>
            inline T Range<T>::getMax() const
            {
                return _max;
            }

            template<>
            inline void Range<int>::zero()
            {
                _min = _max = 0;
            }

            template<>
            inline void Range<size_t>::zero()
            {
                _min = _max = size_t(0);
            }

            template<>
            inline void Range<float>::zero()
            {
                _min = _max = 0.F;
            }

            template<typename T>
            constexpr bool Range<T>::contains(T value) const
            {
                return value >= _min && value <= _max;
            }

            template<typename T>
            constexpr bool Range<T>::intersects(const Range<T> & value) const
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
            constexpr bool Range<T>::operator == (const Range<T> & value) const
            {
                return _min == value._min && _max == value._max;
            }

            template<typename T>
            constexpr bool Range<T>::operator != (const Range<T> & value) const
            {
                return !(*this == value);
            }

            template<typename T>
            constexpr bool Range<T>::operator < (const Range<T> & value) const
            {
                return _min < value._min;
            }

        } // namespace Range
    } // namespace Core

    template<typename T>
    inline std::ostream& operator << (std::ostream& os, const Core::Range::Range<T>& value)
    {
        os << value.getMin() << " ";
        os << value.getMax();
        return os;
    }

    template<typename T>
    inline std::istream& operator >> (std::istream& is, Core::Range::Range<T>& out)
    {
        try
        {
            is.exceptions(std::istream::failbit | std::istream::badbit);
            T min = static_cast<T>(0);
            is >> min;
            T max = static_cast<T>(0);
            is >> max;
            out = Core::Range::Range<T>(min, max);
        }
        catch (const std::exception&)
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
        return is;
    }

} // namespace djv
