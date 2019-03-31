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

#include <djvViewLib/AnnotationsActions.h>

#include <djvViewLib/AnnotationsPrefs.h>
#include <djvViewLib/ShortcutPrefs.h>
#include <djvViewLib/Util.h>
#include <djvViewLib/ViewContext.h>

#include <djvUI/IconLibrary.h>
#include <djvUI/Shortcut.h>
#include <djvUI/StylePrefs.h>

#include <QAction>
#include <QApplication>
#include <QPointer>

namespace djv
{
    namespace ViewLib
    {
        struct AnnotationsActions::Private
        {};

        AnnotationsActions::AnnotationsActions(
            const QPointer<ViewContext> & context,
            QObject * parent) :
            AbstractActions(context, parent),
            _p(new Private)
        {
            // Create the actions.
            for (int i = 0; i < ACTION_COUNT; ++i)
            {
                _actions[i] = new QAction(this);
            }
            _actions[ANNOTATIONS_WIDGET]->setCheckable(true);
            _actions[ANNOTATIONS_WIDGET]->setText(
                qApp->translate("djv::ViewLib::AnnotationsActions", "Annotations Widget"));

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

        AnnotationsActions::~AnnotationsActions()
        {}

        void AnnotationsActions::update()
        {
            const QVector<UI::Shortcut> & shortcuts = context()->shortcutPrefs()->shortcuts();

            _actions[ANNOTATIONS_WIDGET]->setIcon(context()->iconLibrary()->icon("djv/UI/AnnotationsIcon"));
            _actions[ANNOTATIONS_WIDGET]->setShortcut(shortcuts[Enum::SHORTCUT_ANNOTATIONS_WIDGET].value);
            _actions[ANNOTATIONS_WIDGET]->setToolTip(
                qApp->translate("djv::ViewLib::AnnotationsActions",
                "Toggle whether the annotations widget is visible"));
            _actions[ANNOTATIONS_WIDGET]->setWhatsThis(
                qApp->translate("djv::ViewLib::AnnotationsActions",
                "Toggle whether the annotations widget is visible<br><br>"
                "<a href=\"ViewAnnotations.html\">Documentation</a>"));
            
            Q_EMIT changed();
        }

    } // namespace ViewLib
} // namespace djv
