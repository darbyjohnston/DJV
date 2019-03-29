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

#include <QMetaType>

namespace djv
{
    namespace Core
    {
        //! This class provides a two-dimensional axis-aligned box.
        template<typename T, precision P = glm::defaultp>
        struct Box2
        {
            inline Box2();
            inline Box2(const Box2<T, P> &);
            inline Box2(const glm::tvec2<T, P> & position, const glm::tvec2<T, P> & size);
            inline Box2(const glm::tvec2<T, P> & size);
            inline Box2(T x, T y, T w, T h);
            inline Box2(T w, T h);

            glm::tvec2<T, P> position;
            glm::tvec2<T, P> size;

            //! Component access.
            T & x, &y, &w, &h;

            //! Is the box valid (width and height are greater than zero)?
            inline bool isValid() const;

            //! Set the components to zero.
            inline void zero();

            //! Get the lower right hand corner.
            inline glm::tvec2<T, P> lowerRight() const;

            //! Set the lower right hand corner.
            inline void setLowerRight(const glm::tvec2<T, P> &);

            inline Box2<T, P> & operator = (const Box2<T, P> &);

            inline Box2<T, P> & operator *= (const glm::tvec2<T, P> &);
            inline Box2<T, P> & operator /= (const glm::tvec2<T, P> &);

            inline Box2<T, P> & operator *= (T);
            inline Box2<T, P> & operator /= (T);

            inline operator Box2<int, P>() const;
            inline operator Box2<float, P>() const;
        };

        //! This class provides a three-dimensional axis-aligned box.
        template<typename T, precision P = glm::defaultp>
        struct Box3
        {
            inline Box3();
            inline Box3(const Box3<T, P> &);
            inline Box3(const glm::tvec3<T, P> & position, const glm::tvec3<T, P> & size);
            inline Box3(const glm::tvec3<T, P> & size);
            inline Box3(T x, T y, T z, T w, T h, T d);
            inline Box3(T w, T h, T d);

            glm::tvec3<T, P> position;
            glm::tvec3<T, P> size;

            //! Component access.
            T & x, &y, &z, &w, &h, &d;

            //! Is the box valid (width and height are greater than zero)?
            inline bool isValid() const;

            //! Set the components to zero.
            inline void zero();

            //! Get the lower right hand corner.
            inline glm::tvec3<T, P> lowerRight() const;

            //! Set the lower right hand corner.
            inline void setLowerRight(const glm::tvec3<T, P> &);

            inline Box3<T, P> & operator = (const Box3<T, P> &);

            inline Box3<T, P> & operator *= (const glm::tvec3<T, P> &);
            inline Box3<T, P> & operator /= (const glm::tvec3<T, P> &);

            inline Box3<T, P> & operator *= (T);
            inline Box3<T, P> & operator /= (T);

            inline operator Box3<int, P>() const;
            inline operator Box3<float, P>() const;
        };

        typedef Box2<int> Box2i;
        typedef Box2<float> Box2f;
        typedef Box3<int> Box3i;
        typedef Box3<float> Box3f;

    } // namespace Core

    template <typename T, precision P = glm::defaultp>
    inline Core::Box2<T, P> operator + (const Core::Box2<T, P> &, T);
    template <typename T, precision P = glm::defaultp>
    inline Core::Box3<T, P> operator + (const Core::Box3<T, P> &, T);
    template <typename T, precision P = glm::defaultp>
    inline Core::Box2<T, P> operator - (const Core::Box2<T, P> &, T);
    template <typename T, precision P = glm::defaultp>
    inline Core::Box3<T, P> operator - (const Core::Box3<T, P> &, T);
    template <typename T, precision P = glm::defaultp>
    inline Core::Box2<T, P> operator * (const Core::Box2<T, P> &, T);
    template <typename T, precision P = glm::defaultp>
    inline Core::Box3<T, P> operator * (const Core::Box3<T, P> &, T);
    template <typename T, precision P = glm::defaultp>
    inline Core::Box2<T, P> operator / (const Core::Box2<T, P> &, T);
    template <typename T, precision P = glm::defaultp>
    inline Core::Box3<T, P> operator / (const Core::Box3<T, P> &, T);

    template <typename T, precision P = glm::defaultp>
    inline Core::Box2<T, P> operator + (const Core::Box2<T, P> &, const glm::tvec2<T, P> &);
    template <typename T, precision P = glm::defaultp>
    inline Core::Box3<T, P> operator + (const Core::Box3<T, P> &, const glm::tvec3<T, P> &);
    template <typename T, precision P = glm::defaultp>
    inline Core::Box2<T, P> operator - (const Core::Box2<T, P> &, const glm::tvec2<T, P> &);
    template <typename T, precision P = glm::defaultp>
    inline Core::Box3<T, P> operator - (const Core::Box3<T, P> &, const glm::tvec3<T, P> &);
    template <typename T, precision P = glm::defaultp>
    inline Core::Box2<T, P> operator * (const Core::Box2<T, P> &, const glm::tvec2<T, P> &);
    template <typename T, precision P = glm::defaultp>
    inline Core::Box3<T, P> operator * (const Core::Box3<T, P> &, const glm::tvec3<T, P> &);
    template <typename T, precision P = glm::defaultp>
    inline Core::Box2<T, P> operator / (const Core::Box2<T, P> &, const glm::tvec2<T, P> &);
    template <typename T, precision P = glm::defaultp>
    inline Core::Box3<T, P> operator / (const Core::Box3<T, P> &, const glm::tvec3<T, P> &);

    template <typename T, precision P = glm::defaultp>
    inline bool operator == (const Core::Box2<T, P> &, const Core::Box2<T, P> &);
    template <typename T, precision P = glm::defaultp>
    inline bool operator == (const Core::Box3<T, P> &, const Core::Box3<T, P> &);

    template <typename T, precision P = glm::defaultp>
    inline bool operator != (const Core::Box2<T, P> &, const Core::Box2<T, P> &);
    template <typename T, precision P = glm::defaultp>
    inline bool operator != (const Core::Box3<T, P> &, const Core::Box3<T, P> &);

    template<typename T, precision P = glm::defaultp>
    inline QStringList & operator >> (QStringList &, Core::Box2<T, P> &);
    template<typename T, precision P = glm::defaultp>
    inline QStringList & operator >> (QStringList &, Core::Box3<T, P> &);

    template<typename T, precision P = glm::defaultp>
    inline QStringList & operator << (QStringList &, const Core::Box2<T, P> &);
    template<typename T, precision P = glm::defaultp>
    inline QStringList & operator << (QStringList &, const Core::Box3<T, P> &);

    template <typename T, precision P = glm::defaultp>
    inline Core::Debug & operator << (Core::Debug &, const Core::Box2<T, P> &);
    template <typename T, precision P = glm::defaultp>
    inline Core::Debug & operator << (Core::Debug &, const Core::Box3<T, P> &);

} // namespace djv

Q_DECLARE_METATYPE(djv::Core::Box2i)
Q_DECLARE_METATYPE(djv::Core::Box2f)
Q_DECLARE_METATYPE(djv::Core::Box3i)
Q_DECLARE_METATYPE(djv::Core::Box3f)

#include <djvCore/BoxInline.h>
#include <djvCore/Box2Inline.h>
#include <djvCore/Box3Inline.h>

