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

//! \file djvFileBrowserTestDirWorker.h

#ifndef DJV_FILE_BROWSER_TEST_DIR_WORKER_H
#define DJV_FILE_BROWSER_TEST_DIR_WORKER_H

#include <djvFileBrowserTestAbstractWorker.h>
#include <djvFileBrowserTestUtil.h>

#include <djvFileInfo.h>

struct djvFileBrowserTestDirWorkerPrivate;

//------------------------------------------------------------------------------
//! \struct djvFileBrowserTestDirRequest
//!
//! This struct is used to request data from djvFileBrowserTestDirWorker.
//------------------------------------------------------------------------------

struct djvFileBrowserTestDirRequest
{
    //! Constructor.
    
    djvFileBrowserTestDirRequest();
    
    QString                         path;          //!< File system path.
    djvSequence::COMPRESS           sequence;      //!< File sequencing.
    QString                         filterText;    //!< Filter text.
    bool                            showHidden;    //!< Whether to show hidden
                                                   //!< files.
    djvFileBrowserTestUtil::COLUMNS sort;          //!< Sorting.
    bool                            reverseSort;   //!< Whether to reverse the
                                                   //!< sort.
    bool                            sortDirsFirst; //!< Whether to list
                                                   //!< directories first.
    bool                            reload;        //!< Reload the current
                                                   //!< directory.
    quint64                         id;            //!< Request ID.
};

//------------------------------------------------------------------------------
//! \struct djvFileBrowserTestDirResult
//!
//! This struct is used to return data from djvFileBrowserTestDirWorker.
//------------------------------------------------------------------------------

struct djvFileBrowserTestDirResult
{
    //! Constructor.
    
    djvFileBrowserTestDirResult();
    
    djvFileInfoList list; //!< List of files.
    quint64         id;   //!< Request ID.
};

//------------------------------------------------------------------------------
//! \class djvFileBrowserTestDirWorker
//!
//! This class provides the contents of a directory.
//------------------------------------------------------------------------------

class djvFileBrowserTestDirWorker : public djvFileBrowserTestAbstractWorker
{
    Q_OBJECT
    
public:

    //! Constructor.
    
    explicit djvFileBrowserTestDirWorker(QObject * parent = 0);
    
    //! Destructor.
    
    virtual ~djvFileBrowserTestDirWorker();

public Q_SLOTS:

    //! Request the contents of a directory.
    
    void request(const djvFileBrowserTestDirRequest &);
    
    virtual void finish();

Q_SIGNALS:

    //! This signal is emitted when the directory contents are available.

    void result(const djvFileBrowserTestDirResult &);

protected:

    virtual void timerEvent(QTimerEvent *);
    
private:

    void process(djvFileBrowserTestDirResult &);
    
    djvFileBrowserTestDirResult result() const;
    
    DJV_PRIVATE_COPY(djvFileBrowserTestDirWorker)
    
    djvFileBrowserTestDirWorkerPrivate * _p;
};

#endif // DJV_FILE_BROWSER_TEST_DIR_WORKER_H

