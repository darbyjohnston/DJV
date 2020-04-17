// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Vector.h>

#include <glm/vec3.hpp>

namespace djv
{
    namespace Core
    {
        //! This namespace provides ray functionality.
        namespace Ray
        {
            //! This struct provides a 3-dimensional ray.
            template<typename T, glm::precision P = glm::defaultp>
            struct tRay3
            {
                constexpr tRay3();
                constexpr tRay3(const glm::tvec3<T, P> & start, const glm::tvec3<T, P> & end);

                glm::tvec3<T, P> start, end;

                constexpr bool operator == (const tRay3<T, P> &) const;
                constexpr bool operator != (const tRay3<T, P> &) const;
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

        } // namespace Ray

        using Ray::Ray3f;

    } // namespace Core

    template<typename T, glm::precision P = glm::defaultp>
    std::ostream & operator << (std::ostream &, const Core::Ray::tRay3<T, P> &);
    template<typename T, glm::precision P = glm::defaultp>
    std::istream & operator >> (std::istream &, Core::Ray::tRay3<T, P> &);

} // namespace djv

#include <djvCore/RayInline.h>
