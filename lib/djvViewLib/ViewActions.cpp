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

#include <djvViewLib/ViewActions.h>

#include <djvViewLib/Context.h>
#include <djvViewLib/ShortcutPrefs.h>
#include <djvViewLib/Util.h>

#include <djvUI/IconLibrary.h>
#include <djvUI/Shortcut.h>
#include <djvUI/Style.h>

#include <QAction>
#include <QActionGroup>
#include <QApplication>

namespace djv
{
    namespace ViewLib
    {
        struct ViewActions::Private
        {};

        ViewActions::ViewActions(
            Context * context,
            QObject * parent) :
            AbstractActions(context, parent),
            _p(new Private)
        {
            // Create the actions.
            for (int i = 0; i < ACTION_COUNT; ++i)
            {
                _actions[i] = new QAction(this);
            }
            _actions[LEFT]->setText(qApp->translate("djv::ViewLib::ViewActions", "&Left"));
            _actions[RIGHT]->setText(qApp->translate("djv::ViewLib::ViewActions", "&Right"));
            _actions[UP]->setText(qApp->translate("djv::ViewLib::ViewActions", "&Up"));
            _actions[DOWN]->setText(qApp->translate("djv::ViewLib::ViewActions", "&Down"));
            _actions[CENTER]->setText(qApp->translate("djv::ViewLib::ViewActions", "&Center"));
            _actions[ZOOM_IN]->setText(qApp->translate("djv::ViewLib::ViewActions", "Zoom &In"));
            _actions[ZOOM_OUT]->setText(qApp->translate("djv::ViewLib::ViewActions", "Zoom &Out"));
            _actions[ZOOM_RESET]->setText(qApp->translate("djv::ViewLib::ViewActions", "Zoom Rese&t"));
            _actions[RESET]->setText(qApp->translate("djv::ViewLib::ViewActions", "R&eset"));
            _actions[FIT]->setText(qApp->translate("djv::ViewLib::ViewActions", "&Fit"));
            _actions[HUD]->setText(qApp->translate("djv::ViewLib::ViewActions", "&HUD"));
            _actions[HUD]->setCheckable(true);

            // Create the action groups.
            for (int i = 0; i < GROUP_COUNT; ++i)
            {
                _groups[i] = new QActionGroup(this);
            }
            _groups[GRID_GROUP]->setExclusive(true);
            for (int i = 0; i < Util::gridLabels().count(); ++i)
            {
                QAction * action = new QAction(this);
                action->setText(Util::gridLabels()[i]);
                action->setCheckable(true);
                action->setData(i);

                _groups[GRID_GROUP]->addAction(action);
            }

            // Initialize.
            update();

            // Setup the callbacks.
            connect(
                context->shortcutPrefs(),
                SIGNAL(shortcutsChanged(const QVector<djv::UI::Shortcut> &)),
                SLOT(update()));
            connect(
                context->style(),
                SIGNAL(sizeMetricsChanged()),
                SLOT(update()));
        }

        ViewActions::~ViewActions()
        {}

        void ViewActions::update()
        {
            const int iconDPI = context()->style()->sizeMetric().iconDPI;
            const QVector<UI::Shortcut> & shortcuts = context()->shortcutPrefs()->shortcuts();

            _actions[LEFT]->setShortcut(shortcuts[Util::SHORTCUT_VIEW_LEFT].value);
            _actions[LEFT]->setToolTip(
                qApp->translate("djv::ViewLib::ViewActions", "Move the view left\n\nShortcut: %1").
                arg(shortcuts[Util::SHORTCUT_VIEW_LEFT].value.toString()));
            _actions[RIGHT]->setShortcut(shortcuts[Util::SHORTCUT_VIEW_RIGHT].value);
            _actions[RIGHT]->setToolTip(
                qApp->translate("djv::ViewLib::ViewActions", "Move the view right\n\nShortcut: %1").
                arg(shortcuts[Util::SHORTCUT_VIEW_RIGHT].value.toString()));
            _actions[UP]->setShortcut(shortcuts[Util::SHORTCUT_VIEW_UP].value);
            _actions[UP]->setToolTip(
                qApp->translate("djv::ViewLib::ViewActions", "Move the view up\n\nShortcut: %1").
                arg(shortcuts[Util::SHORTCUT_VIEW_UP].value.toString()));
            _actions[DOWN]->setShortcut(shortcuts[Util::SHORTCUT_VIEW_DOWN].value);
            _actions[DOWN]->setToolTip(
                qApp->translate("djv::ViewLib::ViewActions", "Move the view down\n\nShortcut: %1").
                arg(shortcuts[Util::SHORTCUT_VIEW_DOWN].value.toString()));
            _actions[CENTER]->setShortcut(shortcuts[Util::SHORTCUT_VIEW_CENTER].value);
            _actions[CENTER]->setToolTip(
                qApp->translate("djv::ViewLib::ViewActions", "Center the view\n\nShortcut: %1").
                arg(shortcuts[Util::SHORTCUT_VIEW_CENTER].value.toString()));
            _actions[ZOOM_IN]->setIcon(context()->iconLibrary()->icon("djvViewZoomInIcon", iconDPI));
            _actions[ZOOM_IN]->setShortcut(shortcuts[Util::SHORTCUT_VIEW_ZOOM_IN].value);
            _actions[ZOOM_IN]->setToolTip(
                qApp->translate("djv::ViewLib::ViewActions", "Zoom into the view\n\nShortcut: %1").
                arg(shortcuts[Util::SHORTCUT_VIEW_ZOOM_IN].value.toString()));
            _actions[ZOOM_OUT]->setIcon(context()->iconLibrary()->icon("djvViewZoomOutIcon", iconDPI));
            _actions[ZOOM_OUT]->setShortcut(shortcuts[Util::SHORTCUT_VIEW_ZOOM_OUT].value);
            _actions[ZOOM_OUT]->setToolTip(
                qApp->translate("djv::ViewLib::ViewActions", "Zoom out of the view\n\nShortcut: %1").
                arg(shortcuts[Util::SHORTCUT_VIEW_ZOOM_OUT].value.toString()));
            _actions[ZOOM_RESET]->setIcon(context()->iconLibrary()->icon("djvViewZoomResetIcon", iconDPI));
            _actions[ZOOM_RESET]->setShortcut(shortcuts[Util::SHORTCUT_VIEW_ZOOM_RESET].value);
            _actions[ZOOM_RESET]->setToolTip(
                qApp->translate("djv::ViewLib::ViewActions", "Reset the zoom\n\nShortcut: %1").
                arg(shortcuts[Util::SHORTCUT_VIEW_ZOOM_RESET].value.toString()));
            _actions[RESET]->setShortcut(shortcuts[Util::SHORTCUT_VIEW_RESET].value);
            _actions[RESET]->setToolTip(
                qApp->translate("djv::ViewLib::ViewActions", "Reset the position and zoom\n\nShortcut: %1").
                arg(shortcuts[Util::SHORTCUT_VIEW_RESET].value.toString()));
            _actions[FIT]->setIcon(context()->iconLibrary()->icon("djvViewFitIcon", iconDPI));
            _actions[FIT]->setShortcut(shortcuts[Util::SHORTCUT_VIEW_FIT].value);
            _actions[FIT]->setToolTip(
                qApp->translate("djv::ViewLib::ViewActions", "Fit the image to the view\n\nShortcut: %1").
                arg(shortcuts[Util::SHORTCUT_VIEW_FIT].value.toString()));
            _actions[HUD]->setShortcut(shortcuts[Util::SHORTCUT_VIEW_HUD].value);
            _actions[HUD]->setToolTip(
                qApp->translate("djv::ViewLib::ViewActions", "Toggle the HUD (Heads Up Display)\n\nShortcut: %1").
                arg(shortcuts[Util::SHORTCUT_VIEW_HUD].value.toString()));

            Q_EMIT changed();
        }

    } // namespace ViewLib
} // namespace djv
