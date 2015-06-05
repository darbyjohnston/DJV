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

//! \file djvFileBrowserModel.cpp

#include <djvFileBrowserModel.h>

#include <djvFileBrowserModelPrivate.h>
#include <djvGuiContext.h>

#include <djvAssert.h>
#include <djvDebug.h>
#include <djvDebugLog.h>
#include <djvError.h>
#include <djvErrorUtil.h>
#include <djvFileInfoUtil.h>
#include <djvMemory.h>
#include <djvStyle.h>
#include <djvUser.h>
#include <djvVectorUtil.h>

#include <QCoreApplication>
#include <QMimeData>

//------------------------------------------------------------------------------
// djvFileBrowserModelPrivate
//------------------------------------------------------------------------------

struct djvFileBrowserModelPrivate
{
    djvFileBrowserModelPrivate(djvGuiContext * context) :
        sequence      (djvSequence::COMPRESS_RANGE),
        showHidden    (false),
        sort          (djvFileBrowserModel::NAME),
        reverseSort   (false),
        sortDirsFirst (true),
        thumbnails    (djvFileBrowserModel::THUMBNAILS_HIGH),
        thumbnailsSize(djvFileBrowserModel::THUMBNAILS_MEDIUM),
        context       (context)
    {}
    
    QString                              path;
    djvSequence::COMPRESS                sequence;
    QString                              filterText;
    bool                                 showHidden;
    djvFileBrowserModel::COLUMNS         sort;
    bool                                 reverseSort;
    bool                                 sortDirsFirst;
    djvFileBrowserModel::THUMBNAILS      thumbnails;
    djvFileBrowserModel::THUMBNAILS_SIZE thumbnailsSize;
    
    djvFileInfoList list;
    djvFileInfoList listTmp;
    
    mutable QVector<djvFileBrowserItem *> items;
    
    djvGuiContext * context;
};

//------------------------------------------------------------------------------
// djvFileBrowserModel
//------------------------------------------------------------------------------

const QStringList & djvFileBrowserModel::columnsLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvFileBrowserModel", "Name") <<
        qApp->translate("djvFileBrowserModel", "Size") <<
#if ! defined(DJV_WINDOWS)
        qApp->translate("djvFileBrowserModel", "User") <<
#endif
        qApp->translate("djvFileBrowserModel", "Permissions") <<
        qApp->translate("djvFileBrowserModel", "Time");

    DJV_ASSERT(data.count() == COLUMNS_COUNT);

    return data;
}

djvFileBrowserModel::djvFileBrowserModel(djvGuiContext * context, QObject * parent) :
    QAbstractItemModel(parent),
    _p(new djvFileBrowserModelPrivate(context))
{
    //DJV_DEBUG("djvFileBrowserModel::djvFileBrowserModel");
    
    //dirUpdate();
    //modelUpdate();
}

djvFileBrowserModel::~djvFileBrowserModel()
{
    for (int i = 0; i < _p->items.count(); ++i)
    {
        delete _p->items[i];
        
        _p->items[i] = 0;
    }
    
    _p->items.clear();
    
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
    {
        fileInfo = (djvFileInfo *)index.internalPointer();
    }
    
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
        qApp->translate("djvFileBrowserModel", "Off") <<
        qApp->translate("djvFileBrowserModel", "Low Quality") <<
        qApp->translate("djvFileBrowserModel", "High Quality");

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
        qApp->translate("djvFileBrowserModel", "Small") <<
        qApp->translate("djvFileBrowserModel", "Medium") <<
        qApp->translate("djvFileBrowserModel", "Large");

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

void djvFileBrowserModel::imageInfoCallback()
{
    //DJV_DEBUG("djvFileBrowserModel::imageInfoCallback");
    
    const int row = _p->items.indexOf(
        qobject_cast<djvFileBrowserItem *>(sender()));
    
    if (row != -1)
    {
        const QModelIndex index = this->index(row, 0);
        
        Q_EMIT dataChanged(index, index);
    }
}

void djvFileBrowserModel::thumbnailCallback()
{
    //DJV_DEBUG("djvFileBrowserModel::thumbnailCallback");
    
    const int row = _p->items.indexOf(
        qobject_cast<djvFileBrowserItem *>(sender()));
    
    if (row != -1)
    {
        const QModelIndex index = this->index(row, 0);
        
        Q_EMIT dataChanged(index, index);
    }
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
    
    djvFileBrowserItem * item = _p->items[row];
    
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
                        item->requestImage();
                        
                        return item->thumbnail();
                    }

                    return pixmaps[fileInfo.type()];

                default: break;
            }
            break;
        
        case Qt::DisplayRole: return item->displayRole(column);
        case Qt::EditRole:    return item->editRole(column);

        case Qt::SizeHintRole:
            
            if (NAME == column && ! item->thumbnail().isNull())
            {
                const int margin = _p->context->style()->sizeMetric().margin;
                
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
    
    for (int i = 0; i < _p->items.count(); ++i)
    {
        delete _p->items[i];
        
        _p->items[i] = 0;
    }
    
    _p->items.clear();
    _p->items.resize(_p->listTmp.count());
    
    for (int i = 0; i < _p->listTmp.count(); ++i)
    {
        //DJV_DEBUG_PRINT("i = " << i);
        
        djvFileBrowserItem * item = new djvFileBrowserItem(
            _p->listTmp[i],
            _p->thumbnails,
            _p->thumbnailsSize,
            _p->context,
            this);
        
        _p->items[i] = item;
        
        connect(item, SIGNAL(imageInfoAvailable()), SLOT(imageInfoCallback()));
        connect(item, SIGNAL(thumbnailAvailable()), SLOT(thumbnailCallback()));
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
