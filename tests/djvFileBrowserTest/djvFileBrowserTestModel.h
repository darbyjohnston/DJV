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

//! \file djvFileBrowserTestModel.h

#ifndef DJV_FILE_BROWSER_TEST_MODEL_H
#define DJV_FILE_BROWSER_TEST_MODEL_H

#include <djvFileBrowserTestDirWorker.h>
#include <djvFileBrowserTestModelItem.h>

#include <djvImageIo.h>

#include <QAbstractItemModel>
#include <QDir>
#include <QFontMetrics>
#include <QScopedPointer>
#include <QThread>

class djvFileBrowserTestImageInfoRequester;
class djvFileBrowserTestImageInfoWorker;

class djvGuiContext;

class djvFileBrowserTestModel : public QAbstractItemModel
{
    Q_OBJECT
    
public:

    explicit djvFileBrowserTestModel(djvGuiContext *, QObject * parent = 0);
    
    virtual ~djvFileBrowserTestModel();
    
    const QDir & dir() const;
    
    QString path() const;
    
    djvSequence::COMPRESS sequence() const;

    virtual int columnCount(
        const QModelIndex & parent = QModelIndex()) const;

    virtual QVariant data(
        const QModelIndex & index,
        int                 role = Qt::DisplayRole) const;

    virtual Qt::ItemFlags flags(const QModelIndex &) const;

    virtual QVariant headerData(
        int             section,
        Qt::Orientation orientation,
        int             role = Qt::DisplayRole) const;

    virtual QModelIndex	index(
        int                 row,
        int                 column,
        const QModelIndex & parent = QModelIndex()) const;
    
    virtual QModelIndex	parent(
        const QModelIndex & = QModelIndex()) const;

    virtual int rowCount(
        const QModelIndex & parent = QModelIndex()) const;

public Q_SLOTS:

    void setDir(const QDir &);
    
    void setPath(const QString &);
    
    void setSequence(djvSequence::COMPRESS);
    
    void up();
    
    void back();
    
    void reload();

Q_SIGNALS:

    void dirChanged(const QDir &);

    void pathChanged(const QString &);

    void sequenceChanged(djvSequence::COMPRESS);
    
    void requestDir(const QString &, djvSequence::COMPRESS, quint64 id);
    
    void requestDirFinished();
    
    void requestImageInfo(const djvFileInfo &, int, quint64 id);
    
private Q_SLOTS:

    void dirCallback(const djvFileInfoList &, quint64 id);
    void imageInfoCallback(const djvImageIoInfo &, int row, quint64 id);

private:

    djvGuiContext *                                 _context;
    QDir                                            _dir;
    QDir                                            _dirPrev;
    djvSequence::COMPRESS                           _sequence;
    djvFileInfoList                                 _list;
    quint64                                         _id;
    QVector<djvFileBrowserTestModelItem>            _data;
    QScopedPointer<djvFileBrowserTestDirWorker>     _dirWorker;
    QThread                                         _dirWorkerThread;
    int                                             _imageInfoCurrent;
    QVector<djvFileBrowserTestImageInfoRequester *> _imageInfoRequesters;
    QVector<djvFileBrowserTestImageInfoWorker *>    _imageInfoWorkers;
    QVector<QThread *>                              _imageInfoThreads;
    QFontMetrics                                    _fontMetrics;
};

#endif // DJV_FILE_BROWSER_TEST_MODEL_H

