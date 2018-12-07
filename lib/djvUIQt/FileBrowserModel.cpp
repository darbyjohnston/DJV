//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
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

#include <djvUIQt/FileBrowserPrivate.h>

#include <djvUIQt/System.h>

#include <djvCore/Context.h>
#include <djvCore/DirectoryModel.h>
#include <djvCore/FileInfo.h>

#include <QMimeData>
#include <QPointer>
#include <QStyle>

namespace djv
{
    namespace UIQt
    {
        struct FileBrowserHistory::Private
        {
            std::string path;
            std::vector<std::string> history;
            int historyIndex = -1;
        };

        FileBrowserHistory::FileBrowserHistory(QObject * parent) :
            QObject(parent),
            _p(new Private)
        {}

        FileBrowserHistory::~FileBrowserHistory()
        {}

        const std::vector<std::string> & FileBrowserHistory::getHistory() const
        {
            return _p->history;
        }

        size_t FileBrowserHistory::getHistorySize() const
        {
            return _p->history.size();
        }

        int FileBrowserHistory::getHistoryIndex() const
        {
            return _p->historyIndex;
        }

        bool FileBrowserHistory::hasBack() const
        {
            return _p->historyIndex > 0;
        }

        bool FileBrowserHistory::hasForward() const
        {
            return _p->historyIndex < static_cast<int>(_p->history.size()) - 1;
        }

        void FileBrowserHistory::setPath(const std::string & value)
        {
            if (value == _p->path)
                return;
            _p->path = value;
            while (_p->history.size() > 0 && static_cast<int>(_p->history.size()) - 1 > _p->historyIndex)
            {
                _p->history.pop_back();
            }
            _p->history.push_back(_p->path);
            ++_p->historyIndex;
            Q_EMIT pathChanged(_p->path);
            Q_EMIT backEnabled(hasBack());
            Q_EMIT forwardEnabled(hasForward());
        }

        void FileBrowserHistory::back()
        {
            if (_p->historyIndex > 0)
            {
                --_p->historyIndex;
                _p->path = _p->history[_p->historyIndex];
                Q_EMIT pathChanged(_p->path);
                Q_EMIT backEnabled(hasBack());
                Q_EMIT forwardEnabled(hasForward());
            }
        }

        void FileBrowserHistory::forward()
        {
            if (_p->historyIndex < static_cast<int>(_p->history.size()) - 1)
            {
                ++_p->historyIndex;
                _p->path = _p->history[_p->historyIndex];
                Q_EMIT pathChanged(_p->path);
                Q_EMIT backEnabled(hasBack());
                Q_EMIT forwardEnabled(hasForward());
            }
        }

        struct FileBrowserHistoryModel::Private
        {
            QPointer<FileBrowserHistory> history;
            std::vector<QString> items;
        };

        FileBrowserHistoryModel::FileBrowserHistoryModel(const QPointer<FileBrowserHistory> & history) :
            _p(new Private)
        {
            _p->history = history;
            _updateModel();
            connect(
                _p->history,
                SIGNAL(pathChanged(const std::string &)),
                SLOT(_updateModel()));
            connect(
                _p->history,
                SIGNAL(backEnabled(bool)),
                SLOT(_updateModel()));
            connect(
                _p->history,
                SIGNAL(forwardEnabled(bool)),
                SLOT(_updateModel()));
        }

        FileBrowserHistoryModel::~FileBrowserHistoryModel()
        {}

        int FileBrowserHistoryModel::rowCount(const QModelIndex & parent) const
        {
            return static_cast<int>(_p->items.size());
        }

        QVariant FileBrowserHistoryModel::data(const QModelIndex & index, int role) const
        {
            if (!index.isValid())
                return QVariant();
            switch (role)
            {
            case Qt::DisplayRole:
                return _p->items[index.row()];
            case Qt::DecorationRole:
                if (index.row() == _p->history->getHistoryIndex())
                {
                    return QColor(255, 0, 0);
                }
                break;
            default: break;
            }
            return QVariant();
        }

        void FileBrowserHistoryModel::_updateModel()
        {
            beginResetModel();
            _p->items.clear();
            for (const auto & i : _p->history->getHistory())
            {
                _p->items.push_back(QString::fromStdString(i));
            }
            endResetModel();
        }

        struct FileBrowserModel::Private
        {
            std::weak_ptr<Core::Context> context;
            std::shared_ptr<Core::DirectoryModel> directoryModel;
            std::vector<Core::FileInfo> fileInfoList;
            std::shared_ptr<Core::ListObserver<Core::FileInfo> > fileInfoObserver;
            QScopedPointer<FileBrowserHistory> history;
        };

        FileBrowserModel::FileBrowserModel(const std::shared_ptr<Core::Context> & context, QObject * parent) :
            QAbstractListModel(parent),
            _p(new Private)
        {
            _p->context = context;
            _p->directoryModel = Core::DirectoryModel::create(context);
            _p->history.reset(new FileBrowserHistory);
            _p->history->setPath(_p->directoryModel->getPath()->get());
            connect(
                _p->history.data(),
                SIGNAL(pathChanged(const std::string &)),
                SLOT(setPath(const std::string &)));
            connect(
                _p->history.data(),
                SIGNAL(backEnabled(bool)),
                SIGNAL(backEnabled(bool)));
            connect(
                _p->history.data(),
                SIGNAL(forwardEnabled(bool)),
                SIGNAL(forwardEnabled(bool)));
            _p->fileInfoObserver = Core::ListObserver<Core::FileInfo>::create(
                _p->directoryModel->getFileInfoList(),
                [this](const std::vector<Core::FileInfo> & list)
            {
                beginResetModel();
                _p->fileInfoList = list;
                endResetModel();
            });
        }

        FileBrowserModel::~FileBrowserModel()
        {}

        std::string FileBrowserModel::getPath() const
        {
            return _p->directoryModel->getPath()->get();
        }

        bool FileBrowserModel::hasUp() const
        {
            return !_p->directoryModel->getPath()->get().isRoot();
        }

        bool FileBrowserModel::hasBack() const
        {
            return _p->history->hasBack();
        }

        bool FileBrowserModel::hasForward() const
        {
            return _p->history->hasForward();
        }

        QPointer<FileBrowserHistory> FileBrowserModel::getHistory() const
        {
            return _p->history.data();
        }

        int FileBrowserModel::rowCount(const QModelIndex & parent) const
        {
            return static_cast<int>(_p->fileInfoList.size());
        }

        QVariant FileBrowserModel::data(const QModelIndex & index, int role) const
        {
            if (!index.isValid())
                return QVariant();
            switch (role)
            {
            case Qt::DisplayRole:
                return QString::fromStdString(_p->fileInfoList[index.row()].getFileName(Core::Frame::Invalid, false));
            case Qt::DecorationRole:
                if (auto context = _p->context.lock())
                {
                    if (auto system = context->getSystemT<System>())
                    {
                        const auto & fileInfo = _p->fileInfoList[index.row()];
                        switch (fileInfo.getType())
                        {
                        case Core::FileType::File: return system->getQStyle()->standardIcon(QStyle::SP_FileIcon);
                        case Core::FileType::Directory:  return system->getQStyle()->standardIcon(QStyle::SP_DirIcon);
                        default: break;
                        }
                    }
                }
                break;
            case Qt::EditRole:
                return QString::fromStdString(_p->fileInfoList[index.row()].getFileName());
            default: break;
            }
            return QVariant();
        }

        Qt::ItemFlags FileBrowserModel::flags(const QModelIndex & index) const
        {
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
        }

        QStringList FileBrowserModel::mimeTypes() const
        {
            return QStringList() << "text/plain";
        }

        QMimeData * FileBrowserModel::mimeData(const QModelIndexList & index) const
        {
            auto out = new QMimeData;
            Q_FOREACH(const auto i, index)
            {
                out->setText(QString::fromStdString(_p->fileInfoList[i.row()].getFileName()));
            }
            return out;
        }

        void FileBrowserModel::setPath(const std::string & value)
        {
            try
            {
                const auto path = Core::Path::getAbsolute(value);
                if (path == _p->directoryModel->getPath()->get())
                    return;
                _p->directoryModel->setPath(path);
                _p->history->setPath(_p->directoryModel->getPath()->get());
                Q_EMIT pathChanged(_p->directoryModel->getPath()->get());
                Q_EMIT upEnabled(hasUp());
            }
            catch (const std::exception &)
            {
                //! \todo Error handling.
            }
        }

        void FileBrowserModel::up()
        {
            auto path = _p->directoryModel->getPath()->get();
            if (path.cdUp())
            {
                _p->directoryModel->setPath(path);
                _p->history->setPath(_p->directoryModel->getPath()->get());
                Q_EMIT pathChanged(_p->directoryModel->getPath()->get());
                Q_EMIT upEnabled(hasUp());
            }
        }

        void FileBrowserModel::back()
        {
            _p->history->back();
        }

        void FileBrowserModel::forward()
        {
            _p->history->forward();
        }

    } // namespace UIQt
} // namespace djv
