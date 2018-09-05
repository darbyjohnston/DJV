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

#pragma once

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
    djvFileBrowserTestInfoRequest();
    
    djvFileInfo                        fileInfo;      //!< File information.
    djvFileBrowserTestUtil::THUMBNAILS thumbnails;    //!< Thumbnails mode.
    int                                thumbnailSize; //!< Thumbnail size.
    int                                row;           //!< Model row.
    quint64                            id;            //!< Request ID.
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
    explicit djvFileBrowserTestInfoRequester(QObject * parent = nullptr);
    
Q_SIGNALS:
    void request(const djvFileBrowserTestInfoRequest &);
};

//------------------------------------------------------------------------------
//! \struct djvFileBrowserTestInfoResult
//!
//! This struct is used to return data from djvFileBrowserTestInfoWorker.
//------------------------------------------------------------------------------

struct djvFileBrowserTestInfoResult
{
    djvFileBrowserTestInfoResult();
    
    djvImageIoInfo info;   //!< Image I/O information.
    QPixmap        pixmap; //!< Thumbnail.
    int            row;    //!< Model row.
    quint64        id;     //!< Request ID.
};

//------------------------------------------------------------------------------
//! \class djvFileBrowserTestInfoWorker
//!
//! This class provides an image information worker.
//------------------------------------------------------------------------------

class djvFileBrowserTestInfoWorker : public djvFileBrowserTestAbstractWorker
{
    Q_OBJECT
    
public:
    explicit djvFileBrowserTestInfoWorker(
        djvImageContext * context,
        QObject *         parent = nullptr);

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

//------------------------------------------------------------------------------
//! \class djvFileBrowserTestInfo
//!
//! This class provides image information.
//------------------------------------------------------------------------------

class djvFileBrowserTestInfo : public QObject
{
    Q_OBJECT
    
public:
    explicit djvFileBrowserTestInfo(djvImageContext *, QObject * parent = nullptr);
    
    virtual ~djvFileBrowserTestInfo();
    
public Q_SLOTS:
    //! Request image information.
    void request(const djvFileBrowserTestInfoRequest &);
    
    //! Set the request ID.
    void setId(quint64);
    
Q_SIGNALS:
    //! This signal is emitted when a request has been completed.
    void result(const djvFileBrowserTestInfoResult &);
    
private:
    djvFileBrowserTestInfoRequester * nextRequester();

    djvImageContext *                          _context;
    QVector<djvFileBrowserTestInfoRequester *> _requesters;
    QVector<djvFileBrowserTestInfoWorker *>    _workers;
    QVector<QThread *>                         _threads;
    int                                        _threadIndex;
};

