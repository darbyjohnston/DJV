// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace djv
{
    namespace Core
    {
        //! This namespace provides bounding box functionality.
        namespace BBox
        {
            //! This class provides an axis aligned bounding box.
            template<typename T, glm::precision P = glm::defaultp>
            class tBBox2
            {
            public:
                tBBox2() noexcept;
                explicit tBBox2(const glm::tvec2<T, P>&) noexcept;
                tBBox2(const glm::tvec2<T, P>& min, const glm::tvec2<T, P>& max) noexcept;
                tBBox2(T x, T y, T w, T h) noexcept;

                //! \name Box Components
                ///@{

                glm::tvec2<T, P> min;
                glm::tvec2<T, P> max;

                T x() const noexcept;
                T y() const noexcept;
                T w() const noexcept;
                T h() const noexcept;

                constexpr bool isValid() const noexcept;
                glm::tvec2<T, P> getSize() const noexcept;
                glm::tvec2<T, P> getCenter() const noexcept;
                T getArea() const noexcept;
                float getAspect() const noexcept;
                
                void zero() noexcept;

                ///@}

                //! \name Utilities
                ///@{

                bool contains(const tBBox2<T, P>&) const noexcept;
                bool contains(const glm::tvec2<T, P>&) const noexcept;

                bool intersects(const tBBox2<T, P>&) const noexcept;
                tBBox2<T, P> intersect(const tBBox2<T, P>&) const;

                void expand(const tBBox2<T, P>&);
                void expand(const glm::tvec2<T, P>&);

                constexpr tBBox2<T, P> margin(const glm::tvec2<T, P>&) const noexcept;
                constexpr tBBox2<T, P> margin(T) const noexcept;
                constexpr tBBox2<T, P> margin(T x0, T y0, T x1, T y1) const noexcept;

                ///@}

                constexpr bool operator == (const tBBox2<T, P>&) const noexcept;
                constexpr bool operator != (const tBBox2<T, P>&) const noexcept;
            };

            //! This class provides an axis aligned bounding box.
            template<typename T, glm::precision P = glm::defaultp>
            class tBBox3
            {
            public:
                tBBox3() noexcept;
                explicit tBBox3(const glm::tvec3<T, P>&) noexcept;
                tBBox3(const glm::tvec3<T, P>& min, const glm::tvec3<T, P>& max) noexcept;
                tBBox3(T x, T y, T z, T w, T h, T d) noexcept;

                //! \name Components
                ///@{

                glm::tvec3<T, P> min;
                glm::tvec3<T, P> max;

                T x() const noexcept;
                T y() const noexcept;
                T z() const noexcept;
                T w() const noexcept;
                T h() const noexcept;
                T d() const noexcept;

                constexpr bool isValid() const noexcept;
                glm::tvec3<T, P> getSize() const noexcept;
                glm::tvec3<T, P> getCenter() const noexcept;
                
                void zero() noexcept;

                ///@}

                //! \name Box Utilities
                ///@{

                bool contains(const tBBox3<T, P>&) const noexcept;
                bool contains(const glm::tvec3<T, P>&) const noexcept;

                bool intersects(const tBBox3<T, P>&) const noexcept;
                tBBox3<T, P> intersect(const tBBox3<T, P>&) const;
                bool intersect(const glm::tvec3<T, P>& start, const glm::tvec3<T, P>& end, glm::tvec3<T, P>& out) const;

                void expand(const tBBox3<T, P>&);
                void expand(const glm::tvec3<T, P>&);

                ///@}

                constexpr bool operator == (const tBBox3<T, P>&) const noexcept;
                constexpr bool operator != (const tBBox3<T, P>&) const noexcept;
            };

            typedef tBBox2<int, glm::lowp>      BBox2i_lowp;
            typedef tBBox2<int, glm::mediump>   BBox2i_mediump;
            typedef tBBox2<int, glm::highp>     BBox2i_highp;

            typedef tBBox2<float, glm::lowp>    BBox2f_lowp;
            typedef tBBox2<float, glm::mediump> BBox2f_mediump;
            typedef tBBox2<float, glm::highp>   BBox2f_highp;

            typedef tBBox3<float, glm::lowp>    BBox3f_lowp;
            typedef tBBox3<float, glm::mediump> BBox3f_mediump;
            typedef tBBox3<float, glm::highp>   BBox3f_highp;

#if(defined(GLM_PRECISION_LOWP_INT))
            typedef BBox2i_lowp    BBox2i;
#elif(defined(GLM_PRECISION_MEDIUMP_INT))
            typedef BBox2i_mediump BBox2i;
#else //defined(GLM_PRECISION_HIGHP_INT)
            typedef BBox2i_highp   BBox2i;
#endif //GLM_PRECISION

#if(defined(GLM_PRECISION_LOWP_FLOAT))
            typedef BBox2f_lowp    BBox2f;
            typedef BBox3f_lowp    BBox3f;
#elif(defined(GLM_PRECISION_MEDIUMP_FLOAT))
            typedef BBox2f_mediump BBox2f;
            typedef BBox3f_mediump BBox3f;
#else //defined(GLM_PRECISION_HIGHP_FLOAT)
            typedef BBox2f_highp   BBox2f;
            typedef BBox3f_highp   BBox3f;
#endif //GLM_PRECISION

        } // namespace BBox

        using BBox::BBox2i;
        using BBox::BBox2f;
        using BBox::BBox3f;

    } // namespace Core
} // namespace djv

#include <djvCore/BBoxInline.h>

