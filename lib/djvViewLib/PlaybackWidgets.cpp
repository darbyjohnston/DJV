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

namespace djv
{
    namespace ViewLib
    {
        struct PlaybackButtons::Private
        {
            Private() :
                buttonGroup(0),
                shuttle(0)
            {}

            QButtonGroup *     buttonGroup;
            djvShuttleButton * shuttle;
        };

        PlaybackButtons::PlaybackButtons(
            QActionGroup *  actionGroup,
            djvUIContext * context,
            QWidget *       parent) :
            QWidget(parent),
            _p(new Private)
        {
            _p->buttonGroup = new QButtonGroup(this);
            _p->buttonGroup->setExclusive(true);

            const QList<QAction *> actions = actionGroup->actions();

            for (int i = 0; i < actions.count(); ++i)
            {
                djvToolButton* button = new djvToolButton;
                button->setDefaultAction(actions[i]);
                button->setIconSize(context->iconLibrary()->defaultSize());
                _p->buttonGroup->addButton(button);
            }

            _p->shuttle = new djvShuttleButton(djvShuttleButton::iconsDefault(context));
            _p->shuttle->setToolTip(
                qApp->translate("djv::ViewLib::PlaybackButtons", "Playback shuttle\n\nClick and drag to start playback; the speed is "
                    "determined by how far you drag."));

            QHBoxLayout * layout = new QHBoxLayout(this);
            layout->setMargin(0);
            layout->setSpacing(0);
            Q_FOREACH(QAbstractButton * button, _p->buttonGroup->buttons())
                layout->addWidget(button);
            layout->addWidget(_p->shuttle);

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

        struct LoopWidget::Private
        {
            Private(Context * context) :
                context(context)
            {}

            Util::LOOP loop = Util::LOOP_REPEAT;
            djvChoiceButton * button = nullptr;
            Context * context;
        };

        LoopWidget::LoopWidget(
            QActionGroup * actionGroup,
            Context * context,
            QWidget * parent) :
            QWidget(parent),
            _p(new Private(context))
        {
            _p->button = new djvChoiceButton(actionGroup);

            QHBoxLayout * layout = new QHBoxLayout(this);
            layout->setMargin(0);
            layout->setSpacing(0);
            layout->addWidget(_p->button);

            widgetUpdate();

            connect(
                context->shortcutPrefs(),
                SIGNAL(shortcutsChanged(const QVector<djvShortcut> &)),
                SLOT(widgetUpdate()));
        }

        LoopWidget::~LoopWidget()
        {}

        void LoopWidget::widgetUpdate()
        {
            // Update shortcuts.
            const QVector<djvShortcut> & shortcuts =
                _p->context->shortcutPrefs()->shortcuts();
            _p->button->setShortcut(
                shortcuts[Util::SHORTCUT_PLAYBACK_LOOP].value);

            // Update tool tips.
            _p->button->setToolTip(
                qApp->translate("djv::ViewLib::PlaybackButtons", "Loop mode: %1\n\nClick to cycle through values: %2\n\nShortcut: %3").
                arg(djvStringUtil::label(_p->loop).join(", ")).
                arg(Util::loopLabels().join(", ")).
                arg(shortcuts[Util::SHORTCUT_PLAYBACK_LOOP].value.toString()));
        }

        struct FrameButtons::Private
        {
            QButtonGroup *     buttonGroup = nullptr;
            djvShuttleButton * shuttle = nullptr;
        };

        FrameButtons::FrameButtons(
            QActionGroup *  actionGroup,
            djvUIContext * context,
            QWidget *       parent) :
            QWidget(parent),
            _p(new Private)
        {
            _p->buttonGroup = new QButtonGroup(this);
            _p->buttonGroup->setExclusive(false);

            Q_FOREACH(QAction * action, actionGroup->actions())
            {
                if (!action->icon().isNull())
                {
                    djvToolButton* button = new djvToolButton;
                    button->setDefaultAction(action);
                    button->setIconSize(context->iconLibrary()->defaultSize());
                    _p->buttonGroup->addButton(button);
                }
            }

            _p->shuttle = new djvShuttleButton(djvShuttleButton::iconsDefault(context));
            _p->shuttle->setToolTip(
                qApp->translate("djv::ViewLib::PlaybackButtons", "Frame shuttle\n\nClick and drag to change the current frame."));

            QHBoxLayout * layout = new QHBoxLayout(this);
            layout->setMargin(0);
            layout->setSpacing(0);
            Q_FOREACH(QAbstractButton * button, _p->buttonGroup->buttons())
                layout->addWidget(button);
            layout->addWidget(_p->shuttle);

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

    } // namespace ViewLib
} // namespace djv
