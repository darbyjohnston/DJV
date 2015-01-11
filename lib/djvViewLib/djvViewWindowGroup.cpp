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

//! \file djvViewWindowGroup.cpp

#include <djvViewWindowGroup.h>

#include <djvViewImageView.h>
#include <djvViewMainWindow.h>
#include <djvViewWindowActions.h>
#include <djvViewWindowMenu.h>
#include <djvViewWindowPrefs.h>
#include <djvViewWindowToolBar.h>

#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QToolButton>

//------------------------------------------------------------------------------
// djvViewWindowGroup::P
//------------------------------------------------------------------------------

struct djvViewWindowGroup::P
{
    P() :
        fullScreen     (false),
        controlsVisible(true),
        toolBarVisible (djvViewWindowPrefs::global()->toolBar()),
        actions        (0),
        menu           (0),
        toolBar        (0)
    {}
    
    bool                   fullScreen;
    bool                   controlsVisible;
    QVector<bool>          toolBarVisible;
    djvViewWindowActions * actions;
    djvViewWindowMenu *    menu;
    djvViewWindowToolBar * toolBar;
};

//------------------------------------------------------------------------------
// djvViewWindowGroup
//------------------------------------------------------------------------------

djvViewWindowGroup::djvViewWindowGroup(
    djvViewMainWindow *        mainWindow,
    const djvViewWindowGroup * copy) :
    djvViewAbstractGroup(mainWindow),
    _p(new P)
{
    //DJV_DEBUG("djvViewWindowGroup::djvViewWindowGroup");

    if (copy)
    {
        _p->controlsVisible = copy->_p->controlsVisible;
        _p->toolBarVisible  = copy->_p->toolBarVisible;
    }

    // Create the actions.

    _p->actions = new djvViewWindowActions(this);
    
    // Create the menus.

    _p->menu = new djvViewWindowMenu(_p->actions, mainWindow->menuBar());

    mainWindow->menuBar()->addMenu(_p->menu);

    // Create the widgets.

    _p->toolBar = new djvViewWindowToolBar(_p->actions);

    mainWindow->addToolBar(_p->toolBar);

    // Initialize.

    update();

    // Setup the action callbacks.

    connect(
        _p->actions->action(djvViewWindowActions::NEW),
        SIGNAL(triggered()),
        SLOT(newCallback()));

    connect(
        _p->actions->action(djvViewWindowActions::COPY),
        SIGNAL(triggered()),
        SLOT(copyCallback()));

    connect(
        _p->actions->action(djvViewWindowActions::CLOSE),
        SIGNAL(triggered()),
        SLOT(closeCallback()));

    connect(
        _p->actions->action(djvViewWindowActions::FIT),
        SIGNAL(triggered()),
        SLOT(fitCallback()));

    connect(
        _p->actions->action(djvViewWindowActions::FULL_SCREEN),
        SIGNAL(toggled(bool)),
        SLOT(setFullScreen(bool)));

    connect(
        _p->actions->action(djvViewWindowActions::CONTROLS_VISIBLE),
        SIGNAL(toggled(bool)),
        SLOT(setControlsVisible(bool)));

    connect(
        _p->actions->group(djvViewWindowActions::TOOL_BAR_VISIBLE_GROUP),
        SIGNAL(triggered(QAction *)),
        SLOT(toolBarVisibleCallback(QAction *)));

    // Setup the preferences callbacks.
    
    connect(
        djvViewWindowPrefs::global(),
        SIGNAL(toolBarChanged(const QVector<bool> &)),
        SLOT(setToolBarVisible(const QVector<bool> &)));
}

djvViewWindowGroup::~djvViewWindowGroup()
{
    //DJV_DEBUG("djvViewWindowGroup::~djvViewWindowGroup");

    delete _p;
}

bool djvViewWindowGroup::hasFullScreen() const
{
    return _p->fullScreen;
}

bool djvViewWindowGroup::hasControlsVisible() const
{
    return _p->controlsVisible;
}

const QVector<bool> & djvViewWindowGroup::toolBarVisible() const
{
    return _p->toolBarVisible;
}

QToolBar * djvViewWindowGroup::toolBar() const
{
    return _p->toolBar;
}

void djvViewWindowGroup::setFullScreen(bool fullScreen)
{
    if (fullScreen == _p->fullScreen)
        return;
    
    _p->fullScreen = fullScreen;

    if (_p->fullScreen)
    {
        mainWindow()->showFullScreen();

        if (! djvViewWindowPrefs::global()->hasFullScreenControls())
        {
            setControlsVisible(false);
        }
    }
    else
    {
        mainWindow()->showNormal();

        if (!djvViewWindowPrefs::global()->hasFullScreenControls())
        {
            setControlsVisible(true);
        }
    }

    update();
    
    Q_EMIT fullScreenChanged(_p->fullScreen);
}

void djvViewWindowGroup::setControlsVisible(bool visible)
{
    if (visible == _p->controlsVisible)
        return;

    _p->controlsVisible = visible;

    update();

    Q_EMIT controlsVisibleChanged(_p->controlsVisible);
}

void djvViewWindowGroup::setToolBarVisible(const QVector<bool> & visible)
{
    if (visible == _p->toolBarVisible)
        return;
    
    _p->toolBarVisible = visible;
    
    update();

    Q_EMIT toolBarVisibleChanged(_p->toolBarVisible);
}

void djvViewWindowGroup::newCallback()
{
    (new djvViewMainWindow)->show();
}

void djvViewWindowGroup::copyCallback()
{
    (new djvViewMainWindow(mainWindow()))->show();
}

void djvViewWindowGroup::closeCallback()
{
    mainWindow()->close();
}

void djvViewWindowGroup::fitCallback()
{
    mainWindow()->fitWindow();
}

void djvViewWindowGroup::toolBarVisibleCallback(QAction * action)
{
    QVector<bool> tmp = _p->toolBarVisible;

    const int index = _p->actions->group(djvViewWindowActions::TOOL_BAR_VISIBLE_GROUP)->
        actions().indexOf(action);

    tmp[index] = ! tmp[index];

    setToolBarVisible(tmp);
}

void djvViewWindowGroup::update()
{
    _p->actions->action(djvViewWindowActions::FULL_SCREEN)->
        setChecked(_p->fullScreen);

    _p->actions->action(djvViewWindowActions::CONTROLS_VISIBLE)->
        setChecked(_p->controlsVisible);

    for (int i = 0; i < djvViewUtil::TOOL_BAR_COUNT; ++i)
    {
        _p->actions->group(djvViewWindowActions::TOOL_BAR_VISIBLE_GROUP)->
            actions()[i]->setChecked(_p->toolBarVisible[i]);
    }
}
