//------------------------------------------------------------------------------
// Copyright (c) 2004-2014 Darby Johnston
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

//! \file djvFileBrowserModel.cpp

#include <djvFileBrowserModel.h>

#include <djvApplication.h>
#include <djvFileBrowserCache.h>

#include <djvAssert.h>
#include <djvDebug.h>
#include <djvError.h>
#include <djvFileInfoUtil.h>
#include <djvImage.h>
#include <djvImageIo.h>
#include <djvMemory.h>
#include <djvOpenGlImage.h>
#include <djvPixelDataUtil.h>
#include <djvStyle.h>
#include <djvTime.h>
#include <djvUser.h>
#include <djvVectorUtil.h>

#include <QAbstractItemView>
#include <QDateTime>
#include <QMimeData>
#include <QPair>
#include <QPixmap>

//------------------------------------------------------------------------------
// djvFileBrowserItem
//------------------------------------------------------------------------------

class djvFileBrowserItem
{
public:

    djvFileBrowserItem(
        const djvFileInfo &             fileInfo       = djvFileInfo(),
        djvFileBrowserModel::THUMBNAILS thumbnails     =
        static_cast<djvFileBrowserModel::THUMBNAILS>(0),
            int                         thumbnailsSize = 0);

    //! Get the file information.

    const djvFileInfo & fileInfo() const;

    //! Initialize the image I/O.

    void imageIoInit();

    //! Get whether the image I/O has been initialized.

    bool isImageIoInit() const;

    //! Get whether the image I/O is valid.

    bool isImageIoValid() const;

    //! Load the image.

    void loadImage();

    //! Get whether the image is loaded.

    bool isImageLoaded() const;

    //! Get the thumbnail image.

    const QPixmap & thumbnail() const;

    //! Get the display role data.

    const QVariant & displayRole(int column) const;

    //! Get the edit role data.

    const QVariant & editRole(int column) const;

private:

    djvFileInfo                     _fileInfo;
    djvFileBrowserModel::THUMBNAILS _thumbnails;
    int                             _thumbnailsSize;

    bool                    _imageIoInit;
    bool                    _imageIoValid;
    djvImageIoInfo          _imageIoInfo;
    bool                    _imageLoaded;
    djvPixelDataInfo::PROXY _proxy;
    QPixmap                 _thumbnail;

    QVariant _displayRole[djvFileBrowserModel::COLUMNS_COUNT];
    QVariant _editRole[djvFileBrowserModel::COLUMNS_COUNT];
};

djvFileBrowserItem::djvFileBrowserItem(
    const djvFileInfo &             fileInfo,
    djvFileBrowserModel::THUMBNAILS thumbnails,
    int                             thumbnailsSize) :
    _fileInfo      (fileInfo),
    _thumbnails    (thumbnails),
    _thumbnailsSize(thumbnailsSize),
    _imageIoInit   (false),
    _imageIoValid  (false),
    _imageLoaded   (false),
    _proxy         (static_cast<djvPixelDataInfo::PROXY>(0))
{
    // Initialize the display role data.

    _displayRole[djvFileBrowserModel::NAME] = fileInfo.name();
    _displayRole[djvFileBrowserModel::SIZE] =
        djvMemory::sizeLabel(fileInfo.size());
#if ! defined(DJV_WINDOWS)
    _displayRole[djvFileBrowserModel::USER] =
        djvUser::uidToString(fileInfo.user());
#endif
    _displayRole[djvFileBrowserModel::PERMISSIONS] =
        djvFileInfo::permissionsLabel(fileInfo.permissions());
    _displayRole[djvFileBrowserModel::TIME] =
        djvTime::timeToString(fileInfo.time());

    // Initialize the edit role data.

    _editRole[djvFileBrowserModel::NAME].setValue<djvFileInfo>(fileInfo);
    _editRole[djvFileBrowserModel::SIZE] = fileInfo.size();
#if ! defined(DJV_WINDOWS)
    _editRole[djvFileBrowserModel::USER] = fileInfo.user();
#endif
    _editRole[djvFileBrowserModel::PERMISSIONS] = fileInfo.permissions();
    _editRole[djvFileBrowserModel::TIME] = QDateTime::fromTime_t(fileInfo.time());
}

const djvFileInfo & djvFileBrowserItem::fileInfo() const
{
    return _fileInfo;
}

namespace
{

djvVector2i thumbnailsSize(
    djvFileBrowserModel::THUMBNAILS thumbnails,
    const djvVector2i &             in,
    int                             size,
    djvPixelDataInfo::PROXY *       proxy)
{
    const int imageSize = djvMath::max(in.x, in.y);

    if (imageSize <= 0)
        return djvVector2i();

    int _proxy = 0;
    
    double proxyScale = static_cast<double>(
        djvPixelDataUtil::proxyScale(djvPixelDataInfo::PROXY(_proxy)));

    if (djvFileBrowserModel::THUMBNAILS_LOW == thumbnails)
    {
        while (
            (imageSize / proxyScale) > size * 2 &&
            _proxy < djvPixelDataInfo::PROXY_COUNT)
        {
            proxyScale = static_cast<double>(
                djvPixelDataUtil::proxyScale(djvPixelDataInfo::PROXY(++_proxy)));
        }
    }
    
    if (proxy)
    {
        *proxy = djvPixelDataInfo::PROXY(_proxy);
    }

    const double scale = size / static_cast<double>(imageSize / proxyScale);
    
    return djvVectorUtil::ceil<double, int>(djvVector2f(in) / proxyScale * scale);
}

} // namespace

void djvFileBrowserItem::imageIoInit()
{
    if (! _imageIoInit)
    {
        //DJV_DEBUG("djvFileBrowserItem::imageIoInfo");

        QScopedPointer<djvImageLoad> load;

        try
        {
            load.reset(djvImageIoFactory::global()->load(_fileInfo, _imageIoInfo));
        }
        catch (const djvError & error)
        {
            //DJV_APP->printError(error);
        }

        if (load.data())
        {
            //DJV_DEBUG_PRINT("info = " << _imageIoInfo);
            
            _displayRole[djvFileBrowserModel::NAME] =
                QString("%1\n%2x%3:%4 %5\n%6@%7").
                    arg(_fileInfo.name()).
                    arg(_imageIoInfo.size.x).
                    arg(_imageIoInfo.size.y).
                    arg(djvVectorUtil::aspect(_imageIoInfo.size), 0, 'f', 2).
                    arg(djvStringUtil::label(_imageIoInfo.pixel).join(", ")).
                    arg(djvTime::frameToString(
                        _imageIoInfo.sequence.frames.count(),
                        _imageIoInfo.sequence.speed)).
                    arg(djvSpeed::speedToFloat(_imageIoInfo.sequence.speed));

            const djvVector2i size = thumbnailsSize(
                _thumbnails, _imageIoInfo.size, _thumbnailsSize, &_proxy);

            //DJV_DEBUG_PRINT("size = " << size);
            //DJV_DEBUG_PRINT("proxy = " << _proxy);

            _thumbnail = QPixmap(size.x, size.y);
            _thumbnail.fill(Qt::transparent);

            _imageIoValid = true;
        }

        _imageIoInit = true;
    }
}

bool djvFileBrowserItem::isImageIoInit() const
{
    return _imageIoInit;
}

bool djvFileBrowserItem::isImageIoValid() const
{
    return _imageIoValid;
}

void djvFileBrowserItem::loadImage()
{
    //DJV_DEBUG("djvFileBrowserItem::loadImage");
    //DJV_DEBUG_PRINT("file = " << _fileInfo);
    //DJV_DEBUG_PRINT("size = " << _thumbnail.width() << " " << _thumbnail.height());

    djvFileBrowserCache * cache = djvFileBrowserCache::global();

    if (cache->contains(_fileInfo))
    {
        //DJV_DEBUG_PRINT("cached");
        
        _thumbnail = *(cache->object(_fileInfo));

        _imageLoaded = true;
        
        return;
    }

    try
    {
        //qApp->setOverrideCursor(QCursor(Qt::WaitCursor));

        // Get the image I/O load plugin.
        
        QScopedPointer<djvImageLoad> load(
            djvImageIoFactory::global()->load(_fileInfo, _imageIoInfo));

        if (load.data())
        {
            //DJV_DEBUG_PRINT("info = " << _imageIoInfo);

            // Read the image.
            
            djvImage image;

            load->read(image, djvImageIoFrameInfo(-1, 0, _proxy));

            //DJV_DEBUG_PRINT("image = " << image);

            // Scale the image.
            
            djvImage tmp(djvPixelDataInfo(
                djvVector2i(_thumbnail.width(), _thumbnail.height()), _imageIoInfo.pixel));

            djvOpenGlImageOptions options;

            options.xform.scale =
                djvVector2f(tmp.size()) /
                (djvVector2f(image.size() * djvPixelDataUtil::proxyScale(image.info().proxy)));

            options.colorProfile = image.colorProfile;
            
            if (djvFileBrowserModel::THUMBNAILS_HIGH == _thumbnails)
            {
                options.filter = djvOpenGlImageFilter::filterHighQuality();
            }

            djvOpenGlImage::copy(image, tmp, options);

            _thumbnail = djvPixelDataUtil::toQt(tmp);

            _imageLoaded = true;
            
            cache->insert(
                _fileInfo,
                new QPixmap(_thumbnail),
                _thumbnail.width() * _thumbnail.height() * 4);

            //DJV_DEBUG_PRINT("cache size = " << cache->totalCost());
        }

        //qApp->restoreOverrideCursor();
    }
    catch (const djvError & error)
    {
        DJV_APP->printError(error);
    }
}

bool djvFileBrowserItem::isImageLoaded() const
{
    return _imageLoaded;
}

const QPixmap & djvFileBrowserItem::thumbnail() const
{
    return _thumbnail;
}

const QVariant & djvFileBrowserItem::displayRole(int column) const
{
    return _displayRole[column];
}

const QVariant & djvFileBrowserItem::editRole(int column) const
{
    return _editRole[column];
}

//------------------------------------------------------------------------------
// djvFileBrowserModel::P
//------------------------------------------------------------------------------

struct djvFileBrowserModel::P
{
    P() :
        sequence           (djvSequence::COMPRESS_RANGE),
        showHidden         (false),
        sort               (NAME),
        reverseSort        (false),
        sortDirsFirst      (true),
        thumbnails         (THUMBNAILS_HIGH),
        thumbnailsSize     (THUMBNAILS_MEDIUM),
        view               (0),
        thumbnailQueueTimer(-1)
    {}
    
    QString               path;
    djvSequence::COMPRESS sequence;
    QString               filterText;
    bool                  showHidden;
    COLUMNS               sort;
    bool                  reverseSort;
    bool                  sortDirsFirst;
    THUMBNAILS            thumbnails;
    THUMBNAILS_SIZE       thumbnailsSize;
    QAbstractItemView *   view;
    
    djvFileInfoList                     list;
    djvFileInfoList                     listTmp;
    mutable QVector<djvFileBrowserItem> items;

    typedef QPair<int, QModelIndex> Pair;
    
    mutable QVector<Pair> thumbnailQueue;
    mutable int           thumbnailQueueTimer;
};

//------------------------------------------------------------------------------
// djvFileBrowserModel
//------------------------------------------------------------------------------

const QStringList & djvFileBrowserModel::columnsLabels()
{
    static const QStringList data = QStringList() <<
        tr("Name") <<
        tr("Size") <<
#if ! defined(DJV_WINDOWS)
        tr("User") <<
#endif
        tr("Permissions") <<
        tr("Time");

    DJV_ASSERT(data.count() == COLUMNS_COUNT);

    return data;
}

djvFileBrowserModel::djvFileBrowserModel(QObject * parent) :
    QAbstractItemModel(parent),
    _p(new P)
{
    //DJV_DEBUG("djvFileBrowserModel::djvFileBrowserModel");
    
    //dirUpdate();
    //modelUpdate();
}

djvFileBrowserModel::~djvFileBrowserModel()
{
    delete _p;
}

const QString & djvFileBrowserModel::path() const
{
    return _p->path;
}

const djvFileInfoList & djvFileBrowserModel::contents() const
{
    return _p->listTmp;
}

djvFileInfo djvFileBrowserModel::fileInfo(const QModelIndex & index) const
{
    //DJV_DEBUG("djvFileBrowserModel::fileInfo");
    //DJV_DEBUG_PRINT("index = " << index.isValid());
    
    djvFileInfo * fileInfo = 0;
    
    if (index.isValid())
        fileInfo = (djvFileInfo *)index.internalPointer();
    
    return fileInfo ? *fileInfo : djvFileInfo();
}

djvSequence::COMPRESS djvFileBrowserModel::sequence() const
{
    return _p->sequence;
}

const QString & djvFileBrowserModel::filterText() const
{
    return _p->filterText;
}

bool djvFileBrowserModel::hasShowHidden() const
{
    return _p->showHidden;
}

djvFileBrowserModel::COLUMNS djvFileBrowserModel::sort() const
{
    return _p->sort;
}

bool djvFileBrowserModel::hasReverseSort() const
{
    return _p->reverseSort;
}

bool djvFileBrowserModel::hasSortDirsFirst() const
{
    return _p->sortDirsFirst;
}

const QStringList & djvFileBrowserModel::thumbnailsLabels()
{
    static const QStringList data = QStringList() <<
        tr("Off") <<
        tr("Low Quality") <<
        tr("High Quality");

    DJV_ASSERT(data.count() == THUMBNAILS_COUNT);

    return data;
}

djvFileBrowserModel::THUMBNAILS djvFileBrowserModel::thumbnails() const
{
    return _p->thumbnails;
}

const QStringList & djvFileBrowserModel::thumbnailsSizeLabels()
{
    static const QStringList data = QStringList() <<
        tr("Small") <<
        tr("Medium") <<
        tr("Large");

    DJV_ASSERT(data.count() == THUMBNAILS_COUNT);

    return data;
}

int djvFileBrowserModel::thumbnailsSizeValue(THUMBNAILS_SIZE size)
{
    static const QVector<int> data = QVector<int>() <<
        100 <<
        200 <<
        300;

    DJV_ASSERT(data.count() == THUMBNAILS_SIZE_COUNT);

    return data[size];
}

djvFileBrowserModel::THUMBNAILS_SIZE djvFileBrowserModel::thumbnailsSize() const
{
    return _p->thumbnailsSize;
}

void djvFileBrowserModel::setView(QAbstractItemView * view)
{
    _p->view = view;
}

QModelIndex	djvFileBrowserModel::index(
    int                 row,
    int                 column,
    const QModelIndex & parent) const
{
    if (! hasIndex(row, column, parent))
        return QModelIndex();

    return createIndex(row, column, &_p->listTmp[row]);
}

QModelIndex	djvFileBrowserModel::parent(const QModelIndex & index) const
{
    return QModelIndex();
}
    
QVariant djvFileBrowserModel::headerData(
    int             section,
    Qt::Orientation orientation,
    int             role) const
{
    switch (role)
    {
        case Qt::DisplayRole: return columnsLabels()[section];

        default: break;
    }
    
    return QVariant();
}

QVariant djvFileBrowserModel::data(
    const QModelIndex & index,
    int                 role) const
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
        column < 0 || column >= COLUMNS_COUNT)
        return QVariant();
    
    djvFileBrowserItem * item = &_p->items[row];
    
    const djvFileInfo & fileInfo = item->fileInfo();

    static const QVector<QPixmap> pixmaps = QVector<QPixmap>() <<
        QPixmap(":djvFileIcon.png") <<
        QPixmap(":djvSeqIcon.png") <<
        QPixmap(":djvDirIcon.png");
    
    switch (role)
    {
        case Qt::DecorationRole:
            switch (column)
            {
                case NAME:

                    if (_p->thumbnails != THUMBNAILS_OFF)
                    {
                        if (! item->isImageIoInit())
                        {
                            const P::Pair pair(row, index);

                            if (! _p->thumbnailQueue.contains(pair))
                            {
                                _p->thumbnailQueue += P::Pair(row, index);

                                if (-1 == _p->thumbnailQueueTimer)
                                {
                                    _p->thumbnailQueueTimer =
                                        const_cast<djvFileBrowserModel *>(this)->
                                        startTimer(0);
                                }
                            }

                            return pixmaps[fileInfo.type()];
                        }
                        else
                        {
                            if (item->isImageIoValid())
                            {
                                if (! item->isImageLoaded())
                                {
                                    const P::Pair pair(row, index);

                                    if (! _p->thumbnailQueue.contains(pair))
                                    {
                                        _p->thumbnailQueue += P::Pair(row, index);

                                        if (-1 == _p->thumbnailQueueTimer)
                                        {
                                            _p->thumbnailQueueTimer =
                                                const_cast<djvFileBrowserModel *>(this)->
                                                startTimer(0);
                                        }
                                    }
                                }

                                return item->thumbnail();
                            }
                        }
                    }

                    return pixmaps[fileInfo.type()];

                default: break;
            }
            break;
        
        case Qt::DisplayRole: return item->displayRole(column);
        case Qt::EditRole:    return item->editRole(column);

        case Qt::SizeHintRole:
            
            if (NAME == column && item->isImageIoValid())
            {
                const int margin = djvStyle::global()->sizeMetric().margin;
                
                return QSize(0, item->thumbnail().height() + margin * 2);
            }
            
            break;
            
        default: break;
    }
    
    return QVariant();
}

Qt::ItemFlags djvFileBrowserModel::flags(const QModelIndex & index) const
{
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);

    if (index.isValid())
        return Qt::ItemIsDragEnabled | defaultFlags;
    else
        return defaultFlags;
}

int djvFileBrowserModel::rowCount(const QModelIndex & parent) const
{
    return parent.isValid() ? 0 : _p->listTmp.count();
}

int djvFileBrowserModel::columnCount(const QModelIndex & parent) const
{
    return parent.isValid() ? 0 : COLUMNS_COUNT;
}

QStringList djvFileBrowserModel::mimeTypes() const
{
    return QStringList() << "application/x-filebrowser";
}

QMimeData * djvFileBrowserModel::mimeData(const QModelIndexList & indexes) const
{
    QMimeData * mimeData = new QMimeData();

    if (indexes.count())
    {
        const djvFileInfo fileInfo = this->fileInfo(indexes[0]);
        
        QStringList tmp;
        tmp << fileInfo;
        
        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);
        stream << tmp;
        
        mimeData->setData("application/x-filebrowser", data);
    }

    return mimeData;
}

void djvFileBrowserModel::setPath(const QString & path)
{
    if (path == _p->path)
        return;
    
    _p->path = path;
    
    dirUpdate();
    modelUpdate();
    
    Q_EMIT pathChanged(_p->path);
}

void djvFileBrowserModel::reload()
{
    dirUpdate();
    modelUpdate();
}

void djvFileBrowserModel::setSequence(djvSequence::COMPRESS in)
{
    if (in == _p->sequence)
        return;

    _p->sequence = in;

    dirUpdate();
    modelUpdate();
    
    Q_EMIT sequenceChanged(_p->sequence);
    Q_EMIT optionChanged();
}

void djvFileBrowserModel::setFilterText(const QString & text)
{
    if (text == _p->filterText)
        return;

    _p->filterText = text;

    modelUpdate();
    
    Q_EMIT filterTextChanged(_p->filterText);
    Q_EMIT optionChanged();
}

void djvFileBrowserModel::setShowHidden(bool show)
{
    if (show == _p->showHidden)
        return;

    _p->showHidden = show;
    
    modelUpdate();

    Q_EMIT showHiddenChanged(_p->showHidden);
    Q_EMIT optionChanged();
}

void djvFileBrowserModel::setSort(COLUMNS sort)
{
    if (sort == _p->sort)
        return;

    _p->sort = sort;

    modelUpdate();

    Q_EMIT sortChanged(_p->sort);
    Q_EMIT optionChanged();
}

void djvFileBrowserModel::setReverseSort(bool value)
{
    if (value == _p->reverseSort)
        return;

    _p->reverseSort = value;

    modelUpdate();

    Q_EMIT reverseSortChanged(_p->reverseSort);
    Q_EMIT optionChanged();
}

void djvFileBrowserModel::setSortDirsFirst(bool sort)
{
    if (sort == _p->sortDirsFirst)
        return;

    _p->sortDirsFirst = sort;

    modelUpdate();

    Q_EMIT sortDirsFirstChanged(_p->sortDirsFirst);
    Q_EMIT optionChanged();
}

void djvFileBrowserModel::setThumbnails(THUMBNAILS thumbnails)
{
    if (thumbnails == _p->thumbnails)
        return;

    _p->thumbnails = thumbnails;

    modelUpdate();

    Q_EMIT thumbnailsChanged(_p->thumbnails);
    Q_EMIT optionChanged();
}

void djvFileBrowserModel::setThumbnailsSize(THUMBNAILS_SIZE size)
{
    if (size == _p->thumbnailsSize)
        return;

    _p->thumbnailsSize = size;

    modelUpdate();

    Q_EMIT thumbnailsSizeChanged(_p->thumbnailsSize);
    Q_EMIT optionChanged();
}

void djvFileBrowserModel::timerEvent(QTimerEvent *)
{
    if (_p->view && _p->thumbnailQueue.count() > 0)
    {
        //DJV_DEBUG("djvFileBrowserModel::timerEvent");
        //DJV_DEBUG_PRINT("count = " << _p->thumbnailQueue.count());

        const QRect viewport(0, 0, _p->view->width(), _p->view->height());

        //DJV_DEBUG_PRINT("viewport = " << djvBoxUtil::fromQt(viewport));

        const P::Pair pair = _p->thumbnailQueue.first();

        _p->thumbnailQueue.pop_front();

        djvFileBrowserItem & item = _p->items[pair.first];

        const QRect rect = _p->view->visualRect(pair.second);

        //DJV_DEBUG_PRINT("rect = " << djvBoxUtil::fromQt(rect));

        if (rect.intersects(viewport))
        {
            //DJV_DEBUG_PRINT("intersect");

            if (! item.isImageIoInit())
            {
                item.imageIoInit();

                if (item.isImageIoValid())
                {
                    Q_EMIT dataChanged(
                        pair.second,
                        pair.second);
                        //QVector<int>() << Qt::DisplayRole << Qt::SizeHintRole);

                    _p->thumbnailQueue += pair;
                }
            }
            else
            {
                item.loadImage();
            }

            Q_EMIT dataChanged(
                pair.second,
                pair.second);
                //QVector<int>() << Qt::DisplayRole);
        }
    }
    else
    {
        killTimer(_p->thumbnailQueueTimer);

        _p->thumbnailQueueTimer = -1;

        _p->thumbnailQueue.clear();
    }
}

void djvFileBrowserModel::dirUpdate()
{
    //DJV_DEBUG("djvFileBrowserModel::dirUpdate");
    //DJV_DEBUG_PRINT("path = " << _p->path);

    // Get directory contents.

    _p->list = djvFileInfoUtil::list(_p->path, _p->sequence);

    // Add parent directory.

    if (djvFileInfo(_p->path).exists())
    {
        _p->list.push_front(djvFileInfo(_p->path + ".."));
    }
    
    //DJV_DEBUG_PRINT("list = " << _p->list.count());
    //Q_FOREACH(const djvFileInfo & fileInfo, _p->list)
    //    DJV_DEBUG_PRINT("fileInfo = " << fileInfo << " " << fileInfo.type());
}

void djvFileBrowserModel::modelUpdate()
{
    if (_p->path.isEmpty())
        return;

    //DJV_DEBUG("djvFileBrowserModel::modelUpdate");
    //DJV_DEBUG_PRINT("path = " << _p->path);

    beginResetModel();
    
    _p->listTmp = _p->list;
    
    // File sequence directory contents.

    //djvFileInfoUtil::compressSequence(_p->listTmp, _p->seq);

    // Filter directory contents.

    if (_p->filterText.length() > 0 || !_p->showHidden)
    {
        const djvFileInfoUtil::FILTER filter =
            !_p->showHidden ?
            djvFileInfoUtil::FILTER_HIDDEN :
            djvFileInfoUtil::FILTER_NONE;

        djvFileInfoUtil::filter(_p->listTmp, filter, _p->filterText);
    }

    // Sort directory contents.

    djvFileInfoUtil::SORT sort = static_cast<djvFileInfoUtil::SORT>(0);

    switch (_p->sort)
    {
        case NAME:        sort = djvFileInfoUtil::SORT_NAME; break;
        case SIZE:        sort = djvFileInfoUtil::SORT_SIZE; break;
#if ! defined(DJV_WINDOWS)
        case USER:        sort = djvFileInfoUtil::SORT_USER; break;
#endif
        case PERMISSIONS: sort = djvFileInfoUtil::SORT_PERMISSIONS; break;
        case TIME:        sort = djvFileInfoUtil::SORT_TIME; break;

        default: break;
    }

    djvFileInfoUtil::sort(_p->listTmp, sort, _p->reverseSort);

    if (_p->sortDirsFirst)
    {
        djvFileInfoUtil::sortDirsFirst(_p->listTmp);
    }
    
    _p->items.clear();
    
    for (int i = 0; i < _p->listTmp.count(); ++i)
    {
        _p->items += djvFileBrowserItem(
            _p->listTmp[i],
            _p->thumbnails,
            thumbnailsSizeValue(_p->thumbnailsSize));
    }
    
    endResetModel();
}

//------------------------------------------------------------------------------

_DJV_STRING_OPERATOR_LABEL(djvFileBrowserModel::COLUMNS,
    djvFileBrowserModel::columnsLabels())
_DJV_STRING_OPERATOR_LABEL(djvFileBrowserModel::THUMBNAILS,
    djvFileBrowserModel::thumbnailsLabels())
_DJV_STRING_OPERATOR_LABEL(djvFileBrowserModel::THUMBNAILS_SIZE,
    djvFileBrowserModel::thumbnailsSizeLabels())
