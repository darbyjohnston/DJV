// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/String.h>

#include <sstream>

namespace djv
{
    namespace Core
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

                //! \name Utilities
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

        } // namespace Math

        //! This typedef provides an integer range.
        typedef Math::Range<int> IntRange;

        //! This typedef provides a size_t range.
        typedef Math::Range<size_t> SizeTRange;

        //! This typedef provides a floating point range.
        typedef Math::Range<float> FloatRange;

    } // namespace Core

    template<typename T>
    std::ostream& operator << (std::ostream&, const Core::Math::Range<T>&);

    //! Throws:
    //! - std::exception
    template<typename T>
    std::istream& operator >> (std::istream&, Core::Math::Range<T>&);

} // namespace djv

#include <djvCore/RangeInline.h>
