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

#include <djvFileBrowserCache.h>

#include <djvAssert.h>
#include <djvDebug.h>
#include <djvDebugLog.h>
#include <djvError.h>
#include <djvErrorUtil.h>
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
#include <QFutureWatcher>
#include <QMimeData>
#include <QPixmap>
#include <QtConcurrent/QtConcurrent>

//------------------------------------------------------------------------------
// djvFileBrowserItem
//------------------------------------------------------------------------------

class djvFileBrowserItem
{
public:

    //! Constructor.
    
    djvFileBrowserItem(const djvFileInfo & fileInfo = djvFileInfo());

    //! Get the file information.

    const djvFileInfo & fileInfo() const;

    //! Get whether the image information has been requested.
    
    bool hasImageInfoRequest() const;
    
    //! Set that the image information has been requesetd.
    
    void setImageInfoRequest();
    
    //! Set the image information.
    
    void setImageInfo(
        const djvImageIoInfo &          info,
        djvFileBrowserModel::THUMBNAILS thumbnails,
        int                             thumbnailsSize);

    //! Get the proxy scale.
    
    djvPixelDataInfo::PROXY proxy() const;

    //! Get whether the image has been requested.
    
    bool hasImageRequest() const;
    
    //! Set that the image has been requesetd.
    
    void setImageRequest();
    
    //! Set the image.
    
    void setImage(const djvImage & image);
    
    //! Get the thumbnail image.

    const QPixmap & thumbnail() const;

    //! Get the display role data.

    const QVariant & displayRole(int column) const;

    //! Get the edit role data.

    const QVariant & editRole(int column) const;

private:

    djvFileInfo                     _fileInfo;
    bool                            _imageInfoRequest;
    bool                            _imageRequest;
    djvFileBrowserModel::THUMBNAILS _thumbnails;
    QPixmap                         _thumbnail;
    djvPixelDataInfo::PROXY         _proxy;

    QVariant _displayRole[djvFileBrowserModel::COLUMNS_COUNT];
    QVariant _editRole   [djvFileBrowserModel::COLUMNS_COUNT];
};

djvFileBrowserItem::djvFileBrowserItem(const djvFileInfo & fileInfo) :
    _fileInfo        (fileInfo),
    _imageInfoRequest(false),
    _imageRequest    (false),
    _thumbnails      (static_cast<djvFileBrowserModel::THUMBNAILS>(0)),
    _proxy           (static_cast<djvPixelDataInfo::PROXY>(0))
{
    // Initialize the display role data.

    _displayRole[djvFileBrowserModel::NAME] =
        fileInfo.name();
    _displayRole[djvFileBrowserModel::SIZE] =
        djvMemory::sizeLabel(fileInfo.size());
#if ! defined(DJV_WINDOWS)
    _displayRole[djvFileBrowserModel::USER] =
        djvUser::uidToString(fileInfo.user());
#endif // DJV_WINDOWS
    _displayRole[djvFileBrowserModel::PERMISSIONS] =
        djvFileInfo::permissionsLabel(fileInfo.permissions());
    _displayRole[djvFileBrowserModel::TIME] =
        djvTime::timeToString(fileInfo.time());

    // Initialize the edit role data.

    _editRole[djvFileBrowserModel::NAME].setValue<djvFileInfo>(fileInfo);
    _editRole[djvFileBrowserModel::SIZE] =
        fileInfo.size();
#if ! defined(DJV_WINDOWS)
    _editRole[djvFileBrowserModel::USER] =
        fileInfo.user();
#endif // DJV_WINDOWS
    _editRole[djvFileBrowserModel::PERMISSIONS] =
        fileInfo.permissions();
    _editRole[djvFileBrowserModel::TIME] =
        QDateTime::fromTime_t(fileInfo.time());
}

const djvFileInfo & djvFileBrowserItem::fileInfo() const
{
    return _fileInfo;
}
    
bool djvFileBrowserItem::hasImageInfoRequest() const
{
    return _imageInfoRequest;
}

void djvFileBrowserItem::setImageInfoRequest()
{
    _imageInfoRequest = true;
}

namespace
{

djvVector2i _thumbnailsSize(
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

void djvFileBrowserItem::setImageInfo(
    const djvImageIoInfo &          info,
    djvFileBrowserModel::THUMBNAILS thumbnails,
    int                             thumbnailsSize)
{
    //DJV_DEBUG("djvFileBrowserItem::setImageInfo");
    //DJV_DEBUG_PRINT("info = " << info);
    
    _thumbnails = thumbnails;
    
    _displayRole[djvFileBrowserModel::NAME] =
        QString("%1\n%2x%3:%4 %5\n%6@%7").
            arg(_fileInfo.name()).
            arg(info.size.x).
            arg(info.size.y).
            arg(djvVectorUtil::aspect(info.size), 0, 'f', 2).
            arg(djvStringUtil::label(info.pixel).join(", ")).
            arg(djvTime::frameToString(
                info.sequence.frames.count(),
                info.sequence.speed)).
            arg(djvSpeed::speedToFloat(info.sequence.speed));

    const djvVector2i size = _thumbnailsSize(
        _thumbnails, info.size, thumbnailsSize, &_proxy);

    //DJV_DEBUG_PRINT("size = " << size);
    //DJV_DEBUG_PRINT("proxy = " << _proxy);

    _thumbnail = QPixmap(size.x, size.y);
    _thumbnail.fill(Qt::transparent);
}

djvPixelDataInfo::PROXY djvFileBrowserItem::proxy() const
{
    return _proxy;
}

bool djvFileBrowserItem::hasImageRequest() const
{
    return _imageRequest;
}

void djvFileBrowserItem::setImageRequest()
{
    _imageRequest = true;
}

void djvFileBrowserItem::setImage(const djvImage & image)
{
    //DJV_DEBUG("djvFileBrowserItem::setImage");
    //DJV_DEBUG_PRINT("image = " << image);
    //DJV_DEBUG_PRINT("size = " << _thumbnail.width() << " " << _thumbnail.height());

    try
    {
        //qApp->setOverrideCursor(QCursor(Qt::WaitCursor));

        // Scale the image.
        
        djvImage tmp(djvPixelDataInfo(
            djvVector2i(_thumbnail.width(), _thumbnail.height()),
            image.pixel()));

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

        //qApp->restoreOverrideCursor();
    }
    catch (const djvError & error)
    {
        DJV_LOG("djvFileBrowserItem", djvErrorUtil::format(error).join("\n"));
    }
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
        view               (0)
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
    
    djvFileInfoList list;
    djvFileInfoList listTmp;
    
    mutable QVector<djvFileBrowserItem> items;
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

namespace
{

struct ImageIoInfo
{
    ImageIoInfo() :
        item (0),
        row  (0),
        valid(false)
    {}
    
    djvFileBrowserItem * item;
    int                  row;
    bool                 valid;
    djvImageIoInfo       info;
};

ImageIoInfo imageIoInfo(
    djvFileBrowserItem * item,
    int                  row,
    const djvFileInfo &  fileInfo)
{
    //DJV_DEBUG("imageIoInfo");
    
    ImageIoInfo out;
    out.item = item;
    out.row  = row;
    
    try
    {
        QScopedPointer<djvImageLoad> load(
            djvImageIoFactory::global()->load(fileInfo, out.info));
        
        out.valid = true;
    }
    catch (const djvError & error)
    {}

    return out;
}

struct Image
{
    Image() :
        item (0),
        row  (0),
        valid(false)
    {}
    
    djvFileBrowserItem * item;
    int                  row;
    bool                 valid;
    djvImage             image;
};

Image imageLoad(
    djvFileBrowserItem *    item,
    int                     row,
    const djvFileInfo &     fileInfo,
    djvPixelDataInfo::PROXY proxy)
{
    Image out;
    out.item = item;
    out.row  = row;
    
    try
    {
        djvImageIoInfo imageIoInfo;
        
        QScopedPointer<djvImageLoad> load(
            djvImageIoFactory::global()->load(fileInfo, imageIoInfo));
        
        load->read(out.image, djvImageIoFrameInfo(-1, 0, proxy));

        out.valid = true;
    }
    catch (const djvError & error)
    {
        if (out.image.isValid())
        {
            out.valid = true;
        }
    }

    return out;
}

} // namespace

void djvFileBrowserModel::imageIoInfoCallback()
{
    //DJV_DEBUG("djvFileBrowserModel::imageIoInfoCallback");
    
    QFutureWatcher<ImageIoInfo> * watcher =
        dynamic_cast<QFutureWatcher<ImageIoInfo> *>(sender());
    
    const QFuture<ImageIoInfo> & future = watcher->future();
    
    const ImageIoInfo & info = future.result();
    
    //DJV_DEBUG_PRINT("info = " << info.info);
    
    if (info.row < _p->items.count() &&
        info.item == &_p->items[info.row] &&
        info.valid)
    {
        _p->items[info.row].setImageInfo(
            info.info,
            _p->thumbnails,
            thumbnailsSizeValue(_p->thumbnailsSize));

        const QModelIndex index = this->index(info.row, 0);

        Q_EMIT dataChanged(index, index);
    }
    
    watcher->deleteLater();
}

void djvFileBrowserModel::imageLoadCallback()
{
    //DJV_DEBUG("djvFileBrowserModel::imageLoadCallback");
    
    QFutureWatcher<Image> * watcher =
        dynamic_cast<QFutureWatcher<Image> *>(sender());
    
    const QFuture<Image> & future = watcher->future();
    
    const Image & image = future.result();
    
    //DJV_DEBUG_PRINT("image = " << info.image);
    
    if (image.row < _p->items.count() &&
        image.item == &_p->items[image.row] &&
        image.valid)
    {
        _p->items[image.row].setImage(image.image);
        
        const QModelIndex index = this->index(image.row, 0);
        
        Q_EMIT dataChanged(index, index);
    }
    
    watcher->deleteLater();
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
    
    djvFileBrowserItem & item = _p->items[row];
    
    const djvFileInfo & fileInfo = item.fileInfo();

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
                        if (! item.hasImageInfoRequest())
                        {
                            item.setImageInfoRequest();
                        
                            QFuture<ImageIoInfo> future = QtConcurrent::run(
                                imageIoInfo,
                                &item,
                                row,
                                item.fileInfo());
                            
                            QFutureWatcher<ImageIoInfo> * watcher =
                                new QFutureWatcher<ImageIoInfo>;
                            watcher->setFuture(future);
                            
                            connect(
                                watcher,
                                SIGNAL(finished()),
                                SLOT(imageIoInfoCallback()));
                        }
                        else if (! item.hasImageRequest())
                        {
                            item.setImageRequest();

                            QFuture<Image> future = QtConcurrent::run(
                                imageLoad,
                                &item,
                                row,
                                item.fileInfo(),
                                item.proxy());
                            
                            QFutureWatcher<Image> * watcher =
                                new QFutureWatcher<Image>;
                            watcher->setFuture(future);
                            
                            connect(
                                watcher,
                                SIGNAL(finished()),
                                SLOT(imageLoadCallback()));
                        }
                        else if (! item.thumbnail().isNull())
                        {
                            return item.thumbnail();
                        }
                    }

                    return pixmaps[fileInfo.type()];

                default: break;
            }
            break;
        
        case Qt::DisplayRole: return item.displayRole(column);
        case Qt::EditRole:    return item.editRole(column);

        case Qt::SizeHintRole:
            
            if (NAME == column && ! item.thumbnail().isNull())
            {
                const int margin = djvStyle::global()->sizeMetric().margin;
                
                return QSize(0, item.thumbnail().height() + margin * 2);
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
        _p->items += djvFileBrowserItem(_p->listTmp[i]);
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
