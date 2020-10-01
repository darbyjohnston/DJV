// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

#include <stddef.h>

namespace djv
{
    namespace Math
    {
        //! This class provides a range of numbers.
        template<typename T>
        struct Range
        {
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

        //! This typedef provides an integer range.
        typedef Range<int> IntRange;

        //! This typedef provides a size_t range.
        typedef Range<size_t> SizeTRange;

        //! This typedef provides a floating point range.
        typedef Range<float> FloatRange;

    } // namespace Math
} // namespace djv

#include <djvMath/RangeInline.h>

