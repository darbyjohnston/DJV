//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

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
                inline tRay3();
                inline tRay3(const glm::tvec3<T, P>& start, const glm::tvec3<T, P>& end);

                glm::tvec3<T, P> start, end;

                inline bool operator == (const tRay3<T, P>&) const;
                inline bool operator != (const tRay3<T, P>&) const;
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
    inline std::ostream & operator << (std::ostream &, const Core::Ray::tRay3<T, P>&);
    template<typename T, glm::precision P = glm::defaultp>
    inline std::istream & operator >> (std::istream &, Core::Ray::tRay3<T, P>&);

} // namespace djv

#include <djvCore/RayInline.h>
