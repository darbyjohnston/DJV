// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

#include <glm/vec3.hpp>

#include <sstream>

namespace djv
{
    namespace Math
    {
        //! This struct provides a 3-dimensional ray.
        template<typename T, glm::precision P = glm::defaultp>
        struct tRay3
        {
            constexpr tRay3() noexcept;
            constexpr tRay3(const glm::tvec3<T, P>& start, const glm::tvec3<T, P>& end) noexcept;

            glm::tvec3<T, P> start, end;

            constexpr bool operator == (const tRay3<T, P>&) const noexcept;
            constexpr bool operator != (const tRay3<T, P>&) const noexcept;
        };

        typedef tRay3<float, glm::lowp>    Ray3f_lowp;
        typedef tRay3<float, glm::mediump> Ray3f_mediump;
        typedef tRay3<float, glm::highp>   Ray3f_highp;

#if(defined(GLM_PRECISION_LOWP_FLOAT))
        typedef Ray3f_lowp    Ray3f;
#elif(defined(GLM_PRECISION_MEDIUMP_FLOAT))
        typedef Ray3f_mediump Ray3f;
#else //defined(GLM_PRECISION_HIGHP_FLOAT)
        typedef Ray3f_highp   Ray3f;
#endif //GLM_PRECISION

    } // namespace Math

    template<typename T, glm::precision P = glm::defaultp>
    std::ostream& operator << (std::ostream&, const Math::tRay3<T, P>&);

    //! Throws:
    //! - std::exception
    template<typename T, glm::precision P = glm::defaultp>
    std::istream& operator >> (std::istream&, Math::tRay3<T, P>&);

} // namespace djv

#include <djvMath/RayInline.h>

