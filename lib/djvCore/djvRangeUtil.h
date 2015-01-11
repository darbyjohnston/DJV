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

//! \file djvRangeUtil.h

#ifndef DJV_RANGE_UTIL_H
#define DJV_RANGE_UTIL_H

#include <djvRange.h>
#include <djvSequence.h>

//! \addtogroup djvCoreMath
//@{

//------------------------------------------------------------------------------
//! \class djvRangeUtil
//!
//! This class provides number range utilities.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvRangeUtil
{
public:

    //! Destructor.
    
    inline virtual ~djvRangeUtil() = 0;
    
    //! Get whether a value is inside a range.

    template<typename T>
    static inline bool intersect(T, const djvRange<T> &);

    //! Get whether a value is inside a range.

    static inline bool intersect(int, const djvRange<int> &);

    //! Get the intersection of two ranges.

    template<typename T>
    static inline bool intersect(
        const djvRange<T> &,
        const djvRange<T> &,
        djvRange<T> *);

    //! Get the intersection of two ranges.

    static inline bool intersect(
        const djvRange<int> &,
        const djvRange<int> &,
        djvRange<int> *);

    //! Get the bound of two ranges.

    template<typename T>
    static inline void bound(const djvRange<T> &, const djvRange<T> &, djvRange<T> *);

    //! Convert a sorted list of frames into ranges.

    static inline djvFrameRangeList range(const djvFrameList &);
    
    //! Convert a range into frames.

    static inline djvFrameList frames(const djvFrameRange &);
    
    //! Convert a list of ranges into frames.

    static inline djvFrameList frames(const djvFrameRangeList &);
};

//@} // djvCoreMath

#include <djvRangeUtilInline.h>

#endif // DJV_RANGE_UTIL_H

