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

//! \file djvListUtil.h

#ifndef DJV_LIST_UTIL_H
#define DJV_LIST_UTIL_H

#include <djvConfig.h>
#include <djvCoreExport.h>

#include <QVector>

//! \addtogroup djvCoreMisc
//@{

//------------------------------------------------------------------------------
//! \class djvListUtil
//!
//! This class provides utilities for lists.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvListUtil
{
public:

    //! Destructor.
    
    inline virtual ~djvListUtil() = 0;
    
    //! Convert list types.

    template<typename A, typename B>
    static inline void convert(const QVector<A> &, QVector<B> &);

    //! Convert list types.

    template<typename A, typename B>
    static inline QVector<B> convert(const QVector<A> &);

    //! Convert and append list types.

    template<typename A, typename B>
    static inline void convertAndAppend(const QVector<A> &, QVector<B> &);
};

//@} // djvCoreMisc

#include <djvListUtilInline.h>

#endif // DJV_LIST_UTIL_H

