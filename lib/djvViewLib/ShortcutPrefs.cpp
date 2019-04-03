//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
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

#include <djvViewLib/ShortcutPrefs.h>

#include <djvViewLib/Enum.h>

#include <djvUI/Prefs.h>

#include <djvCore/Assert.h>

#include <iostream>

namespace djv
{
    namespace ViewLib
    {
        namespace
        {
            const QVector<UI::Shortcut> shortcutsDefault = QVector<UI::Shortcut>() <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_EXIT], QKeySequence("Ctrl+Q")) <<

                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_FILE_OPEN], QKeySequence("Ctrl+O")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_FILE_RELOAD], QKeySequence("Ctrl+R")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_FILE_RELOAD_FRAME], QKeySequence("Ctrl+Shift+R")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_FILE_EXPORT_SEQUENCE], QKeySequence("Ctrl+S")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_FILE_EXPORT_FRAME], QKeySequence("Ctrl+Shift+S")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_FILE_CLOSE], QKeySequence("Ctrl+W")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_FILE_LAYER_DEFAULT], QKeySequence("Ctrl+`")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_FILE_LAYER_1], QKeySequence("Ctrl+1")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_FILE_LAYER_2], QKeySequence("Ctrl+2")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_FILE_LAYER_3], QKeySequence("Ctrl+3")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_FILE_LAYER_4], QKeySequence("Ctrl+4")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_FILE_LAYER_5], QKeySequence("Ctrl+5")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_FILE_LAYER_6], QKeySequence("Ctrl+6")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_FILE_LAYER_7], QKeySequence("Ctrl+7")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_FILE_LAYER_8], QKeySequence("Ctrl+8")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_FILE_LAYER_9], QKeySequence("Ctrl+9")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_FILE_LAYER_10], QKeySequence("Ctrl+0")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_FILE_LAYER_NEXT], QKeySequence("Ctrl+=")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_FILE_LAYER_PREV], QKeySequence("Ctrl+-")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_FILE_PROXY_NONE], QKeySequence()) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_FILE_PROXY_1_2], QKeySequence()) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_FILE_PROXY_1_4], QKeySequence()) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_FILE_PROXY_1_8], QKeySequence()) <<

                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_WINDOW_NEW], QKeySequence("Ctrl+N")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_WINDOW_DUPLICATE], QKeySequence("Ctrl+D")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_WINDOW_CLOSE], QKeySequence("Ctrl+E")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_WINDOW_FIT], QKeySequence("F")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_WINDOW_FULL_SCREEN], QKeySequence("U")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_WINDOW_UI_VISIBLE], QKeySequence("`")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_WINDOW_FILE_TOOL_BAR_VISIBLE], QKeySequence()) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_WINDOW_WINDOW_TOOL_BAR_VISIBLE], QKeySequence()) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_WINDOW_VIEW_TOOL_BAR_VISIBLE], QKeySequence()) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_WINDOW_IMAGE_TOOL_BAR_VISIBLE], QKeySequence()) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_WINDOW_TOOLS_TOOL_BAR_VISIBLE], QKeySequence()) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_WINDOW_PLAYBACK_VISIBLE], QKeySequence()) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_WINDOW_INFO_VISIBLE], QKeySequence()) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_WINDOW_DETACH_CONTROLS], QKeySequence("Ctrl+T")) <<

                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_VIEW_LEFT], QKeySequence("Left")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_VIEW_RIGHT], QKeySequence("Right")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_VIEW_UP], QKeySequence("Up")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_VIEW_DOWN], QKeySequence("Down")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_VIEW_CENTER], QKeySequence(Qt::Key_Insert)) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_VIEW_ZOOM_IN], QKeySequence("=")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_VIEW_ZOOM_OUT], QKeySequence("-")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_VIEW_ZOOM_RESET], QKeySequence("0")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_VIEW_RESET], QKeySequence(Qt::Key_Delete)) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_VIEW_FIT], QKeySequence(Qt::Key_Backspace)) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_VIEW_HUD], QKeySequence("H")) <<

                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_SHOW_FRAME_STORE], QKeySequence("E")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_SET_FRAME_STORE], QKeySequence("Shift+E")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_MIRROR_HORIZONTAL], QKeySequence()) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_MIRROR_VERTICAL], QKeySequence()) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_SCALE_NONE], QKeySequence()) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_SCALE_16_9], QKeySequence()) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_SCALE_1_0], QKeySequence()) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_SCALE_1_33], QKeySequence()) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_SCALE_1_78], QKeySequence()) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_SCALE_1_85], QKeySequence()) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_SCALE_2_0], QKeySequence()) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_SCALE_2_35], QKeySequence()) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_SCALE_2_39], QKeySequence()) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_SCALE_2_40], QKeySequence()) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_SCALE_1_1], QKeySequence()) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_SCALE_2_1], QKeySequence()) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_SCALE_3_2], QKeySequence()) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_SCALE_4_3], QKeySequence()) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_SCALE_5_3], QKeySequence()) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_SCALE_5_4], QKeySequence()) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_ROTATE_0], QKeySequence()) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_ROTATE_90], QKeySequence()) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_ROTATE_180], QKeySequence()) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_ROTATE_270], QKeySequence()) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_PREMULTIPLIED_ALPHA], QKeySequence()) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_COLOR_PROFILE], QKeySequence()) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_CHANNEL_DEFAULT], QKeySequence("C")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_CHANNEL_RED], QKeySequence("R")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_CHANNEL_GREEN], QKeySequence("G")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_CHANNEL_BLUE], QKeySequence("B")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_CHANNEL_ALPHA], QKeySequence("A")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_DISPLAY_PROFILE], QKeySequence()) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_DISPLAY_PROFILE_RESET], QKeySequence("Shift+`")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_DISPLAY_PROFILE_1], QKeySequence("Shift+1")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_DISPLAY_PROFILE_2], QKeySequence("Shift+2")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_DISPLAY_PROFILE_3], QKeySequence("Shift+3")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_DISPLAY_PROFILE_4], QKeySequence("Shift+4")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_DISPLAY_PROFILE_5], QKeySequence("Shift+5")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_DISPLAY_PROFILE_6], QKeySequence("Shift+6")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_DISPLAY_PROFILE_7], QKeySequence("Shift+7")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_DISPLAY_PROFILE_8], QKeySequence("Shift+8")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_DISPLAY_PROFILE_9], QKeySequence("Shift+9")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_IMAGE_DISPLAY_PROFILE_10], QKeySequence("Shift+0")) <<

                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_PLAYBACK_FORWARD], QKeySequence("L")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_PLAYBACK_STOP], QKeySequence("K")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_PLAYBACK_REVERSE], QKeySequence("J")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_PLAYBACK_TOGGLE], QKeySequence("Space")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_PLAYBACK_LOOP], QKeySequence(";")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_PLAYBACK_START], QKeySequence(Qt::Key_Home)) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_PLAYBACK_END], QKeySequence(Qt::Key_End)) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_PLAYBACK_NEXT], QKeySequence(']')) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_PLAYBACK_NEXT_10], QKeySequence('}')) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_PLAYBACK_NEXT_100], QKeySequence(Qt::CTRL + ']')) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_PLAYBACK_PREV], QKeySequence('[')) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_PLAYBACK_PREV_10], QKeySequence('{')) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_PLAYBACK_PREV_100], QKeySequence(Qt::CTRL + '[')) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_PLAYBACK_IN_OUT_POINTS], QKeySequence("P")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_PLAYBACK_MARK_IN], QKeySequence("I")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_PLAYBACK_RESET_IN], QKeySequence("Shift+I")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_PLAYBACK_MARK_OUT], QKeySequence("O")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_PLAYBACK_RESET_OUT], QKeySequence("Shift+O")) <<

                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_TOOL_MAGNIFY], QKeySequence("1")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_TOOL_COLOR_PICKER], QKeySequence("2")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_TOOL_HISTOGRAM], QKeySequence("3")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_TOOL_INFO], QKeySequence("4")) <<
                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_TOOL_ANNOTATE], QKeySequence("Ctrl+A")) <<

                UI::Shortcut(Enum::shortcutLabels()[Enum::SHORTCUT_HELP_WHATS_THIS], QKeySequence("F1"));

        } // namespace

        ShortcutPrefs::ShortcutPrefs(const QPointer<ViewContext> & context, QObject * parent) :
            AbstractPrefs(context, parent),
            _shortcuts(shortcutsDefault)
        {
            UI::Prefs prefs("djv::ViewLib::ShortcutPrefs");
            for (int i = 0; i < _shortcuts.count(); ++i)
            {
                QStringList tmp;
                if (prefs.get(_shortcuts[i].name, tmp))
                {
                    UI::Shortcut::serialize(tmp, _shortcuts[i].value);
                }
            }
        }

        ShortcutPrefs::~ShortcutPrefs()
        {
            UI::Prefs prefs("djv::ViewLib::ShortcutPrefs");
            for (int i = 0; i < _shortcuts.count(); ++i)
            {
                prefs.set(
                    _shortcuts[i].name,
                    UI::Shortcut::serialize(_shortcuts[i].value));
            }
        }

        const QVector<UI::Shortcut> & ShortcutPrefs::shortcuts() const
        {
            return _shortcuts;
        }

        void ShortcutPrefs::reset()
        {
            setShortcuts(shortcutsDefault);
        }

        void ShortcutPrefs::setShortcuts(const QVector<UI::Shortcut> & in)
        {
            if (in == _shortcuts)
                return;
            _shortcuts = in;
            Q_EMIT shortcutsChanged(_shortcuts);
            Q_EMIT prefChanged();
        }

    } // namespace ViewLib
} // namespace djv

