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

//! \file djvMatrixUtil.h

#ifndef DJV_MATRIX_UTIL_H
#define DJV_MATRIX_UTIL_H

#include <djvMatrix.h>

//! \addtogroup djvCoreMath
//@{

//------------------------------------------------------------------------------
//! \class djvMatrixUtil
//!
//! This class provides matrix utilities.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvMatrixUtil
{
public:

    //! Destructor.
    
    inline virtual ~djvMatrixUtil() = 0;
    
    //! Transpose a matrix.

    template<typename T, int D>
    static inline djvMatrix<T, D> transpose(const djvMatrix<T, D> &);

    //! Create a translation matrix.

    template<typename T, int D, int D2>
    static inline djvMatrix<T, D> translate(const djvVector<T, D2> &);

    //! Create a translation matrix.

    static inline djvMatrix3f translate3f(const djvVector2f &);

    //! Create a scale matrix.

    template<typename T, int D, int D2>
    static inline djvMatrix<T, D> scale(const djvVector<T, D2> &);

    //! Create a scale matrix.

    static inline djvMatrix3f scale3f(const djvVector2f &);

    //! Create a rotation matrix.

    static inline djvMatrix3f rotate3f(double);

    //! Convert matrix types.

    template <typename T, typename T2, int D>
    static inline djvMatrix<T2, D> convert(const djvMatrix<T, D> &);

    //! Convert a 3x3 matrix to a 4x4 matrix.

    template <typename T>
    static inline djvMatrix<T, 4> matrix4(const djvMatrix<T, 3> &);
};

//@} // djvCoreMath

#include <djvMatrixUtilInline.h>

#endif // DJV_MATRIX_UTIL_H
