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

#include <djvSpinner.h>

#include <QAction>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QTreeView>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvFileBrowserTestWidget
//------------------------------------------------------------------------------

djvFileBrowserTestWidget::djvFileBrowserTestWidget(
    djvGuiContext * context,
    const QString & path,
    QWidget *       parent) :
    QWidget(parent),
    _context       (context),
    _model         (new djvFileBrowserTestModel(context)),
    _upAction      (0),
    _backAction    (0),
    _reloadAction  (0),
    _sequenceWidget(0),
    _filterWidget  (0),
    _spinner       (0),
    _pathWidget    (0),
    _view          (0)
{
    _upAction = new QAction(this);
    _upAction->setText("Up");
    
    _backAction = new QAction(this);
    _backAction->setText("Back");
    
    _reloadAction = new QAction(this);
    _reloadAction->setText("Reload");
    
    QToolButton * upButton = new QToolButton;
    upButton->setAutoRaise(true);
    upButton->setDefaultAction(_upAction);
    
    QToolButton * backButton = new QToolButton;
    backButton->setAutoRaise(true);
    backButton->setDefaultAction(_backAction);
    
    QToolButton * reloadButton = new QToolButton;
    reloadButton->setAutoRaise(true);
    reloadButton->setDefaultAction(_reloadAction);
    
    _sequenceWidget = new QComboBox;
    _sequenceWidget->addItems(djvSequence::compressLabels());
    
    _filterWidget = new QLineEdit;
    
    _spinner = new djvSpinner(context);
    
    _pathWidget = new QLineEdit;
    
    _view = new QTreeView;
    _view->setRootIsDecorated(false);
    _view->setItemsExpandable(false);
    _view->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    _view->setAlternatingRowColors(true);
    _view->setModel(_model.data());
    
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
    
    hLayout->addWidget(new QLabel("Sequence:"));
    hLayout->addWidget(_sequenceWidget);
    hLayout->addStretch();
    hLayout->addWidget(_filterWidget);
    hLayout->addWidget(_spinner);
    layout->addLayout(hLayout);
    
    layout->addWidget(_view);

    hLayout = new QHBoxLayout;
    hLayout->setMargin(5);
    hLayout->addWidget(_pathWidget);
    layout->addLayout(hLayout);
    
    _model->connect(
        _upAction,
        SIGNAL(triggered()),
        SLOT(up()));
    
    _model->connect(
        _backAction,
        SIGNAL(triggered()),
        SLOT(back()));
    
    _model->connect(
        _reloadAction,
        SIGNAL(triggered()),
        SLOT(reload()));
    
    _model->connect(
        _filterWidget,
        SIGNAL(textChanged(const QString &)),
        SLOT(setFilterText(const QString &)));

    _spinner->connect(
        _model.data(),
        SIGNAL(requestDir(const djvFileBrowserTestDirRequest &)),
        SLOT(start()));
    
    _spinner->connect(
        _model.data(),
        SIGNAL(requestDirComplete()),
        SLOT(stop()));
    
    _pathWidget->connect(
        _model.data(),
        SIGNAL(pathChanged(const QString &)),
        SLOT(setText(const QString &)));

    connect(
        _sequenceWidget,
        SIGNAL(activated(int)),
        SLOT(sequenceCallback(int)));

    _model->setPath(path);
}

djvFileBrowserTestWidget::~djvFileBrowserTestWidget()
{
    _view->setModel(0);
}

void djvFileBrowserTestWidget::sequenceCallback(int index)
{
    _model->setSequence(static_cast<djvSequence::COMPRESS>(index));
}


