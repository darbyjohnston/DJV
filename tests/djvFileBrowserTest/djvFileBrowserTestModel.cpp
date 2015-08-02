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

//! \file djvFileBrowserTestModel.cpp

#include <djvFileBrowserTestModel.h>

#include <djvFileBrowserTestImageInfoRequester.h>
#include <djvFileBrowserTestImageInfoWorker.h>

#include <djvGuiContext.h>

#include <djvDebug.h>

#include <QApplication>

djvFileBrowserTestModel::djvFileBrowserTestModel(djvGuiContext * context, QObject * parent) :
    QAbstractItemModel(parent),
    _context         (context),
    //_sequence        (static_cast<djvSequence::COMPRESS>(0)),
    _sequence        (djvSequence::COMPRESS_RANGE),
    _id              (0),
    _dirWorker       (new djvFileBrowserTestDirWorker),
    _imageInfoCurrent(0),
    _fontMetrics     (qApp->font())
{
    //DJV_DEBUG("djvFileBrowserTestModel::djvFileBrowserTestModel");
    //DJV_DEBUG_PRINT("thread = " << qint64(thread()));
    
    for (int i = 0; i < 6; ++i)
    {
        _imageInfoRequesters.append(new djvFileBrowserTestImageInfoRequester);
        _imageInfoWorkers.append(new djvFileBrowserTestImageInfoWorker(context));
        _imageInfoThreads.append(new QThread);
    }
    
    connect(
        _dirWorker.data(),
        SIGNAL(dirFinished(const djvFileInfoList &, quint64)),
        SLOT(dirCallback(const djvFileInfoList &, quint64)));
   
    _dirWorker->connect(
        this,
        SIGNAL(requestDir(const QString &, djvSequence::COMPRESS, quint64)),
        SLOT(dir(const QString &, djvSequence::COMPRESS, quint64)));
   
    _dirWorker->connect(
        &_dirWorkerThread,
        SIGNAL(finished()),
        SLOT(finish()));

    for (int i = 0; i < _imageInfoRequesters.count(); ++i)
    {
        connect(
            _imageInfoWorkers[i],
            SIGNAL(infoFinished(const djvImageIoInfo &, int, quint64)),
            SLOT(imageInfoCallback(const djvImageIoInfo &, int, quint64)));

        _imageInfoWorkers[i]->connect(
            _imageInfoRequesters[i],
            SIGNAL(request(const djvFileInfo &, int, quint64)),
            SLOT(info(const djvFileInfo &,int, quint64)));

        _imageInfoWorkers[i]->connect(
            _imageInfoThreads[i],
            SIGNAL(finished()),
            SLOT(finish()));
    }
    
    _dirWorker->moveToThread(&_dirWorkerThread);
    _dirWorkerThread.start();

    for (int i = 0; i < _imageInfoWorkers.count(); ++i)
    {
        _imageInfoWorkers[i]->moveToThread(_imageInfoThreads[i]);
        _imageInfoThreads[i]->start();

        //DJV_DEBUG_PRINT("thread = " << qint64(_imageInfoWorkers[i]->thread()));
    }
}

djvFileBrowserTestModel::~djvFileBrowserTestModel()
{
    _dirWorkerThread.quit();
    _dirWorkerThread.wait();

    for (int i = 0; i < _imageInfoThreads.count(); ++i)
    {
        _imageInfoThreads[i]->quit();
        _imageInfoThreads[i]->wait();
        
        delete _imageInfoThreads[i];
        delete _imageInfoWorkers[i];
        delete _imageInfoRequesters[i];
    }
}

const QDir & djvFileBrowserTestModel::dir() const
{
    return _dir;
}

QString djvFileBrowserTestModel::path() const
{
    return _dir.absolutePath();
}

djvSequence::COMPRESS djvFileBrowserTestModel::sequence() const
{
    return _sequence;
}

int djvFileBrowserTestModel::columnCount(const QModelIndex & parent) const
{
    return 1;
}

QVariant djvFileBrowserTestModel::data(const QModelIndex & index, int role) const
{
    if (! index.isValid())
        return QVariant();

    if (role != Qt::DecorationRole &&
        role != Qt::DisplayRole    &&
        role != Qt::EditRole       &&
        role != Qt::SizeHintRole)
        return QVariant();

    const int row    = index.row();
    const int column = index.column();

    if (row    < 0 || row    >= _list.count() ||
        column < 0 || column >= 1)
        return QVariant();

    switch (role)
    {
        case Qt::DisplayRole:
        
            if (! _data[row].imageInfoInit)
            {
                djvFileBrowserTestModel * that =
                    const_cast<djvFileBrowserTestModel *>(this);
                
                that->_data[row].imageInfoInit = true;
                
                Q_EMIT that->_imageInfoRequesters[_imageInfoCurrent]->request(
                    _data[row].fileInfo, row, _id);
                
                ++that->_imageInfoCurrent;
                
                if (_imageInfoCurrent >= _imageInfoRequesters.count())
                {
                    that->_imageInfoCurrent = 0;
                }
            }
        
            return _data[row].text;

        case Qt::SizeHintRole:
        
            return _data[row].sizeHint;
        
        default: break;
    }
    
    return QVariant();
}

Qt::ItemFlags djvFileBrowserTestModel::flags(const QModelIndex & index) const
{
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);

    if (index.isValid())
        return Qt::ItemIsDragEnabled | defaultFlags;
    else
        return defaultFlags;
}

QVariant djvFileBrowserTestModel::headerData(
    int             section,
    Qt::Orientation orientation,
    int             role) const
{
    return QVariant();
}

QModelIndex	djvFileBrowserTestModel::index(
    int                 row,
    int                 column,
    const QModelIndex & parent) const
{
    if (! hasIndex(row, column, parent))
        return QModelIndex();

    return createIndex(row, column, (void *)&_list[row]);
}

QModelIndex	djvFileBrowserTestModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}

int djvFileBrowserTestModel::rowCount(const QModelIndex & parent) const
{
    return parent.isValid() ? 0 : _list.count();
}

void djvFileBrowserTestModel::setDir(const QDir & dir)
{
    if (dir == _dir)
        return;
    
    //DJV_DEBUG("djvFileBrowserTestModel::setDir");
    //DJV_DEBUG_PRINT("dir = " << dir.absolutePath());
    
    _dirPrev = _dir;
    
    _dir = dir;
    
    ++_id;

    beginResetModel();
    
    _list.clear();
    
    _data.clear();
    
    endResetModel();

    Q_EMIT dirChanged(_dir);
    Q_EMIT pathChanged(_dir.absolutePath());
    Q_EMIT requestDir(_dir.absolutePath(), _sequence, _id);
}

void djvFileBrowserTestModel::setPath(const QString & path)
{
    setDir(path);
}

void djvFileBrowserTestModel::setSequence(djvSequence::COMPRESS sequence)
{
    if (sequence == _sequence)
        return;

    //DJV_DEBUG("djvFileBrowserTestModel::setSequence");
    //DJV_DEBUG_PRINT("sequence = " << sequence);
    
    _sequence = sequence;
    
    ++_id;

    Q_EMIT requestDir(_dir.absolutePath(), _sequence, _id);
}

void djvFileBrowserTestModel::up()
{
    QDir tmp(_dir);
    
    if (tmp.cdUp())
    {
        setDir(tmp);
    }
}

void djvFileBrowserTestModel::back()
{
    const QDir tmp(_dirPrev);
    
    setDir(tmp);
}

void djvFileBrowserTestModel::reload()
{
    ++_id;

    Q_EMIT requestDir(_dir.absolutePath(), _sequence, _id);
}

void djvFileBrowserTestModel::dirCallback(
    const djvFileInfoList & list,
    quint64                 id)
{
    if (id != _id)
        return;

    //DJV_DEBUG("djvFileBrowserTestModel::dirCallback");
    //DJV_DEBUG_PRINT("list = " << list);
    //DJV_DEBUG_PRINT("id = " << id);
    
    beginResetModel();
    
    _list = list;
    
    _data.clear();
    
    for (int i = 0; i < _list.count(); ++i)
    {
        const QString text = _list[i].name();
        
        djvFileBrowserTestModelItem item;
        item.fileInfo = _list[i];
        item.text     = text;
        item.sizeHint = _fontMetrics.boundingRect(QRect(), Qt::AlignLeft, text).size();

        _data.append(item);
    }

    endResetModel();
    
    Q_EMIT requestDirFinished();
}

void djvFileBrowserTestModel::imageInfoCallback(
    const djvImageIoInfo & info,
    int                    row,
    quint64                id)
{
    if (id != _id)
        return;

    //DJV_DEBUG("djvFileBrowserTestModel::imageInfoCallback");
    //DJV_DEBUG_PRINT("info = " << info);
    //DJV_DEBUG_PRINT("row = " << row);
    //DJV_DEBUG_PRINT("id = " << id);

    const QString text = QString("%1\n%2x%3").
        arg(_data[row].fileInfo.name()).
        arg(info.size.x).
        arg(info.size.y);
    
    _data[row].text     = text;
    _data[row].sizeHint = _fontMetrics.boundingRect(QRect(), Qt::AlignLeft, text).size();

    Q_EMIT dataChanged(index(row, 0), index(row, 0));
}

