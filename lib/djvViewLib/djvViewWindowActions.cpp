//------------------------------------------------------------------------------
// Copyright (c) 2004-2014 Darby Johnston
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

//! \file djvViewWindowActions.cpp

#include <djvViewWindowActions.h>

#include <djvView.h>
#include <djvViewShortcut.h>
#include <djvViewShortcutPrefs.h>

#include <djvIconLibrary.h>
#include <djvShortcut.h>

#include <QAction>
#include <QActionGroup>

//------------------------------------------------------------------------------
// djvViewWindowActions::P
//------------------------------------------------------------------------------

struct djvViewWindowActions::P
{};

//------------------------------------------------------------------------------
// djvViewWindowActions
//------------------------------------------------------------------------------

djvViewWindowActions::djvViewWindowActions(QObject * parent) :
	djvViewAbstractActions(parent),
    _p(new P)
{
    // Create the actions.

    for (int i = 0; i < ACTION_COUNT; ++i)
    {
        _actions[i] = new QAction(this);
    }

    _actions[NEW]->setText("&New");
    _actions[NEW]->setIcon(djvIconLibrary::global()->icon(
        "djvWindowNewIcon.png"));

    _actions[COPY]->setText("&Copy");
    _actions[COPY]->setIcon(djvIconLibrary::global()->icon(
        "djvWindowCopyIcon.png"));

    _actions[CLOSE]->setText("Clos&e");
    _actions[CLOSE]->setIcon(djvIconLibrary::global()->icon(
        "djvWindowCloseIcon.png"));

    _actions[FIT]->setText("&Fit");
    _actions[FIT]->setIcon(djvIconLibrary::global()->icon(
        "djvWindowFitIcon.png"));

    _actions[FULL_SCREEN]->setText("F&ull Screen");
    _actions[FULL_SCREEN]->setIcon(djvIconLibrary::global()->icon(
        "djvWindowFullScreenIcon.png"));
    _actions[FULL_SCREEN]->setCheckable(true);

    _actions[CONTROLS_VISIBLE]->setText("Show &Controls");
    _actions[CONTROLS_VISIBLE]->setCheckable(true);

    // Create the action groups.

    for (int i = 0; i < GROUP_COUNT; ++i)
    {
        _groups[i] = new QActionGroup(this);
    }

    _groups[TOOL_BAR_VISIBLE_GROUP]->setExclusive(false);

    for (int i = 0; i < djvView::toolBarLabels().count(); ++i)
    {
        QAction * action = new QAction(this);
        action->setText(djvView::toolBarLabels()[i]);
        action->setCheckable(true);
        action->setData(i);

        _groups[TOOL_BAR_VISIBLE_GROUP]->addAction(action);
    }

    // Initialize.

    update();

    // Setup the callbacks.

    connect(
        djvViewShortcutPrefs::global(),
        SIGNAL(shortcutsChanged(const QVector<djvShortcut> &)),
        SLOT(update()));
}

djvViewWindowActions::~djvViewWindowActions()
{
    delete _p;
}

void djvViewWindowActions::update()
{
    const QVector<djvShortcut> & shortcuts =
        djvViewShortcutPrefs::global()->shortcuts();

    // Update the actions.

    _actions[NEW]->setShortcut(shortcuts[djvViewShortcut::WINDOW_NEW].value);
    _actions[NEW]->setToolTip(QString("Create a new window\n\nShortcut: %1").
        arg(shortcuts[djvViewShortcut::WINDOW_NEW].value.toString()));

    _actions[COPY]->setShortcut(shortcuts[djvViewShortcut::WINDOW_COPY].value);
    _actions[COPY]->setToolTip(QString("Copy the current window\n\nShortcut: %1").
        arg(shortcuts[djvViewShortcut::WINDOW_COPY].value.toString()));

    _actions[CLOSE]->setShortcut(shortcuts[djvViewShortcut::WINDOW_CLOSE].value);
    _actions[CLOSE]->setToolTip(QString("Close the current window\n\nShortcut: %1").
        arg(shortcuts[djvViewShortcut::WINDOW_CLOSE].value.toString()));

    _actions[FIT]->setShortcut(shortcuts[djvViewShortcut::WINDOW_FIT].value);
    _actions[FIT]->setToolTip(QString("Resize the window to fit the image\n\nShortcut: %1").
        arg(shortcuts[djvViewShortcut::WINDOW_FIT].value.toString()));

    _actions[FULL_SCREEN]->setShortcut(shortcuts[djvViewShortcut::WINDOW_FULL_SCREEN].value);
    _actions[FULL_SCREEN]->setToolTip(QString("Show the window full screen\n\nShortcut: %1").
        arg(shortcuts[djvViewShortcut::WINDOW_FULL_SCREEN].value.toString()));

    _actions[CONTROLS_VISIBLE]->setShortcut(
        shortcuts[djvViewShortcut::WINDOW_CONTROLS_VISIBLE].value);
    _actions[CONTROLS_VISIBLE]->setToolTip(
        QString("Toggle the user interface controls visbility\n\nShortcut: %1").
        arg(shortcuts[djvViewShortcut::WINDOW_CONTROLS_VISIBLE].value.toString()));

    // Fix up the actions.
    
    osxMenuHack();

}
