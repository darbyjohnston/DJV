//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
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

#include <djvCore/Box.h>

class QRect;
class QRectF;

namespace djv
{
    namespace Core
    {
        //! This class provides box utilities.
        class BoxUtil
        {
        public:
            inline virtual ~BoxUtil() = 0;

            //! Create a box with the given corners.
            template<typename T, glm::precision P = glm::defaultp>
            static inline Box2<T, P> corners(
                const glm::tvec2<T, P> & upperLeft,
                const glm::tvec2<T, P> & lowerRight);

            //! Swap the box position and size components.
            template<typename T, glm::precision P = glm::defaultp>
            static inline Box2<T, P> swap(const Box2<T, P> &);

            //! Get the intersection of two boxes.
            template<typename T, glm::precision P = glm::defaultp>
            static inline Box2<T, P> intersect(const Box2<T, P> &, const Box2<T, P> &);

            //! Get whether a point is inside a box.
            template<typename T, glm::precision P = glm::defaultp>
            static inline bool intersect(const Box2<T, P> &, const glm::tvec2<T, P> &);

            //! Get the union of two boxes.
            template<typename T, glm::precision P = glm::defaultp>
            static inline Box2<T, P> bound(const Box2<T, P> &, const Box2<T, P> &);

            //! Expand a box to contain the given point.
            template<typename T, glm::precision P = glm::defaultp>
            static inline Box2<T, P> expand(const Box2<T, P> &, const glm::tvec2<T, P> &);

            //! Add a border to a box.
            template<typename T, glm::precision P = glm::defaultp>
            static inline Box2<T, P> border(const Box2<T, P> &, const glm::tvec2<T, P> &);

            //! Is the box's size greater than zero?
            template<typename T, glm::precision P = glm::defaultp>
            static inline bool isSizeValid(const Box2<T, P> &);

            //! Convert to Qt.
            static inline QRect toQt(const Box2<int> &);

            //! Convert to Qt.
            static inline QRectF toQt(const Box2<float> &);

            //! Convert from Qt.
            static inline Box2i fromQt(const QRect &);

            //! Convert from Qt.
            static inline Box2f fromQt(const QRectF &);
        };

    } // namespace Core
} // namespace djv

#include <djvCore/BoxUtilInline.h>

