//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
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
    namespace Core
    {
        //! \todo Is this specialization for integer box types correct?
        //! \todo Why do these specializations need to come first on Windows (error
        //! C2908)?
        template<>
        inline glm::ivec3 Box3<int>::lowerRight() const
        {
            return position + size - 1;
        }

        template<>
        inline void Box3<int>::setLowerRight(const glm::ivec3 & in)
        {
            size = in - position + 1;
        }

#define _BOX3_INIT() \
    x(Box3::position.x), \
    y(Box3::position.y), \
    z(Box3::position.z), \
    w(Box3::size.x),     \
    h(Box3::size.y),     \
    d(Box3::size.z)

        template<typename T, glm::precision P>
        inline Box3<T, P>::Box3() :
            position(T(0), T(0), T(0)),
            size(T(0), T(0), T(0)),
            _BOX3_INIT()
        {}

        template<typename T, glm::precision P>
        inline Box3<T, P>::Box3(const Box3<T, P> & in) :
            position(in.position),
            size(in.size),
            _BOX3_INIT()
        {}

        template<typename T, glm::precision P>
        inline Box3<T, P>::Box3(const glm::tvec3<T, P> & position, const glm::tvec3<T, P> & size) :
            position(position),
            size(size),
            _BOX3_INIT()
        {}

        template<typename T, glm::precision P>
        inline Box3<T, P>::Box3(const glm::tvec3<T, P> & size) :
            position(T(0), T(0), T(0)),
            size(size),
            _BOX3_INIT()
        {}

        template<typename T, glm::precision P>
        inline Box3<T, P>::Box3(T _x, T _y, T _z, T _w, T _h, T _d) :
            _BOX3_INIT()
        {
            x = _x;
            y = _y;
            z = _z;
            w = _w;
            h = _h;
            d = _d;
        }

        template<typename T, glm::precision P>
        inline Box3<T, P>::Box3(T _w, T _h, T _d) :
            position(T(0), T(0), T(0)),
            _BOX3_INIT()
        {
            w = _w;
            h = _h;
            d = _d;
        }

        template<typename T, glm::precision P>
        inline bool Box3<T, P>::isValid() const
        {
            return size.x > T(0) && size.y > T(0);
        }

        template<typename T, glm::precision P>
        inline void Box3<T, P>::zero()
        {
            position.x = position.y = position.z = T(0);
            size.x = size.y = size.z = T(0);
        }

        template<typename T, glm::precision P>
        inline glm::tvec3<T, P> Box3<T, P>::lowerRight() const
        {
            return position + size;
        }

        template<typename T, glm::precision P>
        inline void Box3<T, P>::setLowerRight(const glm::tvec3<T, P> & in)
        {
            size = in - position;
        }

        template<typename T, glm::precision P>
        inline Box3<T, P> & Box3<T, P>::operator = (const Box3<T, P> & in)
        {
            if (&in != this)
            {
                position = in.position;
                size = in.size;
            }
            return *this;
        }

        template<typename T, glm::precision P>
        inline Box3<T, P> & Box3<T, P>::operator *= (const glm::tvec3<T, P> & in)
        {
            position *= in;
            size *= in;
            return *this;
        }

        template<typename T, glm::precision P>
        inline Box3<T, P> & Box3<T, P>::operator /= (const glm::tvec3<T, P> & in)
        {
            position /= in;
            size /= in;
            return *this;
        }

        template<typename T, glm::precision P>
        inline Box3<T, P> & Box3<T, P>::operator *= (T in)
        {
            position *= in;
            size *= in;
            return *this;
        }

        template<typename T, glm::precision P>
        inline Box3<T, P> & Box3<T, P>::operator /= (T in)
        {
            position /= in;
            size /= in;
            return *this;
        }

        template<typename T, glm::precision P>
        inline Box3<T, P>::operator Box3<int, P>() const
        {
            return Box3<int, P>(
                static_cast<int>(x), static_cast<int>(y), static_cast<int>(z),
                static_cast<int>(w), static_cast<int>(h), static_cast<int>(d));
        }

        template<typename T, glm::precision P>
        inline Box3<T, P>::operator Box3<float, P>() const
        {
            return Box3<float, P>(
                static_cast<float>(x), static_cast<float>(y), static_cast<float>(z),
                static_cast<float>(w), static_cast<float>(h), static_cast<float>(d));
        }

    } // namespace Core
} // namespace djv
