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

#include <djvGuiContext.h>
#include <djvIconLibrary.h>

#include <djvDebug.h>
#include <djvFileInfoUtil.h>
#include <djvTime.h>
#include <djvVectorUtil.h>

#include <QMutexLocker>

//------------------------------------------------------------------------------
// djvFileBrowserTestModel
//------------------------------------------------------------------------------

djvFileBrowserTestModel::djvFileBrowserTestModel(
    djvGuiContext * context,
    QObject *       parent) :
    QAbstractItemModel(parent),
    _context             (context),
    _sequence            (static_cast<djvSequence::COMPRESS>(0)),
    _showHidden          (false),
    _sort                (static_cast<djvFileBrowserTestUtil::COLUMNS>(0)),
    _reverseSort         (false),
    _sortDirsFirst       (true),
    _id                  (0),
    _dirWorker           (new djvFileBrowserTestDirWorker),
    _thumbnails          (djvFileBrowserTestUtil::THUMBNAILS_HIGH),
    _thumbnailSize       (djvFileBrowserTestUtil::THUMBNAIL_MEDIUM),
    _thumbnailThreadIndex(0),
    _sizeHint            (_context->iconLibrary()->pixmap("djvFileIcon.png").size())
{
    //DJV_DEBUG("djvFileBrowserTestModel::djvFileBrowserTestModel");
    //DJV_DEBUG_PRINT("thread = " << qint64(thread()));
    
    for (int i = 0; i < 6; ++i)
    {
        _thumbnailRequesters.append(new djvFileBrowserTestThumbnailRequester);
        _thumbnailWorkers.append(new djvFileBrowserTestThumbnailWorker(context));
        _thumbnailThreads.append(new QThread);
    }
    
    connect(
        _dirWorker.data(),
        SIGNAL(result(const djvFileBrowserTestDirResult &)),
        SLOT(dirCallback(const djvFileBrowserTestDirResult &)));

    _dirWorker->connect(
        this,
        SIGNAL(requestDir(const djvFileBrowserTestDirRequest &)),
        SLOT(request(const djvFileBrowserTestDirRequest &)));
   
    _dirWorker->connect(
        &_dirWorkerThread,
        SIGNAL(started()),
        SLOT(start()));

    _dirWorker->connect(
        &_dirWorkerThread,
        SIGNAL(finished()),
        SLOT(finish()));

    for (int i = 0; i < _thumbnailRequesters.count(); ++i)
    {
        connect(
            _thumbnailWorkers[i],
            SIGNAL(result(const djvFileBrowserTestThumbnailResult &)),
            SLOT(thumbnailCallback(const djvFileBrowserTestThumbnailResult &)));

        _thumbnailWorkers[i]->connect(
            _thumbnailRequesters[i],
            SIGNAL(info(const djvFileBrowserTestThumbnailRequest &)),
            SLOT(info(const djvFileBrowserTestThumbnailRequest &)));

        _thumbnailWorkers[i]->connect(
            _thumbnailRequesters[i],
            SIGNAL(thumbnail(const djvFileBrowserTestThumbnailRequest &)),
            SLOT(thumbnail(const djvFileBrowserTestThumbnailRequest &)));

        _thumbnailWorkers[i]->connect(
            _thumbnailThreads[i],
            SIGNAL(started()),
            SLOT(start()));

        _thumbnailWorkers[i]->connect(
            _thumbnailThreads[i],
            SIGNAL(finished()),
            SLOT(finish()));
    }
    
    _dirWorker->moveToThread(&_dirWorkerThread);
    _dirWorkerThread.start();

    for (int i = 0; i < _thumbnailWorkers.count(); ++i)
    {
        _thumbnailWorkers[i]->moveToThread(_thumbnailThreads[i]);
        _thumbnailThreads[i]->start();

        //DJV_DEBUG_PRINT("thread = " << qint64(_thumbnailWorkers[i]->thread()));
    }
}

djvFileBrowserTestModel::~djvFileBrowserTestModel()
{
    nextId();
    
    _dirWorkerThread.quit();

    for (int i = 0; i < _thumbnailThreads.count(); ++i)
    {
        _thumbnailThreads[i]->quit();
    }

    _dirWorkerThread.wait();

    for (int i = 0; i < _thumbnailThreads.count(); ++i)
    {
        _thumbnailThreads[i]->wait();
    }
    
    for (int i = 0; i < _thumbnailThreads.count(); ++i)
    {
        delete _thumbnailThreads[i];
        delete _thumbnailWorkers[i];
        delete _thumbnailRequesters[i];
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

const QString & djvFileBrowserTestModel::filterText() const
{
    return _filterText;
}

bool djvFileBrowserTestModel::hasShowHidden() const
{
    return _showHidden;
}

djvFileBrowserTestUtil::COLUMNS djvFileBrowserTestModel::sort() const
{
    return _sort;
}

bool djvFileBrowserTestModel::hasReverseSort() const
{
    return _reverseSort;
}

bool djvFileBrowserTestModel::hasSortDirsFirst() const
{
    return _sortDirsFirst;
}

djvFileBrowserTestUtil::THUMBNAILS djvFileBrowserTestModel::thumbnails() const
{
    return _thumbnails;
}

djvFileBrowserTestUtil::THUMBNAIL_SIZE djvFileBrowserTestModel::thumbnailSize() const
{
    return _thumbnailSize;
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

    if (row    < 0 || row    >= _items.count() ||
        column < 0 || column >= 1)
        return QVariant();

    //DJV_DEBUG("djvFileBrowserTestModel::data");
    //DJV_DEBUG_PRINT("row = " << row);
    //DJV_DEBUG_PRINT("column = " << column);

    djvFileBrowserTestModel * that =
        const_cast<djvFileBrowserTestModel *>(this);

    djvFileBrowserTestModelItem & item = that->_items[row];

    static const QVector<QPixmap> pixmaps = QVector<QPixmap>() <<
        _context->iconLibrary()->pixmap("djvFileIcon.png") <<
        _context->iconLibrary()->pixmap("djvSeqIcon.png") <<
        _context->iconLibrary()->pixmap("djvDirIcon.png");

    switch (role)
    {
        case Qt::DecorationRole:
        
            switch (column)
            {
                case djvFileBrowserTestUtil::NAME:

                    if (_thumbnails != djvFileBrowserTestUtil::THUMBNAILS_OFF)
                    {
                        if (! item.infoInit)
                        {
                            item.infoInit = true;

                            Q_EMIT that->nextThumbnailRequester()->info(
                                thumbnailRequest(item.fileInfo, row));
                        }
                        else if (! item.thumbnailInit)
                        {
                            item.thumbnailInit = true;

                            Q_EMIT that->nextThumbnailRequester()->thumbnail(
                                thumbnailRequest(item.fileInfo, row));
                        }
                        
                        if (! item.thumbnail.isNull())
                            return item.thumbnail;
                    }

                    return pixmaps[item.fileInfo.type()];
                
                default: break;
            }
            
            break;

        case Qt::DisplayRole:

            return item.text;
        
        case Qt::SizeHintRole:
        
            if (! item.thumbnail.isNull())
            {
                return item.thumbnailSize;
            }
        
            return _sizeHint;

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

    return createIndex(row, column, (void *)&_items[row]);
}

QModelIndex	djvFileBrowserTestModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}

int djvFileBrowserTestModel::rowCount(const QModelIndex & parent) const
{
    return parent.isValid() ? 0 : _items.count();
}

void djvFileBrowserTestModel::setDir(const QDir & dir)
{
    if (dir == _dir)
        return;
    
    //DJV_DEBUG("djvFileBrowserTestModel::setDir");
    //DJV_DEBUG_PRINT("dir = " << dir.absolutePath());
    
    _dirPrev = _dir;
    
    _dir = dir;
    
    nextId();

    beginResetModel();
    
    _items.clear();
    
    endResetModel();

    Q_EMIT dirChanged(_dir);
    Q_EMIT pathChanged(_dir.absolutePath());
    Q_EMIT requestDir(dirRequest());
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
    
    nextId();

    Q_EMIT requestDir(dirRequest());
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
    nextId();

    Q_EMIT requestDir(dirRequest());
}

void djvFileBrowserTestModel::setFilterText(const QString & text)
{
    if (text == _filterText)
        return;

    _filterText = text;

    reload();
    
    Q_EMIT filterTextChanged(_filterText);
}

void djvFileBrowserTestModel::setShowHidden(bool show)
{
    if (show == _showHidden)
        return;

    _showHidden = show;
    
    reload();

    Q_EMIT showHiddenChanged(_showHidden);
}

void djvFileBrowserTestModel::setSort(djvFileBrowserTestUtil::COLUMNS sort)
{
    if (sort == _sort)
        return;

    _sort = sort;

    reload();

    Q_EMIT sortChanged(_sort);
}

void djvFileBrowserTestModel::setReverseSort(bool value)
{
    if (value == _reverseSort)
        return;

    _reverseSort = value;

    reload();

    Q_EMIT reverseSortChanged(_reverseSort);
}

void djvFileBrowserTestModel::setSortDirsFirst(bool sort)
{
    if (sort == _sortDirsFirst)
        return;

    _sortDirsFirst = sort;

    reload();

    Q_EMIT sortDirsFirstChanged(_sortDirsFirst);
}

void djvFileBrowserTestModel::setThumbnails(djvFileBrowserTestUtil::THUMBNAILS thumbnails)
{
    if (thumbnails == _thumbnails)
        return;

    _thumbnails = thumbnails;

    reload();

    Q_EMIT thumbnailsChanged(_thumbnails);
}

void djvFileBrowserTestModel::setThumbnailSize(djvFileBrowserTestUtil::THUMBNAIL_SIZE size)
{
    if (size == _thumbnailSize)
        return;

    _thumbnailSize = size;

    reload();

    Q_EMIT thumbnailSizeChanged(_thumbnailSize);
}

void djvFileBrowserTestModel::dirCallback(const djvFileBrowserTestDirResult & result)
{
    if (result.id != _id)
        return;

    //DJV_DEBUG("djvFileBrowserTestModel::dirCallback");
    //DJV_DEBUG_PRINT("list = " << result.list);
    //DJV_DEBUG_PRINT("id = " << result.id);
    
    beginResetModel();
    
    _items.resize(result.list.count());
    
    for (int i = 0; i < result.list.count(); ++i)
    {
        djvFileBrowserTestModelItem & item = _items[i];
        
        item.fileInfo = result.list[i];
        item.text     = result.list[i].name();
    }

    endResetModel();    
    
    Q_EMIT requestDirComplete();
}

void djvFileBrowserTestModel::thumbnailCallback(
    const djvFileBrowserTestThumbnailResult & result)
{
    if (result.id != _id || result.pixmap.isNull())
        return;

    //DJV_DEBUG("djvFileBrowserTestModel::thumbnailCallback");
    //DJV_DEBUG_PRINT("info = " << result.info);
    //DJV_DEBUG_PRINT("row = " << result.row);
    //DJV_DEBUG_PRINT("id = " << result.id);

    _items[result.row].text = QString("%1\n%2x%3:%4 %5\n%6@%7").
        arg(_items[result.row].fileInfo.name()).
        arg(result.info.size.x).
        arg(result.info.size.y).
        arg(djvVectorUtil::aspect(result.info.size), 0, 'f', 2).
        arg(djvStringUtil::label(result.info.pixel).join(", ")).
        arg(djvTime::frameToString(
            result.info.sequence.frames.count(),
            result.info.sequence.speed)).
        arg(djvSpeed::speedToFloat(result.info.sequence.speed));

    _items[result.row].thumbnail     = result.pixmap;
    _items[result.row].thumbnailSize = result.pixmap.size();
    
    Q_EMIT dataChanged(
        index(result.row, 0),
        index(result.row, 0),
        QVector<int>() <<
            Qt::DecorationRole <<
            Qt::DisplayRole <<
            Qt::SizeHintRole <<
            Qt::EditRole);
}

void djvFileBrowserTestModel::nextId()
{
    ++_id;

    {
        QMutexLocker locker(_dirWorker->mutex());
        
        _dirWorker->setId(_id);
    }
    
    for (int i = 0; i < _thumbnailWorkers.count(); ++i)
    {
        QMutexLocker locker(_thumbnailWorkers[i]->mutex());
        
        _thumbnailWorkers[i]->setId(_id);
    }
}

djvFileBrowserTestDirRequest djvFileBrowserTestModel::dirRequest() const
{
    djvFileBrowserTestDirRequest r;
    
    r.path          = _dir.absolutePath();
    r.sequence      = _sequence;
    r.filterText    = _filterText;
    r.showHidden    = _showHidden;
    r.sort          = _sort;
    r.reverseSort   = _reverseSort;
    r.sortDirsFirst = _sortDirsFirst;
    r.id            = _id;
    
    return r;
}

djvFileBrowserTestThumbnailRequest djvFileBrowserTestModel::thumbnailRequest(
    const djvFileInfo & fileInfo,
    int                 row) const
{
    djvFileBrowserTestThumbnailRequest r;
    
    r.fileInfo      = fileInfo;
    r.thumbnails    = _thumbnails;
    r.thumbnailSize = _thumbnailSize;
    r.row           = row;
    r.id            = _id;
    
    return r;
}

djvFileBrowserTestThumbnailRequester *
djvFileBrowserTestModel::nextThumbnailRequester()
{
    djvFileBrowserTestThumbnailRequester * requester =
        _thumbnailRequesters[_thumbnailThreadIndex];

    ++_thumbnailThreadIndex;
    
    if (_thumbnailThreadIndex >= _thumbnailRequesters.count())
    {
        _thumbnailThreadIndex = 0;
    }
    
    return requester;
}

