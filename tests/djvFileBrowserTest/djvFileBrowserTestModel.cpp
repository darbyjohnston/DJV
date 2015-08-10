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

#include <djvFileBrowserTestItem.h>

#include <djvGuiContext.h>
#include <djvIconLibrary.h>

#include <djvDebug.h>
#include <djvFileInfoUtil.h>
#include <djvRangeUtil.h>

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
        dirObject           (new djvFileBrowserTestDir(context)),
        thumbnails          (djvFileBrowserTestUtil::THUMBNAILS_LOW),
        thumbnailSize       (160),
        info                (new djvFileBrowserTestInfo(context)),
        thumbnail           (new djvFileBrowserTestThumbnail(context)),
        sizeHint            (context->iconLibrary()->pixmap("djvFileIcon.png").size())
    {}
    
    djvGuiContext *                             context;
    QDir                                        dir;
    QDir                                        dirPrev;
    djvSequence::COMPRESS                       sequence;
    QString                                     filterText;
    bool                                        showHidden;
    djvFileBrowserTestUtil::COLUMNS             sort;
    bool                                        reverseSort;
    bool                                        sortDirsFirst;            
    quint64                                     id;
    QVector<djvFileBrowserTestItem>             items;
    QScopedPointer<djvFileBrowserTestDir>       dirObject;
    djvFileBrowserTestUtil::THUMBNAILS          thumbnails;
    int                                         thumbnailSize;
    QScopedPointer<djvFileBrowserTestInfo>      info;
    QScopedPointer<djvFileBrowserTestThumbnail> thumbnail;
    QVariant                                    sizeHint;
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

    _p->dirObject->connect(
        this,
        SIGNAL(requestDir(const djvFileBrowserTestDirRequest &)),
        SLOT(request(const djvFileBrowserTestDirRequest &)));

    connect(
        _p->dirObject.data(),
        SIGNAL(result(const djvFileBrowserTestDirResult &)),
        SLOT(dirCallback(const djvFileBrowserTestDirResult &)));

    connect(
        _p->info.data(),
        SIGNAL(result(const djvFileBrowserTestInfoResult &)),
        SLOT(infoCallback(const djvFileBrowserTestInfoResult &)));

    connect(
        _p->thumbnail.data(),
        SIGNAL(result(const djvFileBrowserTestThumbnailResult &)),
        SLOT(thumbnailCallback(const djvFileBrowserTestThumbnailResult &)));

    //startTimer(1000);
}

djvFileBrowserTestModel::~djvFileBrowserTestModel()
{
    nextId();

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

int djvFileBrowserTestModel::thumbnailSize() const
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
        role != Qt::SizeHintRole   &&
        role != SvgRole)
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

    djvFileBrowserTestItem & item = that->_p->items[row];

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

                            _p->info->request(infoRequest(item.fileInfo, row));
                        }
                        
                        if (! item.thumbnailInit)
                        {
                            item.thumbnailInit = true;

                            _p->thumbnail->request(
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

        case SvgRole:
        
            if (! (item.infoComplete || item.thumbnailComplete))
            {
                return djvFileInfo::typeImages()[item.fileInfo.type()];
            }
            
            break;

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

QHash<int, QByteArray> djvFileBrowserTestModel::roleNames() const
{
    QHash<int, QByteArray> r = QAbstractItemModel::roleNames();
    
    r.insert(SvgRole, "svg");
    
    return r;
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

void djvFileBrowserTestModel::setThumbnailSize(int size)
{
    if (size == _p->thumbnailSize)
        return;

    _p->thumbnailSize = size;

    nextId();
    
    djvFrameList itemsChanged;
    
    for (int i = 0; i < _p->items.count(); ++i)
    {
        djvFileBrowserTestItem & item = _p->items[i];
        
        if (item.infoComplete || item.thumbnailComplete)
        {
            djvPixelDataInfo::PROXY proxy = djvPixelDataInfo::PROXY_NONE;

            const djvVector2i size = djvFileBrowserTestUtil::thumbnailSize(
                _p->thumbnails,
                _p->thumbnailSize,
                item.info.size,
                &proxy);

            item.thumbnailInit     = false;
            item.thumbnailComplete = false;
            item.thumbnailSize     = QSize(size.x, size.y);
            
            itemsChanged.append(i);
        }
    }
    
    const djvFrameRangeList ranges = djvRangeUtil::range(itemsChanged);
    
    for (int i = 0; i < ranges.count(); ++i)
    {
        Q_EMIT dataChanged(
            index(ranges[i].min, 0),
            index(ranges[i].max, 0),
            QVector<int>() <<
                Qt::DecorationRole <<
                Qt::DisplayRole <<
                Qt::SizeHintRole <<
                Qt::EditRole <<
                SvgRole);
    }

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
        
        djvFileBrowserTestItem item;
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

    djvFileBrowserTestItem & item = _p->items[result.row];
    item.text         = item.formatText();
    item.infoComplete = true;
    item.info         = result.info;
        
    if (item.thumbnail.isNull())
    {
        item.thumbnail     = result.pixmap;
        item.thumbnailSize = result.pixmap.size();
    }
    
    Q_EMIT dataChanged(
        index(result.row, 0),
        index(result.row, 0),
        QVector<int>() <<
            Qt::DecorationRole <<
            Qt::DisplayRole <<
            Qt::SizeHintRole <<
            Qt::EditRole <<
            SvgRole);
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

    djvFileBrowserTestItem & item = _p->items[result.row];
    item.text              = item.formatText();
    item.thumbnailComplete = true;
    item.thumbnail         = result.pixmap;
    item.thumbnailSize     = result.pixmap.size();
    
    Q_EMIT dataChanged(
        index(result.row, 0),
        index(result.row, 0),
        QVector<int>() <<
            Qt::DecorationRole <<
            Qt::DisplayRole <<
            Qt::SizeHintRole <<
            Qt::EditRole <<
            SvgRole);
}

void djvFileBrowserTestModel::nextId()
{
    ++_p->id;

    _p->dirObject->setId(_p->id);
    _p->info->setId(_p->id);
    _p->thumbnail->setId(_p->id);
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

