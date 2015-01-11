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

//! \file djvMatrix.h

#ifndef DJV_MATRIX_H
#define DJV_MATRIX_H

#include <djvBox.h>

//! \addtogroup djvCoreMath
//@{

//------------------------------------------------------------------------------
//! \class djvMatrix
//!
//! This class provides the base functionality for matrices.
//------------------------------------------------------------------------------

template<typename T, int D>
struct djvMatrix
{
    //! Constructor.

    inline djvMatrix();

    //! Constructor.

    inline djvMatrix(const djvMatrix<T, D> &);

    //! Constructor.

    inline djvMatrix(const T *);

    //! Dimension.

    static const int dimension;

    //! Dimension.

    static const int dimensionX2;

    //! Components.

    T e [D * D];

    //! Set the components from an array.

    inline void set(const T *);

    //! Zero the components.

    inline void zero();

    //! Set to the identity matrix.

    inline void identity();

    inline djvMatrix<T, D> & operator = (const djvMatrix<T, D> &);
};

//------------------------------------------------------------------------------
//! \class djvMatrix<T, 3>
//!
//! This class provides a 3x3 matrix.
//------------------------------------------------------------------------------

template<typename T>
struct djvMatrix<T, 3>
{
    //! Constructor.

    inline djvMatrix();

    //! Constructor.

    inline djvMatrix(const djvMatrix<T, 3> &);

    //! Constructor.

    inline djvMatrix(T, T, T, T, T, T, T, T, T);

    //! Constructor.

    inline djvMatrix(const T *);

    //! Dimension.

    static const int dimension;

    //! Dimension.

    static const int dimensionX2;

    //! Components.

    T e [3 * 3];

    //! Set the components from an array.

    inline void set(const T *);

    //! Set the components individually.

    inline void set(T, T, T, T, T, T, T, T, T);

    //! Zero the components.

    inline void zero();

    //! Set to the identity matrix.

    inline void identity();

    inline djvMatrix<T, 3> & operator = (const djvMatrix<T, 3> &);
};

//------------------------------------------------------------------------------
//! \class djvMatrix<T, 4>
//!
//! This class provides a 4x4 matrix.
//------------------------------------------------------------------------------

template<typename T>
struct djvMatrix<T, 4>
{
    //! Constructor.

    inline djvMatrix();

    //! Constructor.

    inline djvMatrix(const djvMatrix<T, 4> &);

    //! Constructor.

    inline djvMatrix(T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T);

    //! Constructor.

    inline djvMatrix(const T *);

    //! Dimension.

    static const int dimension;

    //! Dimension.

    static const int dimensionX2;

    //! Components.

    T e [4 * 4];

    //! Set the components from an array.

    inline void set(const T *);

    //! Set the components individually.

    inline void set(T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T);

    //! Zero the components.

    inline void zero();

    //! Set to the identity matrix.

    inline void identity();

    inline djvMatrix<T, 4> & operator = (const djvMatrix<T, 4> &);
};

//------------------------------------------------------------------------------

//! This typedef provides a 3x3 floating-point matrix.

typedef djvMatrix<double, 3> djvMatrix3f;

//! This typedef provides a 4x4 floating-point matrix.

typedef djvMatrix<double, 4> djvMatrix4f;

Q_DECLARE_METATYPE(djvMatrix3f)
Q_DECLARE_METATYPE(djvMatrix4f)

template<typename T, int D>
inline djvMatrix<T, D> operator * (const djvMatrix<T, D> &, const djvMatrix<T, D> &);

template<typename T, int D, int D2>
inline djvVector<T, D2> operator * (const djvMatrix<T, D> &, const djvVector<T, D2> &);

template<typename T>
inline djvBox<T, 2> operator * (const djvMatrix<T, 3> &, const djvBox<T, 2> &);

template<typename T, int D>
inline bool operator == (const djvMatrix<T, D> &, const djvMatrix<T, D> &);

template<typename T, int D>
inline bool operator != (const djvMatrix<T, D> &, const djvMatrix<T, D> &);

template<typename T, int D>
inline QStringList & operator >> (QStringList &, djvMatrix<T, D> &) throw (QString);

template<typename T, int D>
inline QStringList & operator << (QStringList &, const djvMatrix<T, D> &);

template<typename T, int D>
inline djvDebug & operator << (djvDebug &, const djvMatrix<T, D> &);

//@} // djvCoreMath

#include <djvMatrixInline.h>
#include <djvMatrix3Inline.h>
#include <djvMatrix4Inline.h>

#endif // DJV_MATRIX_H
