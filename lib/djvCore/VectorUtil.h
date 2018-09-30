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

#include <djvCore/Vector.h>

namespace djv
{
    namespace Core
    {
        //! This class provides vector utilities.
        class VectorUtil
        {
        public:
            inline virtual ~VectorUtil() = 0;

            //! Are all of the components greater than zero?
            template<typename T, glm::precision P = glm::defaultp>
            static inline bool isSizeValid(const glm::tvec2<T, P> &);

            //! Get the minimum vector components.
            template<typename T, glm::precision P = glm::defaultp>
            static inline glm::tvec2<T, P> min(const glm::tvec2<T, P> &, const glm::tvec2<T, P> &);

            //! Get the maximum vector components.
            template<typename T, glm::precision P = glm::defaultp>
            static inline glm::tvec2<T, P> max(const glm::tvec2<T, P> &, const glm::tvec2<T, P> &);

            //! Swap the vector components.
            template<typename T, glm::precision P = glm::defaultp>
            static inline glm::tvec2<T, P> swap(const glm::tvec2<T, P> &);

            //! Get the aspect ratio of the components.
            template<typename T, glm::precision P = glm::defaultp>
            static inline float aspect(const glm::tvec2<T, P> &);

            //! Round to the smallest integer value.
            static inline glm::ivec2 floor(const glm::vec2&);

            //! Round to the largest integer value.
            static inline glm::ivec2 ceil(const glm::vec2&);
        };

    } // namespace Core
} // namspace djv

#include <djvCore/VectorUtilInline.h>
