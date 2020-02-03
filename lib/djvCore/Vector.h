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

#pragma once

#include <djvCore/PicoJSON.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace djv
{
    template<typename T, glm::precision P = glm::defaultp>
    bool fuzzyCompare(const glm::tvec2<T, P> &, const glm::tvec2<T, P> &);
    template<typename T, glm::precision P = glm::defaultp>
    bool fuzzyCompare(const glm::tvec3<T, P> &, const glm::tvec3<T, P> &);
    template<typename T, glm::precision P = glm::defaultp>
    bool fuzzyCompare(const glm::tvec4<T, P> &, const glm::tvec4<T, P> &);

    picojson::value toJSON(const glm::ivec2 &);
    picojson::value toJSON(const glm::vec2 &);
    picojson::value toJSON(const glm::vec3 &);
    picojson::value toJSON(const glm::vec4 &);

    //! Throws:
    //! - std::exception
    void fromJSON(const picojson::value &, glm::ivec2 &);
    void fromJSON(const picojson::value &, glm::vec2 &);
    void fromJSON(const picojson::value &, glm::vec3 &);
    void fromJSON(const picojson::value &, glm::vec4 &);

    template<typename T, glm::precision P = glm::defaultp>
    std::ostream & operator << (std::ostream &, const glm::tvec2<T, P> &);
    template<typename T, glm::precision P = glm::defaultp>
    std::ostream & operator << (std::ostream &, const glm::tvec3<T, P> &);
    template<typename T, glm::precision P = glm::defaultp>
    std::ostream & operator << (std::ostream &, const glm::tvec4<T, P> &);
    template<typename T, glm::precision P = glm::defaultp>
    std::istream & operator >> (std::istream &, glm::tvec2<T, P> &);
    template<typename T, glm::precision P = glm::defaultp>
    std::istream & operator >> (std::istream &, glm::tvec3<T, P> &);
    template<typename T, glm::precision P = glm::defaultp>
    std::istream & operator >> (std::istream &, glm::tvec4<T, P> &);

} // namespace djv

namespace std
{
    template<>
    struct hash<glm::ivec2>
    {
        std::size_t operator() (const glm::ivec2 &) const noexcept;
    };

    template<>
    struct hash<glm::vec2>
    {
        std::size_t operator() (const glm::vec2 &) const noexcept;
    };

} // namespace std

#include <djvCore/VectorInline.h>
