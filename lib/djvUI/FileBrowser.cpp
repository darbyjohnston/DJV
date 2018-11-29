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

#include <djvUI/FileBrowser.h>

#include <djvUI/FileBrowserPrivate.h>

#include <djvUI/Context.h>

#include <QAction>
#include <QFileInfo>
#include <QListView>
#include <QPointer>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>

#include <iostream>

namespace djv
{
    namespace UI
    {
        struct FileBrowserWidget::Private
        {
            QPointer<FileBrowserModel> model;
            QPointer<QSortFilterProxyModel> proxyModel;
            QListView::ViewMode viewMode = QListView::ListMode;
            QPointer<QListView> listView;
            QPointer<FileBrowserHeader> header;
            QPointer<FileBrowserFooter> footer;
        };

        FileBrowserWidget::FileBrowserWidget(const QPointer<Context> & context, QWidget * parent) :
            QWidget(parent),
            _p(new Private)
        {
            _p->model = new FileBrowserModel(context, this);
            _p->proxyModel = new QSortFilterProxyModel(this);
            _p->proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
            _p->proxyModel->setSourceModel(_p->model);

            _p->listView = new QListView;
            _p->listView->setDragDropMode(QAbstractItemView::DragOnly);
            _p->listView->setModel(_p->proxyModel);

            _p->header = new FileBrowserHeader(context);
            _p->footer = new FileBrowserFooter(context);

            auto layout = new QVBoxLayout(this);
            layout->addWidget(_p->header);
            layout->addWidget(_p->listView);
            layout->addWidget(_p->footer);

            _updateWidget();

            connect(
                _p->model,
                &FileBrowserModel::pathChanged,
                [this](const QString & value)
            {
                _p->header->setPath(value);
            });
            connect(
                _p->model,
                &FileBrowserModel::upEnabled,
                [this](bool value)
            {
                _p->header->setUpEnabled(value);
            });
            connect(
                _p->model,
                &FileBrowserModel::backEnabled,
                [this](bool value)
            {
                _p->header->setBackEnabled(value);
            });
            connect(
                _p->model,
                &FileBrowserModel::forwardEnabled,
                [this](bool value)
            {
                _p->header->setForwardEnabled(value);
            });

            connect(
                _p->listView,
                &QAbstractItemView::doubleClicked,
                [this](const QModelIndex & value)
            {
                _p->model->setPath(value.data(Qt::EditRole).toString());
            });

            connect(
                _p->header,
                &FileBrowserHeader::pathChanged,
                [this](const QString & value)
            {
                _p->model->setPath(value);
            });
            connect(
                _p->header,
                &FileBrowserHeader::up,
                [this]
            {
                _p->model->up();
            });
            connect(
                _p->header,
                &FileBrowserHeader::back,
                [this]
            {
                _p->model->back();
            });
            connect(
                _p->header,
                &FileBrowserHeader::forward,
                [this]
            {
                _p->model->forward();
            });

            connect(
                _p->footer,
                &FileBrowserFooter::filterChanged,
                [this](const QString & value)
            {
                _p->proxyModel->setFilterFixedString(value);
            });
            connect(
                _p->footer,
                &FileBrowserFooter::viewModeChanged,
                [this](QListView::ViewMode value)
            {
                _p->viewMode = value;
                _updateWidget();
            });
        }

        FileBrowserWidget::~FileBrowserWidget()
        {}

        void FileBrowserWidget::_updateWidget()
        {
            switch (_p->viewMode)
            {
            case QListView::ListMode:
                _p->listView->setGridSize(QSize());
                _p->listView->setResizeMode(QListView::Fixed);
                break;
            case QListView::IconMode:
                _p->listView->setGridSize(QSize(100, 100));
                _p->listView->setResizeMode(QListView::Adjust);
                break;
            }
            _p->listView->setViewMode(_p->viewMode);
            _p->header->setPath(_p->model->getPath());
            _p->header->setUpEnabled(_p->model->hasUp());
            _p->header->setBackEnabled(_p->model->hasBack());
            _p->header->setForwardEnabled(_p->model->hasForward());
            _p->footer->setViewMode(_p->listView->viewMode());
        }

    } // namespace UI
} // namespace djv
