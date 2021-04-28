// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

#include <sstream>

#include <stddef.h>

namespace djv
{
    namespace Math
    {
        //! Range of numbers.
        template<typename T>
        class Range
        {
        public:
            constexpr Range() noexcept;
            explicit constexpr Range(T minMax) noexcept;
            constexpr Range(T min, T max);
            ~Range();

            T getMin() const noexcept;
            T getMax() const noexcept;

            //! \name Utility
            ///@{

            void zero() noexcept;

            constexpr bool contains(T) const noexcept;

            constexpr bool intersects(const Range<T>&) const noexcept;

            void expand(T);
            void expand(const Range<T>&);

            ///@}

            constexpr bool operator == (const Range<T>&) const;
            constexpr bool operator != (const Range<T>&) const;
            constexpr bool operator  < (const Range<T>&) const;

        private:
            T _min = static_cast<T>(0);
            T _max = static_cast<T>(0);
        };

        //! Integer range.
        typedef Range<int> IntRange;

        //! Size range.
        typedef Range<size_t> SizeTRange;

        //! Floating point range.
        typedef Range<float> FloatRange;

    } // namespace Math
    
    template<typename T>
    std::ostream& operator << (std::ostream&, const Math::Range<T>&);

    //! Throws:
    //! - std::exception
    template<typename T>
    std::istream& operator >> (std::istream&, Math::Range<T>&);
    
} // namespace djv

#include <djvMath/RangeInline.h>

