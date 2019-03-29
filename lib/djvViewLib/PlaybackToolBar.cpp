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

#include <djvViewLib/PlaybackToolBar.h>

#include <djvViewLib/MiscWidget.h>
#include <djvViewLib/PlaybackActions.h>
#include <djvViewLib/PlaybackWidgets.h>
#include <djvViewLib/ViewContext.h>
#include <djvUI/ToolButton.h>

#include <djvUI/IconLibrary.h>

#include <djvCore/SignalBlocker.h>

#include <QApplication>
#include <QHBoxLayout>
#include <QPointer>
#include <QStyle>

namespace djv
{
    namespace ViewLib
    {
        struct PlaybackToolBar::Private
        {
            QPointer<ViewContext> context;
            Enum::LAYOUT layout = static_cast<Enum::LAYOUT>(0);
            QPointer<QWidget> widget;
            QPointer<PlaybackButtons> playbackButtons;
            QPointer<LoopWidget> loopWidget;
            QPointer<SpeedWidget> speedWidget;
            QPointer<SpeedDisplay> actualSpeedDisplay;
            QPointer<UI::ToolButton> everyFrameButton;
            QPointer<FrameWidget> frameWidget;
            QPointer<FrameSlider> frameSlider;
            QPointer<FrameWidget> startWidget;
            QPointer<FrameWidget> endWidget;
            QPointer<FrameButtons> frameButtons;
            QPointer<FrameDisplay> durationDisplay;
            QPointer<UI::ToolButton> inOutEnabledButton;
            QPointer<UI::ToolButton> markInPointButton;
            QPointer<UI::ToolButton> markOutPointButton;
            QPointer<UI::ToolButton> resetInPointButton;
            QPointer<UI::ToolButton> resetOutPointButton;
            QPointer<QHBoxLayout> widgetLayout;
            QPointer<QHBoxLayout> actualSpeedAndEveryFrameLayout;
            QPointer<QHBoxLayout> loopAndInOutEnabledLayout;
            QPointer<QHBoxLayout> inPointLayout;
            QPointer<QHBoxLayout> outPointLayout;
        };

        PlaybackToolBar::PlaybackToolBar(
            const QPointer<AbstractActions> & actions,
            const QPointer<ViewContext> & context,
            QWidget * parent) :
            AbstractToolBar(qApp->translate("djv::ViewLib::PlaybackToolBar", "Playback"), actions, context, parent),
            _p(new Private)
        {
            _p->context = context;

            // Create the playback widgets.
            _p->playbackButtons = new PlaybackButtons(
                actions->group(PlaybackActions::PLAYBACK_GROUP),
                context.data());

            _p->loopWidget = new LoopWidget(
                actions->group(PlaybackActions::LOOP_GROUP),
                context.data());

            // Create the speed widgets.
            _p->speedWidget = new SpeedWidget(context.data());
            _p->speedWidget->setToolTip(
                qApp->translate("djv::ViewLib::PlaybackToolBar", "Playback speed"));

            _p->actualSpeedDisplay = new SpeedDisplay(context.data());
            _p->actualSpeedDisplay->setToolTip(
                qApp->translate("djv::ViewLib::PlaybackToolBar", "Actual playback speed"));

            _p->everyFrameButton = new UI::ToolButton(context.data());
            _p->everyFrameButton->setDefaultAction(
                actions->action(PlaybackActions::EVERY_FRAME));

            // Create the frame widgets.
            _p->frameWidget = new FrameWidget(context.data());
            _p->frameWidget->setToolTip(
                qApp->translate("djv::ViewLib::PlaybackToolBar", "Current frame"));

            _p->frameSlider = new FrameSlider(context.data());
            _p->frameSlider->setToolTip(
                qApp->translate("djv::ViewLib::PlaybackToolBar", "Frame slider"));

            _p->startWidget = new FrameWidget(context.data());
            _p->startWidget->setToolTip(
                qApp->translate("djv::ViewLib::PlaybackToolBar", "Start frame / in point"));

            _p->frameButtons = new FrameButtons(
                actions->group(PlaybackActions::FRAME_GROUP),
                context.data());

            _p->endWidget = new FrameWidget(context.data());
            _p->endWidget->setToolTip(
                qApp->translate("djv::ViewLib::PlaybackToolBar", "End frame / out point"));

            _p->durationDisplay = new FrameDisplay(context.data());
            _p->durationDisplay->setToolTip(
                qApp->translate("djv::ViewLib::PlaybackToolBar", "Playback duration"));

            // Create the in/out point widgets.
            _p->inOutEnabledButton = new UI::ToolButton(context.data());
            _p->inOutEnabledButton->setDefaultAction(
                actions->group(PlaybackActions::IN_OUT_GROUP)->actions()[Enum::IN_OUT_ENABLE]);

            _p->markInPointButton = new UI::ToolButton(context.data());
            _p->markInPointButton->setDefaultAction(
                actions->group(PlaybackActions::IN_OUT_GROUP)->actions()[Enum::MARK_IN]);

            _p->markOutPointButton = new UI::ToolButton(context.data());
            _p->markOutPointButton->setDefaultAction(
                actions->group(PlaybackActions::IN_OUT_GROUP)->actions()[Enum::MARK_OUT]);

            _p->resetInPointButton = new UI::ToolButton(context.data());
            _p->resetInPointButton->setDefaultAction(
                actions->group(PlaybackActions::IN_OUT_GROUP)->actions()[Enum::RESET_IN]);

            _p->resetOutPointButton = new UI::ToolButton(context.data());
            _p->resetOutPointButton->setDefaultAction(
                actions->group(PlaybackActions::IN_OUT_GROUP)->actions()[Enum::RESET_OUT]);

            // Layout the widgets.
            _p->widget = new QWidget;
            addWidget(_p->widget);

            // Initialize.
            layoutUpdate();
            styleUpdate();

            // Setup callbacks.
            connect(
                _p->playbackButtons,
                SIGNAL(shuttlePressed(bool)),
                SIGNAL(playbackShuttlePressed(bool)));
            connect(
                _p->playbackButtons,
                SIGNAL(shuttleChanged(int)),
                SIGNAL(playbackShuttleValue(int)));
            connect(
                _p->speedWidget,
                SIGNAL(speedChanged(const djv::Core::Speed &)),
                SIGNAL(speedChanged(const djv::Core::Speed &)));
            connect(
                _p->frameWidget,
                SIGNAL(frameChanged(qint64)),
                SIGNAL(frameChanged(qint64)));
            connect(
                _p->frameSlider,
                SIGNAL(pressed(bool)),
                SIGNAL(frameSliderPressed(bool)));
            connect(
                _p->frameSlider,
                SIGNAL(frameChanged(qint64)),
                SIGNAL(frameSliderChanged(qint64)));
            connect(
                _p->frameSlider,
                SIGNAL(inPointChanged(qint64)),
                SIGNAL(inPointChanged(qint64)));
            connect(
                _p->frameSlider,
                SIGNAL(outPointChanged(qint64)),
                SIGNAL(outPointChanged(qint64)));
            connect(
                _p->startWidget,
                SIGNAL(frameChanged(qint64)),
                _p->frameSlider,
                SLOT(setInPoint(qint64)));
            connect(
                _p->endWidget,
                SIGNAL(frameChanged(qint64)),
                _p->frameSlider,
                SLOT(setOutPoint(qint64)));
            connect(
                _p->frameButtons,
                SIGNAL(shuttlePressed(bool)),
                SIGNAL(frameShuttlePressed(bool)));
            connect(
                _p->frameButtons,
                SIGNAL(shuttleChanged(int)),
                SIGNAL(frameShuttleValue(int)));
            connect(
                _p->frameButtons,
                SIGNAL(pressed()),
                SIGNAL(frameButtonsPressed()));
            connect(
                _p->frameButtons,
                SIGNAL(released()),
                SIGNAL(frameButtonsReleased()));
        }

        PlaybackToolBar::~PlaybackToolBar()
        {}

        void PlaybackToolBar::setSpeed(const Core::Speed & speed)
        {
            _p->speedWidget->setSpeed(speed);
            _p->frameWidget->setSpeed(speed);
            _p->frameSlider->setSpeed(speed);
            _p->startWidget->setSpeed(speed);
            _p->endWidget->setSpeed(speed);
            _p->durationDisplay->setSpeed(speed);
        }

        void PlaybackToolBar::setDefaultSpeed(const Core::Speed & speed)
        {
            _p->speedWidget->setDefaultSpeed(speed);
        }

        void PlaybackToolBar::setActualSpeed(float speed)
        {
            _p->actualSpeedDisplay->setSpeed(speed);
        }

        void PlaybackToolBar::setDroppedFrames(bool in)
        {
            _p->actualSpeedDisplay->setDroppedFrames(in);
        }

        void PlaybackToolBar::setFrameList(const Core::FrameList & in)
        {
            _p->frameWidget->setFrameList(in);
            _p->frameSlider->setFrameList(in);
            _p->startWidget->setFrameList(in);
            _p->endWidget->setFrameList(in);
        }

        void PlaybackToolBar::setFrame(qint64 in)
        {
            Core::SignalBlocker signalBlocker(QObjectList() << _p->frameWidget << _p->frameSlider);
            _p->frameWidget->setFrame(in);
            _p->frameSlider->setFrame(in);
        }

        void PlaybackToolBar::setCachedFrames(const Core::FrameList & in)
        {
            _p->frameSlider->setCachedFrames(in);
        }

        void PlaybackToolBar::setStartFrame(qint64 in)
        {
            _p->startWidget->setFrame(in);
        }

        void PlaybackToolBar::setEndFrame(qint64 in)
        {
            _p->endWidget->setFrame(in);
        }

        void PlaybackToolBar::setDuration(qint64 in, bool inOutEnabled)
        {
            _p->durationDisplay->setFrame(in);
            _p->durationDisplay->setInOutEnabled(inOutEnabled);
        }

        void PlaybackToolBar::setInOutEnabled(bool in)
        {
            _p->frameSlider->setInOutEnabled(in);
        }

        void PlaybackToolBar::setInPoint(qint64 in)
        {
            _p->frameSlider->setInPoint(in);
        }

        void PlaybackToolBar::setOutPoint(qint64 in)
        {
            _p->frameSlider->setOutPoint(in);
        }

        void PlaybackToolBar::markInPoint()
        {
            _p->frameSlider->markInPoint();
        }

        void PlaybackToolBar::markOutPoint()
        {
            _p->frameSlider->markOutPoint();
        }

        void PlaybackToolBar::resetInPoint()
        {
            _p->frameSlider->resetInPoint();
        }

        void PlaybackToolBar::resetOutPoint()
        {
            _p->frameSlider->resetOutPoint();
        }

        void PlaybackToolBar::setLayout(Enum::LAYOUT layout)
        {
            if (layout == _p->layout)
                return;
            _p->layout = layout;
            layoutUpdate();
            styleUpdate();
        }

        bool PlaybackToolBar::event(QEvent * event)
        {
            if (QEvent::StyleChange == event->type())
            {
                styleUpdate();
            }
            return AbstractToolBar::event(event);
        }

        void PlaybackToolBar::layoutUpdate()
        {
            //DJV_DEBUG("PlaybackToolBar::layoutUpdate");
            delete _p->widgetLayout;
            _p->widgetLayout = new QHBoxLayout(_p->widget);
            _p->widgetLayout->setMargin(0);

            switch (_p->layout)
            {
            case Enum::LAYOUT_DEFAULT:
            case Enum::LAYOUT_LEFT:
            {
                auto leftLayout = new QVBoxLayout;
                leftLayout->setMargin(0);

                auto hLayout = new QHBoxLayout;
                hLayout->setMargin(0);
                hLayout->addWidget(_p->playbackButtons);
                hLayout->addWidget(_p->frameButtons);
                leftLayout->addLayout(hLayout);

                auto hLayout2 = new QHBoxLayout;
                hLayout2->setMargin(0);
                auto hLayout3 = new QHBoxLayout;
                hLayout3->setMargin(0);
                hLayout3->addWidget(_p->speedWidget);
                _p->actualSpeedAndEveryFrameLayout = new QHBoxLayout;
                _p->actualSpeedAndEveryFrameLayout->setMargin(0);
                _p->actualSpeedAndEveryFrameLayout->addWidget(_p->actualSpeedDisplay);
                _p->actualSpeedAndEveryFrameLayout->addWidget(_p->everyFrameButton);
                hLayout3->addLayout(_p->actualSpeedAndEveryFrameLayout);
                hLayout2->addLayout(hLayout3);
                hLayout2->addStretch(1000);
                _p->loopAndInOutEnabledLayout = new QHBoxLayout;
                _p->loopAndInOutEnabledLayout->setMargin(0);
                _p->loopAndInOutEnabledLayout->addWidget(_p->loopWidget);
                _p->loopAndInOutEnabledLayout->addWidget(_p->inOutEnabledButton);
                hLayout2->addLayout(_p->loopAndInOutEnabledLayout);
                leftLayout->addLayout(hLayout2);
                _p->widgetLayout->addLayout(leftLayout);

                auto rightLayout = new QVBoxLayout;
                rightLayout->setMargin(0);

                hLayout = new QHBoxLayout;
                hLayout->setMargin(0);
                hLayout->addWidget(_p->frameSlider, 1);
                rightLayout->addLayout(hLayout);

                hLayout2 = new QHBoxLayout;
                hLayout2->setMargin(0);
                hLayout2->addWidget(_p->frameWidget);
                _p->inPointLayout = new QHBoxLayout;
                _p->inPointLayout->setMargin(0);
                _p->inPointLayout->addWidget(_p->markInPointButton);
                _p->inPointLayout->addWidget(_p->resetInPointButton);
                _p->inPointLayout->addWidget(_p->startWidget);
                hLayout2->addLayout(_p->inPointLayout);
                if (Enum::LAYOUT_DEFAULT == _p->layout)
                    hLayout2->addStretch(1000);
                _p->outPointLayout = new QHBoxLayout;
                _p->outPointLayout->setMargin(0);
                _p->outPointLayout->addWidget(_p->endWidget);
                _p->outPointLayout->addWidget(_p->resetOutPointButton);
                _p->outPointLayout->addWidget(_p->markOutPointButton);
                hLayout2->addLayout(_p->outPointLayout);
                hLayout2->addWidget(_p->durationDisplay);
                if (Enum::LAYOUT_LEFT == _p->layout)
                    hLayout2->addStretch(1000);
                rightLayout->addLayout(hLayout2);
                _p->widgetLayout->addLayout(rightLayout, 1);
            }
            break;
            case Enum::LAYOUT_CENTER:
            {
                auto vLayout = new QVBoxLayout;
                vLayout->setMargin(0);
                vLayout->addWidget(_p->frameSlider);

                auto hLayout = new QHBoxLayout;
                hLayout->setMargin(0);
                hLayout->addStretch(1000);
                hLayout->addWidget(_p->frameWidget);
                _p->inPointLayout = new QHBoxLayout;
                _p->inPointLayout->setMargin(0);
                _p->inPointLayout->addWidget(_p->markInPointButton);
                _p->inPointLayout->addWidget(_p->resetInPointButton);
                _p->inPointLayout->addWidget(_p->startWidget);
                hLayout->addLayout(_p->inPointLayout);
                hLayout->addWidget(_p->playbackButtons);
                hLayout->addWidget(_p->frameButtons);
                _p->outPointLayout = new QHBoxLayout;
                _p->outPointLayout->setMargin(0);
                _p->outPointLayout->addWidget(_p->endWidget);
                _p->outPointLayout->addWidget(_p->resetOutPointButton);
                _p->outPointLayout->addWidget(_p->markOutPointButton);
                hLayout->addLayout(_p->outPointLayout);
                hLayout->addWidget(_p->durationDisplay);
                hLayout->addStretch(1000);
                vLayout->addLayout(hLayout);

                _p->widgetLayout->addLayout(vLayout, 1);
            }
            break;
            case Enum::LAYOUT_MINIMAL:
            {
                _p->widgetLayout->addWidget(_p->playbackButtons);
                _p->widgetLayout->addWidget(_p->frameSlider, 1);
            }
            break;
            default: break;
            }

            switch (_p->layout)
            {
            case Enum::LAYOUT_DEFAULT:
            case Enum::LAYOUT_LEFT:
                _p->speedWidget->show();
                _p->actualSpeedDisplay->show();
                _p->everyFrameButton->show();
                _p->loopWidget->show();
                _p->inOutEnabledButton->show();
                _p->frameSlider->show();
                _p->frameWidget->show();
                _p->frameButtons->show();
                _p->startWidget->show();
                _p->endWidget->show();
                _p->markInPointButton->show();
                _p->resetInPointButton->show();
                _p->markOutPointButton->show();
                _p->resetOutPointButton->show();
                _p->durationDisplay->show();
                break;

            case Enum::LAYOUT_CENTER:
                _p->speedWidget->hide();
                _p->actualSpeedDisplay->hide();
                _p->everyFrameButton->hide();
                _p->loopWidget->hide();
                _p->inOutEnabledButton->hide();
                _p->frameSlider->show();
                _p->frameWidget->show();
                _p->frameButtons->show();
                _p->startWidget->show();
                _p->endWidget->show();
                _p->markInPointButton->show();
                _p->resetInPointButton->show();
                _p->markOutPointButton->show();
                _p->resetOutPointButton->show();
                _p->durationDisplay->show();
                break;

            case Enum::LAYOUT_MINIMAL:
                _p->speedWidget->hide();
                _p->actualSpeedDisplay->hide();
                _p->everyFrameButton->hide();
                _p->loopWidget->hide();
                _p->inOutEnabledButton->hide();
                _p->frameWidget->hide();
                _p->frameButtons->hide();
                _p->startWidget->hide();
                _p->endWidget->hide();
                _p->markInPointButton->hide();
                _p->resetInPointButton->hide();
                _p->markOutPointButton->hide();
                _p->resetOutPointButton->hide();
                _p->durationDisplay->hide();
                break;
            default: break;
            }
        }

        void PlaybackToolBar::styleUpdate()
        {
            const int toolBarSpacing = style()->pixelMetric(QStyle::PM_ToolBarItemSpacing);
            if (_p->actualSpeedAndEveryFrameLayout)
            {
                _p->actualSpeedAndEveryFrameLayout->setSpacing(toolBarSpacing);
            }
            if (_p->loopAndInOutEnabledLayout)
            {
                _p->loopAndInOutEnabledLayout->setSpacing(toolBarSpacing);
            }
            if (_p->inPointLayout)
            {
                _p->inPointLayout->setSpacing(toolBarSpacing);
            }
            if (_p->outPointLayout)
            {
                _p->outPointLayout->setSpacing(toolBarSpacing);
            }
        }
        
    } // namespace ViewLib
} // namespace djv
