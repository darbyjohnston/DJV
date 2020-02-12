//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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

