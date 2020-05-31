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
        //! This namespace provides number range functionality.
        namespace Range
        {
            //! This class provides a range of numbers.
            template<typename T>
            struct Range
            {
                constexpr Range();
                explicit constexpr Range(T minMax);
                constexpr Range(T min, T max);
                virtual ~Range();

                T getMin() const;
                T getMax() const;

                //! \name Utilities
                ///@{

                void zero();

                constexpr bool contains(T) const;

                constexpr bool intersects(const Range<T> &) const;

                void expand(T);
                void expand(const Range<T> &);

                ///@}

                constexpr bool operator == (const Range<T> &) const;
                constexpr bool operator != (const Range<T> &) const;
                constexpr bool operator  < (const Range<T> &) const;

            private:
                T _min = static_cast<T>(0);
                T _max = static_cast<T>(0);
            };

        } // namespace Range

        //! This typedef provides an integer range.
        typedef Range::Range<int> IntRange;

        //! This typedef provides a size_t range.
        typedef Range::Range<size_t> SizeTRange;

        //! This typedef provides a floating point range.
        typedef Range::Range<float> FloatRange;

    } // namespace Core

    template<typename T>
    std::ostream & operator << (std::ostream &, const Core::Range::Range<T> &);
    template<typename T>
    std::istream & operator >> (std::istream &, Core::Range::Range<T> &);

} // namespace djv

#include <djvCore/RangeInline.h>
