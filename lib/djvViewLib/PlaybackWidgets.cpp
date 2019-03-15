//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
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

#include <djvViewLib/PlaybackWidgets.h>

#include <djvViewLib/ShortcutPrefs.h>
#include <djvViewLib/ViewContext.h>

#include <djvUI/ChoiceButton.h>
#include <djvUI/IconLibrary.h>
#include <djvUI/ShuttleButton.h>
#include <djvUI/ToolButton.h>

#include <QActionGroup>
#include <QApplication>
#include <QHBoxLayout>
#include <QPointer>
#include <QStyle>

namespace djv
{
    namespace ViewLib
    {
        struct PlaybackButtons::Private
        {
            QPointer<UI::UIContext> context;
            QPointer<UI::ShuttleButton> shuttle;
            QPointer<QHBoxLayout> layout;
        };

        PlaybackButtons::PlaybackButtons(
            const QPointer<QActionGroup> & actionGroup,
            const QPointer<UI::UIContext> & context,
            QWidget * parent) :
            QWidget(parent),
            _p(new Private)
        {
            _p->context = context;

            QVector<UI::ToolButton *> buttons;
            const auto actions =
            {
                actionGroup->actions()[Enum::REVERSE],
                actionGroup->actions()[Enum::STOP],
                actionGroup->actions()[Enum::FORWARD]
            };
            for (const auto i : actions)
            {
                auto button = new UI::ToolButton(context);
                button->setDefaultAction(i);
                buttons.push_back(button);
            }

            _p->shuttle = new UI::ShuttleButton(context);
            _p->shuttle->setToolTip(
                qApp->translate("djv::ViewLib::PlaybackButtons",
                "Click and drag to start playback; the playback speed is determined by how far you drag"));
            _p->shuttle->setWhatsThis(
                qApp->translate("djv::ViewLib::PlaybackButtons",
                "Click and drag to start playback; the playback speed is determined by how far you drag<br><br>"
                "<a href=\"ViewPlayback.html\">Documentation</a>"));

            _p->layout = new QHBoxLayout(this);
            _p->layout->setMargin(0);
            Q_FOREACH(auto button, buttons)
                _p->layout->addWidget(button);
            _p->layout->addWidget(_p->shuttle);

            styleUpdate();
            
            connect(
                _p->shuttle,
                SIGNAL(mousePressed(bool)),
                SIGNAL(shuttlePressed(bool)));
            connect(
                _p->shuttle,
                SIGNAL(valueChanged(int)),
                SIGNAL(shuttleChanged(int)));
        }

        PlaybackButtons::~PlaybackButtons()
        {}

        bool PlaybackButtons::event(QEvent * event)
        {
            if (QEvent::StyleChange == event->type())
            {
                styleUpdate();
            }
            return QWidget::event(event);
        }

        void PlaybackButtons::styleUpdate()
        {
            _p->layout->setSpacing(style()->pixelMetric(QStyle::PM_ToolBarItemSpacing));
        }   

        struct LoopWidget::Private
        {
            Private(const QPointer<ViewContext> & context) :
                context(context)
            {}

            QString text;
            QPointer<UI::ChoiceButton> button;
            QPointer<ViewContext> context;
        };

        LoopWidget::LoopWidget(
            const QPointer<QActionGroup> & actionGroup,
            const QPointer<ViewContext> & context,
            QWidget * parent) :
            QWidget(parent),
            _p(new Private(context))
        {
            _p->button = new UI::ChoiceButton(actionGroup, context.data());

            QHBoxLayout * layout = new QHBoxLayout(this);
            layout->setMargin(0);
            layout->addWidget(_p->button);

            Q_FOREACH(auto action, actionGroup->actions())
            {
                if (action->isChecked())
                {
                    _p->text = action->text();
                    break;
                }
            }
            widgetUpdate();

            Q_FOREACH(auto action, actionGroup->actions())
            {
                connect(
                    action,
                    SIGNAL(toggled(bool)),
                    SLOT(actionCallback(bool)));
            }
            connect(
                context->shortcutPrefs(),
                SIGNAL(shortcutsChanged(const QVector<djv::UI::Shortcut> &)),
                SLOT(widgetUpdate()));
        }

        LoopWidget::~LoopWidget()
        {}

        void LoopWidget::actionCallback(bool)
        {
            _p->text = qobject_cast<QAction *>(sender())->text();
            widgetUpdate();
        }
        
        void LoopWidget::widgetUpdate()
        {
            const QVector<UI::Shortcut> & shortcuts =  _p->context->shortcutPrefs()->shortcuts();
            _p->button->setShortcut(shortcuts[Enum::SHORTCUT_PLAYBACK_LOOP].value);

            _p->button->setToolTip(
                qApp->translate("djv::ViewLib::LoopWidget",
                "Loop mode: %1<br><br>"
                "Keyboard shortcut: %2").
                arg(_p->text).
                arg(shortcuts[Enum::SHORTCUT_PLAYBACK_LOOP].value.toString()));
            _p->button->setWhatsThis(
                qApp->translate("djv::ViewLib::LoopWidget",
                "Loop mode: %1<br><br>"
                "Keyboard shortcut: %2<br><br>"
                "<a href=\"ViewPlayback.html\">Documentation</a>").
                arg(_p->text).
                arg(shortcuts[Enum::SHORTCUT_PLAYBACK_LOOP].value.toString()));
        }

        struct FrameButtons::Private
        {
            QPointer<UI::UIContext> context;
            QPointer<UI::ShuttleButton> shuttle;
            QPointer<QHBoxLayout> layout;
        };

        FrameButtons::FrameButtons(
            const QPointer<QActionGroup> & actionGroup,
            const QPointer<UI::UIContext> & context,
            QWidget * parent) :
            QWidget(parent),
            _p(new Private)
        {
            _p->context = context;

            const auto actions =
            {
                actionGroup->actions()[Enum::FRAME_START],
                actionGroup->actions()[Enum::FRAME_PREV],
                actionGroup->actions()[Enum::FRAME_NEXT],
                actionGroup->actions()[Enum::FRAME_END]
            };
            QVector<UI::ToolButton *> buttons;
            for (const auto i : actions)
            {
                auto button = new UI::ToolButton(context);
                button->setDefaultAction(i);
                buttons.append(button);
            }

            _p->shuttle = new UI::ShuttleButton(context);
            _p->shuttle->setToolTip(
                qApp->translate("djv::ViewLib::FrameButtons",
                "Click and drag to change the current frame"));
            _p->shuttle->setWhatsThis(
                qApp->translate("djv::ViewLib::FrameButtons",
                "Click and drag to change the current frame<br><br>"
                "<a href=\"ViewPlayback.html\">Documentation</a>"));

            _p->layout = new QHBoxLayout(this);
            _p->layout->setMargin(0);
            _p->layout->setSpacing(0);
            Q_FOREACH(auto button, buttons)
                _p->layout->addWidget(button);
            _p->layout->addWidget(_p->shuttle);

            styleUpdate();

            Q_FOREACH(auto button, buttons)
            {
                connect(button, SIGNAL(pressed()), SIGNAL(pressed()));
                connect(button, SIGNAL(released()), SIGNAL(released()));
            }
            connect(
                _p->shuttle,
                SIGNAL(mousePressed(bool)),
                SIGNAL(shuttlePressed(bool)));
            connect(
                _p->shuttle,
                SIGNAL(valueChanged(int)),
                SIGNAL(shuttleChanged(int)));
            connect(_p->shuttle, SIGNAL(pressed()), SIGNAL(pressed()));
            connect(_p->shuttle, SIGNAL(released()), SIGNAL(released()));
        }

        FrameButtons::~FrameButtons()
        {}

        bool FrameButtons::event(QEvent * event)
        {
            if (QEvent::StyleChange == event->type())
            {
                styleUpdate();
            }
            return QWidget::event(event);
        }

        void FrameButtons::styleUpdate()
        {
            _p->layout->setSpacing(style()->pixelMetric(QStyle::PM_ToolBarItemSpacing));
        }   

    } // namespace ViewLib
} // namespace djv

