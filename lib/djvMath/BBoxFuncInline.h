// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvMath/VectorFunc.h>

#include <algorithm>

namespace djv
{
    template<typename T, glm::precision P>
    inline bool fuzzyCompare(const Math::tBBox2<T, P>& a, const Math::tBBox2<T, P>& b) noexcept
    {
        return fuzzyCompare(a.min, b.min) && fuzzyCompare(a.max, b.max);
    }

    template<typename T, glm::precision P>
    inline bool fuzzyCompare(const Math::tBBox3<T, P>& a, const Math::tBBox3<T, P>& b) noexcept
    {
        return fuzzyCompare(a.min, b.min) && fuzzyCompare(a.max, b.max);
    }

    template<typename T, glm::precision P>
    inline Math::tBBox3<T, P> operator * (const Math::tBBox3<T, P>& bbox, const glm::mat4& m) noexcept
    {
        const glm::tvec4<T, P> pts[] =
        {
            glm::tvec4<T, P>(bbox.min.x, bbox.min.y, bbox.min.z, 1.F),
            glm::tvec4<T, P>(bbox.max.x, bbox.min.y, bbox.min.z, 1.F),
            glm::tvec4<T, P>(bbox.max.x, bbox.min.y, bbox.max.z, 1.F),
            glm::tvec4<T, P>(bbox.min.x, bbox.min.y, bbox.max.z, 1.F),
            glm::tvec4<T, P>(bbox.min.x, bbox.max.y, bbox.min.z, 1.F),
            glm::tvec4<T, P>(bbox.max.x, bbox.max.y, bbox.min.z, 1.F),
            glm::tvec4<T, P>(bbox.max.x, bbox.max.y, bbox.max.z, 1.F),
            glm::tvec4<T, P>(bbox.min.x, bbox.max.y, bbox.max.z, 1.F)
        };
        Math::tBBox3<T, P> out(m * pts[0]);
        for (size_t i = 1; i < 8; ++i)
        {
            out.expand(m * pts[i]);
        }
        return out;
    }

    template<typename T, glm::precision P>
    inline std::ostream& operator << (std::ostream& s, const Math::tBBox2<T, P>& value)
    {
        s << value.min << " ";
        s << value.max;
        return s;
    }

    template<typename T, glm::precision P>
    inline std::ostream& operator << (std::ostream& s, const Math::tBBox3<T, P>& value)
    {
        s << value.min << " ";
        s << value.max;
        return s;
    }

    template<typename T, glm::precision P>
    inline std::istream& operator >> (std::istream& s, Math::tBBox2<T, P>& out)
    {
        try
        {
            s.exceptions(std::istream::failbit | std::istream::badbit);
            s >> out.min;
            s >> out.max;
        }
        catch (const std::exception&)
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
        return s;
    }

    template<typename T, glm::precision P>
    inline std::istream& operator >> (std::istream& s, Math::tBBox3<T, P>& out)
    {
        try
        {
            s.exceptions(std::istream::failbit | std::istream::badbit);
            s >> out.min;
            s >> out.max;
        }
        catch (const std::exception&)
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
        return s;
    }

} // namespace djv
