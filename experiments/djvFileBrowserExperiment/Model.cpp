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

#include <Model.h>

#include <Item.h>

#include <djvIconLibrary.h>
#include <djvUIContext.h>

#include <djvDebug.h>
#include <djvFileInfoUtil.h>
#include <djvRangeUtil.h>

#include <QMutexLocker>
#include <QScopedPointer>
#include <QThread>

//------------------------------------------------------------------------------
// Model::Private
//------------------------------------------------------------------------------

struct Model::Private
{
    Private(djvUIContext * context) :
        context(context),
        dirObject(new Dir(context)),
        info(new Info(context)),
        thumbnail(new Thumbnail(context)),
        sizeHint(context->iconLibrary()->pixmap("djvFileIcon.png").size())
    {}

    djvUIContext * context = nullptr;
    QDir dir;
    QDir dirPrev;
    djvSequence::COMPRESS sequence = static_cast<djvSequence::COMPRESS>(0);
    QString filterText;
    bool showHidden = false;
    Util::COLUMNS sort = Util::NAME;
    bool reverseSort = false;
    bool sortDirsFirst = true;
    quint64 id = 0;
    QVector<Item> items;
    QScopedPointer<Dir> dirObject;
    Util::THUMBNAILS thumbnails = Util::THUMBNAILS_LOW;
    int thumbnailSize = 160;
    QScopedPointer<Info> info;
    QScopedPointer<Thumbnail> thumbnail;
    QVariant sizeHint;
};

//------------------------------------------------------------------------------
// Model
//------------------------------------------------------------------------------

const int Model::SvgRole = Qt::UserRole + 1;

Model::Model(
    djvUIContext * context,
    QObject *      parent) :
    QAbstractItemModel(parent),
    _p(new Private(context))
{
    //DJV_DEBUG("Model::Model");
    _p->dirObject->connect(
        this,
        SIGNAL(requestDir(const DirRequest &)),
        SLOT(request(const DirRequest &)));
    connect(
        _p->dirObject.data(),
        SIGNAL(result(const DirResult &)),
        SLOT(dirCallback(const DirResult &)));
    connect(
        _p->info.data(),
        SIGNAL(result(const InfoResult &)),
        SLOT(infoCallback(const InfoResult &)));
    connect(
        _p->thumbnail.data(),
        SIGNAL(result(const ThumbnailResult &)),
        SLOT(thumbnailCallback(const ThumbnailResult &)));
    //startTimer(1000);
}

Model::~Model()
{
    nextId();
}

const QDir & Model::dir() const
{
    return _p->dir;
}

QString Model::path() const
{
    return _p->dir.absolutePath();
}

djvSequence::COMPRESS Model::sequence() const
{
    return _p->sequence;
}

const QString & Model::filterText() const
{
    return _p->filterText;
}

bool Model::hasShowHidden() const
{
    return _p->showHidden;
}

Util::COLUMNS Model::sort() const
{
    return _p->sort;
}

bool Model::hasReverseSort() const
{
    return _p->reverseSort;
}

bool Model::hasSortDirsFirst() const
{
    return _p->sortDirsFirst;
}

Util::THUMBNAILS Model::thumbnails() const
{
    return _p->thumbnails;
}

int Model::thumbnailSize() const
{
    return _p->thumbnailSize;
}

int Model::columnCount(const QModelIndex & parent) const
{
    return 1;
}

QVariant Model::data(const QModelIndex & index, int role) const
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

    //DJV_DEBUG("Model::data");
    //DJV_DEBUG_PRINT("row = " << row);
    //DJV_DEBUG_PRINT("column = " << column);

    Model * that = const_cast<Model *>(this);
    Item & item = that->_p->items[row];
    static const QVector<QPixmap> pixmaps = QVector<QPixmap>() <<
        _p->context->iconLibrary()->pixmap("djvFileIcon.png") <<
        _p->context->iconLibrary()->pixmap("djvSeqIcon.png") <<
        _p->context->iconLibrary()->pixmap("djvDirIcon.png");
    switch (role)
    {
        case Qt::DecorationRole:
            switch (column)
            {
                case Util::NAME:
                    if (_p->thumbnails != Util::THUMBNAILS_OFF)
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

Qt::ItemFlags Model::flags(const QModelIndex & index) const
{
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);
    if (index.isValid())
        return Qt::ItemIsDragEnabled | defaultFlags;
    else
        return defaultFlags;
}

QVariant Model::headerData(
    int             section,
    Qt::Orientation orientation,
    int             role) const
{
    return QVariant();
}

QModelIndex	Model::index(
    int                 row,
    int                 column,
    const QModelIndex & parent) const
{
    if (! hasIndex(row, column, parent))
        return QModelIndex();
    return createIndex(row, column, (void *)&_p->items[row]);
}

QModelIndex	Model::parent(const QModelIndex &) const
{
    return QModelIndex();
}

QHash<int, QByteArray> Model::roleNames() const
{
    QHash<int, QByteArray> r = QAbstractItemModel::roleNames();
    r.insert(SvgRole, "svg");
    return r;
}

int Model::rowCount(const QModelIndex & parent) const
{
    return parent.isValid() ? 0 : _p->items.count();
}

void Model::setDir(const QDir & dir)
{
    if (dir == _p->dir)
        return;
    //DJV_DEBUG("Model::setDir");
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

void Model::setPath(const QString & path)
{
    setDir(path);
}

void Model::setSequence(djvSequence::COMPRESS sequence)
{
    if (sequence == _p->sequence)
        return;
    //DJV_DEBUG("Model::setSequence");
    //DJV_DEBUG_PRINT("sequence = " << sequence);
    _p->sequence = sequence;
    nextId();
    Q_EMIT requestDir(dirRequest());
}

void Model::up()
{
    QDir tmp(_p->dir);
    if (tmp.cdUp())
    {
        setDir(tmp);
    }
}

void Model::back()
{
    const QDir tmp(_p->dirPrev);
    setDir(tmp);
}

void Model::reload()
{
    nextId();
    DirRequest request = dirRequest();
    request.reload = true;
    Q_EMIT requestDir(request);
}

void Model::setFilterText(const QString & text)
{
    if (text == _p->filterText)
        return;
    _p->filterText = text;
    nextId();
    Q_EMIT requestDir(dirRequest());
    Q_EMIT filterTextChanged(_p->filterText);
}

void Model::setShowHidden(bool show)
{
    if (show == _p->showHidden)
        return;
    _p->showHidden = show;
    nextId();
    Q_EMIT requestDir(dirRequest());
    Q_EMIT showHiddenChanged(_p->showHidden);
}

void Model::setSort(Util::COLUMNS sort)
{
    if (sort == _p->sort)
        return;
    _p->sort = sort;
    nextId();
    Q_EMIT requestDir(dirRequest());
    Q_EMIT sortChanged(_p->sort);
}

void Model::setReverseSort(bool value)
{
    if (value == _p->reverseSort)
        return;
    _p->reverseSort = value;
    nextId();
    Q_EMIT requestDir(dirRequest());
    Q_EMIT reverseSortChanged(_p->reverseSort);
}

void Model::setSortDirsFirst(bool sort)
{
    if (sort == _p->sortDirsFirst)
        return;
    _p->sortDirsFirst = sort;
    nextId();
    Q_EMIT requestDir(dirRequest());
    Q_EMIT sortDirsFirstChanged(_p->sortDirsFirst);
}

void Model::setThumbnails(Util::THUMBNAILS thumbnails)
{
    if (thumbnails == _p->thumbnails)
        return;
    _p->thumbnails = thumbnails;
    nextId();
    Q_EMIT requestDir(dirRequest());
    Q_EMIT thumbnailsChanged(_p->thumbnails);
}

void Model::setThumbnailSize(int size)
{
    if (size == _p->thumbnailSize)
        return;
    _p->thumbnailSize = size;
    nextId();
    djvFrameList itemsChanged;
    for (int i = 0; i < _p->items.count(); ++i)
    {
        Item & item = _p->items[i];
        if (item.infoComplete || item.thumbnailComplete)
        {
            djvPixelDataInfo::PROXY proxy = djvPixelDataInfo::PROXY_NONE;
            const glm::ivec2 size = Util::thumbnailSize(
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

void Model::timerEvent(QTimerEvent * event)
{
    /*DJV_DEBUG("Model::timerEvent");
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

void Model::dirCallback(const DirResult & result)
{
    if (result.id != _p->id)
        return;
    //DJV_DEBUG("Model::dirCallback");
    //DJV_DEBUG_PRINT("list = " << result.list);
    //DJV_DEBUG_PRINT("id = " << result.id);
    beginResetModel();
    _p->items.clear();
    for (int i = 0; i < result.list.count(); ++i)
    {
        const djvFileInfo & fileInfo = result.list[i];
        Item item;
        item.fileInfo = fileInfo;
        item.text     = fileInfo.name();
        _p->items.append(item);
    }
    endResetModel();
    Q_EMIT requestDirComplete();
}

void Model::infoCallback(const InfoResult & result)
{
    if (result.id != _p->id || result.pixmap.isNull())
        return;
    //DJV_DEBUG("Model::infoCallback");
    //DJV_DEBUG_PRINT("info = " << result.info);
    //DJV_DEBUG_PRINT("row = " << result.row);
    //DJV_DEBUG_PRINT("id = " << result.id);
    Item & item = _p->items[result.row];
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

void Model::thumbnailCallback(const ThumbnailResult & result)
{
    if (result.id != _p->id || result.pixmap.isNull())
        return;
    //DJV_DEBUG("Model::thumbnailCallback");
    //DJV_DEBUG_PRINT("info = " << result.info);
    //DJV_DEBUG_PRINT("row = " << result.row);
    //DJV_DEBUG_PRINT("id = " << result.id);
    Item & item = _p->items[result.row];
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

void Model::nextId()
{
    ++_p->id;
    _p->dirObject->setId(_p->id);
    _p->info->setId(_p->id);
    _p->thumbnail->setId(_p->id);
}

DirRequest Model::dirRequest() const
{
    DirRequest r;
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

InfoRequest Model::infoRequest(
    const djvFileInfo & fileInfo,
    int                 row) const
{
    InfoRequest r;
    r.fileInfo      = fileInfo;
    r.thumbnails    = _p->thumbnails;
    r.thumbnailSize = _p->thumbnailSize;
    r.row           = row;
    r.id            = _p->id;
    return r;
}

ThumbnailRequest Model::thumbnailRequest(
    const djvFileInfo & fileInfo,
    int                 row) const
{
    ThumbnailRequest r;
    r.fileInfo      = fileInfo;
    r.thumbnails    = _p->thumbnails;
    r.thumbnailSize = _p->thumbnailSize;
    r.row           = row;
    r.id            = _p->id;
    return r;
}

