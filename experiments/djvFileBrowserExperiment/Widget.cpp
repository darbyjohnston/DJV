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

#include <Widget.h>

#include <Actions.h>
#include <Controls.h>
#include <Model.h>
#include <View.h>

#include <djvSearchBox.h>

#include <QAction>
#include <QApplication>
#include <QComboBox>
#include <QDockWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSlider>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// Widget::Private
//------------------------------------------------------------------------------

struct Widget::Private
{
    Private(djvUIContext * context) :
        context            (context),
        model              (new Model(context))
    {}

    djvUIContext * context = nullptr;
    QScopedPointer<Model> model;

    QAction * upAction = nullptr;
    QAction * backAction = nullptr;
    QAction * reloadAction = nullptr;
    QActionGroup * sequenceActionGroup = nullptr;
    QActionGroup * viewActionGroup = nullptr;
    QAction * leftDockAction = nullptr;
    QAction * rightDockAction = nullptr;

    QSlider * thumbnailSlider = nullptr;
    djvSearchBox * searchBox = nullptr;
    View * viewWidget = nullptr;
    QLineEdit * pathWidget = nullptr;

    QToolBar * dirToolBar = nullptr;
    QToolBar * sequenceToolBar = nullptr;
    QToolBar * viewToolBar = nullptr;
    QToolBar * searchToolBar = nullptr;
    QToolBar * fileToolBar = nullptr;
    QDockWidget * leftDockWidget = nullptr;
    QDockWidget * rightDockWidget = nullptr;
};

//------------------------------------------------------------------------------
// Widget
//------------------------------------------------------------------------------

Widget::Widget(
    djvUIContext * context,
    QWidget *      parent) :
    QMainWindow(parent),
    _p(new Private(context))
{
    // Create the actions.
    _p->upAction = Actions::upAction(this);
    _p->backAction = Actions::backAction(this);
    _p->reloadAction = Actions::reloadAction(this);
    _p->sequenceActionGroup = Actions::sequenceActionGroup(this);
    _p->viewActionGroup = Actions::viewActionGroup(this);
    _p->leftDockAction = Actions::leftDockAction(this);
    _p->rightDockAction = Actions::rightDockAction(this);

    // Create the widgets.
    _p->thumbnailSlider = new QSlider;
    _p->thumbnailSlider->setOrientation(Qt::Horizontal);
    _p->thumbnailSlider->setRange(100, 300);
    _p->thumbnailSlider->setToolTip(
        qApp->translate("djvFileBrowserExperiment", "Set the thumbnail size"));

    _p->searchBox = new djvSearchBox(context);

    _p->viewWidget = new View(_p->model.data(), context);

    _p->pathWidget = new QLineEdit;

    // Create the tool bars.
    _p->dirToolBar = Controls::toolBar();
    _p->dirToolBar->addAction(_p->upAction);
    _p->dirToolBar->addAction(_p->backAction);
    _p->dirToolBar->addAction(_p->reloadAction);
    addToolBar(Qt::TopToolBarArea, _p->dirToolBar);

    _p->sequenceToolBar = Controls::toolBar();
    Q_FOREACH(QAction * action, _p->sequenceActionGroup->actions())
        _p->sequenceToolBar->addAction(action);
    addToolBar(Qt::TopToolBarArea, _p->sequenceToolBar);

    _p->viewToolBar = Controls::toolBar();
    Q_FOREACH(QAction * action, _p->viewActionGroup->actions())
        _p->viewToolBar->addAction(action);
    _p->viewToolBar->addWidget(_p->thumbnailSlider);
    _p->viewToolBar->addAction(_p->leftDockAction);
    _p->viewToolBar->addAction(_p->rightDockAction);
    addToolBar(Qt::TopToolBarArea, _p->viewToolBar);
     _p->searchToolBar = Controls::toolBar();
    _p->searchToolBar->addWidget(_p->searchBox);
    addToolBar(Qt::TopToolBarArea, _p->searchToolBar);

    _p->fileToolBar = Controls::toolBar();
    _p->fileToolBar->addWidget(_p->pathWidget);
    addToolBar(Qt::BottomToolBarArea, _p->fileToolBar);
    // Create the docks.
    _p->leftDockWidget = Controls::dockWidget();
    _p->leftDockWidget->hide();
    addDockWidget(Qt::LeftDockWidgetArea, _p->leftDockWidget);
    _p->rightDockWidget = Controls::dockWidget();
    _p->rightDockWidget->hide();
    addDockWidget(Qt::RightDockWidgetArea, _p->rightDockWidget);
    // Layout the widgets.
    setCentralWidget(_p->viewWidget);

    // Initialize.
    _p->sequenceActionGroup->actions()[_p->model->sequence()]->setChecked(true);
    _p->viewActionGroup->actions()[_p->viewWidget->mode()]->setChecked(true);
    _p->thumbnailSlider->setValue(_p->model->thumbnailSize());

    // Setup callbacks.
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
        _p->thumbnailSlider,
        SIGNAL(valueChanged(int)),
        SLOT(setThumbnailSize(int)));
    _p->model->connect(
        _p->searchBox,
        SIGNAL(textChanged(const QString &)),
        SLOT(setFilterText(const QString &)));
    _p->pathWidget->connect(
        _p->model.data(),
        SIGNAL(pathChanged(const QString &)),
        SLOT(setText(const QString &)));
    _p->leftDockWidget->connect(
        _p->leftDockAction,
        SIGNAL(toggled(bool)),
        SLOT(setVisible(bool)));
    _p->rightDockWidget->connect(
        _p->rightDockAction,
        SIGNAL(toggled(bool)),
        SLOT(setVisible(bool)));
    connect(
        _p->model.data(),
        SIGNAL(dirChanged(const QDir &)),
        SIGNAL(dirChanged(const QDir &)));
    connect(
        _p->model.data(),
        SIGNAL(pathChanged(const QString &)),
        SIGNAL(pathChanged(const QString &)));
    connect(
        _p->sequenceActionGroup,
        SIGNAL(triggered(QAction *)),
        SLOT(sequenceCallback(QAction *)));
    connect(
        _p->viewActionGroup,
        SIGNAL(triggered(QAction *)),
        SLOT(viewModeCallback(QAction *)));
}

Widget::~Widget()
{}

const QDir & Widget::dir() const
{
    return _p->model->dir();
}

QString Widget::path() const
{
    return _p->model->path();
}

void Widget::setDir(const QDir & dir)
{
    _p->model->setDir(dir);
}

void Widget::setPath(const QString & path)
{
    _p->model->setPath(path);
}

void Widget::sequenceCallback(QAction * action)
{
    _p->model->setSequence(static_cast<djvSequence::COMPRESS>(
        _p->sequenceActionGroup->actions().indexOf(action)));
}

void Widget::viewModeCallback(QAction * action)
{
    _p->viewWidget->setMode(static_cast<View::MODE>(_p->viewActionGroup->actions().indexOf(action)));
}


