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

//! \file djvFileBrowserTestUtil.h

#ifndef DJV_FILE_BROWSER_TEST_UTIL_H
#define DJV_FILE_BROWSER_TEST_UTIL_H

#include <djvPixelData.h>
#include <djvVector.h>

#include <QMetaType>
#include <QStringList>

//------------------------------------------------------------------------------
//! \class djvFileBrowserTestUtil
//!
//! This class provides enumerations and utilities.
//------------------------------------------------------------------------------

class djvFileBrowserTestUtil
{
    Q_GADGET
    Q_ENUMS(COLUMNS)
    Q_ENUMS(THUMBNAILS)
    Q_ENUMS(THUMBNAIL_SIZE)
    
public:

    //! This enumeration provides the image thumbnail mode.

    enum THUMBNAILS
    {
        THUMBNAILS_OFF,
        THUMBNAILS_LOW,
        THUMBNAILS_HIGH,

        THUMBNAILS_COUNT
    };

    //! Get the image thumbnail mode labels.

    static const QStringList & thumbnailsLabels();

    //! This enumeration provides the image thumbnail size.

    enum THUMBNAIL_SIZE
    {
        THUMBNAIL_SMALL,
        THUMBNAIL_MEDIUM,
        THUMBNAIL_LARGE,

        THUMBNAIL_SIZE_COUNT
    };

    //! Get the image thumbnail size labels.

    static const QStringList & thumbnailSizeLabels();

    //! Get an image thumbnail size.

    static int thumbnailSize(THUMBNAIL_SIZE);

    //! Get an image thumbnail size.

    static djvVector2i thumbnailSize(
        THUMBNAILS                thumbnails,
        int                       thumbnailSize,
        const djvVector2i &       imageSize,
        djvPixelDataInfo::PROXY * proxy         = 0);

    //! This enumeration provides the columns.
    
    enum COLUMNS
    {
        NAME,
        SIZE,
#if ! defined(DJV_WINDOWS)
        USER,
#endif
        PERMISSIONS,
        TIME,
        
        COLUMNS_COUNT
    };

    //! Get the column labels.

    static const QStringList & columnsLabels();
};

#endif // DJV_FILE_BROWSER_TEST_UTIL_H

