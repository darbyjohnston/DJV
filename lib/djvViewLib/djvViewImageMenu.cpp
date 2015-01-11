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

//! \file djvViewImageMenu.cpp

#include <djvViewImageMenu.h>

#include <djvViewImageActions.h>

//------------------------------------------------------------------------------
// djvViewImageMenu::P
//------------------------------------------------------------------------------

struct djvViewImageMenu::P
{
    P() :
        displayProfileMenu(0)
    {}

    QMenu * displayProfileMenu;
};

//------------------------------------------------------------------------------
// djvViewImageMenu
//------------------------------------------------------------------------------

djvViewImageMenu::djvViewImageMenu(
    djvViewAbstractActions * actions,
    QWidget *                parent) :
    djvViewAbstractMenu(actions, parent),
    _p(new P)
{
    // Create the menus.

    addAction(actions->action(djvViewImageActions::FRAME_STORE));
    addAction(actions->action(djvViewImageActions::LOAD_FRAME_STORE));

    addSeparator();

    addAction(actions->action(djvViewImageActions::MIRROR_H));
    addAction(actions->action(djvViewImageActions::MIRROR_V));

    QMenu * scaleMenu = addMenu(tr("&Scale"));
    scaleMenu->addActions(
        actions->group(djvViewImageActions::SCALE_GROUP)->actions());

    QMenu * rotateMenu = addMenu(tr("&Rotate"));
    rotateMenu->addActions(
        actions->group(djvViewImageActions::ROTATE_GROUP)->actions());

    addSeparator();

    addAction(actions->action(djvViewImageActions::COLOR_PROFILE));

    addAction(actions->action(djvViewImageActions::DISPLAY_PROFILE_VISIBLE));

    _p->displayProfileMenu = addMenu(tr("&Display Profile"));

    addSeparator();

    QMenu * channelMenu = addMenu(tr("C&hannel"));
    channelMenu->addActions(
        actions->group(djvViewImageActions::CHANNEL_GROUP)->actions());

    // Initialize.

    setTitle(tr("&Image"));

    menuUpdate();
}

djvViewImageMenu::~djvViewImageMenu()
{
    delete _p;
}

void djvViewImageMenu::menuUpdate()
{
    _p->displayProfileMenu->clear();

    _p->displayProfileMenu->addActions(
        actions()->group(djvViewImageActions::DISPLAY_PROFILE_GROUP)->actions());
}
