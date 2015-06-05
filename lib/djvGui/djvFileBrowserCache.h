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

//! \file djvFileBrowserCache.h

#ifndef DJV_FILE_BROWSER_CACHE_H
#define DJV_FILE_BROWSER_CACHE_H

#include <djvGuiExport.h>

#include <djvFileInfo.h>
#include <djvImageIo.h>

#include <QCache>
#include <QPixmap>

//------------------------------------------------------------------------------
//! \struct djvFileBrowserCacheItem
//!
//! This struct provides a file browser thumbnail cache item.
//------------------------------------------------------------------------------

struct djvFileBrowserCacheItem
{
    djvFileBrowserCacheItem();
    
    djvFileBrowserCacheItem(
        const djvImageIoInfo &  imageInfo,
        const djvVector2i &     thumbnailSize,
        djvPixelDataInfo::PROXY thumbnailProxy,
        const QPixmap &         thumbnail);

    djvImageIoInfo          imageInfo;
    djvVector2i             thumbnailSize;
    djvPixelDataInfo::PROXY thumbnailProxy;
    QPixmap                 thumbnail;
};

//------------------------------------------------------------------------------
//! \class djvFileBrowserCache
//!
//! This class provides a file browser thumbnail cache.
//------------------------------------------------------------------------------

class DJV_GUI_EXPORT djvFileBrowserCache :
    public QCache<djvFileInfo, djvFileBrowserCacheItem>
{};

#endif // DJV_FILE_BROWSER_CACHE_H

