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
    inline std::ostream & operator << (std::ostream & s, const Core::Range::Range<T> & value)
    {
        s << value.min << " ";
        s << value.max;
        return s;
    }

    template<typename T>
    inline std::istream & operator >> (std::istream & s, Core::Range::Range<T> & out)
    {
        s >> out.min;
        s >> out.max;
        return s;
    }

} // namespace djv
