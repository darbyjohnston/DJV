// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

namespace djv
{
    template<typename T>
    inline std::ostream& operator << (std::ostream& os, const Core::Math::Range<T>& value)
    {
        os << value.getMin() << " ";
        os << value.getMax();
        return os;
    }

    template<typename T>
    inline std::istream& operator >> (std::istream& is, Core::Math::Range<T>& out)
    {
        try
        {
            is.exceptions(std::istream::failbit | std::istream::badbit);
            T min = static_cast<T>(0);
            is >> min;
            T max = static_cast<T>(0);
            is >> max;
            out = Core::Math::Range<T>(min, max);
        }
        catch (const std::exception&)
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
        return is;
    }

} // namespace djv

