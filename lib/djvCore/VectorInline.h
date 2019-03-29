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

#include <djvCore/StringUtil.h>

namespace djv
{
    template<typename T, precision P>
    inline QStringList & operator << (QStringList & out, const glm::tvec2<T, P> & in)
    {
        return out << in.x << in.y;
    }

    template<typename T, precision P>
    inline QStringList & operator << (QStringList & out, const glm::tvec3<T, P> & in)
    {
        return out << in.x << in.y << in.z;
    }

    template<typename T, precision P>
    inline QStringList & operator << (QStringList & out, const glm::tvec4<T, P> & in)
    {
        return out << in.x << in.y << in.z << in.w;
    }

    template<typename T, precision P>
    inline QStringList & operator >> (QStringList & in, glm::tvec2<T, P> & out)
    {
        return in >> out.x >> out.y;
    }

    template<typename T, precision P>
    inline QStringList & operator >> (QStringList & in, glm::tvec3<T, P> & out)
    {
        return in >> out.x >> out.y >> out.z;
    }

    template<typename T, precision P>
    inline QStringList & operator >> (QStringList & in, glm::tvec4<T, P> & out)
    {
        return in >> out.x >> out.y >> out.z << out.w;
    }

    template<typename T, precision P>
    inline Core::Debug & operator << (Core::Debug & debug, const glm::tvec2<T, P> & in)
    {
        return debug << in.x << " " << in.y;
    }

    template<typename T, precision P>
    inline Core::Debug & operator << (Core::Debug & debug, const glm::tvec3<T, P> & in)
    {
        return debug << in.x << " " << in.y << " " << in.z;
    }

    template<typename T, precision P>
    inline Core::Debug & operator << (Core::Debug & debug, const glm::tvec4<T, P> & in)
    {
        return debug << in.x << " " << in.y << " " << in.z << " " << in.w;
    }

} // namespace djv
