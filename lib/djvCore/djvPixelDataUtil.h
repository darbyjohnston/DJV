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

//! \file djvPixelDataUtil.h

#ifndef DJV_PIXEL_DATA_UTIL_H
#define DJV_PIXEL_DATA_UTIL_H

#include <djvOpenGlImage.h>
#include <djvPixelData.h>

class QPixmap;

//! \addtogroup djvCoreImage
//@{

//------------------------------------------------------------------------------
//! \class djvPixelDataUtil
//!
//! This class provides pixel data utilities.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvPixelDataUtil
{
public:

    //! Destructor.
    
    virtual ~djvPixelDataUtil();
    
    //! Get the number of bytes in a scanline.

    static quint64 scanlineByteCount(const djvPixelDataInfo &);

    //! Get the number of bytes in the data.

    static quint64 dataByteCount(const djvPixelDataInfo &);

    //! Proxy scale pixel data.

    static void proxyScale(
        const djvPixelData &,
        djvPixelData &,
        djvPixelDataInfo::PROXY);

    //! Calculate the proxy scale.

    static int proxyScale(djvPixelDataInfo::PROXY);

    //! Calculate the size of a proxy scale.

    static djvVector2i proxyScale(const djvVector2i &, djvPixelDataInfo::PROXY);

    //! Calculate the size of a proxy scale.

    static djvBox2i proxyScale(const djvBox2i &, djvPixelDataInfo::PROXY);

    //! Interleave pixel data channels.

    static void planarInterleave(
        const djvPixelData &,
        djvPixelData &,
        djvPixelDataInfo::PROXY = djvPixelDataInfo::PROXY_NONE);

    //! De-interleave pixel data channels.

    static void planarDeinterleave(const djvPixelData &, djvPixelData &);

    //! Create a linear gradient.

    static void gradient(djvPixelData &);

    //! Convert to Qt.
    
    static QPixmap toQt(
        const djvPixelData &          pixelData,
        const djvOpenGlImageOptions & options   = djvOpenGlImageOptions(),
        djvOpenGlImageState *         state     = 0,
        djvOpenGlOffscreenBuffer *    buffer    = 0);
};

//@} // djvCoreImage

#endif // DJV_PIXEL_DATA_UTIL_H

