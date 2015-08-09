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

//! \file djvFileBrowserTestInfoWorker.h

#ifndef DJV_FILE_BROWSER_TEST_INFO_WORKER_H
#define DJV_FILE_BROWSER_TEST_INFO_WORKER_H

#include <djvFileBrowserTestAbstractWorker.h>
#include <djvFileBrowserTestUtil.h>

#include <djvFileInfo.h>
#include <djvImageIo.h>

#include <QObject>
#include <QPixmap>

class djvImageContext;

//------------------------------------------------------------------------------
//! \struct djvFileBrowserTestInfoRequest
//!
//! This struct is used to request data from djvFileBrowserTestInfoRequester.
//------------------------------------------------------------------------------

struct djvFileBrowserTestInfoRequest
{
    //! Constructor.
    
    djvFileBrowserTestInfoRequest();
    
    djvFileInfo                            fileInfo;      //!< File information.
    djvFileBrowserTestUtil::THUMBNAILS     thumbnails;    //!< Thumbnails mode.
    djvFileBrowserTestUtil::THUMBNAIL_SIZE thumbnailSize; //!< Thumbnail size.
    int                                    row;           //!< Model row.
    quint64                                id;            //!< Request ID.
};

//------------------------------------------------------------------------------
//! \class djvFileBrowserTestInfoRequester
//!
//! This class is used to request image information.
//------------------------------------------------------------------------------

class djvFileBrowserTestInfoRequester : public QObject
{
    Q_OBJECT
    
public:

    //! Constructor.
    
    explicit djvFileBrowserTestInfoRequester(QObject * parent = 0);
    
Q_SIGNALS:

    //! Request image information.
    
    void request(const djvFileBrowserTestInfoRequest &);
};

//------------------------------------------------------------------------------
//! \struct djvFileBrowserTestInfoResult
//!
//! This struct is used to return data from djvFileBrowserTestInfoWorker.
//------------------------------------------------------------------------------

struct djvFileBrowserTestInfoResult
{
    //! Constructor.
    
    djvFileBrowserTestInfoResult();
    
    djvImageIoInfo info;   //!< Image I/O information.
    QPixmap        pixmap; //!< Thumbnail.
    int            row;    //!< Model row.
    quint64        id;     //!< Request ID.
};
        
//------------------------------------------------------------------------------
//! \class djvFileBrowserTestInfoWorker
//!
//! This class provides image information.
//------------------------------------------------------------------------------

class djvFileBrowserTestInfoWorker : public djvFileBrowserTestAbstractWorker
{
    Q_OBJECT
    
public:

    //! Constructor.
    
    explicit djvFileBrowserTestInfoWorker(
        djvImageContext * context,
        QObject *         parent = 0);

    //! Destructor.
    
    virtual ~djvFileBrowserTestInfoWorker();
    
public Q_SLOTS:

    //! Request image information.
    
    void request(const djvFileBrowserTestInfoRequest &);
    
Q_SIGNALS:

    //! This signal is emitted when a request has been completed.

    void result(const djvFileBrowserTestInfoResult &);

private:

    djvImageContext * _context;
};

#endif // DJV_FILE_BROWSER_TEST_INFO_WORKER_H

