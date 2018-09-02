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

//! \addtogroup djvCoreMath
//@{

//------------------------------------------------------------------------------
//! \class djvBox
//!
//! This class provides the base functionality for axis-aligned boxes.
//------------------------------------------------------------------------------

template<typename T, int D>
struct djvBox
{
    inline djvBox();
    inline djvBox(const djvBox<T, D> &);
    inline djvBox(const djvVector<T, D> & position, const djvVector<T, D> & size);
    inline djvBox(const djvVector<T, D> & size);

    static const int dimension;
    djvVector<T, D> position;
    djvVector<T, D> size;

    //! Is the box valid (width and height are greater than zero).    
    inline bool isValid() const;
    
    //! Zero the box.
    inline void zero();

    //! Get the lower right hand corner.
    inline djvVector<T, D> lowerRight() const;

    //! Set the lower right hand corner.
    inline void setLowerRight(const djvVector<T, D> &);

    inline djvBox<T, D> & operator *= (const djvVector<T, D> &);
    inline djvBox<T, D> & operator /= (const djvVector<T, D> &);

    inline djvBox<T, D> & operator *= (T);
    inline djvBox<T, D> & operator /= (T);
};

//------------------------------------------------------------------------------
//! \class djvBox<T, 2>
//!
//! This class provides a two-dimensional axis-aligned box.
//------------------------------------------------------------------------------

template<typename T>
struct djvBox<T, 2>
{
    inline djvBox();
    inline djvBox(const djvBox<T, 2> &);
    inline djvBox(const djvVector<T, 2> & position, const djvVector<T, 2> & size);
    inline djvBox(const djvVector<T, 2> & size);
    inline djvBox(T x, T y, T w, T h);
    inline djvBox(T w, T h);

    static const int dimension;
    djvVector<T, 2> position;
    djvVector<T, 2> size;

    //! Component access.
    T & x, & y, & w, & h;

    //! Is the box valid (width and height are greater than zero).
    inline bool isValid() const;
    
    //! Zero the box.
    inline void zero();

    //! Get the lower right hand corner.
    inline djvVector<T, 2> lowerRight() const;

    //! Set the lower right hand corner.
    inline void setLowerRight(const djvVector<T, 2> &);

    inline djvBox<T, 2> & operator = (const djvBox<T, 2> &);

    inline djvBox<T, 2> & operator *= (const djvVector<T, 2> &);
    inline djvBox<T, 2> & operator /= (const djvVector<T, 2> &);

    inline djvBox<T, 2> & operator *= (T);
    inline djvBox<T, 2> & operator /= (T);

    inline operator djvBox<int, 2>() const;
    inline operator djvBox<double, 2>() const;
};

//------------------------------------------------------------------------------
//! \class djvBox<T, 3>
//!
//! This class provides a three-dimensional axis-aligned box.
//------------------------------------------------------------------------------

template<typename T>
struct djvBox<T, 3>
{
    inline djvBox();
    inline djvBox(const djvBox<T, 3> &);
    inline djvBox(const djvVector<T, 3> & position, const djvVector<T, 3> & size);
    inline djvBox(const djvVector<T, 3> & size);
    inline djvBox(T x, T y, T z, T w, T h, T d);
    inline djvBox(T w, T h, T d);

    static const int dimension;
    djvVector<T, 3> position;
    djvVector<T, 3> size;

    //! Component access.
    T & x, & y, & z, & w, & h, & d;

    //! Is the box valid (width and height are greater than zero).
    inline bool isValid() const;
    
    //! Zero the box.
    inline void zero();

    //! Get the lower right hand corner.
    inline djvVector<T, 3> lowerRight() const;

    //! Set the lower right hand corner.
    inline void setLowerRight(const djvVector<T, 3> &);

    inline djvBox<T, 3> & operator = (const djvBox<T, 3> &);

    inline djvBox<T, 3> & operator *= (const djvVector<T, 3> &);
    inline djvBox<T, 3> & operator /= (const djvVector<T, 3> &);

    inline djvBox<T, 3> & operator *= (T);
    inline djvBox<T, 3> & operator /= (T);

    inline operator djvBox<int, 3>() const;
    inline operator djvBox<double, 3>() const;
};

typedef djvBox<int, 2> djvBox2i;
typedef djvBox<double, 2> djvBox2f;
typedef djvBox<int, 3> djvBox3i;
typedef djvBox<double, 3> djvBox3f;

Q_DECLARE_METATYPE(djvBox2i)
Q_DECLARE_METATYPE(djvBox2f)
Q_DECLARE_METATYPE(djvBox3i)
Q_DECLARE_METATYPE(djvBox3f)

template <typename T, int D>
inline djvBox<T, D> operator + (const djvBox<T, D> &, T);
template <typename T, int D>
inline djvBox<T, D> operator - (const djvBox<T, D> &, T);
template <typename T, int D>
inline djvBox<T, D> operator * (const djvBox<T, D> &, T);
template <typename T, int D>
inline djvBox<T, D> operator / (const djvBox<T, D> &, T);

template <typename T, int D>
inline djvBox<T, D> operator + (const djvBox<T, D> &, const djvVector<T, D> &);
template <typename T, int D>
inline djvBox<T, D> operator - (const djvBox<T, D> &, const djvVector<T, D> &);
template <typename T, int D>
inline djvBox<T, D> operator * (const djvBox<T, D> &, const djvVector<T, D> &);
template <typename T, int D>
inline djvBox<T, D> operator / (const djvBox<T, D> &, const djvVector<T, D> &);

template <typename T, int D>
inline bool operator == (const djvBox<T, D> &, const djvBox<T, D> &);

template <typename T, int D>
inline bool operator != (const djvBox<T, D> &, const djvBox<T, D> &);

template<typename T, int D>
inline QStringList & operator >> (QStringList &, djvBox<T, D> &) throw (QString);

template<typename T, int D>
inline QStringList & operator << (QStringList &, const djvBox<T, D> &);

template <typename T, int D>
inline djvDebug & operator << (djvDebug &, const djvBox<T, D> &);

//@} // djvCoreMath

#include <djvBoxInline.h>
#include <djvBox2Inline.h>
#include <djvBox3Inline.h>

