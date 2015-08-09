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

//! \file djvFileBrowserTestInfoWorker.cpp

#include <djvFileBrowserTestInfoWorker.h>

#include <djvPixmapUtil.h>

#include <djvDebug.h>
#include <djvImage.h>
#include <djvImageContext.h>
#include <djvPixelDataUtil.h>

#include <QMutex>
#include <QScopedPointer>

//------------------------------------------------------------------------------
// djvFileBrowserTestInfoRequest
//------------------------------------------------------------------------------

djvFileBrowserTestInfoRequest::djvFileBrowserTestInfoRequest() :
    thumbnails   (static_cast<djvFileBrowserTestUtil::THUMBNAILS>(0)),
    thumbnailSize(static_cast<djvFileBrowserTestUtil::THUMBNAIL_SIZE>(0)),
    row          (0),
    id           (0)
{}

//------------------------------------------------------------------------------
// djvFileBrowserTestInfoRequester
//------------------------------------------------------------------------------

djvFileBrowserTestInfoRequester::djvFileBrowserTestInfoRequester(
    QObject * parent) :
    QObject(parent)
{}

//------------------------------------------------------------------------------
// djvFileBrowserTestInfoResult
//------------------------------------------------------------------------------

djvFileBrowserTestInfoResult::djvFileBrowserTestInfoResult() :
    row(0),
    id (0)
{}

//------------------------------------------------------------------------------
// djvFileBrowserTestInfoWorker
//------------------------------------------------------------------------------

djvFileBrowserTestInfoWorker::djvFileBrowserTestInfoWorker(
    djvImageContext * context,
    QObject *         parent) :
    djvFileBrowserTestAbstractWorker(parent),
    _context(context)
{}
    
djvFileBrowserTestInfoWorker::~djvFileBrowserTestInfoWorker()
{}

void djvFileBrowserTestInfoWorker::request(
    const djvFileBrowserTestInfoRequest & request)
{
    {
        QMutexLocker locker(mutex());
        
        if (request.id != id())
            return;
    }

    //DJV_DEBUG("djvFileBrowserTestInfoWorker::request");
    //DJV_DEBUG_PRINT("fileInfo = " << request.fileInfo);
    //DJV_DEBUG_PRINT("thumbnails = " << request.thumbnails);
    //DJV_DEBUG_PRINT("thumbnailSize = " << request.thumbnailSize);
    //DJV_DEBUG_PRINT("row = " << request.row);
    //DJV_DEBUG_PRINT("id = " << request.id);
    
    djvFileBrowserTestInfoResult result;
    result.row = request.row;
    result.id  = request.id;
    
    try
    {
        // Load the image.
        
        QScopedPointer<djvImageLoad> load;
        
        load.reset(_context->imageIoFactory()->load(request.fileInfo, result.info));
        
        djvPixelDataInfo::PROXY proxy = djvPixelDataInfo::PROXY_NONE;

        djvVector2i size = djvFileBrowserTestUtil::thumbnailSize(
            request.thumbnails,
            djvFileBrowserTestUtil::thumbnailSize(request.thumbnailSize),
            result.info.size,
            &proxy);
        
        //DJV_DEBUG_PRINT("size = " << size);
        //DJV_DEBUG_PRINT("proxy = " << proxy);
        
        result.pixmap = QPixmap(size.x, size.y);
        result.pixmap.fill(Qt::transparent);
    }
    catch (djvError error)
    {}

    Q_EMIT this->result(result);
}

