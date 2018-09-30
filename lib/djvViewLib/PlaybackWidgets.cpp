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

#include <djvViewLib/Context.h>
#include <djvViewLib/ShortcutPrefs.h>

#include <djvUI/ChoiceButton.h>
#include <djvUI/IconLibrary.h>
#include <djvUI/ShuttleButton.h>
#include <djvUI/ToolButton.h>

#include <QActionGroup>
#include <QApplication>
#include <QButtonGroup>
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
            QPointer<QButtonGroup> buttonGroup;
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

            _p->buttonGroup = new QButtonGroup(this);
            _p->buttonGroup->setExclusive(true);

            Q_FOREACH(QAction * action, actionGroup->actions())
            {
                UI::ToolButton* button = new UI::ToolButton(context);
                button->setDefaultAction(action);
                _p->buttonGroup->addButton(button);
            }

            _p->shuttle = new UI::ShuttleButton(context);
            _p->shuttle->setToolTip(
                qApp->translate("djv::ViewLib::PlaybackButtons",
                "Click and drag to start playback; the speed is determined by how far you drag"));

            _p->layout = new QHBoxLayout(this);
            _p->layout->setMargin(0);
            Q_FOREACH(QAbstractButton * button, _p->buttonGroup->buttons())
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
            Private(const QPointer<Context> & context) :
                context(context)
            {}

            QString text;
            QPointer<UI::ChoiceButton> button;
            QPointer<Context> context;
        };

        LoopWidget::LoopWidget(
            const QPointer<QActionGroup> & actionGroup,
            const QPointer<Context> & context,
            QWidget * parent) :
            QWidget(parent),
            _p(new Private(context))
        {
            _p->button = new UI::ChoiceButton(actionGroup, context.data());

            QHBoxLayout * layout = new QHBoxLayout(this);
            layout->setMargin(0);
            layout->addWidget(_p->button);

            widgetUpdate();

            connect(
                actionGroup,
                SIGNAL(triggered(QAction *)),
                SLOT(actionCallback(QAction *)));
            connect(
                context->shortcutPrefs(),
                SIGNAL(shortcutsChanged(const QVector<djv::UI::Shortcut> &)),
                SLOT(widgetUpdate()));
        }

        LoopWidget::~LoopWidget()
        {}

        void LoopWidget::actionCallback(QAction * action)
        {
            _p->text = action->text();
            widgetUpdate();
        }
        
        void LoopWidget::widgetUpdate()
        {
            const QVector<UI::Shortcut> & shortcuts =  _p->context->shortcutPrefs()->shortcuts();
            _p->button->setShortcut(shortcuts[Enum::SHORTCUT_PLAYBACK_LOOP].value);

            _p->button->setToolTip(
                qApp->translate("djv::ViewLib::PlaybackButtons",
                "Loop mode: %1\n\nKeyboard shortcut: %2").
                arg(_p->text).
                arg(shortcuts[Enum::SHORTCUT_PLAYBACK_LOOP].value.toString()));
        }

        struct FrameButtons::Private
        {
            QPointer<UI::UIContext> context;
            QPointer<QButtonGroup> buttonGroup;
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

            _p->buttonGroup = new QButtonGroup(this);
            _p->buttonGroup->setExclusive(false);

            Q_FOREACH(QAction * action, actionGroup->actions())
            {
                if (!action->icon().isNull())
                {
                    UI::ToolButton* button = new UI::ToolButton(context);
                    button->setDefaultAction(action);
                    _p->buttonGroup->addButton(button);
                }
            }

            _p->shuttle = new UI::ShuttleButton(context);
            _p->shuttle->setToolTip(
                qApp->translate("djv::ViewLib::PlaybackButtons",
                "Click and drag to change the current frame."));

            _p->layout = new QHBoxLayout(this);
            _p->layout->setMargin(0);
            _p->layout->setSpacing(0);
            Q_FOREACH(QAbstractButton * button, _p->buttonGroup->buttons())
                _p->layout->addWidget(button);
            _p->layout->addWidget(_p->shuttle);

            styleUpdate();

            Q_FOREACH(QAbstractButton * button, _p->buttonGroup->buttons())
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

