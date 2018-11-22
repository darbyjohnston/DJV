//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
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

#include <FileBrowserPrivate.h>

#include <Context.h>
#include <FileUtil.h>

#include <QDir>
#include <QFileInfo>
#include <QMimeData>
#include <QPointer>

#include <iostream>
#include <vector>

namespace djv
{
    namespace ViewExperiment
    {
        struct FileBrowserHistory::Private
        {
            QString path;
            std::vector<QString> history;
            int historyIndex = -1;
        };

        FileBrowserHistory::FileBrowserHistory(QObject * parent) :
            QObject(parent),
            _p(new Private)
        {}

        FileBrowserHistory::~FileBrowserHistory()
        {}

        const std::vector<QString> & FileBrowserHistory::getHistory() const
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

        void FileBrowserHistory::setPath(const QString & value)
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
                SIGNAL(pathChanged(const QString &)),
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
            if (Qt::DisplayRole == role)
            {
                return _p->items[index.row()];
            }
            else if (Qt::DecorationRole == role)
            {
                if (index.row() == _p->history->getHistoryIndex())
                {
                    return QColor(255, 0, 0);
                }
            }
            return QVariant();
        }

        void FileBrowserHistoryModel::_updateModel()
        {
            beginResetModel();
            _p->items = _p->history->getHistory();
            endResetModel();
        }

        struct FileBrowserModel::Private
        {
            QPointer<Context> context;
            QDir dir;
            QFileInfoList fileInfoList;
            QScopedPointer<FileBrowserHistory> history;
        };

        FileBrowserModel::FileBrowserModel(const QPointer<Context> & context, QObject * parent) :
            QAbstractListModel(parent),
            _p(new Private)
        {
            _p->context = context;
            _p->dir.setPath(QFileInfo(QDir().path()).absolutePath());
            _p->history.reset(new FileBrowserHistory);
            _p->history->setPath(_p->dir.path());
            _updateModel();
            connect(
                _p->history.data(),
                SIGNAL(pathChanged(const QString &)),
                SLOT(setPath(const QString &)));
            connect(
                _p->history.data(),
                SIGNAL(backEnabled(bool)),
                SIGNAL(backEnabled(bool)));
            connect(
                _p->history.data(),
                SIGNAL(forwardEnabled(bool)),
                SIGNAL(forwardEnabled(bool)));
        }

        FileBrowserModel::~FileBrowserModel()
        {}

        QString FileBrowserModel::getPath() const
        {
            return _p->dir.path();
        }

        bool FileBrowserModel::hasBack() const
        {
            return _p->history->hasBack();
        }

        bool FileBrowserModel::hasForward() const
        {
            return _p->history->hasForward();
        }

        bool FileBrowserModel::hasUp() const
        {
            return !_p->dir.isRoot();
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
            if (Qt::DisplayRole == role)
            {
                return _p->fileInfoList[index.row()].fileName();
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
                out->setText(_p->fileInfoList[i.row()].filePath());
            }
            return out;
        }

        void FileBrowserModel::setPath(const QString & value)
        {
            const auto tmp = QFileInfo(value).absoluteFilePath();
            if (tmp == _p->dir.path())
                return;
            _p->dir.setPath(tmp);
            _p->history->setPath(_p->dir.path());
            _updateModel();
            Q_EMIT pathChanged(_p->dir.path());
            Q_EMIT upEnabled(hasUp());
        }

        void FileBrowserModel::back()
        {
            _p->history->back();
        }

        void FileBrowserModel::forward()
        {
            _p->history->forward();
        }

        void FileBrowserModel::up()
        {
            if (_p->dir.cdUp())
            {
                _p->history->setPath(_p->dir.path());
                _updateModel();
                Q_EMIT pathChanged(_p->dir.path());
                Q_EMIT upEnabled(hasUp());
            }
        }

        void FileBrowserModel::_updateModel()
        {
            beginResetModel();
            _p->fileInfoList = _p->dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
            endResetModel();
        }

    } // namespace ViewExperiment
} // namespace djv
