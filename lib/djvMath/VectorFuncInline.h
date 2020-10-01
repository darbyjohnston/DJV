// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvMath/MathFunc.h>

#include <djvCore/MemoryFunc.h>

namespace djv
{
    template<typename T, glm::precision P>
    inline bool fuzzyCompare(const glm::tvec2<T, P>& a, const glm::tvec2<T, P>& b) noexcept
    {
        return
            fuzzyCompare(a.x, b.x) &&
            fuzzyCompare(a.y, b.y);
    }

    template<typename T, glm::precision P>
    inline bool fuzzyCompare(const glm::tvec3<T, P>& a, const glm::tvec3<T, P>& b) noexcept
    {
        return
            fuzzyCompare(a.x, b.x) &&
            fuzzyCompare(a.y, b.y) &&
            fuzzyCompare(a.z, b.z);
    }

    template<typename T, glm::precision P>
    inline bool fuzzyCompare(const glm::tvec4<T, P>& a, const glm::tvec4<T, P>& b) noexcept
    {
        return
            fuzzyCompare(a.x, b.x) &&
            fuzzyCompare(a.y, b.y) &&
            fuzzyCompare(a.z, b.z) &&
            fuzzyCompare(a.w, b.w);
    }

    template<typename T, glm::precision P>
    inline std::ostream& operator << (std::ostream& s, const glm::tvec2<T, P>& value)
    {
        s << value.x << " ";
        s << value.y;
        return s;
    }

    template<typename T, glm::precision P>
    inline std::ostream& operator << (std::ostream& s, const glm::tvec3<T, P>& value)
    {
        s << value.x << " ";
        s << value.y << " ";
        s << value.z;
        return s;
    }

    template<typename T, glm::precision P>
    inline std::ostream& operator << (std::ostream& s, const glm::tvec4<T, P>& value)
    {
        s << value.x << " ";
        s << value.y << " ";
        s << value.z << " ";
        s << value.w;
        return s;
    }

    template<typename T, glm::precision P>
    inline std::istream& operator >> (std::istream& s, glm::tvec2<T, P>& out)
    {
        try
        {
            s.exceptions(std::istream::failbit | std::istream::badbit);
            s >> out.x;
            s >> out.y;
        }
        catch (const std::exception&)
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
        return s;
    }

    template<typename T, glm::precision P>
    inline std::istream& operator >> (std::istream& s, glm::tvec3<T, P>& out)
    {
        try
        {
            s.exceptions(std::istream::failbit | std::istream::badbit);
            s >> out.x;
            s >> out.y;
            s >> out.z;
        }
        catch (const std::exception&)
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
        return s;
    }

    template<typename T, glm::precision P>
    inline std::istream& operator >> (std::istream& s, glm::tvec4<T, P>& out)
    {
        try
        {
            s.exceptions(std::istream::failbit | std::istream::badbit);
            s >> out.x;
            s >> out.y;
            s >> out.z;
            s >> out.w;
        }
        catch (const std::exception&)
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
        return s;
    }

} // namespace djv

namespace std
{
    inline std::size_t hash<glm::ivec2>::operator() (const glm::ivec2& value) const noexcept
    {
        size_t hash = 0;
        djv::Core::Memory::hashCombine<int>(hash, value.x);
        djv::Core::Memory::hashCombine<int>(hash, value.y);
        return hash;
    }

    inline std::size_t hash<glm::vec2>::operator() (const glm::vec2& value) const noexcept
    {
        size_t hash = 0;
        djv::Core::Memory::hashCombine<float>(hash, value.x);
        djv::Core::Memory::hashCombine<float>(hash, value.y);
        return hash;
    }

} // namespace std
