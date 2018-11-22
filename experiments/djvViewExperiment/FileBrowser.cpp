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

#include <FileBrowser.h>

#include <FileBrowserPrivate.h>

#include <Context.h>

#include <QAction>
#include <QFileInfo>
#include <QListView>
#include <QPointer>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>

#include <iostream>

namespace djv
{
    namespace ViewExperiment
    {
        struct FileBrowserWidget::Private
        {

        };

        FileBrowserWidget::FileBrowserWidget(const QPointer<Context> & context, QWidget * parent) :
            QWidget(parent)
        {
            auto header = new FileBrowserHeader(context);

            auto model = new FileBrowserModel(context, this);
            auto proxyModel = new QSortFilterProxyModel(this);
            proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
            proxyModel->setSourceModel(model);
            auto listView = new QListView;
            listView->setDragDropMode(QAbstractItemView::DragOnly);
            listView->setModel(proxyModel);

            auto historyModel = new FileBrowserHistoryModel(model->getHistory());
            auto historyListView = new QListView;
            historyListView->setModel(historyModel);

            auto layout = new QVBoxLayout(this);
            layout->addWidget(header);
            layout->addWidget(listView);
            layout->addWidget(historyListView);

            header->setPath(model->getPath());
            header->setBackEnabled(model->hasBack());
            header->setForwardEnabled(model->hasForward());
            header->setUpEnabled(model->hasUp());

            connect(
                model,
                &FileBrowserModel::pathChanged,
                [header](const QString & value)
            {
                header->setPath(value);
            });
            connect(
                model,
                &FileBrowserModel::backEnabled,
                [header](bool value)
            {
                header->setBackEnabled(value);
            });
            connect(
                model,
                &FileBrowserModel::forwardEnabled,
                [header](bool value)
            {
                header->setForwardEnabled(value);
            });
            connect(
                model,
                &FileBrowserModel::upEnabled,
                [header](bool value)
            {
                header->setUpEnabled(value);
            });

            connect(
                header,
                &FileBrowserHeader::pathChanged,
                [model](const QString & value)
            {
                model->setPath(value);
            });
            connect(
                header,
                &FileBrowserHeader::back,
                [model]
            {
                model->back();
            });
            connect(
                header,
                &FileBrowserHeader::forward,
                [model]
            {
                model->forward();
            });
            connect(
                header,
                &FileBrowserHeader::up,
                [model]
            {
                model->up();
            });
            connect(
                header,
                &FileBrowserHeader::filterChanged,
                [proxyModel](const QString & value)
            {
                proxyModel->setFilterFixedString(value);
            });
        }

        FileBrowserWidget::~FileBrowserWidget()
        {}

    } // namespace ViewExperiment
} // namespace djv
