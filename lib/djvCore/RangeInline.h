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
                min(minMax),
                max(minMax)
            {}

            template<typename T>
            constexpr Range<T>::Range(T min, T max) :
                min(min),
                max(max)
            {}

            template<typename T>
            inline Range<T>::~Range()
            {}

            template<>
            inline void Range<int>::zero()
            {
                min = max = 0;
            }

            template<>
            inline void Range<size_t>::zero()
            {
                min = max = size_t(0);
            }

            template<>
            inline void Range<float>::zero()
            {
                min = max = 0.F;
            }

            template<typename T>
            constexpr bool Range<T>::contains(T value) const
            {
                return value >= min && value <= max;
            }

            template<typename T>
            constexpr bool Range<T>::intersects(const Range<T> & value) const
            {
                return !(
                    value.max < min ||
                    value.min > max);
            }

            template<typename T>
            inline void Range<T>::expand(T value)
            {
                min = std::min(min, value);
                max = std::max(max, value);
            }

            template<typename T>
            inline void Range<T>::expand(const Range<T>& value)
            {
                min = std::min(min, value.min);
                max = std::max(max, value.max);
            }

            template<typename T>
            inline void Range<T>::sort()
            {
                if (max < min)
                {
                    const T tmp = min;
                    min = max;
                    max = tmp;
                }
            }

            template<typename T>
            constexpr bool Range<T>::operator == (const Range<T> & value) const
            {
                return min == value.min && max == value.max;
            }

            template<typename T>
            constexpr bool Range<T>::operator != (const Range<T> & value) const
            {
                return !(*this == value);
            }

            template<typename T>
            constexpr bool Range<T>::operator < (const Range<T> & value) const
            {
                return min < value.min;
            }

        } // namespace Range
    } // namespace Core

    template<typename T>
    inline std::ostream& operator << (std::ostream& is, const Core::Range::Range<T>& value)
    {
        is << value.min << " ";
        is << value.max;
        return is;
    }

    template<typename T>
    inline std::istream& operator >> (std::istream& os, Core::Range::Range<T>& out)
    {
        os.exceptions(std::istream::failbit | std::istream::badbit);
        os >> out.min;
        os >> out.max;
        return os;
    }

} // namespace djv
