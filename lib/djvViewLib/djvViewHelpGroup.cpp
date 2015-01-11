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

//! \file djvViewHelpGroup.cpp

#include <djvViewHelpGroup.h>

#include <djvViewApplication.h>
#include <djvViewHelpActions.h>
#include <djvViewHelpMenu.h>
#include <djvViewMainWindow.h>

#include <djvApplicationAboutDialog.h>
#include <djvApplicationInfoDialog.h>

#include <djvDebug.h>
#include <djvFileInfoUtil.h>

#include <QAction>
#include <QMenuBar>

//------------------------------------------------------------------------------
// djvViewHelpGroup::P
//------------------------------------------------------------------------------

struct djvViewHelpGroup::P
{
    P() :
        actions(0),
        menu   (0)
    {}
    
    djvViewHelpActions * actions;
    djvViewHelpMenu *    menu;
};

//------------------------------------------------------------------------------
// djvViewHelpGroup
//------------------------------------------------------------------------------

djvViewHelpGroup::djvViewHelpGroup(
    djvViewMainWindow *      mainWindow,
    const djvViewHelpGroup * copy) :
    djvViewAbstractGroup(mainWindow),
    _p(new P)
{
    //DJV_DEBUG("djvViewHelpGroup::djvViewHelpGroup");

    // Create the actions.

    _p->actions = new djvViewHelpActions(this);
    
    // Create the menus.

    _p->menu = new djvViewHelpMenu(_p->actions, mainWindow->menuBar());

    mainWindow->menuBar()->addMenu(_p->menu);

    // Setup the action callbacks.

    connect(
        _p->actions->action(djvViewHelpActions::HELP),
        SIGNAL(triggered()),
        SLOT(helpCallback()));

    connect(
        _p->actions->action(djvViewHelpActions::INFO),
        SIGNAL(triggered()),
        SLOT(infoCallback()));

    connect(
        _p->actions->action(djvViewHelpActions::ABOUT),
        SIGNAL(triggered()),
        SLOT(aboutCallback()));
}

djvViewHelpGroup::~djvViewHelpGroup()
{
    //DJV_DEBUG("djvViewHelpGroup::~djvViewHelpGroup");

    delete _p;
}

void djvViewHelpGroup::helpCallback()
{
    //DJV_DEBUG("Application::help");

    DJV_VIEW_APP->help();
}

void djvViewHelpGroup::infoCallback()
{
    djvApplicationInfoDialog::global()->show();
}

void djvViewHelpGroup::aboutCallback()
{
    djvApplicationAboutDialog::global()->show();
}
