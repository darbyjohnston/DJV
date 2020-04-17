// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/Math.h>
#include <djvCore/String.h>

namespace djv
{
    namespace Core
    {
        namespace Ray
        {
            template<typename T, glm::precision P>
            constexpr tRay3<T, P>::tRay3()
            {}

            template<typename T, glm::precision P>
            constexpr tRay3<T, P>::tRay3(const glm::tvec3<T, P> & start, const glm::tvec3<T, P> & end) :
                start(start),
                end(end)
            {}

            template<typename T, glm::precision P>
            constexpr bool tRay3<T, P>::operator == (const tRay3<T, P> & value) const
            {
                return start == value.start && end == value.end;
            }

            template<typename T, glm::precision P>
            constexpr bool tRay3<T, P>::operator != (const tRay3<T, P> & value) const
            {
                return !(*this == value);
            }

        } // namespace Ray
    } // namespace Core

    template<typename T, glm::precision P>
    inline std::ostream & operator << (std::ostream & s, const Core::Ray::tRay3<T, P> & value)
    {
        s << value.start << " ";
        s << value.end;
        return s;
    }

    template<typename T, glm::precision P>
    inline std::istream & operator >> (std::istream & s, Core::Ray::tRay3<T, P> & out)
    {
        s.exceptions(std::istream::failbit | std::istream::badbit);
        s >> value.start;
        s >> value.end;
        return s;
    }

} // namespace djv

