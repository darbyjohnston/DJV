// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/TimelineWidget.h>

#include <djvViewApp/FileSettings.h>
#include <djvViewApp/FileSystem.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/PlaybackSettings.h>
#include <djvViewApp/PlaybackSystem.h>
#include <djvViewApp/TimelineSlider.h>
#include <djvViewApp/TimelineWidgetPrivate.h>
#include <djvViewApp/ViewWidget.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/GridLayout.h>
#include <djvUI/Label.h>
#include <djvUI/MultiStateButton.h>
#include <djvUI/PopupButton.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/ToolBar.h>
#include <djvUI/ToolButton.h>

#include <djvAV/AVSystem.h>
#include <djvAV/TimeFunc.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct TimelineWidget::Private
        {
            AV::Time::Units timeUnits = AV::Time::Units::First;
            std::shared_ptr<Media> media;
            AV::IO::Info ioInfo;
            Math::IntRational speed;
            Math::IntRational defaultSpeed;
            float realSpeed = 0.F;
            PlaybackMode playbackMode = PlaybackMode::First;
            Math::Frame::Sequence sequence;
            Math::Frame::Index currentFrame = Math::Frame::invalidIndex;
            AV::IO::InOutPoints inOutPoints;
            Playback playbackPrev = Playback::Count;
            bool audioEnabled = false;
            float audioVolume = 0.F;
            bool audioMute = false;

            std::shared_ptr<UI::PopupButton> speedPopupButton;
            std::shared_ptr<UI::Text::Label> realSpeedLabel;
            std::shared_ptr<UI::MultiStateButton> playbackModeButton;
            std::shared_ptr<FrameWidget> currentFrameWidget;
            std::shared_ptr<FrameWidget> inPointWidget;
            std::shared_ptr<FrameWidget> outPointWidget;
            std::shared_ptr<UI::Text::Label> durationLabel;
            std::shared_ptr<TimelineSlider> timelineSlider;
            std::shared_ptr<UI::PopupButton> audioPopupButton;
            std::shared_ptr<UI::GridLayout> layout;

            std::shared_ptr<Observer::Value<AV::Time::Units> > timeUnitsObserver;
            std::shared_ptr<Observer::Value<std::shared_ptr<Media> > > currentMediaObserver;
            std::shared_ptr<Observer::Value<AV::IO::Info> > ioInfoObserver;
            std::shared_ptr<Observer::Value<Math::IntRational> > speedObserver;
            std::shared_ptr<Observer::Value<Math::IntRational> > defaultSpeedObserver;
            std::shared_ptr<Observer::Value<float> > realSpeedObserver;
            std::shared_ptr<Observer::Value<PlaybackMode> > playbackModeObserver;
            std::shared_ptr<Observer::Value<Math::Frame::Sequence> > sequenceObserver;
            std::shared_ptr<Observer::Value<Math::Frame::Index> > currentFrameObserver;
            std::shared_ptr<Observer::Value<AV::IO::InOutPoints> > inOutPointsObserver;
            std::shared_ptr<Observer::Value<bool> > audioEnabledObserver;
            std::shared_ptr<Observer::Value<float> > volumeObserver;
            std::shared_ptr<Observer::Value<bool> > muteObserver;
            std::shared_ptr<Observer::Value<bool> > cacheEnabledObserver;
            std::shared_ptr<Observer::Value<Math::Frame::Sequence> > cacheSequenceObserver;
            std::shared_ptr<Observer::Value<Math::Frame::Sequence> > cachedFramesObserver;
        };

        void TimelineWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::TimelineWidget");
            setBackgroundColorRole(UI::ColorRole::BackgroundToolBar);

            p.speedPopupButton = UI::PopupButton::create(UI::MenuButtonStyle::Tool, context);
            p.speedPopupButton->setPopupIcon("djvIconPopupMenu");
            p.speedPopupButton->setPopupDefault(UI::Popup::AboveRight);
            p.speedPopupButton->setFontFamily(Render2D::Font::familyMono);
            p.speedPopupButton->setFontSizeRole(UI::MetricsRole::FontSmall);
            p.realSpeedLabel = UI::Text::Label::create(context);
            p.realSpeedLabel->setFontFamily(Render2D::Font::familyMono);
            p.realSpeedLabel->setFontSizeRole(UI::MetricsRole::FontSmall);
            p.realSpeedLabel->setMargin(UI::MetricsRole::MarginSmall);
            p.playbackModeButton = UI::MultiStateButton::create(context);
            p.playbackModeButton->setIconSizeRole(UI::MetricsRole::IconSmall);
            p.playbackModeButton->addIcon("djvIconPlayOnce");
            p.playbackModeButton->addIcon("djvIconPlayLoop");
            p.playbackModeButton->addIcon("djvIconPlayPingPong");

            p.currentFrameWidget = FrameWidget::create(context);
            p.inPointWidget = FrameWidget::create(context);
            auto inPointSetButton = UI::ToolButton::create(context);
            inPointSetButton->setIconSizeRole(UI::MetricsRole::IconSmall);
            inPointSetButton->setVAlign(UI::VAlign::Center);
            auto playbackSystem = context->getSystemT<PlaybackSystem>();
            auto playbackActions = playbackSystem->getActions();
            inPointSetButton->addAction(playbackActions["SetInPoint"]);
            auto inPointResetButton = UI::ToolButton::create(context);
            inPointResetButton->setIconSizeRole(UI::MetricsRole::IconSmall);
            inPointResetButton->setVAlign(UI::VAlign::Center);
            inPointResetButton->addAction(playbackActions["ResetInPoint"]);
            p.outPointWidget = FrameWidget::create(context);
            auto outPointSetButton = UI::ToolButton::create(context);
            outPointSetButton->setIconSizeRole(UI::MetricsRole::IconSmall);
            outPointSetButton->setVAlign(UI::VAlign::Center);
            outPointSetButton->addAction(playbackActions["SetOutPoint"]);
            auto outPointResetButton = UI::ToolButton::create(context);
            outPointResetButton->setIconSizeRole(UI::MetricsRole::IconSmall);
            outPointResetButton->setVAlign(UI::VAlign::Center);
            outPointResetButton->addAction(playbackActions["ResetOutPoint"]);

            p.durationLabel = UI::Text::Label::create(context);
            p.durationLabel->setFontFamily(Render2D::Font::familyMono);
            p.durationLabel->setFontSizeRole(UI::MetricsRole::FontSmall);
            p.durationLabel->setMargin(UI::MetricsRole::MarginSmall);

            p.timelineSlider = TimelineSlider::create(context);
            p.timelineSlider->setMedia(p.media);
            p.timelineSlider->setMargin(UI::MetricsRole::Border);

            p.audioPopupButton = UI::PopupButton::create(UI::MenuButtonStyle::Tool, context);
            p.audioPopupButton->setPopupDefault(UI::Popup::AboveLeft);
            p.audioPopupButton->setVAlign(UI::VAlign::Center);

            auto toolBar = UI::ToolBar::create(context);
            toolBar->setVAlign(UI::VAlign::Center);
            toolBar->setBackgroundColorRole(UI::ColorRole::None);
            toolBar->addAction(playbackActions["InPoint"]);
            toolBar->addAction(playbackActions["PrevFrame"]);
            toolBar->addAction(playbackActions["Reverse"]);
            toolBar->addAction(playbackActions["Forward"]);
            toolBar->addAction(playbackActions["NextFrame"]);
            toolBar->addAction(playbackActions["OutPoint"]);

            p.layout = UI::GridLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->addChild(toolBar);
            p.layout->setGridPos(toolBar, 0, 0);
            p.layout->addChild(p.timelineSlider);
            p.layout->setGridPos(p.timelineSlider, 1, 0);
            p.layout->setStretch(p.timelineSlider, UI::GridStretch::Horizontal);
            p.layout->addChild(p.audioPopupButton);
            p.layout->setGridPos(p.audioPopupButton, 2, 0);
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::None);
            hLayout->addChild(p.speedPopupButton);
            hLayout->addChild(p.realSpeedLabel);
            hLayout->addChild(p.playbackModeButton);
            p.layout->addChild(hLayout);
            p.layout->setGridPos(hLayout, 0, 1);
            hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::None);
            hLayout->addChild(p.currentFrameWidget);
            auto hLayout2 = UI::HorizontalLayout::create(context);
            hLayout2->setSpacing(UI::MetricsRole::None);
            hLayout2->addChild(p.inPointWidget);
            hLayout2->addChild(inPointSetButton);
            hLayout2->addChild(inPointResetButton);
            hLayout->addChild(hLayout2);
            hLayout->addExpander();
            hLayout2 = UI::HorizontalLayout::create(context);
            hLayout2->setSpacing(UI::MetricsRole::None);
            hLayout2->addChild(outPointResetButton);
            hLayout2->addChild(outPointSetButton);
            hLayout2->addChild(p.outPointWidget);
            hLayout->addChild(hLayout2);
            hLayout->addChild(p.durationLabel);
            p.layout->addChild(hLayout);
            p.layout->setGridPos(hLayout, 1, 1);
            addChild(p.layout);

            _widgetUpdate();
            _speedUpdate();
            _realSpeedUpdate();
            _audioUpdate();

            auto weak = std::weak_ptr<TimelineWidget>(std::dynamic_pointer_cast<TimelineWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.speedPopupButton->setOpenCallback(
                [weak, contextWeak]() -> std::shared_ptr<UI::Widget>
                {
                    std::shared_ptr<UI::Widget> out;
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            if (auto media = widget->_p->media)
                            {
                                out = SpeedWidget::create(media, context);
                            }
                        }
                    }
                    return out;
                });

            p.playbackModeButton->setCurrentCallback(
                [weak, contextWeak](int index)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            if (auto media = widget->_p->media)
                            {
                                const PlaybackMode playbackMode = static_cast<PlaybackMode>(index);
                                if (auto media = widget->_p->media)
                                {
                                    media->setPlaybackMode(playbackMode);
                                }
                                auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                                if (auto playbackSettings = settingsSystem->getSettingsT<PlaybackSettings>())
                                {
                                    playbackSettings->setPlaybackMode(playbackMode);
                                }
                                widget->_widgetUpdate();
                            }
                        }
                    }
                });

            p.currentFrameWidget->setCallback(
                [weak](Math::Frame::Index value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto media = widget->_p->media)
                        {
                            media->setCurrentFrame(value);
                        }
                    }
                });

            p.inPointWidget->setCallback(
                [weak](Math::Frame::Index value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto media = widget->_p->media)
                        {
                            const size_t sequenceFrameCount = widget->_p->sequence.getFrameCount();
                            media->setInOutPoints(AV::IO::InOutPoints(
                                widget->_p->inOutPoints.isEnabled(),
                                Math::clamp(value, static_cast<Math::Frame::Index>(0), static_cast<Math::Frame::Index>(sequenceFrameCount > 0 ? (sequenceFrameCount - 1) : 0)),
                                widget->_p->inOutPoints.getOut()));
                            widget->_widgetUpdate();
                        }
                    }
                });

            p.outPointWidget->setCallback(
                [weak](Math::Frame::Index value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto media = widget->_p->media)
                        {
                            const size_t sequenceFrameCount = widget->_p->sequence.getFrameCount();
                            media->setInOutPoints(AV::IO::InOutPoints(
                                widget->_p->inOutPoints.isEnabled(),
                                widget->_p->inOutPoints.getIn(),
                                Math::clamp(value, static_cast<Math::Frame::Index>(0), static_cast<Math::Frame::Index>(sequenceFrameCount > 0 ? (sequenceFrameCount - 1) : 0))));
                            widget->_widgetUpdate();
                        }
                    }
                });

            p.timelineSlider->setCurrentFrameCallback(
                [weak](Math::Frame::Index value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto media = widget->_p->media)
                        {
                            media->setCurrentFrame(value, false);
                        }
                    }
                });

            p.timelineSlider->setCurrentFrameDragCallback(
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto media = widget->_p->media)
                        {
                            if (value)
                            {
                                widget->_p->playbackPrev = media->observePlayback()->get();
                            }
                            else if (widget->_p->playbackPrev != Playback::Count)
                            {
                                media->setPlayback(widget->_p->playbackPrev);
                            }
                        }
                    }
                });

            p.audioPopupButton->setOpenCallback(
                [weak, contextWeak]() -> std::shared_ptr<UI::Widget>
                {
                    std::shared_ptr<UI::Widget> out;
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            if (auto media = widget->_p->media)
                            {
                                out = AudioWidget::create(media, context);
                            }
                        }
                    }
                    return out;
                });

            auto avSystem = context->getSystemT<AV::AVSystem>();
            p.timeUnitsObserver = Observer::Value<AV::Time::Units>::create(
                avSystem->observeTimeUnits(),
                [weak](AV::Time::Units value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->timeUnits = value;
                        widget->_widgetUpdate();
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            if (auto fileSettings = settingsSystem->getSettingsT<FileSettings>())
            {
                p.cacheEnabledObserver = Observer::Value<bool>::create(
                    fileSettings->observeCacheEnabled(),
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->timelineSlider->setCacheEnabled(value);
                        }
                    });
            }

            if (auto fileSystem = context->getSystemT<FileSystem>())
            {
                p.currentMediaObserver = Observer::Value<std::shared_ptr<Media>>::create(
                    fileSystem->observeCurrentMedia(),
                    [weak](const std::shared_ptr<Media>& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->media = value;
                            widget->_p->timelineSlider->setMedia(value);
                            if (widget->_p->media)
                            {
                                widget->_p->ioInfoObserver = Observer::Value<AV::IO::Info>::create(
                                    widget->_p->media->observeInfo(),
                                    [weak](const AV::IO::Info& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->ioInfo = value;
                                            widget->_widgetUpdate();
                                            widget->_audioUpdate();
                                        }
                                    });

                                widget->_p->speedObserver = Observer::Value<Math::IntRational>::create(
                                    widget->_p->media->observeSpeed(),
                                    [weak](const Math::IntRational& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->speed = value;
                                            widget->_widgetUpdate();
                                            widget->_speedUpdate();
                                        }
                                    });

                                widget->_p->defaultSpeedObserver = Observer::Value<Math::IntRational>::create(
                                    widget->_p->media->observeDefaultSpeed(),
                                    [weak](const Math::IntRational& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->defaultSpeed = value;
                                            widget->_widgetUpdate();
                                            widget->_speedUpdate();
                                        }
                                    });

                                widget->_p->realSpeedObserver = Observer::Value<float>::create(
                                    widget->_p->media->observeRealSpeed(),
                                    [weak](float value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->realSpeed = value;
                                            widget->_realSpeedUpdate();
                                        }
                                    });

                                widget->_p->playbackModeObserver = Observer::Value<PlaybackMode>::create(
                                    widget->_p->media->observePlaybackMode(),
                                    [weak](PlaybackMode value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->playbackMode = value;
                                            widget->_widgetUpdate();
                                            widget->_speedUpdate();
                                        }
                                    });

                                widget->_p->sequenceObserver = Observer::Value<Math::Frame::Sequence>::create(
                                    widget->_p->media->observeSequence(),
                                    [weak](const Math::Frame::Sequence& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->sequence = value;
                                            widget->_widgetUpdate();
                                        }
                                    });

                                widget->_p->currentFrameObserver = Observer::Value<Math::Frame::Index>::create(
                                    widget->_p->media->observeCurrentFrame(),
                                    [weak](Math::Frame::Index value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->currentFrame = value;
                                            widget->_widgetUpdate();
                                        }
                                    });

                                widget->_p->inOutPointsObserver = Observer::Value<AV::IO::InOutPoints>::create(
                                    widget->_p->media->observeInOutPoints(),
                                    [weak](const AV::IO::InOutPoints& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->inOutPoints = value;
                                            widget->_widgetUpdate();
                                        }
                                    });

                                widget->_p->audioEnabledObserver = Observer::Value<bool>::create(
                                    widget->_p->media->observeAudioEnabled(),
                                    [weak](bool value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->audioEnabled = value;
                                            widget->_audioUpdate();
                                        }
                                    });

                                widget->_p->volumeObserver = Observer::Value<float>::create(
                                    widget->_p->media->observeVolume(),
                                    [weak](float value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->audioVolume = value;
                                            widget->_audioUpdate();
                                        }
                                    });

                                widget->_p->muteObserver = Observer::Value<bool>::create(
                                    widget->_p->media->observeMute(),
                                    [weak](bool value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->audioMute = value;
                                            widget->_audioUpdate();
                                        }
                                    });

                                widget->_p->cacheSequenceObserver = Observer::Value<Math::Frame::Sequence>::create(
                                    widget->_p->media->observeCacheSequence(),
                                    [weak](const Math::Frame::Sequence& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->timelineSlider->setCacheSequence(value);
                                        }
                                    });

                                widget->_p->cachedFramesObserver = Observer::Value<Math::Frame::Sequence>::create(
                                    widget->_p->media->observeCachedFrames(),
                                    [weak](const Math::Frame::Sequence& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->timelineSlider->setCachedFrames(value);
                                        }
                                    });
                            }
                            else
                            {
                                widget->_p->ioInfo = AV::IO::Info();
                                widget->_p->speed = Math::IntRational();
                                widget->_p->defaultSpeed = Math::IntRational();
                                widget->_p->realSpeed = 0.F;
                                widget->_p->playbackMode = PlaybackMode::First;
                                widget->_p->sequence = Math::Frame::Sequence();
                                widget->_p->currentFrame = Math::Frame::invalidIndex;
                                widget->_p->inOutPoints = AV::IO::InOutPoints();
                                widget->_p->playbackPrev = Playback::Count;
                                widget->_p->audioEnabled = false;
                                widget->_p->audioVolume = 0.F;
                                widget->_p->audioMute = false;

                                widget->_p->ioInfoObserver.reset();
                                widget->_p->speedObserver.reset();
                                widget->_p->defaultSpeedObserver.reset();
                                widget->_p->realSpeedObserver.reset();
                                widget->_p->playbackModeObserver.reset();
                                widget->_p->sequenceObserver.reset();
                                widget->_p->currentFrameObserver.reset();
                                widget->_p->inOutPointsObserver.reset();
                                widget->_p->audioEnabledObserver.reset();
                                widget->_p->volumeObserver.reset();
                                widget->_p->muteObserver.reset();
                                widget->_p->cacheSequenceObserver.reset();
                                widget->_p->cachedFramesObserver.reset();
                                widget->_widgetUpdate();
                                widget->_speedUpdate();
                                widget->_realSpeedUpdate();
                                widget->_audioUpdate();
                            }
                        }
                    });
            }
        }

        TimelineWidget::TimelineWidget() :
            _p(new Private)
        {}

        TimelineWidget::~TimelineWidget()
        {}

        std::shared_ptr<TimelineWidget> TimelineWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<TimelineWidget>(new TimelineWidget);
            out->_init(context);
            return out;
        }

        void TimelineWidget::_preLayoutEvent(System::Event::PreLayout& event)
        {
            DJV_PRIVATE_PTR();
            _setMinimumSize(p.layout->getMinimumSize());
        }

        void TimelineWidget::_layoutEvent(System::Event::Layout& event)
        {
            DJV_PRIVATE_PTR();
            p.layout->setGeometry(getGeometry());
        }

        void TimelineWidget::_initEvent(System::Event::Init & event)
        {
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.speedPopupButton->setTooltip(_getText(DJV_TEXT("playback_speed_popup_tooltip")));
                p.realSpeedLabel->setTooltip(_getText(DJV_TEXT("playback_real_speed_tooltip")));
                p.currentFrameWidget->setTooltip(_getText(DJV_TEXT("playback_current_frame_tooltip")));
                p.inPointWidget->setTooltip(_getText(DJV_TEXT("playback_in_point_tooltip")));
                p.outPointWidget->setTooltip(_getText(DJV_TEXT("playback_out_point_tooltip")));
                p.durationLabel->setTooltip(_getText(DJV_TEXT("playback_duration_tooltip")));

                p.audioPopupButton->setTooltip(_getText(DJV_TEXT("audio_popup_tooltip")));

                _widgetUpdate();
                _speedUpdate();
            }
        }

        void TimelineWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                setEnabled(p.media.get() && p.sequence.getFrameCount() > 1);

                p.playbackModeButton->setCurrentIndex(static_cast<int>(p.playbackMode));
                switch (p.playbackMode)
                {
                case PlaybackMode::Once:
                    p.playbackModeButton->setTooltip(_getText(DJV_TEXT("playback_mode_once_tooltip")));
                    break;
                case PlaybackMode::Loop:
                    p.playbackModeButton->setTooltip(_getText(DJV_TEXT("playback_mode_loop_tooltip")));
                    break;
                case PlaybackMode::PingPong:
                    p.playbackModeButton->setTooltip(_getText(DJV_TEXT("playback_mode_ping-pong_tooltip")));
                    break;
                default: break;
                }

                p.currentFrameWidget->setSequence(p.sequence);
                p.currentFrameWidget->setSpeed(p.defaultSpeed);
                p.currentFrameWidget->setFrame(p.currentFrame);

                p.inPointWidget->setSequence(p.sequence);
                p.inPointWidget->setSpeed(p.defaultSpeed);
                p.inPointWidget->setFrame(p.inOutPoints.getIn());
                p.inPointWidget->setEnabled(p.inOutPoints.isEnabled());

                p.outPointWidget->setSequence(p.sequence);
                p.outPointWidget->setSpeed(p.defaultSpeed);
                p.outPointWidget->setFrame(p.inOutPoints.getOut());
                p.outPointWidget->setEnabled(p.inOutPoints.isEnabled());

                std::string text;
                if (p.sequence.getFrameCount() > 1)
                {
                    text = AV::Time::toString(p.sequence.getFrameCount(), p.defaultSpeed, p.timeUnits);
                }
                p.durationLabel->setText(text);

                p.timelineSlider->setInOutPointsEnabled(p.inOutPoints.isEnabled());
                p.timelineSlider->setInPoint(p.inOutPoints.getIn());
                p.timelineSlider->setOutPoint(p.inOutPoints.getOut());
            }
        }

        void TimelineWidget::_speedUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                std::stringstream ss;
                ss.precision(2);
                ss << std::fixed << p.speed.toFloat();
                p.speedPopupButton->setText(ss.str());
            }
        }

        void TimelineWidget::_realSpeedUpdate()
        {
            DJV_PRIVATE_PTR();
            std::stringstream ss;
            ss.precision(2);
            ss << std::fixed << p.realSpeed;
            p.realSpeedLabel->setText(ss.str());
        }

        void TimelineWidget::_audioUpdate()
        {
            DJV_PRIVATE_PTR();
            p.audioPopupButton->setVisible(p.audioEnabled);
            if (p.audioMute)
            {
                p.audioPopupButton->setIcon("djvIconAudioMute");
            }
            else if (p.audioVolume < 1.F / 4.F)
            {
                p.audioPopupButton->setIcon("djvIconAudio0");
            }
            else if (p.audioVolume < 2.F / 4.F)
            {
                p.audioPopupButton->setIcon("djvIconAudio1");
            }
            else if (p.audioVolume < 3.F / 4.F)
            {
                p.audioPopupButton->setIcon("djvIconAudio2");
            }
            else
            {
                p.audioPopupButton->setIcon("djvIconAudio3");
            }
        }

    } // namespace ViewApp
} // namespace djv

