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

//! \file djvVectorUtil.h

#ifndef DJV_VECTOR_UTIL_H
#define DJV_VECTOR_UTIL_H

#include <djvVector.h>

class QPoint;
class QSize;

//! \addtogroup djvCoreMath
//@{

//------------------------------------------------------------------------------
//! \class djvVectorUtil
//!
//! This class provides vector utilities.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvVectorUtil
{
public:

    //! Destructor.
    
    virtual ~djvVectorUtil() = 0;
    
    //! Get the absolute value of the vector components.

    template<typename T, int D>
    static inline djvVector<T, D> abs(const djvVector<T, D> &);

    //! Swap the vector components.

    template<typename T, int D>
    static inline djvVector<T, D> swap(const djvVector<T, D> &);

    //! Get the minimum of the vector components.

    template<typename T, int D>
    static inline djvVector<T, D> min(const djvVector<T, D> &, const djvVector<T, D> &);

    //! Get the maximum of the vector components.

    template<typename T, int D>
    static inline djvVector<T, D> max(const djvVector<T, D> &, const djvVector<T, D> &);

    //! Clamp the vector components.

    template<typename T, int D>
    static inline djvVector<T, D> clamp(
        const djvVector<T, D> &,
        const djvVector<T, D> &,
        const djvVector<T, D> &);

    //! Is the vector's size greater than zero?

    template<typename T, int D>
    static inline bool isSizeValid(const djvVector<T, D> &);

    //! Get the length of a vector.

    template<typename T, int D>
    static inline double length(const djvVector<T, D> &);

    //! Get the area of a vector.

    template<typename T, int D>
    static inline double area(const djvVector<T, D> &);

    //! Normalize a vector.

    template<typename T, int D>
    static inline djvVector<T, D> normalize(const djvVector<T, D> &);

    //! Vector dot product.

    template<typename T, int D>
    static inline double dot(const djvVector<T, D> &, const djvVector<T, D> &);

    //! Aspect ratio of a vector.

    template<typename T, int D>
    static inline double aspect(const djvVector<T, D> &);

    //! Convert vector types.

    template<typename T, typename T2, int D>
    static inline djvVector<T2, D> convert(const djvVector<T, D> &);

    //! Round to the largest integer value.

    template<typename T, typename T2, int D>
    static inline djvVector<T2, D> ceil(const djvVector<T, D> &);

    //! Round to the smallest integer value.

    template<typename T, typename T2, int D>
    static inline djvVector<T2, D> floor(const djvVector<T, D> &);

    //! Convert spherical to cartesian coordinates.

    static void sphericalToCartesian(const djvVector3f &, djvVector3f *);

    //! Convert cartesian to spherical coordinates.

    static void cartesianToSpherical(const djvVector3f &, djvVector3f *);
    
    //! Convert to Qt.
    
    static QPoint toQPoint(const djvVector2i &);
    
    //! Convert to Qt.
    
    static QSize toQSize(const djvVector2i &);
    
    //! Convert from Qt.
    
    static djvVector2i fromQPoint(const QPoint &);
    
    //! Convert from Qt.
    
    static djvVector2i fromQSize(const QSize &);
};

//@} // djvCoreMath

#include <djvVectorUtilInline.h>

#endif // DJV_VECTOR_UTIL_H

