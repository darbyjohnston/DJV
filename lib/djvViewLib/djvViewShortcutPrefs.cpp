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

//! \file djvViewShortcutPrefs.cpp

#include <djvViewShortcutPrefs.h>

#include <djvViewApplication.h>
#include <djvViewUtil.h>

#include <djvPrefs.h>

#include <djvAssert.h>

//------------------------------------------------------------------------------
// djvViewShortcutPrefs
//------------------------------------------------------------------------------

djvViewShortcutPrefs::djvViewShortcutPrefs(QObject * parent) :
    djvViewAbstractPrefs(parent),
    _shortcuts(shortcutsDefault())
{
    djvPrefs prefs("djvViewShortcutPrefs");

    for (int i = 0; i < _shortcuts.count(); ++i)
    {
        QStringList tmp;
        
        if (prefs.get(_shortcuts[i].name, tmp))
        {
            djvShortcut::serialize(tmp, _shortcuts[i].value);
        }
    }
}

djvViewShortcutPrefs::~djvViewShortcutPrefs()
{
    djvPrefs prefs("djvViewShortcutPrefs");

    for (int i = 0; i < _shortcuts.count(); ++i)
    {
        prefs.set(
            _shortcuts[i].name,
            djvShortcut::serialize(_shortcuts[i].value));
    }
}

const QVector<djvShortcut> & djvViewShortcutPrefs::shortcutsDefault()
{
    static const QVector<djvShortcut> data = QVector<djvShortcut>() <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_EXIT], QKeySequence("Ctrl+Q")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_FILE_OPEN], QKeySequence("Ctrl+O")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_FILE_RELOAD], QKeySequence("Ctrl+R")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_FILE_RELOAD_FRAME], QKeySequence("Ctrl+Shift+R")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_FILE_SAVE], QKeySequence("Ctrl+S")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_FILE_SAVE_FRAME], QKeySequence("Ctrl+Shift+S")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_FILE_CLOSE], QKeySequence("Ctrl+W")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_FILE_LAYER_DEFAULT], QKeySequence("Ctrl+`")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_FILE_LAYER_1], QKeySequence("Ctrl+1")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_FILE_LAYER_2], QKeySequence("Ctrl+2")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_FILE_LAYER_3], QKeySequence("Ctrl+3")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_FILE_LAYER_4], QKeySequence("Ctrl+4")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_FILE_LAYER_5], QKeySequence("Ctrl+5")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_FILE_LAYER_6], QKeySequence("Ctrl+6")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_FILE_LAYER_7], QKeySequence("Ctrl+7")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_FILE_LAYER_8], QKeySequence("Ctrl+8")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_FILE_LAYER_9], QKeySequence("Ctrl+9")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_FILE_LAYER_10], QKeySequence("Ctrl+0")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_FILE_LAYER_PREV], QKeySequence("Ctrl+-")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_FILE_LAYER_NEXT], QKeySequence("Ctrl+=")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_FILE_PROXY_NONE], QKeySequence()) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_FILE_PROXY_1_2], QKeySequence()) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_FILE_PROXY_1_4], QKeySequence()) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_FILE_PROXY_1_8], QKeySequence()) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_WINDOW_NEW], QKeySequence("Ctrl+n")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_WINDOW_COPY], QKeySequence("Ctrl+c")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_WINDOW_CLOSE], QKeySequence("Ctrl+e")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_WINDOW_FIT], QKeySequence("F")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_WINDOW_FULL_SCREEN], QKeySequence("U")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_WINDOW_CONTROLS_VISIBLE], QKeySequence("C")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_WINDOW_TOOL_BARS_VISIBLE], QKeySequence()) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_WINDOW_PLAYBACK_VISIBLE], QKeySequence()) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_WINDOW_INFO_VISIBLE], QKeySequence()) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_VIEW_LEFT], QKeySequence("Left")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_VIEW_RIGHT], QKeySequence("Right")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_VIEW_UP], QKeySequence("Up")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_VIEW_DOWN], QKeySequence("Down")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_VIEW_CENTER], QKeySequence(Qt::Key_Insert)) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_VIEW_ZOOM_IN], QKeySequence("=")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_VIEW_ZOOM_OUT], QKeySequence("-")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_VIEW_ZOOM_RESET], QKeySequence("0")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_VIEW_RESET], QKeySequence(Qt::Key_Delete)) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_VIEW_FIT], QKeySequence(Qt::Key_Backspace)) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_VIEW_HUD], QKeySequence("H")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_FRAME_STORE], QKeySequence("E")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_FRAME_STORE_LOAD], QKeySequence("Shift+E")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_MIRROR_HORIZONTAL], QKeySequence(Qt::Key_F1)) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_MIRROR_VERTICAL], QKeySequence(Qt::Key_F2)) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_SCALE_NONE], QKeySequence(Qt::Key_F5)) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_SCALE_16_9], QKeySequence(Qt::Key_F6)) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_SCALE_1_0], QKeySequence()) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_SCALE_1_33], QKeySequence()) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_SCALE_1_78], QKeySequence()) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_SCALE_1_85], QKeySequence(Qt::Key_F7)) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_SCALE_2_0], QKeySequence()) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_SCALE_2_35], QKeySequence(Qt::Key_F8)) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_SCALE_2_39], QKeySequence()) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_SCALE_2_40], QKeySequence()) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_SCALE_1_1], QKeySequence()) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_SCALE_2_1], QKeySequence()) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_SCALE_3_2], QKeySequence()) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_SCALE_4_3], QKeySequence()) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_SCALE_5_3], QKeySequence()) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_SCALE_5_4], QKeySequence()) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_ROTATE_0], QKeySequence(Qt::Key_F9)) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_ROTATE_90], QKeySequence(Qt::Key_F10)) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_ROTATE_180], QKeySequence(Qt::Key_F11)) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_ROTATE_270], QKeySequence(Qt::Key_F12)) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_COLOR_PROFILE], QKeySequence()) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_CHANNEL_RED], QKeySequence("R")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_CHANNEL_GREEN], QKeySequence("G")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_CHANNEL_BLUE], QKeySequence("B")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_CHANNEL_ALPHA], QKeySequence("A")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_DISPLAY_PROFILE], QKeySequence()) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_DISPLAY_PROFILE_RESET], QKeySequence("Shift+`")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_DISPLAY_PROFILE_1], QKeySequence("Shift+1")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_DISPLAY_PROFILE_2], QKeySequence("Shift+2")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_DISPLAY_PROFILE_3], QKeySequence("Shift+3")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_DISPLAY_PROFILE_4], QKeySequence("Shift+4")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_DISPLAY_PROFILE_5], QKeySequence("Shift+5")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_DISPLAY_PROFILE_6], QKeySequence("Shift+6")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_DISPLAY_PROFILE_7], QKeySequence("Shift+7")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_DISPLAY_PROFILE_8], QKeySequence("Shift+8")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_DISPLAY_PROFILE_9], QKeySequence("Shift+9")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_IMAGE_DISPLAY_PROFILE_10], QKeySequence("Shift+0")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_PLAYBACK_REVERSE], QKeySequence("J")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_PLAYBACK_STOP], QKeySequence("K")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_PLAYBACK_FORWARD], QKeySequence("L")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_PLAYBACK_TOGGLE], QKeySequence("Space")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_PLAYBACK_LOOP], QKeySequence(";")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_PLAYBACK_START], QKeySequence(Qt::Key_Home)) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_PLAYBACK_START_ABS], QKeySequence(Qt::SHIFT + Qt::Key_Home)) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_PLAYBACK_PREV], QKeySequence('[')) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_PLAYBACK_PREV_10], QKeySequence('{')) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_PLAYBACK_PREV_100], QKeySequence(Qt::CTRL + '[')) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_PLAYBACK_NEXT], QKeySequence(']')) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_PLAYBACK_NEXT_10], QKeySequence('}')) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_PLAYBACK_NEXT_100], QKeySequence(Qt::CTRL + ']')) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_PLAYBACK_END], QKeySequence(Qt::Key_End)) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_PLAYBACK_END_ABS], QKeySequence(Qt::SHIFT + Qt::Key_End)) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_PLAYBACK_IN_OUT], QKeySequence("P")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_PLAYBACK_MARK_IN], QKeySequence("I")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_PLAYBACK_RESET_IN], QKeySequence("Shift+I")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_PLAYBACK_MARK_OUT], QKeySequence("O")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_PLAYBACK_RESET_OUT], QKeySequence("Shift+O")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_TOOL_MAGNIFY], QKeySequence("1")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_TOOL_COLOR_PICKER], QKeySequence("2")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_TOOL_HISTOGRAM], QKeySequence("3")) <<
        djvShortcut(djvViewUtil::shortcutLabels()[
            djvViewUtil::SHORTCUT_TOOL_INFO], QKeySequence("4"));

    DJV_ASSERT(data.count() == djvViewUtil::SHORTCUT_COUNT);

    return data;
}

const QVector<djvShortcut> & djvViewShortcutPrefs::shortcuts() const
{
    return _shortcuts;
}

djvViewShortcutPrefs * djvViewShortcutPrefs::global()
{
    static djvViewShortcutPrefs * prefs = 0;

    if (! prefs)
    {
        prefs = new djvViewShortcutPrefs(DJV_VIEW_APP);
    }

    return prefs;
}

void djvViewShortcutPrefs::setShortcuts(const QVector<djvShortcut> & in)
{
    if (in == _shortcuts)
        return;

    _shortcuts = in;

    Q_EMIT shortcutsChanged(_shortcuts);
    Q_EMIT prefChanged();
}
