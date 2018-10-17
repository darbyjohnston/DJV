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

namespace djv
{
#define _BOX_FNC_OP(BOX, IN) \
    template<typename T, glm::qualifier P> \
    inline BOX<T, P> operator IN (const BOX<T, P> & a, T b) \
    { \
        return BOX<T, P>(a.position IN b, a.size IN b); \
    }

    _BOX_FNC_OP(Core::Box2, +)
    _BOX_FNC_OP(Core::Box2, -)
    _BOX_FNC_OP(Core::Box2, *)
    _BOX_FNC_OP(Core::Box2, / )
    _BOX_FNC_OP(Core::Box3, +)
    _BOX_FNC_OP(Core::Box3, -)
    _BOX_FNC_OP(Core::Box3, *)
    _BOX_FNC_OP(Core::Box3, /)

#define _BOX_FNC_OP2(IN) \
    template<typename T, glm::qualifier P> \
    inline Core::Box2<T, P> operator IN (const Core::Box2<T, P> & a, const glm::tvec2<T, P> & b) \
    { \
        return Core::Box2<T, P>(a.position IN b, a.size IN b); \
    }

    _BOX_FNC_OP2(+)
    _BOX_FNC_OP2(-)
    _BOX_FNC_OP2(*)
    _BOX_FNC_OP2(/)

#define _BOX_FNC_OP3(IN) \
    template<typename T, glm::qualifier P> \
    inline Core::Box3<T, P> operator IN (const Core::Box3<T, P> & a, const glm::tvec3<T, P> & b) \
    { \
        return Core::Box3<T, P>(a.position IN b, a.size IN b); \
    }

    _BOX_FNC_OP3(+)
    _BOX_FNC_OP3(-)
    _BOX_FNC_OP3(*)
    _BOX_FNC_OP3(/)

    template<typename T, glm::qualifier P>
    inline bool operator == (const Core::Box2<T, P> & a, const Core::Box2<T, P> & b)
    {
        return a.position == b.position && a.size == b.size;
    }

    template<typename T, glm::qualifier P>
    inline bool operator == (const Core::Box3<T, P> & a, const Core::Box3<T, P> & b)
    {
        return a.position == b.position && a.size == b.size;
    }

    template<typename T, glm::qualifier P>
    inline bool operator != (const Core::Box2<T, P> & a, const Core::Box2<T, P> & b)
    {
        return !(a == b);
    }

    template<typename T, glm::qualifier P>
    inline bool operator != (const Core::Box3<T, P> & a, const Core::Box3<T, P> & b)
    {
        return !(a == b);
    }

    template<typename T, glm::qualifier P>
    inline QStringList & operator << (QStringList & out, const Core::Box2<T, P> & in)
    {
        return out << in.position << in.size;
    }

    template<typename T, glm::qualifier P>
    inline QStringList & operator << (QStringList & out, const Core::Box3<T, P> & in)
    {
        return out << in.position << in.size;
    }

    template<typename T, glm::qualifier P>
    inline QStringList & operator >> (QStringList & in, Core::Box2<T, P> & out)
    {
        return in >> out.position >> out.size;
    }

    template<typename T, glm::qualifier P>
    inline QStringList & operator >> (QStringList & in, Core::Box3<T, P> & out)
    {
        return in >> out.position >> out.size;
    }


    template<typename T, glm::qualifier P>
    inline Core::Debug & operator << (Core::Debug & debug, const Core::Box2<T, P> & in)
    {
        return debug << in.position << " " << in.size;
    }

    template<typename T, glm::qualifier P>
    inline Core::Debug & operator << (Core::Debug & debug, const Core::Box3<T, P> & in)
    {
        return debug << in.position << " " << in.size;
    }

} // namespace djv
