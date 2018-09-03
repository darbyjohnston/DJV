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

#include <djvViewFileMenu.h>

#include <djvViewFileActions.h>

#include <QApplication>

//------------------------------------------------------------------------------
// djvViewFileMenu::Private
//------------------------------------------------------------------------------

struct djvViewFileMenu::Private
{
    Private() :
        recent(0),
        layer (0)
    {}

    QMenu * recent;
    QMenu * layer;
};

//------------------------------------------------------------------------------
// djvViewFileMenu
//------------------------------------------------------------------------------

djvViewFileMenu::djvViewFileMenu(
    djvViewAbstractActions * actions,
    QWidget *                parent) :
    djvViewAbstractMenu(actions, parent),
    _p(new Private)
{
    // Create the menus.
    addAction(actions->action(djvViewFileActions::OPEN));
    _p->recent = addMenu(qApp->translate("djvViewFileMenu", "&Recent"));
    addAction(actions->action(djvViewFileActions::RELOAD));
    addAction(actions->action(djvViewFileActions::RELOAD_FRAME));
    addAction(actions->action(djvViewFileActions::CLOSE));
    addAction(actions->action(djvViewFileActions::SAVE));
    addAction(actions->action(djvViewFileActions::SAVE_FRAME));
    addSeparator();
    _p->layer = addMenu(qApp->translate("djvViewFileMenu", "La&yer"));
    addAction(actions->action(djvViewFileActions::LAYER_PREV));
    addAction(actions->action(djvViewFileActions::LAYER_NEXT));
    QMenu * proxyMenu = addMenu(qApp->translate("djvViewFileMenu", "Pro&xy Scale"));
    Q_FOREACH(QAction * action,
        actions->group(djvViewFileActions::PROXY_GROUP)->actions())
        proxyMenu->addAction(action);
    addAction(actions->action(djvViewFileActions::U8_CONVERSION));
    addAction(actions->action(djvViewFileActions::CACHE));
    addAction(actions->action(djvViewFileActions::PRELOAD));
    addAction(actions->action(djvViewFileActions::CLEAR_CACHE));
    addSeparator();
    addAction(actions->action(djvViewFileActions::MESSAGES));
    addAction(actions->action(djvViewFileActions::PREFS));
    addAction(actions->action(djvViewFileActions::DEBUG_LOG));
    addSeparator();
    addAction(actions->action(djvViewFileActions::EXIT));

    // Initialize.
    setTitle(qApp->translate("djvViewFileMenu", "&File"));
    menuUpdate();
}

djvViewFileMenu::~djvViewFileMenu()
{}

void djvViewFileMenu::menuUpdate()
{
    _p->recent->clear();
    Q_FOREACH(QAction * action,
        actions()->group(djvViewFileActions::RECENT_GROUP)->actions())
        _p->recent->addAction(action);
    _p->layer->clear();
    Q_FOREACH(QAction * action,
        actions()->group(djvViewFileActions::LAYER_GROUP)->actions())
        _p->layer->addAction(action);
}
