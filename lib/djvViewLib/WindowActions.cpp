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

#include <djvViewLib/WindowActions.h>

#include <djvViewLib/ShortcutPrefs.h>
#include <djvViewLib/Util.h>
#include <djvViewLib/ViewContext.h>

#include <djvUI/IconLibrary.h>
#include <djvUI/Shortcut.h>
#include <djvUI/StylePrefs.h>

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QPointer>

namespace djv
{
    namespace ViewLib
    {
        WindowActions::WindowActions(
            const QPointer<ViewContext> & context,
            QObject * parent) :
            AbstractActions(context, parent)
        {
            // Create the actions.
            for (int i = 0; i < ACTION_COUNT; ++i)
            {
                _actions[i] = new QAction(this);
            }
            _actions[NEW]->setText(qApp->translate("djv::ViewLib::WindowActions", "&New"));
            _actions[DUPLICATE]->setText(qApp->translate("djv::ViewLib::WindowActions", "&Duplicate"));
            _actions[CLOSE]->setText(qApp->translate("djv::ViewLib::WindowActions", "Clos&e"));
            _actions[FIT]->setText(qApp->translate("djv::ViewLib::WindowActions", "&Fit"));
            _actions[FULL_SCREEN]->setText(qApp->translate("djv::ViewLib::WindowActions", "F&ull Screen"));
            _actions[FULL_SCREEN]->setCheckable(true);
            _actions[UI_VISIBLE]->setText(qApp->translate("djv::ViewLib::WindowActions", "User &Interface"));
            _actions[UI_VISIBLE]->setCheckable(true);
            _actions[DETACH_CONTROLS]->setText(qApp->translate("djv::ViewLib::ViewActions", "De&tach Controls"));
            _actions[DETACH_CONTROLS]->setCheckable(true);

            // Create the action groups.
            for (int i = 0; i < GROUP_COUNT; ++i)
            {
                _groups[i] = new QActionGroup(this);
            }
            _groups[UI_COMPONENT_VISIBLE_GROUP]->setExclusive(false);
            for (int i = 0; i < Enum::uiComponentLabels().count(); ++i)
            {
                auto action = new QAction(this);
                action->setText(Enum::uiComponentLabels()[i]);
                action->setCheckable(true);
                action->setData(i);
                _groups[UI_COMPONENT_VISIBLE_GROUP]->addAction(action);
            }

            // Initialize.
            update();

            // Setup the callbacks.
            connect(
                context->shortcutPrefs(),
                SIGNAL(shortcutsChanged(const QVector<djv::UI::Shortcut> &)),
                SLOT(update()));
            connect(
                context->stylePrefs(),
                SIGNAL(prefChanged()),
                SLOT(update()));
        }

        WindowActions::~WindowActions()
        {}

        void WindowActions::update()
        {
            const QVector<UI::Shortcut> & shortcuts = context()->shortcutPrefs()->shortcuts();

            _actions[NEW]->setIcon(context()->iconLibrary()->icon("djv/UI/WindowNewIcon"));
            _actions[NEW]->setShortcut(shortcuts[Enum::SHORTCUT_WINDOW_NEW].value);
            _actions[NEW]->setToolTip(
                qApp->translate("djv::ViewLib::WindowActions",
                "Create a new window<br><br>"
                "Keyboard shortcut: %1").
                arg(shortcuts[Enum::SHORTCUT_WINDOW_NEW].value.toString()));
            _actions[NEW]->setWhatsThis(
                qApp->translate("djv::ViewLib::WindowActions",
                "Create a new window<br><br>"
                "Keyboard shortcut: %1<br><br>"
                "<a href=\"ViewWindows.html\">Documentation</a>").
                arg(shortcuts[Enum::SHORTCUT_WINDOW_NEW].value.toString()));
            
            _actions[DUPLICATE]->setIcon(context()->iconLibrary()->icon("djv/UI/WindowDuplicateIcon"));
            _actions[DUPLICATE]->setShortcut(shortcuts[Enum::SHORTCUT_WINDOW_DUPLICATE].value);
            _actions[DUPLICATE]->setToolTip(
                qApp->translate("djv::ViewLib::WindowActions",
                "Duplicate the window<br><br>"
                "Keyboard shortcut: %1").
                arg(shortcuts[Enum::SHORTCUT_WINDOW_DUPLICATE].value.toString()));
            _actions[DUPLICATE]->setWhatsThis(
                qApp->translate("djv::ViewLib::WindowActions",
                "Duplicate the window<br><br>"
                "Keyboard shortcut: %1<br><br>"
                "<a href=\"ViewWindows.html\">Documentation</a>").
                arg(shortcuts[Enum::SHORTCUT_WINDOW_DUPLICATE].value.toString()));
            
            _actions[CLOSE]->setIcon(context()->iconLibrary()->icon("djv/UI/WindowCloseIcon"));
            _actions[CLOSE]->setShortcut(shortcuts[Enum::SHORTCUT_WINDOW_CLOSE].value);
            _actions[CLOSE]->setToolTip(
                qApp->translate("djv::ViewLib::WindowActions",
                "Close the window<br><br>"
                "Keyboard shortcut: %1").
                arg(shortcuts[Enum::SHORTCUT_WINDOW_CLOSE].value.toString()));
            _actions[CLOSE]->setWhatsThis(
                qApp->translate("djv::ViewLib::WindowActions",
                "Close the window<br><br>"
                "Keyboard shortcut: %1<br><br>"
                "<a href=\"ViewWindows.html\">Documentation</a>").
                arg(shortcuts[Enum::SHORTCUT_WINDOW_CLOSE].value.toString()));
            
            _actions[FIT]->setIcon(context()->iconLibrary()->icon("djv/UI/WindowFitIcon"));
            _actions[FIT]->setShortcut(shortcuts[Enum::SHORTCUT_WINDOW_FIT].value);
            _actions[FIT]->setToolTip(
                qApp->translate("djv::ViewLib::WindowActions",
                "Resize the window to fit the image<br><br>"
                "Keyboard shortcut: %1").
                arg(shortcuts[Enum::SHORTCUT_WINDOW_FIT].value.toString()));
            _actions[FIT]->setWhatsThis(
                qApp->translate("djv::ViewLib::WindowActions",
                "Resize the window to fit the image<br><br>"
                "Keyboard shortcut: %1<br><br>"
                "<a href=\"ViewWindows.html\">Documentation</a>").
                arg(shortcuts[Enum::SHORTCUT_WINDOW_FIT].value.toString()));
            
            _actions[FULL_SCREEN]->setIcon(context()->iconLibrary()->icon("djv/UI/WindowFullScreenIcon"));
            _actions[FULL_SCREEN]->setShortcut(shortcuts[Enum::SHORTCUT_WINDOW_FULL_SCREEN].value);
            _actions[FULL_SCREEN]->setToolTip(
                qApp->translate("djv::ViewLib::WindowActions",
                "Show the window full screen<br><br>"
                "Keyboard shortcut: %1").
                arg(shortcuts[Enum::SHORTCUT_WINDOW_FULL_SCREEN].value.toString()));
            _actions[FULL_SCREEN]->setWhatsThis(
                qApp->translate("djv::ViewLib::WindowActions",
                "Show the window full screen<br><br>"
                "Keyboard shortcut: %1<br><br>"
                "<a href=\"ViewWindows.html\">Documentation</a>").
                arg(shortcuts[Enum::SHORTCUT_WINDOW_FULL_SCREEN].value.toString()));

            _actions[UI_VISIBLE]->setShortcut(shortcuts[Enum::SHORTCUT_WINDOW_UI_VISIBLE].value);

            const Enum::SHORTCUT uiComponentVisibilityShortcuts[] =
            {
                Enum::SHORTCUT_WINDOW_FILE_TOOL_BAR_VISIBLE,
                Enum::SHORTCUT_WINDOW_WINDOW_TOOL_BAR_VISIBLE,
                Enum::SHORTCUT_WINDOW_VIEW_TOOL_BAR_VISIBLE,
                Enum::SHORTCUT_WINDOW_IMAGE_TOOL_BAR_VISIBLE,
                Enum::SHORTCUT_WINDOW_TOOLS_TOOL_BAR_VISIBLE,
                Enum::SHORTCUT_WINDOW_PLAYBACK_VISIBLE,
                Enum::SHORTCUT_WINDOW_INFO_VISIBLE
            };
            const int uiComponentVisibilityShortcutsCount =
                sizeof(uiComponentVisibilityShortcuts) / sizeof(uiComponentVisibilityShortcuts[0]);
            for (int i = 0; i < uiComponentVisibilityShortcutsCount; ++i)
            {
                _groups[UI_COMPONENT_VISIBLE_GROUP]->actions()[i]->setShortcut(
                    shortcuts[uiComponentVisibilityShortcuts[i]].value);
            }

            _actions[DETACH_CONTROLS]->setShortcut(shortcuts[Enum::SHORTCUT_WINDOW_DETACH_CONTROLS].value);
            _actions[DETACH_CONTROLS]->setToolTip(
                qApp->translate("djv::ViewLib::ViewActions",
                    "Toggle whether the user interface is shown in a separate window<br><br>"
                    "Keyboard shortcut: %1").
                arg(shortcuts[Enum::SHORTCUT_WINDOW_DETACH_CONTROLS].value.toString()));
            _actions[DETACH_CONTROLS]->setWhatsThis(
                qApp->translate("djv::ViewLib::ViewActions",
                    "Toggle whether the user interface is shown in a separate window<br><br>"
                    "Keyboard shortcut: %1<br><br>"
                    "<a href=\"ViewImageView.html\">Documentation</a>").
                arg(shortcuts[Enum::SHORTCUT_WINDOW_DETACH_CONTROLS].value.toString()));

            Q_EMIT changed();
        }

    } // namespace ViewLib
} // namespace djv
