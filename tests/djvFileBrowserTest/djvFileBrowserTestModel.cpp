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

#include <djvFileBrowserTestModelItem.h>

#include <djvGuiContext.h>
#include <djvIconLibrary.h>

#include <djvDebug.h>
#include <djvFileInfoUtil.h>
#include <djvTime.h>
#include <djvVectorUtil.h>

#include <QMutexLocker>
#include <QScopedPointer>
#include <QThread>

//------------------------------------------------------------------------------
// djvFileBrowserTestModelPrivate
//------------------------------------------------------------------------------

struct djvFileBrowserTestModelPrivate
{
    djvFileBrowserTestModelPrivate(djvGuiContext * context) :
        context             (context),
        sequence            (static_cast<djvSequence::COMPRESS>(0)),
        showHidden          (false),
        sort                (static_cast<djvFileBrowserTestUtil::COLUMNS>(0)),
        reverseSort         (false),
        sortDirsFirst       (true),
        id                  (0),
        dirWorker           (new djvFileBrowserTestDirWorker),
        thumbnails          (djvFileBrowserTestUtil::THUMBNAILS_LOW),
        thumbnailSize       (djvFileBrowserTestUtil::THUMBNAIL_MEDIUM),    
        infoThreadIndex     (0),
        thumbnailThreadIndex(0),
        sizeHint            (context->iconLibrary()->pixmap("djvFileIcon.png").size())
    {}
    
    djvGuiContext *                                 context;
    QDir                                            dir;
    QDir                                            dirPrev;
    djvSequence::COMPRESS                           sequence;
    QString                                         filterText;
    bool                                            showHidden;
    djvFileBrowserTestUtil::COLUMNS                 sort;
    bool                                            reverseSort;
    bool                                            sortDirsFirst;            
    quint64                                         id;
    QVector<djvFileBrowserTestModelItem>            items;
    QScopedPointer<djvFileBrowserTestDirWorker>     dirWorker;
    QThread                                         dirWorkerThread;
    djvFileBrowserTestUtil::THUMBNAILS              thumbnails;
    djvFileBrowserTestUtil::THUMBNAIL_SIZE          thumbnailSize;
    QVector<djvFileBrowserTestInfoRequester *>      infoRequesters;
    QVector<djvFileBrowserTestInfoWorker *>         infoWorkers;
    QVector<QThread *>                              infoThreads;
    int                                             infoThreadIndex;
    QVector<djvFileBrowserTestThumbnailRequester *> thumbnailRequesters;
    QVector<djvFileBrowserTestThumbnailWorker *>    thumbnailWorkers;
    QVector<QThread *>                              thumbnailThreads;
    int                                             thumbnailThreadIndex;
    QVariant                                        sizeHint;
};

//------------------------------------------------------------------------------
// djvFileBrowserTestModel
//------------------------------------------------------------------------------

djvFileBrowserTestModel::djvFileBrowserTestModel(
    djvGuiContext * context,
    QObject *       parent) :
    QAbstractItemModel(parent),
    _p(new djvFileBrowserTestModelPrivate(context))
{
    //DJV_DEBUG("djvFileBrowserTestModel::djvFileBrowserTestModel");
    //DJV_DEBUG_PRINT("thread = " << qint64(thread()));
    
    static const int threads = 12;
    
    for (int i = 0; i < threads; ++i)
    {
        _p->infoRequesters.append(new djvFileBrowserTestInfoRequester);
        _p->infoWorkers.append(new djvFileBrowserTestInfoWorker(context));
        _p->infoThreads.append(new QThread);
    }
    
    for (int i = 0; i < threads; ++i)
    {
        _p->thumbnailRequesters.append(new djvFileBrowserTestThumbnailRequester);
        _p->thumbnailWorkers.append(new djvFileBrowserTestThumbnailWorker(context));
        _p->thumbnailThreads.append(new QThread);
    }
    
    connect(
        _p->dirWorker.data(),
        SIGNAL(result(const djvFileBrowserTestDirResult &)),
        SLOT(dirCallback(const djvFileBrowserTestDirResult &)));

    _p->dirWorker->connect(
        this,
        SIGNAL(requestDir(const djvFileBrowserTestDirRequest &)),
        SLOT(request(const djvFileBrowserTestDirRequest &)));
   
    _p->dirWorker->connect(
        &_p->dirWorkerThread,
        SIGNAL(started()),
        SLOT(start()));

    _p->dirWorker->connect(
        &_p->dirWorkerThread,
        SIGNAL(finished()),
        SLOT(finish()));

    for (int i = 0; i < _p->infoWorkers.count(); ++i)
    {
        connect(
            _p->infoWorkers[i],
            SIGNAL(result(const djvFileBrowserTestInfoResult &)),
            SLOT(infoCallback(const djvFileBrowserTestInfoResult &)));

        _p->infoWorkers[i]->connect(
            _p->infoRequesters[i],
            SIGNAL(request(const djvFileBrowserTestInfoRequest &)),
            SLOT(request(const djvFileBrowserTestInfoRequest &)));

        _p->infoWorkers[i]->connect(
            _p->infoThreads[i],
            SIGNAL(started()),
            SLOT(start()));

        _p->infoWorkers[i]->connect(
            _p->infoThreads[i],
            SIGNAL(finished()),
            SLOT(finish()));
    }
    
    for (int i = 0; i < _p->thumbnailWorkers.count(); ++i)
    {
        connect(
            _p->thumbnailWorkers[i],
            SIGNAL(result(const djvFileBrowserTestThumbnailResult &)),
            SLOT(thumbnailCallback(const djvFileBrowserTestThumbnailResult &)));

        _p->thumbnailWorkers[i]->connect(
            _p->thumbnailRequesters[i],
            SIGNAL(request(const djvFileBrowserTestThumbnailRequest &)),
            SLOT(request(const djvFileBrowserTestThumbnailRequest &)));

        _p->thumbnailWorkers[i]->connect(
            _p->thumbnailThreads[i],
            SIGNAL(started()),
            SLOT(start()));

        _p->thumbnailWorkers[i]->connect(
            _p->thumbnailThreads[i],
            SIGNAL(finished()),
            SLOT(finish()));
    }
    
    _p->dirWorker->moveToThread(&_p->dirWorkerThread);
    _p->dirWorkerThread.start();

    for (int i = 0; i < _p->infoWorkers.count(); ++i)
    {
        _p->infoWorkers[i]->moveToThread(_p->infoThreads[i]);
        _p->infoThreads[i]->start();

        //DJV_p->DEBUG_p->PRINT("thread = " << qint64(_p->infoWorkers[i]->thread()));
    }
    
    for (int i = 0; i < _p->infoWorkers.count(); ++i)
    {
        _p->thumbnailWorkers[i]->moveToThread(_p->thumbnailThreads[i]);
        _p->thumbnailThreads[i]->start();

        //DJV_p->DEBUG_p->PRINT("thread = " << qint64(_p->thumbnailWorkers[i]->thread()));
    }
    
    //startTimer(1000);
}

djvFileBrowserTestModel::~djvFileBrowserTestModel()
{
    nextId();
    
    _p->dirWorkerThread.quit();

    for (int i = 0; i < _p->infoThreads.count(); ++i)
    {
        _p->infoThreads[i]->quit();
    }

    for (int i = 0; i < _p->thumbnailThreads.count(); ++i)
    {
        _p->thumbnailThreads[i]->quit();
    }

    _p->dirWorkerThread.wait();

    for (int i = 0; i < _p->infoThreads.count(); ++i)
    {
        _p->infoThreads[i]->wait();
    }
    
    for (int i = 0; i < _p->thumbnailThreads.count(); ++i)
    {
        _p->thumbnailThreads[i]->wait();
    }
    
    for (int i = 0; i < _p->infoThreads.count(); ++i)
    {
        delete _p->infoThreads[i];
        delete _p->infoWorkers[i];
        delete _p->infoRequesters[i];
    }

    for (int i = 0; i < _p->thumbnailThreads.count(); ++i)
    {
        delete _p->thumbnailThreads[i];
        delete _p->thumbnailWorkers[i];
        delete _p->thumbnailRequesters[i];
    }
    
    delete _p;
}

const QDir & djvFileBrowserTestModel::dir() const
{
    return _p->dir;
}

QString djvFileBrowserTestModel::path() const
{
    return _p->dir.absolutePath();
}

djvSequence::COMPRESS djvFileBrowserTestModel::sequence() const
{
    return _p->sequence;
}

const QString & djvFileBrowserTestModel::filterText() const
{
    return _p->filterText;
}

bool djvFileBrowserTestModel::hasShowHidden() const
{
    return _p->showHidden;
}

djvFileBrowserTestUtil::COLUMNS djvFileBrowserTestModel::sort() const
{
    return _p->sort;
}

bool djvFileBrowserTestModel::hasReverseSort() const
{
    return _p->reverseSort;
}

bool djvFileBrowserTestModel::hasSortDirsFirst() const
{
    return _p->sortDirsFirst;
}

djvFileBrowserTestUtil::THUMBNAILS djvFileBrowserTestModel::thumbnails() const
{
    return _p->thumbnails;
}

djvFileBrowserTestUtil::THUMBNAIL_SIZE djvFileBrowserTestModel::thumbnailSize() const
{
    return _p->thumbnailSize;
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

    if (row    < 0 || row    >= _p->items.count() ||
        column < 0 || column >= 1)
        return QVariant();

    //DJV_DEBUG("djvFileBrowserTestModel::data");
    //DJV_DEBUG_PRINT("row = " << row);
    //DJV_DEBUG_PRINT("column = " << column);

    djvFileBrowserTestModel * that =
        const_cast<djvFileBrowserTestModel *>(this);

    djvFileBrowserTestModelItem & item = that->_p->items[row];

    static const QVector<QPixmap> pixmaps = QVector<QPixmap>() <<
        _p->context->iconLibrary()->pixmap("djvFileIcon.png") <<
        _p->context->iconLibrary()->pixmap("djvSeqIcon.png") <<
        _p->context->iconLibrary()->pixmap("djvDirIcon.png");

    switch (role)
    {
        case Qt::DecorationRole:
        
            switch (column)
            {
                case djvFileBrowserTestUtil::NAME:

                    if (_p->thumbnails != djvFileBrowserTestUtil::THUMBNAILS_OFF)
                    {
                        if (! item.infoInit)
                        {
                            item.infoInit = true;

                            Q_EMIT that->nextInfoRequester()->request(
                                infoRequest(item.fileInfo, row));
                        }
                        
                        if (! item.thumbnailInit)
                        {
                            item.thumbnailInit = true;

                            Q_EMIT that->nextThumbnailRequester()->request(
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

            return _p->sizeHint;

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

    return createIndex(row, column, (void *)&_p->items[row]);
}

QModelIndex	djvFileBrowserTestModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}

int djvFileBrowserTestModel::rowCount(const QModelIndex & parent) const
{
    return parent.isValid() ? 0 : _p->items.count();
}

void djvFileBrowserTestModel::setDir(const QDir & dir)
{
    if (dir == _p->dir)
        return;
    
    //DJV_DEBUG("djvFileBrowserTestModel::setDir");
    //DJV_DEBUG_PRINT("dir = " << dir.absolutePath());
    
    _p->dirPrev = _p->dir;
    
    _p->dir = dir;
    
    nextId();

    beginResetModel();
    
    _p->items.clear();
    
    endResetModel();

    Q_EMIT dirChanged(_p->dir);
    Q_EMIT pathChanged(_p->dir.absolutePath());
    Q_EMIT requestDir(dirRequest());
}

void djvFileBrowserTestModel::setPath(const QString & path)
{
    setDir(path);
}

void djvFileBrowserTestModel::setSequence(djvSequence::COMPRESS sequence)
{
    if (sequence == _p->sequence)
        return;

    //DJV_DEBUG("djvFileBrowserTestModel::setSequence");
    //DJV_DEBUG_PRINT("sequence = " << sequence);
    
    _p->sequence = sequence;
    
    nextId();

    Q_EMIT requestDir(dirRequest());
}

void djvFileBrowserTestModel::up()
{
    QDir tmp(_p->dir);
    
    if (tmp.cdUp())
    {
        setDir(tmp);
    }
}

void djvFileBrowserTestModel::back()
{
    const QDir tmp(_p->dirPrev);
    
    setDir(tmp);
}

void djvFileBrowserTestModel::reload()
{
    nextId();

    djvFileBrowserTestDirRequest request = dirRequest();
    request.reload = true;

    Q_EMIT requestDir(request);
}

void djvFileBrowserTestModel::setFilterText(const QString & text)
{
    if (text == _p->filterText)
        return;

    _p->filterText = text;

    nextId();

    Q_EMIT requestDir(dirRequest());
    Q_EMIT filterTextChanged(_p->filterText);
}

void djvFileBrowserTestModel::setShowHidden(bool show)
{
    if (show == _p->showHidden)
        return;

    _p->showHidden = show;
    
    nextId();

    Q_EMIT requestDir(dirRequest());
    Q_EMIT showHiddenChanged(_p->showHidden);
}

void djvFileBrowserTestModel::setSort(djvFileBrowserTestUtil::COLUMNS sort)
{
    if (sort == _p->sort)
        return;

    _p->sort = sort;

    nextId();

    Q_EMIT requestDir(dirRequest());
    Q_EMIT sortChanged(_p->sort);
}

void djvFileBrowserTestModel::setReverseSort(bool value)
{
    if (value == _p->reverseSort)
        return;

    _p->reverseSort = value;

    nextId();

    Q_EMIT requestDir(dirRequest());
    Q_EMIT reverseSortChanged(_p->reverseSort);
}

void djvFileBrowserTestModel::setSortDirsFirst(bool sort)
{
    if (sort == _p->sortDirsFirst)
        return;

    _p->sortDirsFirst = sort;

    nextId();

    Q_EMIT requestDir(dirRequest());
    Q_EMIT sortDirsFirstChanged(_p->sortDirsFirst);
}

void djvFileBrowserTestModel::setThumbnails(djvFileBrowserTestUtil::THUMBNAILS thumbnails)
{
    if (thumbnails == _p->thumbnails)
        return;

    _p->thumbnails = thumbnails;

    nextId();

    Q_EMIT requestDir(dirRequest());
    Q_EMIT thumbnailsChanged(_p->thumbnails);
}

void djvFileBrowserTestModel::setThumbnailSize(djvFileBrowserTestUtil::THUMBNAIL_SIZE size)
{
    if (size == _p->thumbnailSize)
        return;

    _p->thumbnailSize = size;

    nextId();

    Q_EMIT requestDir(dirRequest());
    Q_EMIT thumbnailSizeChanged(_p->thumbnailSize);
}

void djvFileBrowserTestModel::timerEvent(QTimerEvent * event)
{
    /*DJV_DEBUG("djvFileBrowserTestModel::timerEvent");
    DJV_DEBUG_PRINT("items = " << _p->items.count());
    
    int infoInit          = 0;
    int infoComplete      = 0;
    int thumbnailInit     = 0;
    int thumbnailComplete = 0;
    
    for (int i = 0; i < _p->items.count(); ++i)
    {
        if (_p->items[i].infoInit)
            ++infoInit;
        if (_p->items[i].infoComplete)
            ++infoComplete;
        if (_p->items[i].thumbnailInit)
            ++thumbnailInit;
        if (_p->items[i].thumbnailComplete)
            ++thumbnailComplete;
    }
    
    DJV_DEBUG_PRINT("infoInit          = " << infoInit);
    DJV_DEBUG_PRINT("infoComplete      = " << infoComplete);
    DJV_DEBUG_PRINT("thumbnailInit     = " << thumbnailInit);
    DJV_DEBUG_PRINT("thumbnailComplete = " << thumbnailComplete);*/
}

void djvFileBrowserTestModel::dirCallback(const djvFileBrowserTestDirResult & result)
{
    if (result.id != _p->id)
        return;

    //DJV_DEBUG("djvFileBrowserTestModel::dirCallback");
    //DJV_DEBUG_PRINT("list = " << result.list);
    //DJV_DEBUG_PRINT("id = " << result.id);
    
    beginResetModel();
    
    _p->items.clear();
    
    for (int i = 0; i < result.list.count(); ++i)
    {
        const djvFileInfo & fileInfo = result.list[i];
        
        djvFileBrowserTestModelItem item;
        item.fileInfo = fileInfo;
        item.text     = fileInfo.name();
        
        _p->items.append(item);
    }

    endResetModel();    
    
    Q_EMIT requestDirComplete();
}
    
void djvFileBrowserTestModel::infoCallback(
    const djvFileBrowserTestInfoResult & result)
{
    if (result.id != _p->id || result.pixmap.isNull())
        return;

    //DJV_DEBUG("djvFileBrowserTestModel::infoCallback");
    //DJV_DEBUG_PRINT("info = " << result.info);
    //DJV_DEBUG_PRINT("row = " << result.row);
    //DJV_DEBUG_PRINT("id = " << result.id);

    _p->items[result.row].text = formatText(
        _p->items[result.row].fileInfo,
        result.info);
    
    _p->items[result.row].infoComplete = true;
        
    if (_p->items[result.row].thumbnail.isNull())
    {
        _p->items[result.row].thumbnail     = result.pixmap;
        _p->items[result.row].thumbnailSize = result.pixmap.size();
    }
    
    Q_EMIT dataChanged(
        index(result.row, 0),
        index(result.row, 0),
        QVector<int>() <<
            Qt::DecorationRole <<
            Qt::DisplayRole <<
            Qt::SizeHintRole <<
            Qt::EditRole);
}

void djvFileBrowserTestModel::thumbnailCallback(
    const djvFileBrowserTestThumbnailResult & result)
{
    if (result.id != _p->id || result.pixmap.isNull())
        return;

    //DJV_DEBUG("djvFileBrowserTestModel::thumbnailCallback");
    //DJV_DEBUG_PRINT("info = " << result.info);
    //DJV_DEBUG_PRINT("row = " << result.row);
    //DJV_DEBUG_PRINT("id = " << result.id);

    _p->items[result.row].text = formatText(
        _p->items[result.row].fileInfo,
        result.info);

    _p->items[result.row].thumbnailComplete = true;    

    _p->items[result.row].thumbnail     = result.pixmap;
    _p->items[result.row].thumbnailSize = result.pixmap.size();
    
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
    ++_p->id;

    {
        QMutexLocker locker(_p->dirWorker->mutex());
        
        _p->dirWorker->setId(_p->id);
    }
    
    for (int i = 0; i < _p->infoWorkers.count(); ++i)
    {
        QMutexLocker locker(_p->infoWorkers[i]->mutex());
        
        _p->infoWorkers[i]->setId(_p->id);
    }

    for (int i = 0; i < _p->thumbnailWorkers.count(); ++i)
    {
        QMutexLocker locker(_p->thumbnailWorkers[i]->mutex());
        
        _p->thumbnailWorkers[i]->setId(_p->id);
    }
}

djvFileBrowserTestDirRequest djvFileBrowserTestModel::dirRequest() const
{
    djvFileBrowserTestDirRequest r;
    
    r.path          = _p->dir.absolutePath();
    r.sequence      = _p->sequence;
    r.filterText    = _p->filterText;
    r.showHidden    = _p->showHidden;
    r.sort          = _p->sort;
    r.reverseSort   = _p->reverseSort;
    r.sortDirsFirst = _p->sortDirsFirst;
    r.id            = _p->id;
    
    return r;
}

djvFileBrowserTestInfoRequest djvFileBrowserTestModel::infoRequest(
    const djvFileInfo & fileInfo,
    int                 row) const
{
    djvFileBrowserTestInfoRequest r;
    
    r.fileInfo      = fileInfo;
    r.thumbnails    = _p->thumbnails;
    r.thumbnailSize = _p->thumbnailSize;
    r.row           = row;
    r.id            = _p->id;
    
    return r;
}

djvFileBrowserTestInfoRequester *
djvFileBrowserTestModel::nextInfoRequester()
{
    djvFileBrowserTestInfoRequester * requester =
        _p->infoRequesters[_p->infoThreadIndex];

    ++_p->infoThreadIndex;
    
    if (_p->infoThreadIndex >= _p->infoRequesters.count())
    {
        _p->infoThreadIndex = 0;
    }
    
    return requester;
}

djvFileBrowserTestThumbnailRequest djvFileBrowserTestModel::thumbnailRequest(
    const djvFileInfo & fileInfo,
    int                 row) const
{
    djvFileBrowserTestThumbnailRequest r;
    
    r.fileInfo      = fileInfo;
    r.thumbnails    = _p->thumbnails;
    r.thumbnailSize = _p->thumbnailSize;
    r.row           = row;
    r.id            = _p->id;
    
    return r;
}

djvFileBrowserTestThumbnailRequester *
djvFileBrowserTestModel::nextThumbnailRequester()
{
    djvFileBrowserTestThumbnailRequester * requester =
        _p->thumbnailRequesters[_p->thumbnailThreadIndex];

    ++_p->thumbnailThreadIndex;
    
    if (_p->thumbnailThreadIndex >= _p->thumbnailRequesters.count())
    {
        _p->thumbnailThreadIndex = 0;
    }
    
    return requester;
}

QString djvFileBrowserTestModel::formatText(
    const djvFileInfo &    fileInfo,
    const djvImageIoInfo & info)
{
    return QString("%1\n%2x%3:%4 %5\n%6@%7").
        arg(fileInfo.name()).
        arg(info.size.x).
        arg(info.size.y).
        arg(djvVectorUtil::aspect(info.size), 0, 'f', 2).
        arg(djvStringUtil::label(info.pixel).join(", ")).
        arg(djvTime::frameToString(
            info.sequence.frames.count(),
            info.sequence.speed)).
        arg(djvSpeed::speedToFloat(info.sequence.speed));
}

