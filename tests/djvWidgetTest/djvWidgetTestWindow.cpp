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

#include <djvWidgetTestWindow.h>

#include <djvAbstractWidgetTest.h>
#include <djvWidgetTestModel.h>

#include <djvUI/SearchBox.h>

#include <QListView>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvWidgetTestWindow
//------------------------------------------------------------------------------

djvWidgetTestWindow::djvWidgetTestWindow(
    djvWidgetTestModel * model,
    djvUIContext *       context) :
    _model     (model),
    _proxyModel(0),
    _listView  (0)
{
    djvSearchBox * searchBox = new djvSearchBox(context);
    
    _listView = new QListView;
    
    QPushButton * button = new QPushButton("Run");
    
    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->addWidget(searchBox);
    layout->addWidget(_listView);
    layout->addWidget(button);
    
    setWindowTitle("djvWidgetTest");
    
    _proxyModel = new QSortFilterProxyModel(this);
    _proxyModel->setSourceModel(_model);
    _proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    
    _listView->setModel(_proxyModel);
    
    connect(
        searchBox,
        SIGNAL(textChanged(const QString &)),
        _proxyModel,
        SLOT(setFilterFixedString(const QString &)));
    connect(
        _listView,
        SIGNAL(activated(const QModelIndex &)),
        SLOT(runCallback()));
    connect(
        button,
        SIGNAL(clicked()),
        SLOT(runCallback()));
}

void djvWidgetTestWindow::runCallback(const QModelIndex & index)
{
    static_cast<djvAbstractWidgetTest *>(
        _proxyModel->mapToSource(index).internalPointer())->run();
}

void djvWidgetTestWindow::runCallback()
{
    QModelIndexList rows = _listView->selectionModel()->selectedRows();
    Q_FOREACH(QModelIndex row, rows)
    {
        static_cast<djvAbstractWidgetTest *>(
            _proxyModel->mapToSource(row).internalPointer())->run();
    }
}

