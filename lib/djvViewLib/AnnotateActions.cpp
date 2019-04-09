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

#include <djvViewLib/AnnotateActions.h>

#include <djvViewLib/ShortcutPrefs.h>
#include <djvViewLib/ViewContext.h>

#include <djvUI/IconLibrary.h>
#include <djvUI/Shortcut.h>
#include <djvUI/StylePrefs.h>

#include <QAction>
#include <QApplication>
#include <QPointer>
#include <QStyle>

namespace djv
{
    namespace ViewLib
    {
        struct AnnotateActions::Private
        {};

        AnnotateActions::AnnotateActions(
            const QPointer<ViewContext> & context,
            QObject * parent) :
            AbstractActions(context, parent),
            _p(new Private)
        {
            for (int i = 0; i < ACTION_COUNT; ++i)
            {
                _actions[i] = new QAction(this);
            }
            _actions[SHOW]->setText(qApp->translate("djv::ViewLib::AnnotateActions", "&Show Annotations"));
            _actions[SHOW]->setCheckable(true);
            _actions[COLOR]->setText(qApp->translate("djv::ViewLib::AnnotateActions", "Set &Color"));
            _actions[LINE_WIDTH_INC]->setText(qApp->translate("djv::ViewLib::AnnotateActions", "&Increase Line Width"));
            _actions[LINE_WIDTH_DEC]->setText(qApp->translate("djv::ViewLib::AnnotateActions", "&Decrease Line Width"));
            _actions[UNDO]->setText(qApp->translate("djv::ViewLib::AnnotateActions", "&Undo Drawing"));
            _actions[REDO]->setText(qApp->translate("djv::ViewLib::AnnotateActions", "&Redo Drawing"));
            _actions[CLEAR]->setText(qApp->translate("djv::ViewLib::AnnotateActions", "&Clear Drawing"));
            _actions[NEW]->setText(qApp->translate("djv::ViewLib::AnnotateActions", "&New Annotation"));
            _actions[DELETE]->setText(qApp->translate("djv::ViewLib::AnnotateActions", "D&elete Annotation"));
            _actions[NEXT]->setText(qApp->translate("djv::ViewLib::AnnotateActions", "Ne&xt Annotation"));
            _actions[PREV]->setText(qApp->translate("djv::ViewLib::AnnotateActions", "&Previous Annotation"));
            _actions[EXPORT]->setText(qApp->translate("djv::ViewLib::AnnotateActions", "Expor&t Annotations"));

            for (int i = 0; i < GROUP_COUNT; ++i)
            {
                _groups[i] = new QActionGroup(this);
            }
            for (int i = 0; i < Enum::annotatePrimitiveLabels().count(); ++i)
            {
                QAction * action = new QAction(this);
                action->setText(Enum::annotatePrimitiveLabels()[i]);
                action->setCheckable(true);
                action->setData(i);
                _groups[PRIMITIVE_GROUP]->addAction(action);
            }

            update();

            connect(
                context->shortcutPrefs(),
                SIGNAL(shortcutsChanged(const QVector<djv::UI::Shortcut> &)),
                SLOT(update()));
            connect(
                context->stylePrefs(),
                SIGNAL(prefChanged()),
                SLOT(update()));
        }

        AnnotateActions::~AnnotateActions()
        {}

        void AnnotateActions::update()
        {
            const QVector<UI::Shortcut> & shortcuts = context()->shortcutPrefs()->shortcuts();

            _actions[SHOW]->setIcon(context()->iconLibrary()->icon("djv/UI/AnnotateIcon"));
            _actions[SHOW]->setShortcut(shortcuts[Enum::SHORTCUT_ANNOTATE_SHOW].value);
            _actions[SHOW]->setToolTip(
                qApp->translate("djv::ViewLib::AnnotateActions",
                    "Show annotations<br><br>"
                    "Keyboard shortcut: %1").arg(shortcuts[Enum::SHORTCUT_ANNOTATE_SHOW].value.toString()));
            _actions[SHOW]->setWhatsThis(
                qApp->translate("djv::ViewLib::AnnotateActions",
                    "Show annotations<br><br>"
                    "Keyboard shortcut: %1<br><br>"
                    "<a href=\"ViewAnnotate.html\">Documentation</a>").arg(shortcuts[Enum::SHORTCUT_ANNOTATE_SHOW].value.toString()));

            _actions[COLOR]->setShortcut(shortcuts[Enum::SHORTCUT_ANNOTATE_COLOR].value);
            _actions[COLOR]->setToolTip(
                qApp->translate("djv::ViewLib::AnnotateActions",
                    "Set the drawing color<br><br>"
                    "Keyboard shortcut: %1").arg(shortcuts[Enum::SHORTCUT_ANNOTATE_COLOR].value.toString()));
            _actions[COLOR]->setWhatsThis(
                qApp->translate("djv::ViewLib::AnnotateActions",
                    "Set the drawing color<br><br>"
                    "Keyboard shortcut: %1<br><br>"
                    "<a href=\"ViewAnnotate.html\">Documentation</a>").arg(shortcuts[Enum::SHORTCUT_ANNOTATE_COLOR].value.toString()));

            _actions[LINE_WIDTH_INC]->setShortcut(shortcuts[Enum::SHORTCUT_ANNOTATE_LINE_WIDTH_INC].value);
            _actions[LINE_WIDTH_DEC]->setShortcut(shortcuts[Enum::SHORTCUT_ANNOTATE_LINE_WIDTH_DEC].value);

            _actions[UNDO]->setIcon(context()->iconLibrary()->icon("djv/UI/UndoIcon"));
            _actions[UNDO]->setShortcut(shortcuts[Enum::SHORTCUT_ANNOTATE_UNDO].value);
            _actions[UNDO]->setToolTip(
                qApp->translate("djv::ViewLib::AnnotateActions",
                    "Undo the last drawing<br><br>"
                    "Keyboard shortcut: %1").arg(shortcuts[Enum::SHORTCUT_ANNOTATE_UNDO].value.toString()));
            _actions[UNDO]->setWhatsThis(
                qApp->translate("djv::ViewLib::AnnotateActions",
                    "Undo the last drawing<br><br>"
                    "Keyboard shortcut: %1<br><br>"
                    "<a href=\"ViewAnnotate.html\">Documentation</a>").arg(shortcuts[Enum::SHORTCUT_ANNOTATE_UNDO].value.toString()));

            _actions[REDO]->setIcon(context()->iconLibrary()->icon("djv/UI/RedoIcon"));
            _actions[REDO]->setShortcut(shortcuts[Enum::SHORTCUT_ANNOTATE_REDO].value);
            _actions[REDO]->setToolTip(
                qApp->translate("djv::ViewLib::AnnotateActions",
                    "Redo the last drawing<br><br>"
                    "Keyboard shortcut: %1").arg(shortcuts[Enum::SHORTCUT_ANNOTATE_REDO].value.toString()));
            _actions[REDO]->setWhatsThis(
                qApp->translate("djv::ViewLib::AnnotateActions",
                    "Redo the last drawing<br><br>"
                    "Keyboard shortcut: %1<br><br>"
                    "<a href=\"ViewAnnotate.html\">Documentation</a>").arg(shortcuts[Enum::SHORTCUT_ANNOTATE_REDO].value.toString()));

            _actions[CLEAR]->setIcon(context()->iconLibrary()->icon("djv/UI/EraseIcon"));
            _actions[CLEAR]->setShortcut(shortcuts[Enum::SHORTCUT_ANNOTATE_CLEAR].value);
            _actions[CLEAR]->setToolTip(
                qApp->translate("djv::ViewLib::AnnotateActions",
                    "Clear the drawing<br><br>"
                    "Keyboard shortcut: %1").arg(shortcuts[Enum::SHORTCUT_ANNOTATE_CLEAR].value.toString()));
            _actions[CLEAR]->setWhatsThis(
                qApp->translate("djv::ViewLib::AnnotateActions",
                    "Clear the drawing<br><br>"
                    "Keyboard shortcut: %1<br><br>"
                    "<a href=\"ViewAnnotate.html\">Documentation</a>").arg(shortcuts[Enum::SHORTCUT_ANNOTATE_CLEAR].value.toString()));

            _actions[NEW]->setIcon(context()->iconLibrary()->icon("djv/UI/AddIcon"));
            _actions[NEW]->setShortcut(shortcuts[Enum::SHORTCUT_ANNOTATE_NEW].value);
            _actions[NEW]->setToolTip(
                qApp->translate("djv::ViewLib::AnnotateActions",
                    "Create a new annotation<br><br>"
                    "Keyboard shortcut: %1").arg(shortcuts[Enum::SHORTCUT_ANNOTATE_NEW].value.toString()));
            _actions[NEW]->setWhatsThis(
                qApp->translate("djv::ViewLib::AnnotateActions",
                    "Create a new annotation<br><br>"
                    "Keyboard shortcut: %1<br><br>"
                    "<a href=\"ViewAnnotate.html\">Documentation</a>").arg(shortcuts[Enum::SHORTCUT_ANNOTATE_NEW].value.toString()));

            _actions[DELETE]->setIcon(context()->iconLibrary()->icon("djv/UI/RemoveIcon"));
            _actions[DELETE]->setShortcut(shortcuts[Enum::SHORTCUT_ANNOTATE_DELETE].value);
            _actions[DELETE]->setToolTip(
                qApp->translate("djv::ViewLib::AnnotateActions",
                    "Delete the current annotation<br><br>"
                    "Keyboard shortcut: %1").arg(shortcuts[Enum::SHORTCUT_ANNOTATE_DELETE].value.toString()));
            _actions[DELETE]->setWhatsThis(
                qApp->translate("djv::ViewLib::AnnotateActions",
                    "Delete the current annotation<br><br>"
                    "Keyboard shortcut: %1<br><br>"
                    "<a href=\"ViewAnnotate.html\">Documentation</a>").arg(shortcuts[Enum::SHORTCUT_ANNOTATE_DELETE].value.toString()));

            _actions[NEXT]->setIcon(context()->iconLibrary()->icon("djv/UI/RightIcon"));
            _actions[NEXT]->setShortcut(shortcuts[Enum::SHORTCUT_ANNOTATE_NEXT].value);
            _actions[NEXT]->setToolTip(
                qApp->translate("djv::ViewLib::AnnotateActions",
                    "Go to the next annotation<br><br>"
                    "Keyboard shortcut: %1").arg(shortcuts[Enum::SHORTCUT_ANNOTATE_NEXT].value.toString()));
            _actions[NEXT]->setWhatsThis(
                qApp->translate("djv::ViewLib::AnnotateActions",
                    "Go to the next annotation<br><br>"
                    "Keyboard shortcut: %1<br><br>"
                    "<a href=\"ViewAnnotate.html\">Documentation</a>").arg(shortcuts[Enum::SHORTCUT_ANNOTATE_NEXT].value.toString()));

            _actions[PREV]->setIcon(context()->iconLibrary()->icon("djv/UI/LeftIcon"));
            _actions[PREV]->setShortcut(shortcuts[Enum::SHORTCUT_ANNOTATE_PREV].value);
            _actions[PREV]->setToolTip(
                qApp->translate("djv::ViewLib::AnnotateActions",
                    "Go to the previous annotation<br><br>"
                    "Keyboard shortcut: %1").arg(shortcuts[Enum::SHORTCUT_ANNOTATE_PREV].value.toString()));
            _actions[PREV]->setWhatsThis(
                qApp->translate("djv::ViewLib::AnnotateActions",
                    "Go to the previous annotation<br><br>"
                    "Keyboard shortcut: %1<br><br>"
                    "<a href=\"v\">Documentation</a>").arg(shortcuts[Enum::SHORTCUT_ANNOTATE_PREV].value.toString()));

            _actions[EXPORT]->setIcon(context()->iconLibrary()->icon("djv/UI/ExportIcon"));
            _actions[EXPORT]->setShortcut(shortcuts[Enum::SHORTCUT_ANNOTATE_EXPORT].value);

            const auto primitiveIcons = std::vector<QString>(
            {
                "djv/UI/AnnotateIcon",
                "djv/UI/AnnotateIcon",
                "djv/UI/AnnotateRectangleIcon",
                "djv/UI/AnnotateEllipseIcon"
            });
            const QVector<QKeySequence> primitiveShortcuts = QVector<QKeySequence>() <<
                shortcuts[Enum::SHORTCUT_ANNOTATE_FREEHAND_LINE].value <<
                shortcuts[Enum::SHORTCUT_ANNOTATE_LINE].value <<
                shortcuts[Enum::SHORTCUT_ANNOTATE_RECTANGLE].value <<
                shortcuts[Enum::SHORTCUT_ANNOTATE_ELLIPSE].value;
            const QStringList primitiveToolTips = QStringList() <<
                qApp->translate("djv::ViewLib::AnnotateActions",
                    "Draw freehand lines<br><br>"
                    "Keyboard shortcut: %1") <<
                qApp->translate("djv::ViewLib::AnnotateActions",
                    "Draw lines<br><br>"
                    "Keyboard shortcut: %1") <<
                qApp->translate("djv::ViewLib::AnnotateActions",
                    "Draw rectangles<br><br>"
                    "Keyboard shortcut: %1") <<
                qApp->translate("djv::ViewLib::AnnotateActions",
                    "Draw ellipses<br><br>"
                    "Keyboard shortcut: %1");
            const QStringList primitiveWhatsThis = QStringList() <<
                qApp->translate("djv::ViewLib::AnnotateActions",
                    "Draw freehand lines<br><br>"
                    "Keyboard shortcut: %1<br><br>"
                    "<a href=\"ViewAnnotate.html\">Documentation</a>") <<
                qApp->translate("djv::ViewLib::AnnotateActions",
                    "Draw lines<br><br>"
                    "Keyboard shortcut: %1<br><br>"
                    "<a href=\"ViewAnnotate.html\">Documentation</a>") <<
                qApp->translate("djv::ViewLib::AnnotateActions",
                    "Draw rectangles<br><br>"
                    "Keyboard shortcut: %1<br><br>"
                    "<a href=\"ViewAnnotate.html\">Documentation</a>") <<
                qApp->translate("djv::ViewLib::AnnotateActions",
                    "Draw ellipses<br><br>"
                    "Keyboard shortcut: %1<br><br>"
                    "<a href=\"ViewAnnotate.html\">Documentation</a>");
            for (int i = 0; i < Enum::annotatePrimitiveLabels().count(); ++i)
            {
                _groups[PRIMITIVE_GROUP]->actions()[i]->setIcon(context()->iconLibrary()->icon(primitiveIcons[i]));
                auto shortcut = primitiveShortcuts[i];
                _groups[PRIMITIVE_GROUP]->actions()[i]->setShortcut(shortcut);
                _groups[PRIMITIVE_GROUP]->actions()[i]->setToolTip(primitiveToolTips[i].arg(shortcut.toString()));
                _groups[PRIMITIVE_GROUP]->actions()[i]->setWhatsThis(primitiveWhatsThis[i].arg(shortcut.toString()));
            }

            Q_EMIT changed();
        }

    } // namespace ViewLib
} // namespace djv
