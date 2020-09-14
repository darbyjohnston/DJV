// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

namespace djv
{
    template<typename T, glm::precision P>
    inline std::ostream& operator << (std::ostream& s, const Core::Ray::tRay3<T, P>& value)
    {
        s << value.start << " ";
        s << value.end;
        return s;
    }

    template<typename T, glm::precision P>
    inline std::istream& operator >> (std::istream& s, Core::Ray::tRay3<T, P>& out)
    {
        try
        {
            s.exceptions(std::istream::failbit | std::istream::badbit);
            s >> value.start;
            s >> value.end;
        }
        catch (const std::exception&)
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
        return s;
    }

} // namespace djv

