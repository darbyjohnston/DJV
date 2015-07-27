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

//! \file djvFileBrowserTestWindow.cpp

#include <djvFileBrowserTestWindow.h>

#include <QComboBox>
#include <QToolBar>
#include <QToolButton>
#include <QTreeView>

djvFileBrowserTestWindow::djvFileBrowserTestWindow(
    djvGuiContext * context,
    const QString & path,
    QWidget *       parent) :
    QMainWindow(parent),
    _context       (context),
    _model         (new djvFileBrowserTestModel(context)),
    _upAction      (0),
    _backAction    (0),
    _reloadAction  (0),
    _sequenceWidget(0),
    _view          (0)
{
    _sequenceWidget = new QComboBox;
    _sequenceWidget->addItems(djvSequence::compressLabels());	
    
    _view = new QTreeView;
    
    _view->setModel(_model.data());

    QToolBar * toolBar = addToolBar("Tool Bar");
    toolBar->addWidget(_sequenceWidget);

    setCentralWidget(_view);
    
    _model->setPath(path);
    
    connect(
        _sequenceWidget,
        SIGNAL(activated(int)),
        SLOT(sequenceCallback(int)));
}

djvFileBrowserTestWindow::~djvFileBrowserTestWindow()
{
    _view->setModel(0);
}

void djvFileBrowserTestWindow::sequenceCallback(int index)
{
    _model->setSequence(static_cast<djvSequence::COMPRESS>(index));
}


