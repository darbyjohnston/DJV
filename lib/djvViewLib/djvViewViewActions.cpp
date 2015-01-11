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

//! \file djvViewViewActions.cpp

#include <djvViewViewActions.h>

#include <djvViewShortcutPrefs.h>
#include <djvViewUtil.h>

#include <djvIconLibrary.h>
#include <djvShortcut.h>

#include <QAction>
#include <QActionGroup>

//------------------------------------------------------------------------------
// djvViewViewActions::P
//------------------------------------------------------------------------------

struct djvViewViewActions::P
{};

//------------------------------------------------------------------------------
// djvViewViewActions
//------------------------------------------------------------------------------

djvViewViewActions::djvViewViewActions(QObject * parent) :
    djvViewAbstractActions(parent),
    _p(new P)
{
    // Create the actions.

    for (int i = 0; i < ACTION_COUNT; ++i)
    {
        _actions[i] = new QAction(this);
    }

    _actions[LEFT]->setText(tr("&Left"));

    _actions[RIGHT]->setText(tr("&Right"));

    _actions[UP]->setText(tr("&Up"));

    _actions[DOWN]->setText(tr("&Down"));

    _actions[CENTER]->setText(tr("&Center"));

    _actions[ZOOM_IN]->setText(tr("Zoom &In"));
    _actions[ZOOM_IN]->setIcon(djvIconLibrary::global()->icon(
        "djvViewZoomInIcon.png"));

    _actions[ZOOM_OUT]->setText(tr("Zoom &Out"));
    _actions[ZOOM_OUT]->setIcon(djvIconLibrary::global()->icon(
        "djvViewZoomOutIcon.png"));

    _actions[ZOOM_RESET]->setText(tr("Zoom Rese&t"));
    _actions[ZOOM_RESET]->setIcon(djvIconLibrary::global()->icon(
        "djvViewZoomResetIcon.png"));

    _actions[RESET]->setText(tr("R&eset"));

    _actions[FIT]->setText(tr("&Fit"));
    _actions[FIT]->setIcon(djvIconLibrary::global()->icon(
        "djvViewFitIcon.png"));

    _actions[HUD]->setText(tr("&HUD"));
    _actions[HUD]->setCheckable(true);

    // Create the action groups.

    for (int i = 0; i < GROUP_COUNT; ++i)
    {
        _groups[i] = new QActionGroup(this);
    }

    _groups[GRID_GROUP]->setExclusive(true);

    for (int i = 0; i < djvViewUtil::gridLabels().count(); ++i)
    {
        QAction * action = new QAction(this);
        action->setText(djvViewUtil::gridLabels()[i]);
        action->setCheckable(true);
        action->setData(i);

        _groups[GRID_GROUP]->addAction(action);
    }

    // Initialize.

    update();

    // Setup the callbacks.

    connect(
        djvViewShortcutPrefs::global(),
        SIGNAL(shortcutsChanged(const QVector<djvShortcut> &)),
        SLOT(update()));
}

djvViewViewActions::~djvViewViewActions()
{
    delete _p;
}

void djvViewViewActions::update()
{
    const QVector<djvShortcut> & shortcuts =
        djvViewShortcutPrefs::global()->shortcuts();

    // Update the actions.

    _actions[LEFT]->setShortcut(
        shortcuts[djvViewUtil::SHORTCUT_VIEW_LEFT].value);
    _actions[LEFT]->setToolTip(tr("Move the view left\n\nShortcut: %1").
        arg(shortcuts[djvViewUtil::SHORTCUT_VIEW_LEFT].value.toString()));

    _actions[RIGHT]->setShortcut(
        shortcuts[djvViewUtil::SHORTCUT_VIEW_RIGHT].value);
    _actions[RIGHT]->setToolTip(tr("Move the view right\n\nShortcut: %1").
        arg(shortcuts[djvViewUtil::SHORTCUT_VIEW_RIGHT].value.toString()));

    _actions[UP]->setShortcut(
        shortcuts[djvViewUtil::SHORTCUT_VIEW_UP].value);
    _actions[UP]->setToolTip(tr("Move the view up\n\nShortcut: %1").
        arg(shortcuts[djvViewUtil::SHORTCUT_VIEW_UP].value.toString()));

    _actions[DOWN]->setShortcut(
        shortcuts[djvViewUtil::SHORTCUT_VIEW_DOWN].value);
    _actions[DOWN]->setToolTip(tr("Move the view down\n\nShortcut: %1").
        arg(shortcuts[djvViewUtil::SHORTCUT_VIEW_DOWN].value.toString()));

    _actions[CENTER]->setShortcut(
        shortcuts[djvViewUtil::SHORTCUT_VIEW_CENTER].value);
    _actions[CENTER]->setToolTip(tr("Center the view\n\nShortcut: %1").
        arg(shortcuts[djvViewUtil::SHORTCUT_VIEW_CENTER].value.toString()));

    _actions[ZOOM_IN]->setShortcut(
        shortcuts[djvViewUtil::SHORTCUT_VIEW_ZOOM_IN].value);
    _actions[ZOOM_IN]->setToolTip(tr("Zoom into the view\n\nShortcut: %1").
        arg(shortcuts[djvViewUtil::SHORTCUT_VIEW_ZOOM_IN].value.toString()));

    _actions[ZOOM_OUT]->setShortcut(
        shortcuts[djvViewUtil::SHORTCUT_VIEW_ZOOM_OUT].value);
    _actions[ZOOM_OUT]->setToolTip(tr("Zoom out of the view\n\nShortcut: %1").
        arg(shortcuts[djvViewUtil::SHORTCUT_VIEW_ZOOM_OUT].value.toString()));

    _actions[ZOOM_RESET]->setShortcut(
        shortcuts[djvViewUtil::SHORTCUT_VIEW_ZOOM_RESET].value);
    _actions[ZOOM_RESET]->setToolTip(tr("Reset the zoom\n\nShortcut: %1").
        arg(shortcuts[djvViewUtil::SHORTCUT_VIEW_ZOOM_RESET].value.toString()));

    _actions[RESET]->setShortcut(
        shortcuts[djvViewUtil::SHORTCUT_VIEW_RESET].value);
    _actions[RESET]->setToolTip(tr("Reset the position and zoom\n\nShortcut: %1").
        arg(shortcuts[djvViewUtil::SHORTCUT_VIEW_RESET].value.toString()));

    _actions[FIT]->setShortcut(
        shortcuts[djvViewUtil::SHORTCUT_VIEW_FIT].value);
    _actions[FIT]->setToolTip(tr("Fit the image to the view\n\nShortcut: %1").
        arg(shortcuts[djvViewUtil::SHORTCUT_VIEW_FIT].value.toString()));

    _actions[HUD]->setShortcut(
        shortcuts[djvViewUtil::SHORTCUT_VIEW_HUD].value);
    _actions[HUD]->setToolTip(tr("Toggle the HUD (Heads Up Display)\n\nShortcut: %1").
        arg(shortcuts[djvViewUtil::SHORTCUT_VIEW_HUD].value.toString()));

    // Fix up the actions.
    
    osxMenuHack();

    // Emit changed signal.

    Q_EMIT changed();
}
