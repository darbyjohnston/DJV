// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Widgets/BottomToolBar.h>

#include <djvApp/Actions/AudioActions.h>
#include <djvApp/Actions/FrameActions.h>
#include <djvApp/Actions/PlaybackActions.h>
#include <djvApp/Models/AudioModel.h>
#include <djvApp/Models/TimeUnitsModel.h>
#include <djvApp/Widgets/AudioPopup.h>
#include <djvApp/Widgets/ShuttleWidget.h>
#include <djvApp/Widgets/SpeedPopup.h>
#include <djvApp/App.h>

#include <tlTimelineUI/TimeEdit.h>
#include <tlTimelineUI/TimeLabel.h>

#include <tlTimeline/Player.h>

#include <dtk/ui/ComboBox.h>
#include <dtk/ui/DoubleEdit.h>
#include <dtk/ui/DoubleModel.h>
#include <dtk/ui/RowLayout.h>
#include <dtk/ui/Spacer.h>
#include <dtk/ui/ToolButton.h>

namespace djv
{
    namespace app
    {
        struct BottomToolBar::Private
        {
            std::weak_ptr<App> app;
            std::shared_ptr<tl::timeline::Player> player;
            std::shared_ptr<dtk::DoubleModel> speedModel;
            double startSpeed = 0.0;
            OTIO_NS::RationalTime startTime = tl::time::invalidTime;

            std::map<std::string, std::shared_ptr<dtk::ToolButton> > buttons;
            std::shared_ptr<ShuttleWidget> playbackShuttle;
            std::shared_ptr<ShuttleWidget> frameShuttle;
            std::shared_ptr<tl::timelineui::TimeEdit> currentTimeEdit;
            std::shared_ptr<tl::timelineui::TimeLabel> durationLabel;
            std::shared_ptr<dtk::DoubleEdit> speedEdit;
            std::shared_ptr<dtk::ToolButton> speedButton;
            std::shared_ptr<SpeedPopup> speedPopup;
            std::shared_ptr<dtk::ComboBox> timeUnitsComboBox;
            std::shared_ptr<dtk::ToolButton> audioButton;
            std::shared_ptr<AudioPopup> audioPopup;
            std::shared_ptr<dtk::ToolButton> muteButton;
            std::shared_ptr<dtk::HorizontalLayout> layout;

            std::shared_ptr<dtk::ValueObserver<tl::timeline::TimeUnits> > timeUnitsObserver;
            std::shared_ptr<dtk::ValueObserver<std::shared_ptr<tl::timeline::Player> > > playerObserver;
            std::shared_ptr<dtk::ValueObserver<double> > speedObserver;
            std::shared_ptr<dtk::ValueObserver<double> > speedObserver2;
            std::shared_ptr<dtk::ValueObserver<OTIO_NS::RationalTime> > currentTimeObserver;
        };

        void BottomToolBar::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<PlaybackActions>& playbackActions,
            const std::shared_ptr<FrameActions>& frameActions,
            const std::shared_ptr<AudioActions>& audioActions,
            const std::shared_ptr<IWidget>& parent)
        {
            IWidget::_init(
                context,
                "djv::app::BottomToolBar",
                parent);
            DTK_P();

            p.app = app;

            p.speedModel = dtk::DoubleModel::create(context);
            p.speedModel->setRange(dtk::RangeD(0.0, 1000000.0));
            p.speedModel->setStep(1.F);
            p.speedModel->setLargeStep(10.F);

            auto actions = playbackActions->getActions();
            p.buttons["Stop"] = dtk::ToolButton::create(context, actions["Stop"]);
            p.buttons["Forward"] = dtk::ToolButton::create(context, actions["Forward"]);
            p.buttons["Reverse"] = dtk::ToolButton::create(context, actions["Reverse"]);

            p.playbackShuttle = ShuttleWidget::create(context, "PlaybackShuttle");
            p.playbackShuttle->setTooltip("Playback shuttle");

            actions = frameActions->getActions();
            p.buttons["Start"] = dtk::ToolButton::create(context, actions["Start"]);
            p.buttons["End"] = dtk::ToolButton::create(context, actions["End"]);
            p.buttons["Prev"] = dtk::ToolButton::create(context, actions["Prev"]);
            p.buttons["Prev"]->setRepeatClick(true);
            p.buttons["Next"] = dtk::ToolButton::create(context, actions["Next"]);
            p.buttons["Next"]->setRepeatClick(true);

            p.frameShuttle = ShuttleWidget::create(context, "FrameShuttle");
            p.frameShuttle->setTooltip("Frame shuttle");

            auto timeUnitsModel = app->getTimeUnitsModel();
            p.currentTimeEdit = tl::timelineui::TimeEdit::create(context, timeUnitsModel);
            p.currentTimeEdit->setTooltip("Current time");

            p.durationLabel = tl::timelineui::TimeLabel::create(context, timeUnitsModel);
            p.durationLabel->setFontRole(dtk::FontRole::Mono);
            p.durationLabel->setMarginRole(dtk::SizeRole::MarginInside);
            p.durationLabel->setTooltip("Duration");

            p.speedEdit = dtk::DoubleEdit::create(context, p.speedModel);
            p.speedEdit->setTooltip("Current speed");
            p.speedButton = dtk::ToolButton::create(context, "FPS");
            p.speedButton->setIcon("MenuArrow");
            p.speedButton->setTooltip("Speed menu");

            p.timeUnitsComboBox = dtk::ComboBox::create(context, tl::timeline::getTimeUnitsLabels());
            p.timeUnitsComboBox->setTooltip("Time units");

            p.audioButton = dtk::ToolButton::create(context);
            p.audioButton->setIcon("Volume");
            p.audioButton->setTooltip("Audio volume");
            actions = audioActions->getActions();
            p.muteButton = dtk::ToolButton::create(context, actions["Mute"]);

            p.layout = dtk::HorizontalLayout::create(context, shared_from_this());
            p.layout->setMarginRole(dtk::SizeRole::MarginInside);
            p.layout->setSpacingRole(dtk::SizeRole::SpacingSmall);
            auto hLayout = dtk::HorizontalLayout::create(context, p.layout);
            hLayout->setSpacingRole(dtk::SizeRole::None);
            p.buttons["Reverse"]->setParent(hLayout);
            p.buttons["Stop"]->setParent(hLayout);
            p.buttons["Forward"]->setParent(hLayout);
            p.playbackShuttle->setParent(hLayout);
            p.buttons["Start"]->setParent(hLayout);
            p.buttons["Prev"]->setParent(hLayout);
            p.buttons["Next"]->setParent(hLayout);
            p.buttons["End"]->setParent(hLayout);
            p.frameShuttle->setParent(hLayout);
            p.currentTimeEdit->setParent(p.layout);
            p.durationLabel->setParent(p.layout);
            hLayout = dtk::HorizontalLayout::create(context, p.layout);
            hLayout->setSpacingRole(dtk::SizeRole::SpacingTool);
            p.speedEdit->setParent(hLayout);
            p.speedButton->setParent(hLayout);
            p.timeUnitsComboBox->setParent(p.layout);
            auto spacer = dtk::Spacer::create(context, dtk::Orientation::Horizontal, p.layout);
            spacer->setHStretch(dtk::Stretch::Expanding);
            hLayout = dtk::HorizontalLayout::create(context, p.layout);
            hLayout->setSpacingRole(dtk::SizeRole::SpacingTool);
            p.audioButton->setParent(hLayout);
            p.muteButton->setParent(hLayout);

            p.playbackShuttle->setActiveCallback(
                [this](bool value)
                {
                    DTK_P();
                    if (p.player)
                    {
                        if (value)
                        {
                            p.startSpeed = p.player->getSpeed();
                            if (p.player->isStopped())
                            {
                                p.player->forward();
                            }
                        }
                        else
                        {
                            p.player->setSpeed(p.startSpeed);
                        }
                    }
                });
            p.playbackShuttle->setCallback(
                [this](int value)
                {
                    DTK_P();
                    if (p.player)
                    {
                        const double v = value * 2.0;
                        const double speed = std::max(1.0, p.startSpeed + v);
                        p.player->setSpeed(speed);
                    }
                });

            p.frameShuttle->setActiveCallback(
                [this](bool)
                {
                    DTK_P();
                    if (p.player)
                    {
                        p.player->stop();
                        p.startTime = p.player->getCurrentTime();
                    }
                });
            p.frameShuttle->setCallback(
                [this](int value)
                {
                    DTK_P();
                    if (p.player)
                    {
                        p.player->seek(OTIO_NS::RationalTime(
                            p.startTime.value() + value,
                            p.startTime.rate()));
                    }
                });

            p.currentTimeEdit->setCallback(
                [this](const OTIO_NS::RationalTime& value)
                {
                    DTK_P();
                    if (p.player)
                    {
                        p.player->stop();
                        p.player->seek(value);
                        p.currentTimeEdit->setValue(p.player->getCurrentTime());
                    }
                });

            p.speedButton->setPressedCallback(
                [this]
                {
                    _showSpeedPopup();
                });

            auto appWeak = std::weak_ptr<App>(app);
            p.timeUnitsComboBox->setIndexCallback(
                [appWeak](int value)
                {
                    if (auto app = appWeak.lock())
                    {
                        app->getTimeUnitsModel()->setTimeUnits(
                            static_cast<tl::timeline::TimeUnits>(value));
                    }
                });

            p.audioButton->setPressedCallback(
                [this]
                {
                    _showAudioPopup();
                });
            p.muteButton->setCheckedCallback(
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        app->getAudioModel()->setMute(value);
                    }
                });

            p.timeUnitsObserver = dtk::ValueObserver<tl::timeline::TimeUnits>::create(
                app->getTimeUnitsModel()->observeTimeUnits(),
                [this](tl::timeline::TimeUnits value)
                {
                    _p->timeUnitsComboBox->setCurrentIndex(static_cast<int>(value));
                });

            p.playerObserver = dtk::ValueObserver<std::shared_ptr<tl::timeline::Player> >::create(
                app->observePlayer(),
                [this](const std::shared_ptr<tl::timeline::Player>& value)
                {
                    _playerUpdate(value);
                });

            p.speedObserver2 = dtk::ValueObserver<double>::create(
                p.speedModel->observeValue(),
                [this](double value)
                {
                    DTK_P();
                    if (p.player)
                    {
                        p.player->setSpeed(value);
                    }
                });
        }

        BottomToolBar::BottomToolBar() :
            _p(new Private)
        {}

        BottomToolBar::~BottomToolBar()
        {}

        std::shared_ptr<BottomToolBar> BottomToolBar::create(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<PlaybackActions>& playbackActions,
            const std::shared_ptr<FrameActions>& frameActions,
            const std::shared_ptr<AudioActions>& audioActions,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<BottomToolBar>(new BottomToolBar);
            out->_init(context, app, playbackActions, frameActions, audioActions, parent);
            return out;
        }

        void BottomToolBar::focusCurrentFrame()
        {
            _p->currentTimeEdit->takeKeyFocus();
        }

        void BottomToolBar::setGeometry(const dtk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void BottomToolBar::sizeHintEvent(const dtk::SizeHintEvent& event)
        {
            IWidget::sizeHintEvent(event);
            _setSizeHint(_p->layout->getSizeHint());
        }

        void BottomToolBar::_playerUpdate(const std::shared_ptr<tl::timeline::Player>& value)
        {
            DTK_P();

            p.speedObserver.reset();
            p.currentTimeObserver.reset();

            p.player = value;

            p.durationLabel->setValue(
                p.player ?
                p.player->getTimeRange().duration() :
                tl::time::invalidTime);

            if (p.player)
            {
                p.speedObserver = dtk::ValueObserver<double>::create(
                    p.player->observeSpeed(),
                    [this](double value)
                    {
                        _p->speedModel->setValue(value);
                    });

                p.currentTimeObserver = dtk::ValueObserver<OTIO_NS::RationalTime>::create(
                    p.player->observeCurrentTime(),
                    [this](const OTIO_NS::RationalTime& value)
                    {
                        _p->currentTimeEdit->setValue(value);
                    });
            }
            else
            {
                p.speedModel->setValue(0.0);
                p.currentTimeEdit->setValue(tl::time::invalidTime);
            }

            p.playbackShuttle->setEnabled(p.player.get());
            p.frameShuttle->setEnabled(p.player.get());
            p.currentTimeEdit->setEnabled(p.player.get());
            p.durationLabel->setEnabled(p.player.get());
            p.speedEdit->setEnabled(p.player.get());
            p.speedButton->setEnabled(p.player.get());
        }

        void BottomToolBar::_showSpeedPopup()
        {
            DTK_P();
            auto context = getContext();
            auto window = getWindow();
            if (context && window)
            {
                if (!p.speedPopup)
                {
                    const double defaultSpeed =
                        p.player ?
                        p.player->getDefaultSpeed() :
                        0.0;
                    p.speedPopup = SpeedPopup::create(context, defaultSpeed);
                    p.speedPopup->open(window, p.speedButton->getGeometry());
                    std::weak_ptr<BottomToolBar> weak(std::dynamic_pointer_cast<BottomToolBar>(shared_from_this()));
                    p.speedPopup->setCallback(
                        [weak](double value)
                        {
                            if (auto widget = weak.lock())
                            {
                                if (widget->_p->player)
                                {
                                    widget->_p->player->setSpeed(value);
                                }
                                widget->_p->speedPopup->close();
                            }
                        });
                    p.speedPopup->setCloseCallback(
                        [weak]
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->speedPopup.reset();
                            }
                        });
                }
                else
                {
                    p.speedPopup->close();
                    p.speedPopup.reset();
                }
            }
        }

        void BottomToolBar::_showAudioPopup()
        {
            DTK_P();
            auto context = getContext();
            auto app = p.app.lock();
            auto window = getWindow();

            if (context && app && window)
            {
                if (!p.audioPopup)
                {
                    p.audioPopup = AudioPopup::create(context, app);
                    p.audioPopup->open(window, p.audioButton->getGeometry());
                    std::weak_ptr<BottomToolBar> weak(std::dynamic_pointer_cast<BottomToolBar>(shared_from_this()));
                    p.audioPopup->setCloseCallback(
                        [weak]
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->audioPopup.reset();
                            }
                        });
                }
                else
                {
                    p.audioPopup->close();
                    p.audioPopup.reset();
                }
            }
        }
    }
}
