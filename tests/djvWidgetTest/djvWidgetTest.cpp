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

//! \file djvWidgetTest.cpp

#include <djvWidgetTest.h>

#include <djvApplicationAboutDialogTest.h>
#include <djvApplicationInfoDialogTest.h>
#include <djvApplicationMessageDialogTest.h>
#include <djvChoiceDialogTest.h>
#include <djvColorWidgetTest.h>
#include <djvColorDialogTest.h>
#include <djvFileBrowserTest.h>
#include <djvFileEditTest.h>
#include <djvIconLibraryTest.h>
#include <djvInputDialogTest.h>
#include <djvIntSliderTest.h>
#include <djvMessageDialogTest.h>
#include <djvMultiChoiceDialogTest.h>
#include <djvNumWidgetTest.h>
#include <djvPixelWidgetTest.h>
#include <djvPrefsDialogTest.h>
#include <djvProgressDialogTest.h>
#include <djvQuestionDialogTest.h>
#include <djvSearchBoxTest.h>
#include <djvShortcutsWidgetTest.h>
#include <djvToolButtonTest.h>
#include <djvVectorWidgetTest.h>

#include <djvApplication.h>
#include <djvSearchBox.h>

#include <QListView>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>

djvWidgetTestModel::djvWidgetTestModel(QObject * parent) :
    QAbstractListModel(parent),
    _list(QVector<djvWidgetTest *>() <<
        new djvApplicationAboutDialogTest <<
        new djvApplicationInfoDialogTest <<
        new djvApplicationMessageDialogTest <<
        new djvChoiceDialogTest <<
        new djvColorWidgetTest <<
        new djvColorDialogTest <<
        new djvFileBrowserTest <<
        new djvFileEditTest <<
        new djvIconLibraryTest <<
        new djvInputDialogTest <<
        new djvIntSliderTest <<
        new djvMessageDialogTest <<
        new djvMultiChoiceDialogTest <<
        new djvNumWidgetTest <<
        new djvPixelWidgetTest <<
        new djvPrefsDialogTest <<
        new djvProgressDialogTest <<
        new djvQuestionDialogTest <<
        new djvSearchBoxTest <<
        new djvShortcutsWidgetTest <<
        new djvToolButtonTest <<
        new djvVectorWidgetTest)
{}

const QVector<djvWidgetTest *> & djvWidgetTestModel::list() const
{
    return _list;
}

QModelIndex	djvWidgetTestModel::index(
    int                 row,
    int                 column,
    const QModelIndex & parent) const
{
    if (! hasIndex(row, column, parent))
        return QModelIndex();

    return createIndex(row, column, _list[row]);
}

int djvWidgetTestModel::rowCount(const QModelIndex & parent) const
{
    return _list.count();
}

QVariant djvWidgetTestModel::data(const QModelIndex & index, int role) const
{
    if (! index.isValid())
        return QVariant();
    
    if (Qt::DisplayRole == role)
        return _list[index.row()]->name();
    
    return QVariant();
}

djvWidgetTestWindow::djvWidgetTestWindow(djvWidgetTestModel * model) :
    _model     (model),
    _proxyModel(0),
    _listView  (0)
{
    djvSearchBox * searchBox = new djvSearchBox;
    
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
    static_cast<djvWidgetTest *>(
        _proxyModel->mapToSource(index).internalPointer())->run();
}

void djvWidgetTestWindow::runCallback()
{
    QModelIndexList rows = _listView->selectionModel()->selectedRows();
    
    Q_FOREACH(QModelIndex row, rows)
    {
        static_cast<djvWidgetTest *>(
            _proxyModel->mapToSource(row).internalPointer())->run();
    }
}

int main(int argc, char ** argv)
{
    djvApplication app("djvWidgetTest", argc, argv);
    app.setValid(true);
    
    djvWidgetTestModel * model = new djvWidgetTestModel;
    
    djvWidgetTestWindow * window = new djvWidgetTestWindow(model);
    window->show();
    
    if (argc > 1)
    {
        QStringList args;
        
        for (int i = 2; i < argc; ++i)
        {
            args += QString(argv[i]);
        }
        
        const QVector<djvWidgetTest *> & tests = model->list();
        
        for (int i = 0; i < tests.count(); ++i)
        {
            if (argv[1] == tests[i]->name())
            {
                tests[i]->run(args);
                
                break;
            }
        }
    }
    
    return app.run();
}

