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

#include <AbstractWorker.h>
#include <Util.h>

#include <djvImageIO.h>

#include <djvFileInfo.h>

#include <QObject>
#include <QPixmap>

class djvGraphicsContext;

//------------------------------------------------------------------------------
//! \struct InfoRequest
//!
//! This struct is used to request data from InfoRequester.
//------------------------------------------------------------------------------

struct InfoRequest
{
    InfoRequest();
    
    djvFileInfo      fileInfo;                             //!< File information.
    Util::THUMBNAILS thumbnails    = UTIL::THUMBNAILS_OFF; //!< Thumbnails mode.
    int              thumbnailSize = 0;                    //!< Thumbnail size.
    int              row           = 0;                    //!< Model row.
    quint64          id            = 0;                    //!< Request ID.
};

//------------------------------------------------------------------------------
//! \class InfoRequester
//!
//! This class is used to request image information.
//------------------------------------------------------------------------------

class InfoRequester : public QObject
{
    Q_OBJECT
    
public:
    explicit InfoRequester(QObject * parent = nullptr);
    
Q_SIGNALS:
    void request(const InfoRequest &);
};

//------------------------------------------------------------------------------
//! \struct InfoResult
//!
//! This struct is used to return data from InfoWorker.
//------------------------------------------------------------------------------

struct InfoResult
{
    InfoResult();
    
    djvImageIOInfo info;   //!< Image I/O information.
    QPixmap        pixmap; //!< Thumbnail.
    int            row;    //!< Model row.
    quint64        id;     //!< Request ID.
};

//------------------------------------------------------------------------------
//! \class InfoWorker
//!
//! This class provides an image information worker.
//------------------------------------------------------------------------------

class InfoWorker : public AbstractWorker
{
    Q_OBJECT
    
public:
    explicit InfoWorker(
        djvGraphicsContext * context,
        QObject *            parent = nullptr);

    virtual ~InfoWorker();
    
public Q_SLOTS:
    //! Request image information.
    void request(const InfoRequest &);
    
Q_SIGNALS:
    //! This signal is emitted when a request has been completed.
    void result(const InfoResult &);

private:
    djvGraphicsContext * _context = nullptr;
};

//------------------------------------------------------------------------------
//! \class Info
//!
//! This class provides image information.
//------------------------------------------------------------------------------

class Info : public QObject
{
    Q_OBJECT
    
public:
    explicit Info(djvGraphicsContext *, QObject * parent = nullptr);
    
    virtual ~Info();
    
public Q_SLOTS:
    //! Request image information.
    void request(const InfoRequest &);
    
    //! Set the request ID.
    void setId(quint64);
    
Q_SIGNALS:
    //! This signal is emitted when a request has been completed.
    void result(const InfoResult &);
    
private:
    InfoRequester * nextRequester();

    djvGraphicsContext *     _context     = nullptr;
    QVector<InfoRequester *> _requesters;
    QVector<InfoWorker *>    _workers;
    QVector<QThread *>       _threads;
    int                      _threadIndex = 0;
};

