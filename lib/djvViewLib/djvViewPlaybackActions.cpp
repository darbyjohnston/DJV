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

//! \file djvViewPlaybackActions.cpp

#include <djvViewPlaybackActions.h>

#include <djvViewShortcut.h>
#include <djvViewShortcutPrefs.h>

#include <djvIconLibrary.h>

#include <djvStringUtil.h>

//------------------------------------------------------------------------------
// djvViewPlaybackActions::P
//------------------------------------------------------------------------------

struct djvViewPlaybackActions::P
{
    P() :
        playback(static_cast<djvView::PLAYBACK>(0))
    {}

    djvView::PLAYBACK playback;
};

//------------------------------------------------------------------------------
// djvViewPlaybackActions
//------------------------------------------------------------------------------

djvViewPlaybackActions::djvViewPlaybackActions(QObject * parent) :
    djvViewAbstractActions(parent),
    _p(new P)
{
    // Create the actions.

    for (int i = 0; i < ACTION_COUNT; ++i)
    {
        _actions[i] = new QAction(this);
    }

    _actions[PLAYBACK_TOGGLE]->setText("Toggle Playback");
    _actions[PLAYBACK_TOGGLE]->setToolTip("Toggle playback");
    
    _actions[EVERY_FRAME]->setText("Ever&y Frame");
    _actions[EVERY_FRAME]->setCheckable(true);
    _actions[EVERY_FRAME]->setIcon(djvIconLibrary::global()->icon("djvLockIcon.png"));
    _actions[EVERY_FRAME]->setToolTip("Set whether every frame is played back");
    
    // Create the action groups.

    for (int i = 0; i < GROUP_COUNT; ++i)
    {
        _groups[i] = new QActionGroup(this);
    }

    _groups[PLAYBACK_GROUP] = new QActionGroup(this);
    _groups[PLAYBACK_GROUP]->setExclusive(true);

    const QVector<QIcon> playbackIcons = QVector<QIcon>() <<
        djvIconLibrary::global()->icon("djvPlayReverseIcon.png") <<
        djvIconLibrary::global()->icon("djvPlayStopIcon.png") <<
        djvIconLibrary::global()->icon("djvPlayForwardIcon.png");

    for (int i = 0; i < djvView::PLAYBACK_COUNT; ++i)
    {
        QAction * action = new QAction(
            playbackIcons[i],
            djvView::playbackLabels()[i],
            _groups[PLAYBACK_GROUP]);
        action->setCheckable(true);
        action->setData(i);
    }

    _groups[LOOP_GROUP] = new QActionGroup(this);
    _groups[LOOP_GROUP]->setExclusive(true);
    
    const QVector<QIcon> loopIcons = QVector<QIcon>() <<
        djvIconLibrary::global()->icon("djvPlayLoopOnceIcon.png") <<
        djvIconLibrary::global()->icon("djvPlayLoopRepeatIcon.png") <<
        djvIconLibrary::global()->icon("djvPlayLoopPingPongIcon.png");

    for (int i = 0; i < djvView::LOOP_COUNT; ++i)
    {
        QAction * action = new QAction(
            loopIcons[i],
            djvView::loopLabels()[i],
            _groups[LOOP_GROUP]);
        action->setCheckable(true);
        action->setData(i);
    }

    _groups[FRAME_GROUP] = new QActionGroup(this);
    _groups[FRAME_GROUP]->setExclusive(false);

    const QVector<QIcon> frameIcons = QVector<QIcon>() <<
        djvIconLibrary::global()->icon("djvFrameStartIcon.png") <<
        QIcon() <<
        djvIconLibrary::global()->icon("djvFramePrevIcon.png") <<
        QIcon() <<
        QIcon() <<
        djvIconLibrary::global()->icon("djvFrameNextIcon.png") <<
        QIcon() <<
        QIcon() <<
        djvIconLibrary::global()->icon("djvFrameEndIcon.png") <<
        QIcon();

    for (int i = 0; i < djvView::FRAME_COUNT; ++i)
    {
        QAction * action = new QAction(
            frameIcons[i],
            djvView::frameLabels()[i],
            _groups[FRAME_GROUP]);
        action->setData(i);
    }

    _groups[IN_OUT_GROUP] = new QActionGroup(this);
    _groups[IN_OUT_GROUP]->setExclusive(false);

    const QVector<QIcon> inOutIcons = QVector<QIcon>() <<
        djvIconLibrary::global()->icon("djvPlayInOutIcon.png") <<
        djvIconLibrary::global()->icon("djvInPointMarkIcon.png") <<
        djvIconLibrary::global()->icon("djvOutPointMarkIcon.png") <<
        djvIconLibrary::global()->icon("djvInPointResetIcon.png") <<
        djvIconLibrary::global()->icon("djvOutPointResetIcon.png") <<
        QIcon();

    for (int i = 0; i < djvView::IN_OUT_COUNT; ++i)
    {
        QAction * action = new QAction(
            inOutIcons[i],
            djvView::inOutLabels()[i],
            _groups[IN_OUT_GROUP]);
        action->setData(i);
        
        if (djvView::IN_OUT_ENABLE == i)
        {
            action->setCheckable(true);
        }
    }

    _groups[LAYOUT_GROUP] = new QActionGroup(this);
    _groups[LAYOUT_GROUP]->setExclusive(true);

    for (int i = 0; i < djvView::LAYOUT_COUNT; ++i)
    {
        QAction * action = new QAction(this);
        action->setText(djvView::layoutLabels()[i]);
        action->setCheckable(true);
        action->setData(i);

        _groups[LAYOUT_GROUP]->addAction(action);
    }

    // Initialize.

    update();

    // Setup the callbacks.

    connect(
        djvViewShortcutPrefs::global(),
        SIGNAL(shortcutsChanged(const QVector<djvShortcut> &)),
        SLOT(update()));
}

djvViewPlaybackActions::~djvViewPlaybackActions()
{
    delete _p;
}

void djvViewPlaybackActions::update()
{
    const QVector<djvShortcut> & shortcuts =
        djvViewShortcutPrefs::global()->shortcuts();
    
    // Update the actions.
    
    QKeySequence key = shortcuts[djvViewShortcut::PLAYBACK_TOGGLE].value;
    
    _actions[PLAYBACK_TOGGLE]->setShortcut(key);
    _actions[PLAYBACK_TOGGLE]->setToolTip(
        QString("Toggle playback\n\nShortcut: %1").arg(key.toString()));
    
    // Update the action groups.
    
    const QVector<djvViewShortcut::SHORTCUT> playbackShortcuts =
        QVector<djvViewShortcut::SHORTCUT>() <<
        djvViewShortcut::PLAYBACK_REVERSE <<
        djvViewShortcut::PLAYBACK_STOP <<
        djvViewShortcut::PLAYBACK_FORWARD;

    const QStringList playbackToolTips = QStringList() <<
        "Start reverse playback\n\nShortcut: %1" <<
        "Stop playback\n\nShortcut: %1" <<
        "Start forward playback\n\nShortcut: %1";

    for (int i = 0; i < djvView::PLAYBACK_COUNT; ++i)
    {
        const QKeySequence key = shortcuts[playbackShortcuts[i]].value;

        _groups[PLAYBACK_GROUP]->actions()[i]->setShortcut(key);
        _groups[PLAYBACK_GROUP]->actions()[i]->setToolTip(
            playbackToolTips[i].arg(key.toString()));
    }
    
    const QVector<bool> frameAutoRepeat = QVector<bool>() <<
        false <<
        false <<
        true  <<
        true  <<
        true  <<
        true  <<
        true  <<
        true  <<
        false <<
        false;

    const QVector<djvViewShortcut::SHORTCUT> frameShortcuts =
        QVector<djvViewShortcut::SHORTCUT>() <<
        djvViewShortcut::PLAYBACK_START <<
        djvViewShortcut::PLAYBACK_START_ABS <<
        djvViewShortcut::PLAYBACK_PREV <<
        djvViewShortcut::PLAYBACK_PREV_10 <<
        djvViewShortcut::PLAYBACK_PREV_100 <<
        djvViewShortcut::PLAYBACK_NEXT <<
        djvViewShortcut::PLAYBACK_NEXT_10 <<
        djvViewShortcut::PLAYBACK_NEXT_100 <<
        djvViewShortcut::PLAYBACK_END <<
        djvViewShortcut::PLAYBACK_END_ABS;

    const QStringList frameToolTips = QStringList() <<
        "Go to the start frame or in point\n\nShortcut: %1" <<
        QString() <<
        "Go to the previous frame\n\nShortcut: %1" <<
        QString() <<
        QString() <<
        "Go to the next frame\n\nShortcut: %1" <<
        QString() <<
        QString() <<
        "Go to the end frame or out point\n\nShortcut: %1" <<
        QString();

    for (int i = 0; i < djvView::FRAME_COUNT; ++i)
    {
        const QKeySequence key = shortcuts[frameShortcuts[i]].value;

        _groups[FRAME_GROUP]->actions()[i]->setAutoRepeat(frameAutoRepeat[i]);
        _groups[FRAME_GROUP]->actions()[i]->setShortcut(key);
        if (! frameToolTips[i].isEmpty())
            _groups[FRAME_GROUP]->actions()[i]->setToolTip(
                frameToolTips[i].arg(key.toString()));
    }
    
    const QVector<djvViewShortcut::SHORTCUT> inOutShortcuts =
        QVector<djvViewShortcut::SHORTCUT>() <<
        djvViewShortcut::PLAYBACK_IN_OUT <<
        djvViewShortcut::PLAYBACK_MARK_IN <<
        djvViewShortcut::PLAYBACK_MARK_OUT <<
        djvViewShortcut::PLAYBACK_RESET_IN <<
        djvViewShortcut::PLAYBACK_RESET_OUT;

    const QStringList inOutToolTips = QStringList() <<
        "Enable in/out points\n\nShortcut: %1" <<
        "Set the current frame as the in point\n\nShortcut: %1" <<
        "Set the current frame as the out point\n\nShortcut: %1" <<
        "Reset the in point\n\nShortcut: %1" <<
        "Reset the out point\n\nShortcut: %1";

    for (int i = 0; i < djvView::IN_OUT_COUNT; ++i)
    {
        const QKeySequence key = shortcuts[inOutShortcuts[i]].value;

        _groups[IN_OUT_GROUP]->actions()[i]->setShortcut(key);
        _groups[IN_OUT_GROUP]->actions()[i]->setToolTip(
            inOutToolTips[i].arg(key.toString()));
    }

    // Fix up the actions.
    
    osxMenuHack();

    // Emit changed signal.

    Q_EMIT changed();
}
