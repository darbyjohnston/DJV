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

#include <djvViewLib/ToolActions.h>

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
        struct ToolActions::Private
        {
        };

        ToolActions::ToolActions(
            const QPointer<ViewContext> & context,
            QObject * parent) :
            AbstractActions(context, parent),
            _p(new Private)
        {
            // Create the action groups.
            for (int i = 0; i < GROUP_COUNT; ++i)
            {
                _groups[i] = new QActionGroup(this);
            }
            _groups[TOOL_GROUP]->setExclusive(false);
            const QStringList toolText = QStringList() <<
                qApp->translate("djv::ViewLib::ToolActions", "&Magnify") <<
                qApp->translate("djv::ViewLib::ToolActions", "&Color Picker") <<
                qApp->translate("djv::ViewLib::ToolActions", "&Histogram") <<
                qApp->translate("djv::ViewLib::ToolActions", "&Information");
            for (int i = 0; i < Enum::TOOL_COUNT; ++i)
            {
                auto action = new QAction(this);
                action->setText(toolText[i]);
                action->setCheckable(true);
                action->setData(i);
                _groups[TOOL_GROUP]->addAction(action);
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

        ToolActions::~ToolActions()
        {}

        void ToolActions::update()
        {
            const QVector<UI::Shortcut> & shortcuts = context()->shortcutPrefs()->shortcuts();

            const QList<QIcon> toolIcons = QList<QIcon>() <<
                context()->iconLibrary()->icon("djv/UI/MagnifyIcon") <<
                context()->iconLibrary()->icon("djv/UI/ColorPickerIcon") <<
                context()->iconLibrary()->icon("djv/UI/HistogramIcon") <<
                context()->iconLibrary()->icon("djv/UI/InfoIcon");
            const QVector<QKeySequence> toolShortcuts = QVector<QKeySequence>() <<
                shortcuts[Enum::SHORTCUT_TOOL_MAGNIFY].value <<
                shortcuts[Enum::SHORTCUT_TOOL_COLOR_PICKER].value <<
                shortcuts[Enum::SHORTCUT_TOOL_HISTOGRAM].value <<
                shortcuts[Enum::SHORTCUT_TOOL_INFO].value;
            const QStringList toolToolTips = QStringList() <<
                qApp->translate("djv::ViewLib::ToolActions",
                "Show the magnify tool<br><br>"
                "Keyboard shortcut: %1") <<
                qApp->translate("djv::ViewLib::ToolActions",
                "Show the color picker tool<br><br>"
                "Keyboard shortcut: %1") <<
                qApp->translate("djv::ViewLib::ToolActions",
                "Show the histogram tool<br><br>"
                "Keyboard shortcut: %1") <<
                qApp->translate("djv::ViewLib::ToolActions",
                "Show the information tool<br><br>"
                "Keyboard shortcut: %1");
            const QStringList toolWhatsThis = QStringList() <<
                qApp->translate("djv::ViewLib::ToolActions",
                "Show the magnify tool<br><br>"
                "Keyboard shortcut: %1<br><br>"
                "<a href=\"djv_view.html\">Documentation</a>") <<
                qApp->translate("djv::ViewLib::ToolActions",
                "Show the color picker tool<br><br>"
                "Keyboard shortcut: %1<br><br>"
                "<a href=\"djv_view.html\">Documentation</a>") <<
                qApp->translate("djv::ViewLib::ToolActions",
                "Show the histogram tool<br><br>"
                "Keyboard shortcut: %1<br><br>"
                "<a href=\"djv_view.html\">Documentation</a>") <<
                qApp->translate("djv::ViewLib::ToolActions",
                "Show the information tool<br><br>"
                "Keyboard shortcut: %1<br><br>"
                "<a href=\"djv_view.html\">Documentation</a>");
            for (int i = 0; i < Enum::TOOL_COUNT; ++i)
            {
                _groups[TOOL_GROUP]->actions()[i]->setIcon(toolIcons[i]);
                _groups[TOOL_GROUP]->actions()[i]->setShortcut(toolShortcuts[i]);
                _groups[TOOL_GROUP]->actions()[i]->setToolTip(toolToolTips[i].arg(toolShortcuts[i].toString()));
                _groups[TOOL_GROUP]->actions()[i]->setWhatsThis(toolWhatsThis[i].arg(toolShortcuts[i].toString()));
            }

            Q_EMIT changed();
        }

    } // namespace ViewLib
} // namespace djv
