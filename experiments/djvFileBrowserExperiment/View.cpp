//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
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

#include <View.h>

#include <ContactSheetDelegate.h>
#include <Model.h>

#include <djvSpinner.h>
#include <djvUIContext.h>

#include <QApplication>
#include <QListView>
#include <QStackedWidget>
#include <QTreeView>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// View::Private
//------------------------------------------------------------------------------

struct View::Private
{
    Private(Model * model, djvUIContext * context) :
        context(context),
        model(model),
    {}
    
    djvUIContext * context = nullptr;
    Model * model = nullptr;
    View::MODE mode = static_cast<View::MODE>(0);
    QTreeView * treeView = nullptr;
    QListView * listView = nullptr;
    ContactSheetDelegate * delegate = nullptr;
    djvSpinner * spinner = nullptr;
    QStackedWidget * stackedWidget = nullptr;
};

//------------------------------------------------------------------------------
// View
//------------------------------------------------------------------------------

const ActionDataList & View::modeData()
{
    static const ActionDataList data =
        ActionDataList() <<
        ActionData(
            qApp->translate("djvFileBrowserExperiment", "Details"),
            qApp->translate("djvFileBrowserExperiment", "Show the details view"),
            ":djvDetailsViewIcon.png") <<
        ActionData(
            qApp->translate("djvFileBrowserExperiment", "Contact Sheet"),
            qApp->translate("djvFileBrowserExperiment", "Show the contact sheet view"),
            ":djvContactSheetViewIcon.png");
    DJV_ASSERT(data.count() == MODE_COUNT);
    return data;
}

View::View(
    Model * model,
    djvUIContext * context,
    QWidget * parent) :
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
    
    _p->delegate = new ContactSheetDelegate(context, this);

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
        SIGNAL(requestDir(const DirRequest &)),
        SLOT(startCallback()));
    connect(
        _p->model,
        SIGNAL(thumbnailSizeChanged(int)),
        SLOT(thumbnailSizeCallback(int)));
}

View::~View()
{}

View::MODE View::mode() const
{
    return _p->mode;
}
    
QSize View::sizeHint() const
{
    return QSize(700, 500);
}
    
void View::setMode(MODE mode)
{
    if (mode == _p->mode)
        return;
    _p->mode = mode;
    updateWidget();
    Q_EMIT modeChanged(_p->mode);
}

void View::showEvent(QShowEvent *)
{
    _p->spinner->raise();
    _p->spinner->show();
}

void View::hideEvent(QHideEvent *)
{
    _p->spinner->hide();
}

void View::resizeEvent(QResizeEvent *)
{
    _p->spinner->resize(width(), height());
}

void View::startCallback()
{
    _p->spinner->startDelayed(500);
}

void View::thumbnailSizeCallback(int)
{
    _p->listView->reset();
}

void View::updateWidget()
{
    _p->stackedWidget->setCurrentIndex(_p->mode);
}

