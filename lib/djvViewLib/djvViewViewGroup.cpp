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

//! \file djvViewViewGroup.cpp

#include <djvViewViewGroup.h>

#include <djvViewImageView.h>
#include <djvViewMainWindow.h>
#include <djvViewShortcutPrefs.h>
#include <djvViewViewActions.h>
#include <djvViewViewMenu.h>
#include <djvViewViewPrefs.h>
#include <djvViewViewToolBar.h>

#include <djvFloatEdit.h>

#include <djvListUtil.h>

#include <QAction>
#include <QActionGroup>
#include <QMenuBar>
#include <QToolBar>

//------------------------------------------------------------------------------
// djvViewViewGroup::P
//------------------------------------------------------------------------------

struct djvViewViewGroup::P
{
    P() :
        grid      (djvViewViewPrefs::global()->grid()),
        hudEnabled(djvViewViewPrefs::global()->isHudEnabled()),
        actions   (0),
        menu      (0),
        toolBar   (0)
    {}
    
    djvViewUtil::GRID    grid;
    bool                 hudEnabled;
    djvViewViewActions * actions;
    djvViewViewMenu *    menu;
    djvViewViewToolBar * toolBar;
};

//------------------------------------------------------------------------------
// djvViewViewGroup
//------------------------------------------------------------------------------

djvViewViewGroup::djvViewViewGroup(
    djvViewMainWindow *      mainWindow,
    const djvViewViewGroup * copy) :
    djvViewAbstractGroup(mainWindow),
    _p(new P)
{
    if (copy)
    {
        _p->grid       = copy->_p->grid;
        _p->hudEnabled = copy->_p->hudEnabled;
    }

    // Create the actions.

    _p->actions = new djvViewViewActions(this);

    // Create the menus.

    _p->menu = new djvViewViewMenu(_p->actions, mainWindow->menuBar());

    mainWindow->menuBar()->addMenu(_p->menu);

    // Create the widgets.

    _p->toolBar = new djvViewViewToolBar(_p->actions);

    mainWindow->addToolBar(_p->toolBar);

    // Initialize.

    update();

    // Setup the action callbacks.

    connect(
        _p->actions->action(djvViewViewActions::LEFT),
        SIGNAL(triggered()),
        SLOT(leftCallback()));

    connect(
        _p->actions->action(djvViewViewActions::RIGHT),
        SIGNAL(triggered()),
        SLOT(rightCallback()));

    connect(
        _p->actions->action(djvViewViewActions::UP),
        SIGNAL(triggered()),
        SLOT(upCallback()));

    connect(
        _p->actions->action(djvViewViewActions::DOWN),
        SIGNAL(triggered()),
        SLOT(downCallback()));

    connect(
        _p->actions->action(djvViewViewActions::CENTER),
        SIGNAL(triggered()),
        SLOT(centerCallback()));

    connect(
        _p->actions->action(djvViewViewActions::ZOOM_IN),
        SIGNAL(triggered()),
        SLOT(zoomInCallback()));

    connect(
        _p->actions->action(djvViewViewActions::ZOOM_OUT),
        SIGNAL(triggered()),
        SLOT(zoomOutCallback()));

    connect(
        _p->actions->action(djvViewViewActions::ZOOM_RESET),
        SIGNAL(triggered()),
        SLOT(zoomResetCallback()));

    connect(
        _p->actions->action(djvViewViewActions::RESET),
        SIGNAL(triggered()),
        SLOT(resetCallback()));

    connect(
        _p->actions->action(djvViewViewActions::FIT),
        SIGNAL(triggered()),
        SLOT(fitCallback()));

    connect(
        _p->actions->action(djvViewViewActions::HUD),
        SIGNAL(toggled(bool)),
        SLOT(hudEnabledCallback(bool)));

    // Setup the action group callbacks.

    connect(
        _p->actions->group(djvViewViewActions::GRID_GROUP),
        SIGNAL(triggered(QAction *)),
        SLOT(gridCallback(QAction *)));

    // Setup the widget callbacks.

    mainWindow->viewWidget()->connect(
        _p->toolBar->zoomEdit(),
        SIGNAL(valueChanged(double)),
        SLOT(setZoomFocus(double)));

    _p->toolBar->zoomEdit()->connect(
        mainWindow->viewWidget(),
        SIGNAL(viewZoomChanged(double)),
        SLOT(setValue(double)));

    // Setup the preferences callbacks.

    connect(
        djvViewViewPrefs::global(),
        SIGNAL(gridChanged(djvViewUtil::GRID)),
        SLOT(gridCallback(djvViewUtil::GRID)));

    connect(
        djvViewViewPrefs::global(),
        SIGNAL(hudEnabledChanged(bool)),
        SLOT(hudEnabledCallback(bool)));
}

djvViewViewGroup::~djvViewViewGroup()
{
    delete _p;
}

QToolBar * djvViewViewGroup::toolBar() const
{
    return _p->toolBar;
}

void djvViewViewGroup::leftCallback()
{
    viewMove(djvVector2i(-10, 0));
}

void djvViewViewGroup::rightCallback()
{
    viewMove(djvVector2i(10, 0));
}

void djvViewViewGroup::upCallback()
{
    viewMove(djvVector2i(0, 10));
}

void djvViewViewGroup::downCallback()
{
    viewMove(djvVector2i(0, -10));
}

void djvViewViewGroup::centerCallback()
{
    mainWindow()->viewWidget()->viewCenter();
}

void djvViewViewGroup::resetCallback()
{
    mainWindow()->viewWidget()->setViewPosZoom(djvVector2i(), 1.0);
}

void djvViewViewGroup::zoomInCallback()
{
    viewZoom(2.0);
}

void djvViewViewGroup::zoomOutCallback()
{
    viewZoom(0.5);
}

void djvViewViewGroup::zoomIncCallback()
{
    viewZoom(1.1);
}

void djvViewViewGroup::zoomDecCallback()
{
    viewZoom(0.9);
}

void djvViewViewGroup::zoomResetCallback()
{
    mainWindow()->viewWidget()->setZoomFocus(1.0);
}

void djvViewViewGroup::fitCallback()
{
    mainWindow()->viewWidget()->viewFit();
}

void djvViewViewGroup::gridCallback(djvViewUtil::GRID grid)
{
    _p->grid = grid;

    update();
}

void djvViewViewGroup::gridCallback(QAction * action)
{
    gridCallback(static_cast<djvViewUtil::GRID>(action->data().toInt()));
}

void djvViewViewGroup::hudEnabledCallback(bool value)
{
    _p->hudEnabled = value;

    update();
}

void djvViewViewGroup::update()
{
    // Update the actions.

    _p->actions->action(djvViewViewActions::HUD)->
        setChecked(_p->hudEnabled);

    // Update the action groups.

    if (! _p->actions->group(djvViewViewActions::GRID_GROUP)->
        actions()[_p->grid]->isChecked())
    {
        _p->actions->group(djvViewViewActions::GRID_GROUP)->
            actions()[_p->grid]->trigger();
    }

    // Update the widgets.

    mainWindow()->viewWidget()->setGrid(_p->grid);

    mainWindow()->viewWidget()->setHudEnabled(_p->hudEnabled);

    _p->toolBar->zoomEdit()->setValue(
        mainWindow()->viewWidget()->viewZoom());
}

void djvViewViewGroup::viewMove(const djvVector2i & offset)
{
    mainWindow()->viewWidget()->setViewPos(
        mainWindow()->viewWidget()->viewPos() + offset);
}

void djvViewViewGroup::viewZoom(double zoom)
{
    //DJV_DEBUG("djvViewViewGroup::viewZoom");
    //DJV_DEBUG_PRINT("zoom = " << zoom);
    
    mainWindow()->viewWidget()->setZoomFocus(
        mainWindow()->viewWidget()->viewZoom() * zoom);
}
