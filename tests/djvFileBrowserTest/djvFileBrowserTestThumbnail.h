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

//! \file djvFileBrowserTestThumbnail.h

#ifndef DJV_FILE_BROWSER_TEST_THUMBNAIL_H
#define DJV_FILE_BROWSER_TEST_THUMBNAIL_H

#include <djvFileBrowserTestAbstractWorker.h>
#include <djvFileBrowserTestUtil.h>

#include <djvFileInfo.h>
#include <djvImageIo.h>

#include <QObject>
#include <QPixmap>

struct djvFileBrowserTestThumbnailWorkerPrivate;

class djvImageContext;

//------------------------------------------------------------------------------
//! \struct djvFileBrowserTestThumbnailRequest
//!
//! This struct is used to request data from
//! djvFileBrowserTestThumbnailRequester.
//------------------------------------------------------------------------------

struct djvFileBrowserTestThumbnailRequest
{
    //! Constructor.
    
    djvFileBrowserTestThumbnailRequest();
    
    djvFileInfo                        fileInfo;      //!< File information.
    djvFileBrowserTestUtil::THUMBNAILS thumbnails;    //!< Thumbnails mode.
    int                                thumbnailSize; //!< Thumbnail size.
    int                                row;           //!< Model row.
    quint64                            id;            //!< Request ID.
};

//------------------------------------------------------------------------------
//! \class djvFileBrowserTestThumbnailRequester
//!
//! This class is used to request an image thumbnail.
//------------------------------------------------------------------------------

class djvFileBrowserTestThumbnailRequester : public QObject
{
    Q_OBJECT
    
public:

    //! Constructor.
    
    explicit djvFileBrowserTestThumbnailRequester(QObject * parent = 0);
    
Q_SIGNALS:

    //! Request an image thumbnail.
    
    void request(const djvFileBrowserTestThumbnailRequest &);
};

//------------------------------------------------------------------------------
//! \struct djvFileBrowserTestThumbnailResult
//!
//! This struct is used to return data from djvFileBrowserTestThumbnailWorker.
//------------------------------------------------------------------------------

struct djvFileBrowserTestThumbnailResult
{
    //! Constructor.
    
    djvFileBrowserTestThumbnailResult();
    
    djvImageIoInfo info;        //!< Image I/O information.
    QPixmap        pixmap;      //!< Thumbnail.
    bool           pixmapEmpty; //!< Whether the thumbnail is empty.
    int            row;         //!< Model row.
    quint64        id;          //!< Request ID.
};
        
//------------------------------------------------------------------------------
//! \class djvFileBrowserTestThumbnailWorker
//!
//! This class provides an image thumbnail worker.
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

    //! Request an image thumbnail.
    
    void request(const djvFileBrowserTestThumbnailRequest &);
    
    virtual void start();
    
    virtual void finish();

Q_SIGNALS:

    //! This signal is emitted when a request has been completed.

    void result(const djvFileBrowserTestThumbnailResult &);

private:

    DJV_PRIVATE_COPY(djvFileBrowserTestThumbnailWorker)
    
    djvFileBrowserTestThumbnailWorkerPrivate * _p;
};

//------------------------------------------------------------------------------
//! \class djvFileBrowserTestThumbnail
//!
//! This class provides image thumbnails.
//------------------------------------------------------------------------------

class djvFileBrowserTestThumbnail : public QObject
{
    Q_OBJECT
    
public:

    //! Constructor.
    
    explicit djvFileBrowserTestThumbnail(djvImageContext *, QObject * parent = 0);
    
    //! Destructor.
    
    virtual ~djvFileBrowserTestThumbnail();
    
public Q_SLOTS:

    //! Request an image thumbnail.
    
    void request(const djvFileBrowserTestThumbnailRequest &);
    
    //! Set the request ID.
    
    void setId(quint64);
    
Q_SIGNALS:

    //! This signal is emitted when a request has been completed.

    void result(const djvFileBrowserTestThumbnailResult &);
    
private:

    djvFileBrowserTestThumbnailRequester * nextRequester();

    djvImageContext *                               _context;
    QVector<djvFileBrowserTestThumbnailRequester *> _requesters;
    QVector<djvFileBrowserTestThumbnailWorker *>    _workers;
    QVector<QThread *>                              _threads;
    int                                             _threadIndex;
};

#endif // DJV_FILE_BROWSER_TEST_THUMBNAIL_H

