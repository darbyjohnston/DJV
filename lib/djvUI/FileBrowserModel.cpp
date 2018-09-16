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

#include <djvUI/FileBrowserModel.h>

#include <djvUI/FileBrowserModelPrivate.h>
#include <djvUI/UIContext.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>
#include <djvCore/DebugLog.h>
#include <djvCore/Error.h>
#include <djvCore/ErrorUtil.h>
#include <djvCore/FileInfoUtil.h>
#include <djvCore/Memory.h>
#include <djvUI/Style.h>
#include <djvCore/User.h>
#include <djvCore/VectorUtil.h>

#include <QCoreApplication>
#include <QMimeData>

namespace djv
{
    namespace UI
    {
        struct FileBrowserModel::Private
        {
            Private(UIContext * context) :
                context(context)
            {}

            QString path;
            Core::Sequence::COMPRESS sequence = Core::Sequence::COMPRESS_RANGE;
            QString filterText;
            bool showHidden = false;
            FileBrowserModel::COLUMNS columnsSort = FileBrowserModel::NAME;
            bool reverseSort = false;
            bool sortDirsFirst = true;
            FileBrowserModel::THUMBNAILS thumbnails = FileBrowserModel::THUMBNAILS_HIGH;
            FileBrowserModel::THUMBNAILS_SIZE thumbnailsSize = FileBrowserModel::THUMBNAILS_MEDIUM;

            Core::FileInfoList list;
            Core::FileInfoList listTmp;

            mutable QVector<FileBrowserItem *> items;

            UIContext * context = nullptr;
        };

        const QStringList & FileBrowserModel::columnsLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::UI::FileBrowserModel", "Name") <<
                qApp->translate("djv::UI::FileBrowserModel", "Size") <<
#if ! defined(DJV_WINDOWS)
                qApp->translate("djv::UI::FileBrowserModel", "User") <<
#endif
                qApp->translate("djv::UI::FileBrowserModel", "Permissions") <<
                qApp->translate("djv::UI::FileBrowserModel", "Time");
            DJV_ASSERT(data.count() == COLUMNS_COUNT);
            return data;
        }

        FileBrowserModel::FileBrowserModel(UIContext * context, QObject * parent) :
            QAbstractItemModel(parent),
            _p(new Private(context))
        {
            //DJV_DEBUG("FileBrowserModel::FileBrowserModel");
            //dirUpdate();
            //modelUpdate();
        }

        FileBrowserModel::~FileBrowserModel()
        {
            for (int i = 0; i < _p->items.count(); ++i)
            {
                delete _p->items[i];
                _p->items[i] = 0;
            }
            _p->items.clear();
        }

        const QString & FileBrowserModel::path() const
        {
            return _p->path;
        }

        const Core::FileInfoList & FileBrowserModel::contents() const
        {
            return _p->listTmp;
        }

        Core::FileInfo FileBrowserModel::fileInfo(const QModelIndex & index) const
        {
            //DJV_DEBUG("FileBrowserModel::fileInfo");
            //DJV_DEBUG_PRINT("index = " << index.isValid());
            Core::FileInfo * fileInfo = 0;
            if (index.isValid())
            {
                fileInfo = (Core::FileInfo *)index.internalPointer();
            }
            return fileInfo ? *fileInfo : Core::FileInfo();
        }

        Core::Sequence::COMPRESS FileBrowserModel::sequence() const
        {
            return _p->sequence;
        }

        const QString & FileBrowserModel::filterText() const
        {
            return _p->filterText;
        }

        bool FileBrowserModel::hasShowHidden() const
        {
            return _p->showHidden;
        }

        FileBrowserModel::COLUMNS FileBrowserModel::columnsSort() const
        {
            return _p->columnsSort;
        }

        bool FileBrowserModel::hasReverseSort() const
        {
            return _p->reverseSort;
        }

        bool FileBrowserModel::hasSortDirsFirst() const
        {
            return _p->sortDirsFirst;
        }

        const QStringList & FileBrowserModel::thumbnailsLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::UI::FileBrowserModel", "Off") <<
                qApp->translate("djv::UI::FileBrowserModel", "Low Quality") <<
                qApp->translate("djv::UI::FileBrowserModel", "High Quality");
            DJV_ASSERT(data.count() == THUMBNAILS_COUNT);
            return data;
        }

        FileBrowserModel::THUMBNAILS FileBrowserModel::thumbnails() const
        {
            return _p->thumbnails;
        }

        const QStringList & FileBrowserModel::thumbnailsSizeLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::UI::FileBrowserModel", "Small") <<
                qApp->translate("djv::UI::FileBrowserModel", "Medium") <<
                qApp->translate("djv::UI::FileBrowserModel", "Large");
            DJV_ASSERT(data.count() == THUMBNAILS_COUNT);
            return data;
        }

        int FileBrowserModel::thumbnailsSizeValue(THUMBNAILS_SIZE size)
        {
            static const QVector<int> data = QVector<int>() <<
                100 <<
                200 <<
                300;
            DJV_ASSERT(data.count() == THUMBNAILS_SIZE_COUNT);
            return data[size];
        }

        FileBrowserModel::THUMBNAILS_SIZE FileBrowserModel::thumbnailsSize() const
        {
            return _p->thumbnailsSize;
        }

        QModelIndex	FileBrowserModel::index(
            int                 row,
            int                 column,
            const QModelIndex & parent) const
        {
            if (!hasIndex(row, column, parent))
                return QModelIndex();
            return createIndex(row, column, &_p->listTmp[row]);
        }

        QModelIndex	FileBrowserModel::parent(const QModelIndex & index) const
        {
            return QModelIndex();
        }

        QVariant FileBrowserModel::headerData(
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

        void FileBrowserModel::imageInfoCallback()
        {
            //DJV_DEBUG("FileBrowserModel::imageInfoCallback");
            const int row = _p->items.indexOf(
                qobject_cast<FileBrowserItem *>(sender()));
            if (row != -1)
            {
                const QModelIndex index = this->index(row, 0);
                Q_EMIT dataChanged(index, index);
            }
        }

        void FileBrowserModel::thumbnailCallback()
        {
            //DJV_DEBUG("FileBrowserModel::thumbnailCallback");
            const int row = _p->items.indexOf(
                qobject_cast<FileBrowserItem *>(sender()));
            if (row != -1)
            {
                const QModelIndex index = this->index(row, 0);
                Q_EMIT dataChanged(index, index);
            }
        }

        QVariant FileBrowserModel::data(
            const QModelIndex & index,
            int                 role) const
        {
            if (!index.isValid())
                return QVariant();
            if (role != Qt::DecorationRole &&
                role != Qt::DisplayRole    &&
                role != Qt::EditRole       &&
                role != Qt::SizeHintRole)
                return QVariant();

            const int row = index.row();
            const int column = index.column();
            if (row < 0 || row >= _p->items.count() ||
                column < 0 || column >= COLUMNS_COUNT)
                return QVariant();

            FileBrowserItem * item = _p->items[row];
            const Core::FileInfo & fileInfo = item->fileInfo();
            static const QVector<QPixmap> pixmaps = QVector<QPixmap>() <<
                QPixmap(Core::FileInfo::typeIcons()[Core::FileInfo::FILE]) <<
                QPixmap(Core::FileInfo::typeIcons()[Core::FileInfo::SEQUENCE]) <<
                QPixmap(Core::FileInfo::typeIcons()[Core::FileInfo::DIRECTORY]);
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
                if (NAME == column && !item->thumbnail().isNull())
                {
                    const int margin = _p->context->style()->sizeMetric().margin;
                    return QSize(0, item->thumbnail().height() + margin * 2);
                }
                break;
            default: break;
            }
            return QVariant();
        }

        Qt::ItemFlags FileBrowserModel::flags(const QModelIndex & index) const
        {
            Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);
            if (index.isValid())
                return Qt::ItemIsDragEnabled | defaultFlags;
            else
                return defaultFlags;
        }

        int FileBrowserModel::rowCount(const QModelIndex & parent) const
        {
            return parent.isValid() ? 0 : _p->listTmp.count();
        }

        int FileBrowserModel::columnCount(const QModelIndex & parent) const
        {
            return parent.isValid() ? 0 : COLUMNS_COUNT;
        }

        QStringList FileBrowserModel::mimeTypes() const
        {
            return QStringList() << "application/x-filebrowser";
        }

        QMimeData * FileBrowserModel::mimeData(const QModelIndexList & indexes) const
        {
            QMimeData * mimeData = new QMimeData();
            if (indexes.count())
            {
                const Core::FileInfo fileInfo = this->fileInfo(indexes[0]);
                QStringList tmp;
                tmp << fileInfo;
                QByteArray data;
                QDataStream stream(&data, QIODevice::WriteOnly);
                stream << tmp;
                mimeData->setData("application/x-filebrowser", data);
            }
            return mimeData;
        }

        void FileBrowserModel::setPath(const QString & path)
        {
            if (path == _p->path)
                return;
            _p->path = path;
            dirUpdate();
            modelUpdate();
            Q_EMIT pathChanged(_p->path);
        }

        void FileBrowserModel::reload()
        {
            dirUpdate();
            modelUpdate();
        }

        void FileBrowserModel::setSequence(Core::Sequence::COMPRESS in)
        {
            if (in == _p->sequence)
                return;
            _p->sequence = in;
            dirUpdate();
            modelUpdate();
            Q_EMIT sequenceChanged(_p->sequence);
            Q_EMIT optionChanged();
        }

        void FileBrowserModel::setFilterText(const QString & text)
        {
            if (text == _p->filterText)
                return;
            _p->filterText = text;
            modelUpdate();
            Q_EMIT filterTextChanged(_p->filterText);
            Q_EMIT optionChanged();
        }

        void FileBrowserModel::setShowHidden(bool show)
        {
            if (show == _p->showHidden)
                return;
            _p->showHidden = show;
            modelUpdate();
            Q_EMIT showHiddenChanged(_p->showHidden);
            Q_EMIT optionChanged();
        }

        void FileBrowserModel::setColumnsSort(COLUMNS value)
        {
            if (value == _p->columnsSort)
                return;
            _p->columnsSort = value;
            modelUpdate();
            Q_EMIT columnsSortChanged(_p->columnsSort);
            Q_EMIT optionChanged();
        }

        void FileBrowserModel::setReverseSort(bool value)
        {
            if (value == _p->reverseSort)
                return;
            _p->reverseSort = value;
            modelUpdate();
            Q_EMIT reverseSortChanged(_p->reverseSort);
            Q_EMIT optionChanged();
        }

        void FileBrowserModel::setSortDirsFirst(bool value)
        {
            if (value == _p->sortDirsFirst)
                return;
            _p->sortDirsFirst = value;
            modelUpdate();
            Q_EMIT sortDirsFirstChanged(_p->sortDirsFirst);
            Q_EMIT optionChanged();
        }

        void FileBrowserModel::setThumbnails(THUMBNAILS thumbnails)
        {
            if (thumbnails == _p->thumbnails)
                return;
            _p->thumbnails = thumbnails;
            modelUpdate();
            Q_EMIT thumbnailsChanged(_p->thumbnails);
            Q_EMIT optionChanged();
        }

        void FileBrowserModel::setThumbnailsSize(THUMBNAILS_SIZE size)
        {
            if (size == _p->thumbnailsSize)
                return;
            _p->thumbnailsSize = size;
            modelUpdate();
            Q_EMIT thumbnailsSizeChanged(_p->thumbnailsSize);
            Q_EMIT optionChanged();
        }

        void FileBrowserModel::dirUpdate()
        {
            //DJV_DEBUG("FileBrowserModel::dirUpdate");
            //DJV_DEBUG_PRINT("path = " << _p->path);

            // Get directory contents.
            _p->list = Core::FileInfoUtil::list(_p->path, _p->sequence);

            // Add parent directory.
            if (Core::FileInfo(_p->path).exists())
            {
                _p->list.push_front(Core::FileInfo(_p->path + ".."));
            }
            //DJV_DEBUG_PRINT("list = " << _p->list.count());
            //Q_FOREACH(const Core::FileInfo & fileInfo, _p->list)
            //    DJV_DEBUG_PRINT("fileInfo = " << fileInfo << " " << fileInfo.type());
        }

        void FileBrowserModel::modelUpdate()
        {
            if (_p->path.isEmpty())
                return;

            //DJV_DEBUG("FileBrowserModel::modelUpdate");
            //DJV_DEBUG_PRINT("path = " << _p->path);

            beginResetModel();

            _p->listTmp = _p->list;

            // File sequence directory contents.
            //Core::FileInfoUtil::compressSequence(_p->listTmp, _p->seq);

            // Filter directory contents.
            if (_p->filterText.length() > 0 || !_p->showHidden)
            {
                const Core::FileInfoUtil::FILTER filter =
                    !_p->showHidden ?
                    Core::FileInfoUtil::FILTER_HIDDEN :
                    Core::FileInfoUtil::FILTER_NONE;
                Core::FileInfoUtil::filter(_p->listTmp, filter, _p->filterText);
            }

            // Sort directory contents.
            Core::FileInfoUtil::SORT sort = static_cast<Core::FileInfoUtil::SORT>(0);
            switch (_p->columnsSort)
            {
            case NAME:        sort = Core::FileInfoUtil::SORT_NAME; break;
            case SIZE:        sort = Core::FileInfoUtil::SORT_SIZE; break;
#if ! defined(DJV_WINDOWS)
            case USER:        sort = Core::FileInfoUtil::SORT_USER; break;
#endif
            case PERMISSIONS: sort = Core::FileInfoUtil::SORT_PERMISSIONS; break;
            case TIME:        sort = Core::FileInfoUtil::SORT_TIME; break;
            default: break;
            }
            Core::FileInfoUtil::sort(_p->listTmp, sort, _p->reverseSort);
            if (_p->sortDirsFirst)
            {
                Core::FileInfoUtil::sortDirsFirst(_p->listTmp);
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
                FileBrowserItem * item = new FileBrowserItem(
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

    } // namespace UI

    _DJV_STRING_OPERATOR_LABEL(UI::FileBrowserModel::COLUMNS,
        UI::FileBrowserModel::columnsLabels())
    _DJV_STRING_OPERATOR_LABEL(UI::FileBrowserModel::THUMBNAILS,
        UI::FileBrowserModel::thumbnailsLabels())
    _DJV_STRING_OPERATOR_LABEL(UI::FileBrowserModel::THUMBNAILS_SIZE,
        UI::FileBrowserModel::thumbnailsSizeLabels())

} // namespace djv
