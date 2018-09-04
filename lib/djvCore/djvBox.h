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

#pragma once

#include <djvVector.h>

#include <QMetaType>

//! \addtogroup djvCoreMath
//@{

//------------------------------------------------------------------------------
//! \class djvBox2<T, P>
//!
//! This class provides a two-dimensional axis-aligned box.
//------------------------------------------------------------------------------

template<typename T, glm::precision P = glm::defaultp>
struct djvBox2
{
    inline djvBox2();
    inline djvBox2(const djvBox2<T, P> &);
    inline djvBox2(const glm::tvec2<T, P> & position, const glm::tvec2<T, P> & size);
    inline djvBox2(const glm::tvec2<T, P> & size);
    inline djvBox2(T x, T y, T w, T h);
    inline djvBox2(T w, T h);

    glm::tvec2<T, P> position = glm::tvec2<T, P>(T(0), T(0));
    glm::tvec2<T, P> size = glm::tvec2<T, P>(T(0), T(0));

    //! Component access.
    T & x, & y, & w, & h;

    //! Is the box valid (width and height are greater than zero).
    inline bool isValid() const;
    
    //! Zero the box.
    inline void zero();

    //! Get the lower right hand corner.
    inline glm::tvec2<T, P> lowerRight() const;

    //! Set the lower right hand corner.
    inline void setLowerRight(const glm::tvec2<T, P> &);

    inline djvBox2<T, P> & operator = (const djvBox2<T, P> &);

    inline djvBox2<T, P> & operator *= (const glm::tvec2<T, P> &);
    inline djvBox2<T, P> & operator /= (const glm::tvec2<T, P> &);

    inline djvBox2<T, P> & operator *= (T);
    inline djvBox2<T, P> & operator /= (T);

    inline operator djvBox2<int, P>() const;
    inline operator djvBox2<float, P>() const;
};

//------------------------------------------------------------------------------
//! \class djvBox3<T, P>
//!
//! This class provides a three-dimensional axis-aligned box.
//------------------------------------------------------------------------------

template<typename T, glm::precision P = glm::defaultp>
struct djvBox3
{
    inline djvBox3();
    inline djvBox3(const djvBox3<T, P> &);
    inline djvBox3(const glm::tvec3<T, P> & position, const glm::tvec3<T, P> & size);
    inline djvBox3(const glm::tvec3<T, P> & size);
    inline djvBox3(T x, T y, T z, T w, T h, T d);
    inline djvBox3(T w, T h, T d);

    glm::tvec3<T, P> position = glm::tvec3<T, P>(T(0), T(0), T(0));
    glm::tvec3<T, P> size = glm::tvec3<T, P>(T(0), T(0), T(0));

    //! Component access.
    T & x, & y, & z, & w, & h, & d;

    //! Is the box valid (width and height are greater than zero).
    inline bool isValid() const;
    
    //! Zero the box.
    inline void zero();

    //! Get the lower right hand corner.
    inline glm::tvec3<T, P> lowerRight() const;

    //! Set the lower right hand corner.
    inline void setLowerRight(const glm::tvec3<T, P> &);

    inline djvBox3<T, P> & operator = (const djvBox3<T, P> &);

    inline djvBox3<T, P> & operator *= (const glm::tvec3<T, P> &);
    inline djvBox3<T, P> & operator /= (const glm::tvec3<T, P> &);

    inline djvBox3<T, P> & operator *= (T);
    inline djvBox3<T, P> & operator /= (T);

    inline operator djvBox3<int, P>() const;
    inline operator djvBox3<float, P>() const;
};

typedef djvBox2<int> djvBox2i;
typedef djvBox2<float> djvBox2f;
typedef djvBox3<int> djvBox3i;
typedef djvBox3<float> djvBox3f;

Q_DECLARE_METATYPE(djvBox2i)
Q_DECLARE_METATYPE(djvBox2f)
Q_DECLARE_METATYPE(djvBox3i)
Q_DECLARE_METATYPE(djvBox3f)

template <typename T, glm::precision P = glm::defaultp>
inline djvBox2<T, P> operator + (const djvBox2<T, P> &, T);
template <typename T, glm::precision P = glm::defaultp>
inline djvBox3<T, P> operator + (const djvBox3<T, P> &, T);
template <typename T, glm::precision P = glm::defaultp>
inline djvBox2<T, P> operator - (const djvBox2<T, P> &, T);
template <typename T, glm::precision P = glm::defaultp>
inline djvBox3<T, P> operator - (const djvBox3<T, P> &, T);
template <typename T, glm::precision P = glm::defaultp>
inline djvBox2<T, P> operator * (const djvBox2<T, P> &, T);
template <typename T, glm::precision P = glm::defaultp>
inline djvBox3<T, P> operator * (const djvBox3<T, P> &, T);
template <typename T, glm::precision P = glm::defaultp>
inline djvBox2<T, P> operator / (const djvBox2<T, P> &, T);
template <typename T, glm::precision P = glm::defaultp>
inline djvBox3<T, P> operator / (const djvBox3<T, P> &, T);

template <typename T, glm::precision P = glm::defaultp>
inline djvBox2<T, P> operator + (const djvBox2<T, P> &, const glm::tvec2<T, P> &);
template <typename T, glm::precision P = glm::defaultp>
inline djvBox3<T, P> operator + (const djvBox3<T, P> &, const glm::tvec3<T, P> &);
template <typename T, glm::precision P = glm::defaultp>
inline djvBox2<T, P> operator - (const djvBox2<T, P> &, const glm::tvec2<T, P> &);
template <typename T, glm::precision P = glm::defaultp>
inline djvBox3<T, P> operator - (const djvBox3<T, P> &, const glm::tvec3<T, P> &);
template <typename T, glm::precision P = glm::defaultp>
inline djvBox2<T, P> operator * (const djvBox2<T, P> &, const glm::tvec2<T, P> &);
template <typename T, glm::precision P = glm::defaultp>
inline djvBox3<T, P> operator * (const djvBox3<T, P> &, const glm::tvec3<T, P> &);
template <typename T, glm::precision P = glm::defaultp>
inline djvBox2<T, P> operator / (const djvBox2<T, P> &, const glm::tvec2<T, P> &);
template <typename T, glm::precision P = glm::defaultp>
inline djvBox3<T, P> operator / (const djvBox3<T, P> &, const glm::tvec3<T, P> &);

template <typename T, glm::precision P = glm::defaultp>
inline bool operator == (const djvBox2<T, P> &, const djvBox2<T, P> &);
template <typename T, glm::precision P = glm::defaultp>
inline bool operator == (const djvBox3<T, P> &, const djvBox3<T, P> &);

template <typename T, glm::precision P = glm::defaultp>
inline bool operator != (const djvBox2<T, P> &, const djvBox3<T, P> &);
template <typename T, glm::precision P = glm::defaultp>
inline bool operator != (const djvBox2<T, P> &, const djvBox3<T, P> &);

template<typename T, glm::precision P = glm::defaultp>
inline QStringList & operator >> (QStringList &, djvBox2<T, P> &) throw (QString);
template<typename T, glm::precision P = glm::defaultp>
inline QStringList & operator >> (QStringList &, djvBox3<T, P> &) throw (QString);

template<typename T, glm::precision P = glm::defaultp>
inline QStringList & operator << (QStringList &, const djvBox2<T, P> &);
template<typename T, glm::precision P = glm::defaultp>
inline QStringList & operator << (QStringList &, const djvBox3<T, P> &);

template <typename T, glm::precision P = glm::defaultp>
inline djvDebug & operator << (djvDebug &, const djvBox2<T, P> &);
template <typename T, glm::precision P = glm::defaultp>
inline djvDebug & operator << (djvDebug &, const djvBox3<T, P> &);

//@} // djvCoreMath

#include <djvBoxInline.h>
#include <djvBox2Inline.h>
#include <djvBox3Inline.h>

