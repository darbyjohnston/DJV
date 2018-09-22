//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
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

#include <djvFileInfo.h>

#include <QThread>

#include <memory>

//------------------------------------------------------------------------------
//! \struct DirRequest
//!
//! This struct is used to request data from DirWorker.
//------------------------------------------------------------------------------

struct DirRequest
{
    QString               path;
    djvSequence::COMPRESS sequence      = djvSequence::COMPRESS_OFF;
    QString               filterText;
    bool                  showHidden    = false;
    Util::COLUMNS         sort          = Util::NAME;
    bool                  reverseSort   = false;
    bool                  sortDirsFirst = false;
    bool                  reload        = false;
    quint64               id            = 0;
};

//------------------------------------------------------------------------------
//! \struct DirResult
//!
//! This struct is used to return data from DirWorker.
//------------------------------------------------------------------------------

struct DirResult
{
    djvFileInfoList list;
    quint64         id   = 0;
};

//------------------------------------------------------------------------------
//! \class DirWorker
//!
//! This class provides a directory worker.
//------------------------------------------------------------------------------

class DirWorker : public AbstractWorker
{
    Q_OBJECT
    
public:
    explicit DirWorker(QObject * parent = nullptr);
    
    virtual ~DirWorker();

public Q_SLOTS:
    //! Request the contents of a directory.
    void request(const DirRequest &);
    
    virtual void finish();

Q_SIGNALS:
    //! This signal is emitted when the directory contents are available.
    void result(const DirResult &);

protected:
    virtual void timerEvent(QTimerEvent *);
    
private:
    void process(DirResult &);
    
    DirResult result() const;
    
    DJV_PRIVATE_COPY(DirWorker)
    
    struct Private;
    std::unique_ptr<Private> _p;
};

//------------------------------------------------------------------------------
//! \class Dir
//!
//! This class provides directory contents.
//------------------------------------------------------------------------------

class Dir : public QObject
{
    Q_OBJECT
    
public:
    explicit Dir(QObject * parent = nullptr);
    
    virtual ~Dir();
    
public Q_SLOTS:
    //! Request directory contents.
    void request(const DirRequest &);
    
    //! Set the request ID.
    void setId(quint64);
    
Q_SIGNALS:
    //! This signal is emitted when a request has been completed.
    void result(const DirResult &);
    
    //! This signal is emitted to request a directory's contents.
    void requestDir(const DirRequest &);
    
private:
    QScopedPointer<DirWorker> _worker;
    QThread                   _thread;
};

