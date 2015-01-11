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

//! \file djvRange.h

#ifndef DJV_RANGE_H
#define DJV_RANGE_H

#include <djvDebug.h>

//! \addtogroup djvCoreMath
//@{

//------------------------------------------------------------------------------
//! \struct djvRange
//!
//! This struct provides a number range.
//------------------------------------------------------------------------------

template<typename T>
struct djvRange
{
    //! Constructor.

    inline djvRange(T = T(0));

    //! Constructor.

    inline djvRange(T min, T max);

    //! Minimum value.

    T min;

    //! Maximum value.

    T max;
};

//------------------------------------------------------------------------------

//! This typedef provides a frame range.

typedef djvRange<qint64> djvFrameRange;

//! This typedef provides a frame range list.

typedef QVector<djvFrameRange> djvFrameRangeList;

template<typename T>
inline bool operator == (const djvRange<T> &, const djvRange<T> &);

template<typename T>
inline bool operator != (const djvRange<T> &, const djvRange<T> &);

template<typename T>
inline djvDebug & operator << (djvDebug &, const djvRange<T> &);

//@} // djvCoreMath

#include <djvRangeInline.h>

#endif // DJV_RANGE_H

