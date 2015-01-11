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

//! \file djvViewImageActions.cpp

#include <djvViewImageActions.h>

#include <djvViewImagePrefs.h>
#include <djvViewShortcutPrefs.h>
#include <djvViewUtil.h>

#include <djvIconLibrary.h>
#include <djvShortcut.h>

#include <djvOpenGlImage.h>

#include <QAction>
#include <QActionGroup>

//------------------------------------------------------------------------------
// djvViewImageActions::P
//------------------------------------------------------------------------------

struct djvViewImageActions::P
{};

//------------------------------------------------------------------------------
// djvViewImageActions
//------------------------------------------------------------------------------

djvViewImageActions::djvViewImageActions(QObject * parent) :
	djvViewAbstractActions(parent),
    _p(new P)
{
    // Create the actions.

    for (int i = 0; i < ACTION_COUNT; ++i)
    {
        _actions[i] = new QAction(this);
    }

    _actions[FRAME_STORE]->setText(tr("Show &Frame Store"));
    _actions[FRAME_STORE]->setCheckable(true);

    _actions[LOAD_FRAME_STORE]->setText(tr("&Load Frame Store"));

    _actions[MIRROR_H]->setText(tr("Mirror &Horizontal"));
    _actions[MIRROR_H]->setCheckable(true);

    _actions[MIRROR_V]->setText(tr("Mirror &Vertical"));
    _actions[MIRROR_V]->setCheckable(true);

    _actions[COLOR_PROFILE]->setText(tr("&Color Profile"));
    _actions[COLOR_PROFILE]->setCheckable(true);

    _actions[DISPLAY_PROFILE_VISIBLE]->setCheckable(true);
    _actions[DISPLAY_PROFILE_VISIBLE]->setText(tr("Show Display Profile"));
    _actions[DISPLAY_PROFILE_VISIBLE]->setIcon(djvIconLibrary::global()->icon(
        "djvDisplayProfileIcon.png"));

    // Create the action groups.

    for (int i = 0; i < GROUP_COUNT; ++i)
    {
        _groups[i] = new QActionGroup(this);
    }

    _groups[SCALE_GROUP]->setExclusive(true);

    for (int i = 0; i < djvViewUtil::imageScaleLabels().count(); ++i)
    {
        QAction * action = new QAction(this);
        action->setText(djvViewUtil::imageScaleLabels()[i]);
        action->setCheckable(true);
        action->setData(i);

        _groups[SCALE_GROUP]->addAction(action);
    }

    _groups[ROTATE_GROUP]->setExclusive(true);

    for (int i = 0; i < djvViewUtil::imageRotateLabels().count(); ++i)
    {
        QAction * action = new QAction(this);
        action->setText(djvViewUtil::imageRotateLabels()[i]);
        action->setCheckable(true);
        action->setData(i);

        _groups[ROTATE_GROUP]->addAction(action);
    }

    _groups[CHANNEL_GROUP]->setExclusive(true);

    for (int i = 0; i < djvOpenGlImageOptions::channelLabels().count(); ++i)
    {
        QAction * action = new QAction(this);
        action->setText(djvOpenGlImageOptions::channelLabels()[i]);
        action->setCheckable(true);
        action->setData(i);

        _groups[CHANNEL_GROUP]->addAction(action);
    }

    // Initialize.

    update();

    // Setup the callbacks.

    connect(
        djvViewImagePrefs::global(),
        SIGNAL(displayProfilesChanged(const QVector<djvViewDisplayProfile> &)),
        SLOT(update()));

    connect(
        djvViewShortcutPrefs::global(),
        SIGNAL(shortcutsChanged(const QVector<djvShortcut> &)),
        SLOT(update()));
}

djvViewImageActions::~djvViewImageActions()
{
    delete _p;
}

void djvViewImageActions::update()
{
    const QVector<djvShortcut> & shortcuts =
        djvViewShortcutPrefs::global()->shortcuts();

    // Update the actions.

    _actions[FRAME_STORE]->setShortcut(
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_FRAME_STORE].value);

    _actions[LOAD_FRAME_STORE]->setShortcut(
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_FRAME_STORE_LOAD].value);

    _actions[MIRROR_H]->setShortcut(
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_MIRROR_HORIZONTAL].value);

    _actions[MIRROR_V]->setShortcut(
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_MIRROR_VERTICAL].value);

    _actions[COLOR_PROFILE]->setShortcut(
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_COLOR_PROFILE].value);

    // Update the action groups.

    const QVector<QKeySequence> scaleShortcuts = QVector<QKeySequence>() <<
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_SCALE_NONE].value <<
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_SCALE_16_9].value <<
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_SCALE_1_0].value <<
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_SCALE_1_33].value <<
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_SCALE_1_78].value <<
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_SCALE_1_85].value <<
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_SCALE_2_0].value <<
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_SCALE_2_35].value <<
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_SCALE_2_39].value <<
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_SCALE_2_40].value <<
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_SCALE_1_1].value <<
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_SCALE_2_1].value <<
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_SCALE_3_2].value <<
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_SCALE_4_3].value <<
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_SCALE_5_3].value <<
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_SCALE_5_4].value;

    for (int i = 0; i < djvViewUtil::imageScaleLabels().count(); ++i)
    {
        _groups[SCALE_GROUP]->actions()[i]->setShortcut(scaleShortcuts[i]);
    }

    const QVector<QKeySequence> rotateShortcuts = QVector<QKeySequence>() <<
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_ROTATE_0].value <<
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_ROTATE_90].value <<
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_ROTATE_180].value <<
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_ROTATE_270].value;

    for (int i = 0; i < djvViewUtil::imageRotateLabels().count(); ++i)
    {
        _groups[ROTATE_GROUP]->actions()[i]->setShortcut(rotateShortcuts[i]);
    }

    Q_FOREACH(QAction * action, _groups[DISPLAY_PROFILE_GROUP]->actions())
        delete action;

    QStringList displayProfileNames;
    displayProfileNames += tr("Reset");
    displayProfileNames += djvViewImagePrefs::global()->displayProfileNames();

    const QVector<QKeySequence> displayProfileShortcuts = QVector<QKeySequence>() <<
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_DISPLAY_PROFILE_RESET].value <<
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_DISPLAY_PROFILE_1].value <<
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_DISPLAY_PROFILE_2].value <<
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_DISPLAY_PROFILE_3].value <<
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_DISPLAY_PROFILE_4].value <<
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_DISPLAY_PROFILE_5].value <<
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_DISPLAY_PROFILE_6].value <<
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_DISPLAY_PROFILE_7].value <<
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_DISPLAY_PROFILE_8].value <<
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_DISPLAY_PROFILE_9].value <<
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_DISPLAY_PROFILE_10].value;

    for (int i = 0; i < displayProfileNames.count(); ++i)
    {
        QAction * action = new QAction(this);
        action->setText(displayProfileNames[i]);
        if (i < displayProfileShortcuts.count())
            action->setShortcut(displayProfileShortcuts[i]);
        action->setData(i);

        _groups[DISPLAY_PROFILE_GROUP]->addAction(action);
    }

    const QVector<QKeySequence> channelShortcuts = QVector<QKeySequence>() <<
        QKeySequence() <<
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_CHANNEL_RED].value <<
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_CHANNEL_GREEN].value <<
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_CHANNEL_BLUE].value <<
        shortcuts[djvViewUtil::SHORTCUT_IMAGE_CHANNEL_ALPHA].value;

    for (int i = 0; i < djvOpenGlImageOptions::channelLabels().count(); ++i)
    {
        _groups[CHANNEL_GROUP]->actions()[i]->setShortcut(channelShortcuts[i]);
    }

    // Fix up the actions.
    
    osxMenuHack();

    // Emit changed signal.

    Q_EMIT changed();
}
