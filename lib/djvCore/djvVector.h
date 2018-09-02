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

#include <djvDebug.h>
#include <djvStringUtil.h>

#include <QMetaType>
#include <QString>
#include <QVector>

//! \addtogroup djvCoreMath
//@{

//------------------------------------------------------------------------------
//! \class djvVector
//!
//! This class provides the base functionality for vectors.
//------------------------------------------------------------------------------

template<typename T, int D>
struct djvVector
{
    inline djvVector();
    inline djvVector(const djvVector<T, D> &);
    inline djvVector(T);

    static const int dimension;

    T e [D];

    //! Set the components to the same value.
    inline void set(T);

    //! Zero the components.
    inline void zero();

    djvVector<T, D> & operator = (const djvVector<T, D> &);

    inline djvVector<T, D> & operator += (const djvVector<T, D> &);
    inline djvVector<T, D> & operator -= (const djvVector<T, D> &);
    inline djvVector<T, D> & operator *= (const djvVector<T, D> &);
    inline djvVector<T, D> & operator /= (const djvVector<T, D> &);

    inline djvVector<T, D> & operator += (T);
    inline djvVector<T, D> & operator -= (T);
    inline djvVector<T, D> & operator *= (T);
    inline djvVector<T, D> & operator /= (T);
};

//------------------------------------------------------------------------------
//! \class djvVector<T, 2>
//!
//! This class provides a two-dimensional vector.
//------------------------------------------------------------------------------

template<typename T>
struct djvVector<T, 2>
{
    inline djvVector();
    inline djvVector(const djvVector<T, 2> &);
    inline djvVector(T, T);
    inline djvVector(T);

    static const int dimension;

    T e [2];

    //! Component access.
    T & x, & y;

    //! Set the components to the same value.
    inline void set(T);

    //! Zero the components.
    inline void zero();

    inline djvVector<T, 2> & operator = (const djvVector<T, 2> &);

    inline djvVector<T, 2> & operator += (const djvVector<T, 2> &);
    inline djvVector<T, 2> & operator -= (const djvVector<T, 2> &);
    inline djvVector<T, 2> & operator *= (const djvVector<T, 2> &);
    inline djvVector<T, 2> & operator /= (const djvVector<T, 2> &);

    inline djvVector<T, 2> & operator += (T);
    inline djvVector<T, 2> & operator -= (T);
    inline djvVector<T, 2> & operator *= (T);
    inline djvVector<T, 2> & operator /= (T);

    inline operator djvVector<int, 2>() const;
    inline operator djvVector<double, 2>() const;
};

//------------------------------------------------------------------------------
//! \class djvVector<T, 3>
//!
//! This class provides a three-dimensional vector.
//------------------------------------------------------------------------------

template<typename T>
struct djvVector<T, 3>
{
    inline djvVector();
    inline djvVector(const djvVector<T, 3> &);
    inline djvVector(T, T, T);
    inline djvVector(T);

    static const int dimension;

    T e [3];

    //! Component access.
    T & x, & y, & z;

    //! Set the components to the same value.
    inline void set(T);

    //! Zero the components.
    inline void zero();

    inline djvVector<T, 3> & operator = (const djvVector<T, 3> &);

    inline djvVector<T, 3> & operator += (const djvVector<T, 3> &);
    inline djvVector<T, 3> & operator -= (const djvVector<T, 3> &);
    inline djvVector<T, 3> & operator *= (const djvVector<T, 3> &);
    inline djvVector<T, 3> & operator /= (const djvVector<T, 3> &);

    inline djvVector<T, 3> & operator += (T);
    inline djvVector<T, 3> & operator -= (T);
    inline djvVector<T, 3> & operator *= (T);
    inline djvVector<T, 3> & operator /= (T);

    inline operator djvVector<int, 3>() const;
    inline operator djvVector<double, 3>() const;
};

typedef djvVector<int, 2> djvVector2i;
typedef djvVector<double, 2> djvVector2f;
typedef djvVector<int, 3> djvVector3i;
typedef djvVector<double, 3> djvVector3f;

Q_DECLARE_METATYPE(djvVector2i)
Q_DECLARE_METATYPE(djvVector2f)
Q_DECLARE_METATYPE(djvVector3i)
Q_DECLARE_METATYPE(djvVector3f)

template<typename T, int D>
inline djvVector<T, D> operator + (const djvVector<T, D> &, const djvVector<T, D> &);
template<typename T, int D>
inline djvVector<T, D> operator - (const djvVector<T, D> &, const djvVector<T, D> &);
template<typename T, int D>
inline djvVector<T, D> operator * (const djvVector<T, D> &, const djvVector<T, D> &);
template<typename T, int D>
inline djvVector<T, D> operator / (const djvVector<T, D> &, const djvVector<T, D> &);

template<typename T, int D>
inline djvVector<T, D> operator + (const djvVector<T, D> &, T);
template<typename T, int D>
inline djvVector<T, D> operator - (const djvVector<T, D> &, T);
template<typename T, int D>
inline djvVector<T, D> operator * (const djvVector<T, D> &, T);
template<typename T, int D>
inline djvVector<T, D> operator / (const djvVector<T, D> &, T);

template<typename T, int D>
inline djvVector<T, D> operator - (const djvVector<T, D> &);

template<typename T, int D>
inline QStringList & operator >> (QStringList &, djvVector<T, D> &) throw (QString);

template<typename T, int D>
inline QStringList & operator << (QStringList &, const djvVector<T, D> &);

template<typename T, int D>
inline bool operator == (const djvVector<T, D> &, const djvVector<T, D> &);

template<typename T, int D>
inline bool operator != (const djvVector<T, D> &, const djvVector<T, D> &);

template<typename T, int D>
inline bool operator <  (const djvVector<T, D> &, const djvVector<T, D> &);
template<typename T, int D>
inline bool operator <= (const djvVector<T, D> &, const djvVector<T, D> &);

template<typename T, int D>
inline bool operator >  (const djvVector<T, D> &, const djvVector<T, D> &);
template<typename T, int D>
inline bool operator >= (const djvVector<T, D> &, const djvVector<T, D> &);

template<typename T, int D>
inline djvDebug & operator << (djvDebug &, const djvVector<T, D> &);

//@} // djvCoreMath

#include <djvVectorInline.h>
#include <djvVector2Inline.h>
#include <djvVector3Inline.h>

