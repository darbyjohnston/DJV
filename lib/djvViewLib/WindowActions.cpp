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

#include <djvViewLib/WindowActions.h>

#include <djvViewLib/Context.h>
#include <djvViewLib/ShortcutPrefs.h>
#include <djvViewLib/Util.h>

#include <djvUI/IconLibrary.h>
#include <djvUI/Shortcut.h>

#include <QAction>
#include <QActionGroup>
#include <QApplication>

namespace djv
{
    namespace ViewLib
    {
        WindowActions::WindowActions(
            Context * context,
            QObject * parent) :
            AbstractActions(context, parent)
        {
            // Create the actions.
            for (int i = 0; i < ACTION_COUNT; ++i)
            {
                _actions[i] = new QAction(this);
            }
            _actions[NEW]->setText(qApp->translate("djv::ViewLib::WindowActions", "&New"));
            _actions[NEW]->setIcon(context->iconLibrary()->icon("djvWindowNewIcon.png"));
            _actions[COPY]->setText(qApp->translate("djv::ViewLib::WindowActions", "&Copy"));
            _actions[COPY]->setIcon(context->iconLibrary()->icon("djvWindowCopyIcon.png"));
            _actions[CLOSE]->setText(qApp->translate("djv::ViewLib::WindowActions", "Clos&e"));
            _actions[CLOSE]->setIcon(context->iconLibrary()->icon("djvWindowCloseIcon.png"));
            _actions[FIT]->setText(qApp->translate("djv::ViewLib::WindowActions", "&Fit"));
            _actions[FIT]->setIcon(context->iconLibrary()->icon("djvWindowFitIcon.png"));
            _actions[FULL_SCREEN]->setText(qApp->translate("djv::ViewLib::WindowActions", "F&ull Screen"));
            _actions[FULL_SCREEN]->setIcon(context->iconLibrary()->icon("djvWindowFullScreenIcon.png"));
            _actions[FULL_SCREEN]->setCheckable(true);
            _actions[CONTROLS_VISIBLE]->setText(qApp->translate("djv::ViewLib::WindowActions", "Show &Controls"));
            _actions[CONTROLS_VISIBLE]->setCheckable(true);

            // Create the action groups.
            for (int i = 0; i < GROUP_COUNT; ++i)
            {
                _groups[i] = new QActionGroup(this);
            }
            _groups[TOOL_BAR_VISIBLE_GROUP]->setExclusive(false);
            for (int i = 0; i < Util::toolBarLabels().count(); ++i)
            {
                QAction * action = new QAction(this);
                action->setText(Util::toolBarLabels()[i]);
                action->setCheckable(true);
                action->setData(i);
                _groups[TOOL_BAR_VISIBLE_GROUP]->addAction(action);
            }

            // Initialize.
            update();

            // Setup the callbacks.
            connect(
                context->shortcutPrefs(),
                SIGNAL(shortcutsChanged(const QVector<djvShortcut> &)),
                SLOT(update()));
        }

        WindowActions::~WindowActions()
        {}

        void WindowActions::update()
        {
            const QVector<djvShortcut> & shortcuts =
                context()->shortcutPrefs()->shortcuts();

            // Update the actions.
            _actions[NEW]->setShortcut(
                shortcuts[Util::SHORTCUT_WINDOW_NEW].value);
            _actions[NEW]->setToolTip(
                qApp->translate("djv::ViewLib::WindowActions", "Create a new window\n\nShortcut: %1").
                arg(shortcuts[Util::SHORTCUT_WINDOW_NEW].value.toString()));
            _actions[COPY]->setShortcut(
                shortcuts[Util::SHORTCUT_WINDOW_COPY].value);
            _actions[COPY]->setToolTip(
                qApp->translate("djv::ViewLib::WindowActions", "Copy the current window\n\nShortcut: %1").
                arg(shortcuts[Util::SHORTCUT_WINDOW_COPY].value.toString()));
            _actions[CLOSE]->setShortcut(
                shortcuts[Util::SHORTCUT_WINDOW_CLOSE].value);
            _actions[CLOSE]->setToolTip(
                qApp->translate("djv::ViewLib::WindowActions", "Close the current window\n\nShortcut: %1").
                arg(shortcuts[Util::SHORTCUT_WINDOW_CLOSE].value.toString()));
            _actions[FIT]->setShortcut(
                shortcuts[Util::SHORTCUT_WINDOW_FIT].value);
            _actions[FIT]->setToolTip(
                qApp->translate("djv::ViewLib::WindowActions", "Resize the window to fit the image\n\nShortcut: %1").
                arg(shortcuts[Util::SHORTCUT_WINDOW_FIT].value.toString()));
            _actions[FULL_SCREEN]->setShortcut(
                shortcuts[Util::SHORTCUT_WINDOW_FULL_SCREEN].value);
            _actions[FULL_SCREEN]->setToolTip(
                qApp->translate("djv::ViewLib::WindowActions", "Show the window full screen\n\nShortcut: %1").
                arg(shortcuts[Util::SHORTCUT_WINDOW_FULL_SCREEN].value.toString()));
            _actions[CONTROLS_VISIBLE]->setShortcut(
                shortcuts[Util::SHORTCUT_WINDOW_CONTROLS_VISIBLE].value);
            _actions[CONTROLS_VISIBLE]->setToolTip(
                qApp->translate("djv::ViewLib::WindowActions", "Toggle the user interface controls visbility\n\nShortcut: %1").
                arg(shortcuts[Util::SHORTCUT_WINDOW_CONTROLS_VISIBLE].value.toString()));
            const Util::SHORTCUT toolBarShortcuts[] =
            {
                Util::SHORTCUT_WINDOW_TOOL_BARS_VISIBLE,
                Util::SHORTCUT_WINDOW_PLAYBACK_VISIBLE,
                Util::SHORTCUT_WINDOW_INFO_VISIBLE
            };
            const int toolBarShortcutsCount =
                sizeof(toolBarShortcuts) / sizeof(toolBarShortcuts[0]);
            for (int i = 0; i < toolBarShortcutsCount; ++i)
            {
                _groups[TOOL_BAR_VISIBLE_GROUP]->actions()[i]->setShortcut(
                    shortcuts[toolBarShortcuts[i]].value);
            }

            // Fix up the actions.
            osxMenuHack();
        }

    } // namespace ViewLib
} // namespace djv
