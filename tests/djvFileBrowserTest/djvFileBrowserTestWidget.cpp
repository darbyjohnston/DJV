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

//! \file djvFileBrowserTestWidget.cpp

#include <djvFileBrowserTestWidget.h>

#include <djvFileBrowserTestModel.h>
#include <djvFileBrowserTestIconDelegate.h>

#include <djvSpinner.h>

#include <QAction>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QStackedWidget>
#include <QToolButton>
#include <QTreeView>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvFileBrowserTestWidgetPrivate
//------------------------------------------------------------------------------

struct djvFileBrowserTestWidgetPrivate
{
    djvFileBrowserTestWidgetPrivate(djvGuiContext * context) :
        context            (context),
        model              (new djvFileBrowserTestModel(context)),
        upAction           (0),
        backAction         (0),
        reloadAction       (0),
        sequenceWidget     (0),
        thumbnailsWidget   (0),
        thumbnailSizeWidget(0),
        viewWidget         (0),
        filterWidget       (0),
        spinner            (0),
        pathWidget         (0),
        treeView           (0),
        listView           (0),
        delegate           (0),
        stackedWidget      (0)
    {}
    
    djvGuiContext *                         context;
    QScopedPointer<djvFileBrowserTestModel> model;
    QAction *                               upAction;
    QAction *                               backAction;
    QAction *                               reloadAction;
    QComboBox *                             sequenceWidget;
    QComboBox *                             thumbnailsWidget;
    QComboBox *                             thumbnailSizeWidget;
    QComboBox *                             viewWidget;
    QLineEdit *                             filterWidget;
    djvSpinner *                            spinner;
    QLineEdit *                             pathWidget;
    QTreeView *                             treeView;
    QListView *                             listView;
    djvFileBrowserTestIconDelegate *        delegate;
    QStackedWidget *                        stackedWidget;
};

//------------------------------------------------------------------------------
// djvFileBrowserTestWidget
//------------------------------------------------------------------------------

djvFileBrowserTestWidget::djvFileBrowserTestWidget(
    djvGuiContext * context,
    const QString & path,
    QWidget *       parent) :
    QWidget(parent),
    _p(new djvFileBrowserTestWidgetPrivate(context))
{
    _p->upAction = new QAction(this);
    _p->upAction->setText("Up");
    
    _p->backAction = new QAction(this);
    _p->backAction->setText("Back");
    
    _p->reloadAction = new QAction(this);
    _p->reloadAction->setText("Reload");
    
    QToolButton * upButton = new QToolButton;
    upButton->setAutoRaise(true);
    upButton->setDefaultAction(_p->upAction);
    
    QToolButton * backButton = new QToolButton;
    backButton->setAutoRaise(true);
    backButton->setDefaultAction(_p->backAction);
    
    QToolButton * reloadButton = new QToolButton;
    reloadButton->setAutoRaise(true);
    reloadButton->setDefaultAction(_p->reloadAction);
    
    _p->sequenceWidget = new QComboBox;
    _p->sequenceWidget->addItems(djvSequence::compressLabels());
    
    _p->thumbnailsWidget = new QComboBox;
    _p->thumbnailsWidget->addItems(djvFileBrowserTestUtil::thumbnailsLabels());
    
    _p->thumbnailSizeWidget = new QComboBox;
    _p->thumbnailSizeWidget->addItems(djvFileBrowserTestUtil::thumbnailSizeLabels());
    
    _p->viewWidget = new QComboBox;
    _p->viewWidget->addItems(djvFileBrowserTestUtil::viewLabels());
    
    _p->filterWidget = new QLineEdit;
    
    _p->spinner = new djvSpinner(context);
    
    _p->pathWidget = new QLineEdit;
    
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
    
    _p->delegate = new djvFileBrowserTestIconDelegate(context, this);

    _p->stackedWidget = new QStackedWidget;
    _p->stackedWidget->addWidget(_p->treeView);
    _p->stackedWidget->addWidget(_p->listView);
    
    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    
    QHBoxLayout * hLayout = new QHBoxLayout;
    hLayout->setMargin(5);
    hLayout->setSpacing(5);
    
    QHBoxLayout * hLayout2 = new QHBoxLayout;
    hLayout2->setMargin(0);
    hLayout2->setSpacing(0);
    hLayout2->addWidget(upButton);
    hLayout2->addWidget(backButton);
    hLayout2->addWidget(reloadButton);
    hLayout->addLayout(hLayout2);
    
    hLayout->addWidget(_p->sequenceWidget);
    hLayout->addWidget(_p->thumbnailsWidget);
    hLayout->addWidget(_p->thumbnailSizeWidget);
    hLayout->addWidget(_p->viewWidget);
    hLayout->addStretch();
    hLayout->addWidget(_p->filterWidget);
    hLayout->addWidget(_p->spinner);
    layout->addLayout(hLayout);
    
    layout->addWidget(_p->stackedWidget);

    hLayout = new QHBoxLayout;
    hLayout->setMargin(5);
    hLayout->addWidget(_p->pathWidget);
    layout->addLayout(hLayout);
    
    _p->sequenceWidget->setCurrentIndex(_p->model->sequence());
    _p->thumbnailsWidget->setCurrentIndex(_p->model->thumbnails());
    _p->thumbnailSizeWidget->setCurrentIndex(_p->model->thumbnailSize());
    
    _p->treeView->setModel(_p->model.data());
    _p->delegate->setThumbnailSize(_p->model->thumbnailSize());
    _p->listView->setItemDelegate(_p->delegate);
    _p->listView->setModel(_p->model.data());
    
    _p->model->connect(
        _p->upAction,
        SIGNAL(triggered()),
        SLOT(up()));
    
    _p->model->connect(
        _p->backAction,
        SIGNAL(triggered()),
        SLOT(back()));
    
    _p->model->connect(
        _p->reloadAction,
        SIGNAL(triggered()),
        SLOT(reload()));
    
    _p->model->connect(
        _p->filterWidget,
        SIGNAL(textChanged(const QString &)),
        SLOT(setFilterText(const QString &)));

    _p->spinner->connect(
        _p->model.data(),
        SIGNAL(requestDir(const djvFileBrowserTestDirRequest &)),
        SLOT(start()));
    
    _p->spinner->connect(
        _p->model.data(),
        SIGNAL(requestDirComplete()),
        SLOT(stop()));
    
    _p->pathWidget->connect(
        _p->model.data(),
        SIGNAL(pathChanged(const QString &)),
        SLOT(setText(const QString &)));

    connect(
        _p->sequenceWidget,
        SIGNAL(activated(int)),
        SLOT(sequenceCallback(int)));

    connect(
        _p->thumbnailsWidget,
        SIGNAL(activated(int)),
        SLOT(thumbnailsCallback(int)));

    connect(
        _p->thumbnailSizeWidget,
        SIGNAL(activated(int)),
        SLOT(thumbnailSizeCallback(int)));

    connect(
        _p->viewWidget,
        SIGNAL(activated(int)),
        SLOT(viewCallback(int)));

    _p->delegate->connect(
        _p->model.data(),
        SIGNAL(thumbnailSizeChanged(djvFileBrowserTestUtil::THUMBNAIL_SIZE)),
        SLOT(setThumbnailSize(djvFileBrowserTestUtil::THUMBNAIL_SIZE)));

    _p->model->setPath(path);
}

djvFileBrowserTestWidget::~djvFileBrowserTestWidget()
{
    _p->treeView->setModel(0);
    _p->listView->setModel(0);
    
    delete _p;
}

void djvFileBrowserTestWidget::sequenceCallback(int index)
{
    _p->model->setSequence(static_cast<djvSequence::COMPRESS>(index));
}

void djvFileBrowserTestWidget::thumbnailsCallback(int index)
{
    _p->model->setThumbnails(static_cast<djvFileBrowserTestUtil::THUMBNAILS>(index));
}

void djvFileBrowserTestWidget::thumbnailSizeCallback(int index)
{
    _p->model->setThumbnailSize(static_cast<djvFileBrowserTestUtil::THUMBNAIL_SIZE>(index));
}

void djvFileBrowserTestWidget::viewCallback(int index)
{
    _p->stackedWidget->setCurrentIndex(index);
}


