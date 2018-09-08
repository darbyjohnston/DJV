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

#include <djvFileBrowserTestView.h>

#include <djvFileBrowserTestContactSheetDelegate.h>
#include <djvFileBrowserTestModel.h>

#include <djvSpinner.h>
#include <djvUIContext.h>

#include <QApplication>
#include <QListView>
#include <QStackedWidget>
#include <QTreeView>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvFileBrowserTestView::Private
//------------------------------------------------------------------------------

struct djvFileBrowserTestView::Private
{
    Private(
        djvFileBrowserTestModel * model,
        djvUIContext *            context) :
        context      (context),
        model        (model),
        mode         (static_cast<djvFileBrowserTestView::MODE>(0)),
        treeView     (0),
        listView     (0),
        delegate     (0),
        spinner      (0),
        stackedWidget(0)
    {}
    
    djvUIContext *                           context = nullptr;
    djvFileBrowserTestModel *                model;
    djvFileBrowserTestView::MODE             mode;
    QTreeView *                              treeView;
    QListView *                              listView;
    djvFileBrowserTestContactSheetDelegate * delegate;
    djvSpinner *                             spinner;
    QStackedWidget *                         stackedWidget;
};

//------------------------------------------------------------------------------
// djvFileBrowserTestView
//------------------------------------------------------------------------------

const djvFileBrowserTestActionDataList & djvFileBrowserTestView::modeData()
{
    static const djvFileBrowserTestActionDataList data =
        djvFileBrowserTestActionDataList() <<
        djvFileBrowserTestActionData(
            qApp->translate("djvFileBrowserTestView", "Details"),
            qApp->translate("djvFileBrowserTestView", "Show the details view"),
            ":djvDetailsViewIcon.png") <<
        djvFileBrowserTestActionData(
            qApp->translate("djvFileBrowserTestView", "Contact Sheet"),
            qApp->translate("djvFileBrowserTestView", "Show the contact sheet view"),
            ":djvContactSheetViewIcon.png");
    DJV_ASSERT(data.count() == MODE_COUNT);
    return data;
}

djvFileBrowserTestView::djvFileBrowserTestView(
    djvFileBrowserTestModel * model,
    djvUIContext *            context,
    QWidget *                 parent) :
    QWidget(parent),
    _p(new Private(model, context))
{
    _p->treeView = new QTreeView;
    _p->treeView->setRootIsDecorated(false);
    _p->treeView->setItemsExpandable(false);
    _p->treeView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    _p->treeView->setAlternatingRowColors(true);
    
    _p->listView = new QListView;
    _p->listView->setViewMode(QListView::IconMode);
    _p->listView->setResizeMode(QListView::Adjust);
    _p->listView->setSpacing(2);
    _p->listView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    
    _p->delegate = new djvFileBrowserTestContactSheetDelegate(context, this);

    _p->spinner = new djvSpinner(context, this);
    
    _p->stackedWidget = new QStackedWidget;
    _p->stackedWidget->addWidget(_p->treeView);
    _p->stackedWidget->addWidget(_p->listView);
    
    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(_p->stackedWidget);

    _p->treeView->setModel(_p->model);

    _p->listView->setItemDelegate(_p->delegate);
    _p->listView->setModel(_p->model);

    _p->delegate->setThumbnailSize(_p->model->thumbnailSize());
    
    updateWidget();

    _p->delegate->connect(
        _p->model,
        SIGNAL(thumbnailSizeChanged(int)),
        SLOT(setThumbnailSize(int)));
    _p->spinner->connect(
        _p->model,
        SIGNAL(requestDirComplete()),
        SLOT(stop()));
    connect(
        _p->model,
        SIGNAL(requestDir(const djvFileBrowserTestDirRequest &)),
        SLOT(startCallback()));
    connect(
        _p->model,
        SIGNAL(thumbnailSizeChanged(int)),
        SLOT(thumbnailSizeCallback(int)));
}

djvFileBrowserTestView::~djvFileBrowserTestView()
{}

djvFileBrowserTestView::MODE djvFileBrowserTestView::mode() const
{
    return _p->mode;
}
    
QSize djvFileBrowserTestView::sizeHint() const
{
    return QSize(700, 500);
}
    
void djvFileBrowserTestView::setMode(MODE mode)
{
    if (mode == _p->mode)
        return;
    _p->mode = mode;
    updateWidget();
    Q_EMIT modeChanged(_p->mode);
}

void djvFileBrowserTestView::showEvent(QShowEvent *)
{
    _p->spinner->raise();
    _p->spinner->show();
}

void djvFileBrowserTestView::hideEvent(QHideEvent *)
{
    _p->spinner->hide();
}

void djvFileBrowserTestView::resizeEvent(QResizeEvent *)
{
    _p->spinner->resize(width(), height());
}

void djvFileBrowserTestView::startCallback()
{
    _p->spinner->startDelayed(500);
}

void djvFileBrowserTestView::thumbnailSizeCallback(int)
{
    _p->listView->reset();
}

void djvFileBrowserTestView::updateWidget()
{
    _p->stackedWidget->setCurrentIndex(_p->mode);
}

