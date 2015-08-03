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

//! \file djvFileBrowserTestThumbnailWorker.h

#ifndef DJV_FILE_BROWSER_TEST_THUMBNAIL_WORKER_H
#define DJV_FILE_BROWSER_TEST_THUMBNAIL_WORKER_H

#include <djvFileBrowserTestAbstractWorker.h>
#include <djvFileBrowserTestUtil.h>

#include <djvFileInfo.h>
#include <djvImageIo.h>
#include <djvOpenGlContext.h>

#include <QObject>
#include <QPixmap>
#include <QScopedPointer>

class djvImageContext;

//------------------------------------------------------------------------------
//! \struct djvFileBrowserTestThumbnailRequest
//!
//! This struct provides an image thumbnail request.
//------------------------------------------------------------------------------

struct djvFileBrowserTestThumbnailRequest
{
    //! Constructor.
    
    djvFileBrowserTestThumbnailRequest();
    
    djvFileInfo                            fileInfo;
    djvFileBrowserTestUtil::THUMBNAILS     thumbnails;
    djvFileBrowserTestUtil::THUMBNAIL_SIZE thumbnailSize;
    int                                    row;
    quint64                                id;
};

//------------------------------------------------------------------------------
//! \class djvFileBrowserTestThumbnailRequester
//!
//! This class provides functionality for requesting an image thumbnail.
//------------------------------------------------------------------------------

class djvFileBrowserTestThumbnailRequester : public QObject
{
    Q_OBJECT
    
public:

    //! Constructor.
    
    explicit djvFileBrowserTestThumbnailRequester(QObject * parent = 0);
    
Q_SIGNALS:

    //! Request image information.
    
    void info(const djvFileBrowserTestThumbnailRequest &);

    //! Request an image thumbnail.
    
    void thumbnail(const djvFileBrowserTestThumbnailRequest &);
};

//------------------------------------------------------------------------------
//! \struct djvFileBrowserTestThumbnailResult
//!
//! This struct provides an image thumbnail result.
//------------------------------------------------------------------------------

struct djvFileBrowserTestThumbnailResult
{
    //! Constructor.
    
    djvFileBrowserTestThumbnailResult();
    
    QPixmap        pixmap;
    djvImageIoInfo info;
    int            row;
    quint64        id;
};
        
//------------------------------------------------------------------------------
//! \class djvFileBrowserTestThumbnailWorker
//!
//! This class provides functionality for generating image thumbnails.
//------------------------------------------------------------------------------

class djvFileBrowserTestThumbnailWorker : public djvFileBrowserTestAbstractWorker
{
    Q_OBJECT
    
public:

    //! Constructor.
    
    explicit djvFileBrowserTestThumbnailWorker(
        djvImageContext * context,
        QObject *         parent = 0);

    //! Destructor.
    
    virtual ~djvFileBrowserTestThumbnailWorker();
    
public Q_SLOTS:

    //! Request image information.
    
    void info(const djvFileBrowserTestThumbnailRequest &);
    
    //! Request an image thumbnail.
    
    void thumbnail(const djvFileBrowserTestThumbnailRequest &);
    
    virtual void start();
    
    virtual void finish();

Q_SIGNALS:

    //! This signal is emitted when a request has been completed.

    void result(const djvFileBrowserTestThumbnailResult &);

private:

    djvImageContext *                _context;
    QScopedPointer<djvOpenGlContext> _gl;
};

#endif // DJV_FILE_BROWSER_TEST_THUMBNAIL_WORKER_H

