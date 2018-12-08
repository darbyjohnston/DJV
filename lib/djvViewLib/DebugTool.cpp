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

#include <djvViewLib/DebugTool.h>

#include <djvViewLib/Context.h>
#include <djvViewLib/Media.h>

#include <QAction>
#include <QDockWidget>
#include <QLabel>
#include <QMenu>
#include <QTableView>

namespace djv
{
    namespace ViewLib
    {
        struct DebugModel::Private
        {
            std::shared_ptr<Media> media;
            std::vector<std::pair<QString, qint64> > data;
            std::shared_ptr<Core::ValueObserver<AV::Duration> > durationObserver;
            std::shared_ptr<Core::ValueObserver<AV::Timestamp> > currentTimeObserver;
            std::shared_ptr<Core::ValueObserver<size_t> > alUnqueuedBuffersObserver;
        };

        DebugModel::DebugModel(QObject * parent) :
            _p(new Private)
        {}

        DebugModel::~DebugModel()
        {}

        void DebugModel::setMedia(const std::shared_ptr<Media>& media)
        {
            DJV_PRIVATE_PTR();
            p.media = media;
            if (p.media)
            {
                p.durationObserver = Core::ValueObserver<AV::Duration>::create(
                    p.media->getDuration(),
                    [this](AV::Duration)
                {
                    _updateModel();
                });
                p.currentTimeObserver = Core::ValueObserver<AV::Timestamp>::create(
                    p.media->getCurrentTime(),
                    [this](AV::Timestamp)
                {
                    _updateModel();
                });
                p.alUnqueuedBuffersObserver = Core::ValueObserver<size_t>::create(
                    p.media->getALUnqueuedBuffers(),
                    [this](size_t)
                {
                    _updateModel();
                });
            }
            else
            {
                p.durationObserver = nullptr;
                p.currentTimeObserver = nullptr;
                p.alUnqueuedBuffersObserver = nullptr;
            }
            _updateModel();
        }

        int DebugModel::columnCount(const QModelIndex & parent) const
        {
            return 2;
        }

        int DebugModel::rowCount(const QModelIndex & parent ) const
        {
            return static_cast<int>(_p->data.size());
        }

        QVariant DebugModel::data(const QModelIndex & index, int role) const
        {
            if (!index.isValid())
                return QVariant();
            switch (role)
            {
            case Qt::DisplayRole:
                if (0 == index.column())
                {
                    return _p->data[index.row()].first;
                }
                else if (1 == index.column())
                {
                    return _p->data[index.row()].second;
                }
            default: break;
            }
            return QVariant();
        }

        void DebugModel::_updateModel()
        {
            DJV_PRIVATE_PTR();
            beginResetModel();
            p.data.clear();
            if (p.media)
            {
                p.data.push_back(std::make_pair(QString("Duration"), p.media->getDuration()->get()));
                p.data.push_back(std::make_pair(QString("Current time"), p.media->getCurrentTime()->get()));
                p.data.push_back(std::make_pair(QString("Video queue max"), p.media->getVideoQueueMax()->get()));
                p.data.push_back(std::make_pair(QString("Audio queue max"), p.media->getAudioQueueMax()->get()));
                p.data.push_back(std::make_pair(QString("Video queue count"), p.media->getVideoQueueCount()->get()));
                p.data.push_back(std::make_pair(QString("Audio queue count"), p.media->getAudioQueueCount()->get()));
                p.data.push_back(std::make_pair(QString("OpenAL unqueued buffers"), p.media->getALUnqueuedBuffers()->get()));
            }
            endResetModel();
        }

        struct DebugTool::Private
        {
            QScopedPointer<DebugModel> model;
        };

        DebugTool::DebugTool(const std::shared_ptr<Context> & context, QObject * parent) :
            IToolObject("DebugTool", context, parent),
            _p(new Private)
        {
            _p->model.reset(new DebugModel);
        }

        DebugTool::~DebugTool()
        {}

        QPointer<QDockWidget> DebugTool::createDockWidget()
        {
            auto out = new QDockWidget("Debugging");
            auto tableView = new QTableView;
            tableView->setModel(_p->model.data());
            out->setWidget(tableView);
            out->hide();
            return out;
        }

        std::string DebugTool::getDockWidgetSortKey() const
        {
            return "4";
        }

        Qt::DockWidgetArea DebugTool::getDockWidgetArea() const
        {
            return Qt::DockWidgetArea::RightDockWidgetArea;
        }

        void DebugTool::setCurrentMedia(const std::shared_ptr<Media> & value)
        {
            _p->model->setMedia(value);
        }

    } // namespace ViewLib
} // namespace djv

