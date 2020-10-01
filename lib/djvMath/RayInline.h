// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Math
    {
        template<typename T, glm::precision P>
        constexpr tRay3<T, P>::tRay3() noexcept
        {}

        template<typename T, glm::precision P>
        constexpr tRay3<T, P>::tRay3(const glm::tvec3<T, P>& start, const glm::tvec3<T, P>& end) noexcept :
            start(start),
            end(end)
        {}

        template<typename T, glm::precision P>
        constexpr bool tRay3<T, P>::operator == (const tRay3<T, P>& value) const noexcept
        {
            return start == value.start && end == value.end;
        }

        template<typename T, glm::precision P>
        constexpr bool tRay3<T, P>::operator != (const tRay3<T, P>& value) const noexcept
        {
            return !(*this == value);
        }

    } // namespace Math
} // namespace djv
