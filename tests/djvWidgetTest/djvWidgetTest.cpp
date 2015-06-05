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
#include <djvChoiceDialogTest.h>
#include <djvColorWidgetTest.h>
#include <djvColorDialogTest.h>
#include <djvFileBrowserTest.h>
#include <djvFileEditTest.h>
#include <djvIconLibraryTest.h>
#include <djvInputDialogTest.h>
#include <djvIntSliderTest.h>
#include <djvMessageDialogTest.h>
#include <djvMessagesDialogTest.h>
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

#include <djvSearchBox.h>
#include <djvGuiContext.h>

#include <QListView>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>

djvWidgetTest::djvWidgetTest(djvGuiContext * context) :
    _context(context)
{}

djvWidgetTest::~djvWidgetTest()
{}

djvGuiContext * djvWidgetTest::context() const
{
    return _context;
}

djvWidgetTestModel::djvWidgetTestModel(QVector<djvWidgetTest *> & tests) :
    _tests(tests)
{}

QModelIndex	djvWidgetTestModel::index(
    int                 row,
    int                 column,
    const QModelIndex & parent) const
{
    if (! hasIndex(row, column, parent))
        return QModelIndex();

    return createIndex(row, column, _tests[row]);
}

int djvWidgetTestModel::rowCount(const QModelIndex & parent) const
{
    return _tests.count();
}

QVariant djvWidgetTestModel::data(const QModelIndex & index, int role) const
{
    if (! index.isValid())
        return QVariant();
    
    if (Qt::DisplayRole == role)
        return _tests[index.row()]->name();
    
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

djvWidgetTestApplication::djvWidgetTestApplication(int & argc, char ** argv) :
    djvApplication("djvWidgetTest", argc, argv),
    _context(0),
    _model  (0),
    _window (0)
{
    _context = new djvGuiContext;

    setValid(true);
    
    _tests += new djvApplicationAboutDialogTest(_context);
    _tests += new djvApplicationInfoDialogTest(_context);
    _tests += new djvChoiceDialogTest(_context);
    _tests += new djvColorWidgetTest(_context);
    _tests += new djvColorDialogTest(_context);
    _tests += new djvFileBrowserTest(_context);
    _tests += new djvFileEditTest(_context);
    _tests += new djvIconLibraryTest(_context);
    _tests += new djvInputDialogTest(_context);
    _tests += new djvIntSliderTest(_context);
    _tests += new djvMessageDialogTest(_context);
    _tests += new djvMessagesDialogTest(_context);
    _tests += new djvMultiChoiceDialogTest(_context);
    _tests += new djvNumWidgetTest(_context);
    _tests += new djvPixelWidgetTest(_context);
    _tests += new djvPrefsDialogTest(_context);
    _tests += new djvProgressDialogTest(_context);
    _tests += new djvQuestionDialogTest(_context);
    _tests += new djvSearchBoxTest(_context);
    _tests += new djvShortcutsWidgetTest(_context);
    _tests += new djvToolButtonTest(_context);
    _tests += new djvVectorWidgetTest(_context);

    _model = new djvWidgetTestModel(_tests);
    
    _window = new djvWidgetTestWindow(_model);
    _window->show();
    
    if (argc > 1)
    {
        QStringList args;
        
        for (int i = 2; i < argc; ++i)
        {
            args += QString(argv[i]);
        }
        
        for (int i = 0; i < _tests.count(); ++i)
        {
            if (argv[1] == _tests[i]->name())
            {
                _tests[i]->run(args);
                
                break;
            }
        }
    }
}

djvWidgetTestApplication::~djvWidgetTestApplication()
{
    delete _window;
    delete _model;
    
    Q_FOREACH(djvWidgetTest * test, _tests)
    {
        delete test;
    }
    
    delete _context;
}

int main(int argc, char ** argv)
{
    return (djvWidgetTestApplication(argc, argv)).run();
}

