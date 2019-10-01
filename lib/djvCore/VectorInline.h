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

#include <djvCore/Math.h>
#include <djvCore/Memory.h>

namespace djv
{
    namespace Core
    {
        namespace Vector
        {
            template<>
            inline float getAspect<float>(const glm::vec2 & value)
            {
                return value.y > 0.F ? value.x / value.y : 1.F;
            }

            template<>
            inline float getAspect<int>(const glm::ivec2 & value)
            {
                return value.y > 0 ? value.x / static_cast<float>(value.y) : 1.F;
            }

        } // namespace Vector
    } // namespace Core

    template<typename T, glm::precision P>
    inline std::ostream & operator << (std::ostream & s, const glm::tvec2<T, P> & value)
    {
        s << value.x << " ";
        s << value.y;
        return s;
    }

    template<typename T, glm::precision P>
    inline std::ostream & operator << (std::ostream & s, const glm::tvec3<T, P> & value)
    {
        s << value.x << " ";
        s << value.y << " ";
        s << value.z;
        return s;
    }

    template<typename T, glm::precision P>
    inline std::ostream & operator << (std::ostream & s, const glm::tvec4<T, P> & value)
    {
        s << value.x << " ";
        s << value.y << " ";
        s << value.z << " ";
        s << value.w;
        return s;
    }

    template<typename T, glm::precision P>
    inline std::istream & operator >> (std::istream & s, glm::tvec2<T, P> & out)
    {
        s >> out.x;
        s >> out.y;
        return s;
    }

    template<typename T, glm::precision P>
    inline std::istream & operator >> (std::istream & s, glm::tvec3<T, P> & out)
    {
        s >> out.x;
        s >> out.y;
        s >> out.z;
        return s;
    }

    template<typename T, glm::precision P>
    inline std::istream & operator >> (std::istream & s, glm::tvec4<T, P> & out)
    {
        s >> out.x;
        s >> out.y;
        s >> out.z;
        s >> out.w;
        return s;
    }

    template<typename T, glm::precision P>
    inline bool fuzzyCompare(const glm::tvec2<T, P> & a, const glm::tvec2<T, P> & b)
    {
        return
            fuzzyCompare(a.x, b.x) &&
            fuzzyCompare(a.y, b.y);
    }

    template<typename T, glm::precision P>
    inline bool fuzzyCompare(const glm::tvec3<T, P> & a, const glm::tvec3<T, P> & b)
    {
        return
            fuzzyCompare(a.x, b.x) &&
            fuzzyCompare(a.y, b.y) &&
            fuzzyCompare(a.z, b.z);
    }

    template<typename T, glm::precision P>
    inline bool fuzzyCompare(const glm::tvec4<T, P> & a, const glm::tvec4<T, P> & b)
    {
        return
            fuzzyCompare(a.x, b.x) &&
            fuzzyCompare(a.y, b.y) &&
            fuzzyCompare(a.z, b.z) &&
            fuzzyCompare(a.w, b.w);
    }

} // namespace djv

namespace std
{
    inline std::size_t hash<glm::ivec2>::operator() (const glm::ivec2 & value) const noexcept
    {
        size_t hash = 0;
        djv::Core::Memory::hashCombine<int>(hash, value.x);
        djv::Core::Memory::hashCombine<int>(hash, value.y);
        return hash;
    }

    inline std::size_t hash<glm::vec2>::operator() (const glm::vec2 & value) const noexcept
    {
        size_t hash = 0;
        djv::Core::Memory::hashCombine<float>(hash, value.x);
        djv::Core::Memory::hashCombine<float>(hash, value.y);
        return hash;
    }

} // namespace std

