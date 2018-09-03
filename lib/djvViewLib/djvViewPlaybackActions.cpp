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

#include <djvViewPlaybackActions.h>

#include <djvViewContext.h>
#include <djvViewShortcutPrefs.h>

#include <djvIconLibrary.h>

#include <QApplication>

//------------------------------------------------------------------------------
// djvViewPlaybackActions::Private
//------------------------------------------------------------------------------

struct djvViewPlaybackActions::Private
{
    Private() :
        playback(static_cast<djvViewUtil::PLAYBACK>(0))
    {}

    djvViewUtil::PLAYBACK playback;
};

//------------------------------------------------------------------------------
// djvViewPlaybackActions
//------------------------------------------------------------------------------

djvViewPlaybackActions::djvViewPlaybackActions(
    djvViewContext * context,
    QObject *        parent) :
    djvViewAbstractActions(context, parent),
    _p(new Private)
{
    // Create the actions.
    for (int i = 0; i < ACTION_COUNT; ++i)
    {
        _actions[i] = new QAction(this);
    }
    _actions[PLAYBACK_TOGGLE]->setText(
        qApp->translate("djvViewPlaybackActions", "Toggle Playback"));
    _actions[PLAYBACK_TOGGLE]->setToolTip(
        qApp->translate("djvViewPlaybackActions", "Toggle playback"));
    _actions[EVERY_FRAME]->setText(
        qApp->translate("djvViewPlaybackActions", "Ever&y Frame"));
    _actions[EVERY_FRAME]->setCheckable(true);
    _actions[EVERY_FRAME]->setIcon(context->iconLibrary()->icon("djvLockIcon.png"));
    _actions[EVERY_FRAME]->setToolTip(
        qApp->translate("djvViewPlaybackActions", "Set whether every frame is played back"));
    
    // Create the action groups.
    for (int i = 0; i < GROUP_COUNT; ++i)
    {
        _groups[i] = new QActionGroup(this);
    }
    
    _groups[PLAYBACK_GROUP] = new QActionGroup(this);
    _groups[PLAYBACK_GROUP]->setExclusive(true);
    const QVector<QIcon> playbackIcons = QVector<QIcon>() <<
        context->iconLibrary()->icon("djvPlayReverseIcon.png") <<
        context->iconLibrary()->icon("djvPlayStopIcon.png") <<
        context->iconLibrary()->icon("djvPlayForwardIcon.png");
    for (int i = 0; i < djvViewUtil::PLAYBACK_COUNT; ++i)
    {
        QAction * action = new QAction(
            playbackIcons[i],
            djvViewUtil::playbackLabels()[i],
            _groups[PLAYBACK_GROUP]);
        action->setCheckable(true);
        action->setData(i);
    }

    _groups[LOOP_GROUP] = new QActionGroup(this);
    _groups[LOOP_GROUP]->setExclusive(true);
    const QVector<QIcon> loopIcons = QVector<QIcon>() <<
        context->iconLibrary()->icon("djvPlayLoopOnceIcon.png") <<
        context->iconLibrary()->icon("djvPlayLoopRepeatIcon.png") <<
        context->iconLibrary()->icon("djvPlayLoopPingPongIcon.png");
    for (int i = 0; i < djvViewUtil::LOOP_COUNT; ++i)
    {
        QAction * action = new QAction(
            loopIcons[i],
            djvViewUtil::loopLabels()[i],
            _groups[LOOP_GROUP]);
        action->setCheckable(true);
        action->setData(i);
    }

    _groups[FRAME_GROUP] = new QActionGroup(this);
    _groups[FRAME_GROUP]->setExclusive(false);
    const QVector<QIcon> frameIcons = QVector<QIcon>() <<
        context->iconLibrary()->icon("djvFrameStartIcon.png") <<
        QIcon() <<
        context->iconLibrary()->icon("djvFramePrevIcon.png") <<
        QIcon() <<
        QIcon() <<
        context->iconLibrary()->icon("djvFrameNextIcon.png") <<
        QIcon() <<
        QIcon() <<
        context->iconLibrary()->icon("djvFrameEndIcon.png") <<
        QIcon();
    for (int i = 0; i < djvViewUtil::FRAME_COUNT; ++i)
    {
        QAction * action = new QAction(
            frameIcons[i],
            djvViewUtil::frameLabels()[i],
            _groups[FRAME_GROUP]);
        action->setData(i);
    }

    _groups[IN_OUT_GROUP] = new QActionGroup(this);
    _groups[IN_OUT_GROUP]->setExclusive(false);
    const QVector<QIcon> inOutIcons = QVector<QIcon>() <<
        context->iconLibrary()->icon("djvPlayInOutIcon.png") <<
        context->iconLibrary()->icon("djvInPointMarkIcon.png") <<
        context->iconLibrary()->icon("djvOutPointMarkIcon.png") <<
        context->iconLibrary()->icon("djvInPointResetIcon.png") <<
        context->iconLibrary()->icon("djvOutPointResetIcon.png") <<
        QIcon();
    for (int i = 0; i < djvViewUtil::IN_OUT_COUNT; ++i)
    {
        QAction * action = new QAction(
            inOutIcons[i],
            djvViewUtil::inOutLabels()[i],
            _groups[IN_OUT_GROUP]);
        action->setData(i);
        if (djvViewUtil::IN_OUT_ENABLE == i)
        {
            action->setCheckable(true);
        }
    }

    _groups[LAYOUT_GROUP] = new QActionGroup(this);
    _groups[LAYOUT_GROUP]->setExclusive(true);
    for (int i = 0; i < djvViewUtil::LAYOUT_COUNT; ++i)
    {
        QAction * action = new QAction(this);
        action->setText(djvViewUtil::layoutLabels()[i]);
        action->setCheckable(true);
        action->setData(i);
        _groups[LAYOUT_GROUP]->addAction(action);
    }

    // Initialize.
    update();

    // Setup the callbacks.
    connect(
        context->shortcutPrefs(),
        SIGNAL(shortcutsChanged(const QVector<djvShortcut> &)),
        SLOT(update()));
}

djvViewPlaybackActions::~djvViewPlaybackActions()
{}

void djvViewPlaybackActions::update()
{
    const QVector<djvShortcut> & shortcuts =
        context()->shortcutPrefs()->shortcuts();
    
    // Update the actions.
    QKeySequence key = shortcuts[djvViewUtil::SHORTCUT_PLAYBACK_TOGGLE].value;
    _actions[PLAYBACK_TOGGLE]->setShortcut(key);
    _actions[PLAYBACK_TOGGLE]->setToolTip(
        qApp->translate("djvViewPlaybackActions", "Toggle playback\n\nShortcut: %1").
        arg(key.toString()));
    
    // Update the action groups.
    const QVector<djvViewUtil::SHORTCUT> playbackShortcuts =
        QVector<djvViewUtil::SHORTCUT>() <<
        djvViewUtil::SHORTCUT_PLAYBACK_REVERSE <<
        djvViewUtil::SHORTCUT_PLAYBACK_STOP <<
        djvViewUtil::SHORTCUT_PLAYBACK_FORWARD;
    const QStringList playbackToolTips = QStringList() <<
        qApp->translate("djvViewPlaybackActions", "Start reverse playback\n\nShortcut: %1") <<
        qApp->translate("djvViewPlaybackActions", "Stop playback\n\nShortcut: %1") <<
        qApp->translate("djvViewPlaybackActions", "Start forward playback\n\nShortcut: %1");
    for (int i = 0; i < djvViewUtil::PLAYBACK_COUNT; ++i)
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
    const QVector<djvViewUtil::SHORTCUT> frameShortcuts =
        QVector<djvViewUtil::SHORTCUT>() <<
        djvViewUtil::SHORTCUT_PLAYBACK_START <<
        djvViewUtil::SHORTCUT_PLAYBACK_START_ABS <<
        djvViewUtil::SHORTCUT_PLAYBACK_PREV <<
        djvViewUtil::SHORTCUT_PLAYBACK_PREV_10 <<
        djvViewUtil::SHORTCUT_PLAYBACK_PREV_100 <<
        djvViewUtil::SHORTCUT_PLAYBACK_NEXT <<
        djvViewUtil::SHORTCUT_PLAYBACK_NEXT_10 <<
        djvViewUtil::SHORTCUT_PLAYBACK_NEXT_100 <<
        djvViewUtil::SHORTCUT_PLAYBACK_END <<
        djvViewUtil::SHORTCUT_PLAYBACK_END_ABS;
    const QStringList frameToolTips = QStringList() <<
        qApp->translate("djvViewPlaybackActions", "Go to the start frame or in point\n\nShortcut: %1") <<
        QString() <<
        qApp->translate("djvViewPlaybackActions", "Go to the previous frame\n\nShortcut: %1") <<
        QString() <<
        QString() <<
        qApp->translate("djvViewPlaybackActions", "Go to the next frame\n\nShortcut: %1") <<
        QString() <<
        QString() <<
        qApp->translate("djvViewPlaybackActions", "Go to the end frame or out point\n\nShortcut: %1") <<
        QString();
    for (int i = 0; i < djvViewUtil::FRAME_COUNT; ++i)
    {
        const QKeySequence key = shortcuts[frameShortcuts[i]].value;
        _groups[FRAME_GROUP]->actions()[i]->setAutoRepeat(frameAutoRepeat[i]);
        _groups[FRAME_GROUP]->actions()[i]->setShortcut(key);
        if (! frameToolTips[i].isEmpty())
            _groups[FRAME_GROUP]->actions()[i]->setToolTip(
                frameToolTips[i].arg(key.toString()));
    }
    
    const QVector<djvViewUtil::SHORTCUT> inOutShortcuts =
        QVector<djvViewUtil::SHORTCUT>() <<
        djvViewUtil::SHORTCUT_PLAYBACK_IN_OUT <<
        djvViewUtil::SHORTCUT_PLAYBACK_MARK_IN <<
        djvViewUtil::SHORTCUT_PLAYBACK_MARK_OUT <<
        djvViewUtil::SHORTCUT_PLAYBACK_RESET_IN <<
        djvViewUtil::SHORTCUT_PLAYBACK_RESET_OUT;
    const QStringList inOutToolTips = QStringList() <<
        qApp->translate("djvViewPlaybackActions", "Enable in/out points\n\nShortcut: %1") <<
        qApp->translate("djvViewPlaybackActions", "Set the current frame as the in point\n\nShortcut: %1") <<
        qApp->translate("djvViewPlaybackActions", "Set the current frame as the out point\n\nShortcut: %1") <<
        qApp->translate("djvViewPlaybackActions", "Reset the in point\n\nShortcut: %1") <<
        qApp->translate("djvViewPlaybackActions", "Reset the out point\n\nShortcut: %1");
    for (int i = 0; i < djvViewUtil::IN_OUT_COUNT; ++i)
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
