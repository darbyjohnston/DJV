// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/TimelineWidget.h>

#include <djvViewApp/FileSettings.h>
#include <djvViewApp/FileSystem.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/PlaybackSettings.h>
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
            Math::Rational speed;
            Math::Rational defaultSpeed;
            float realSpeed = 0.F;
            PlaybackMode playbackMode = PlaybackMode::First;
            Math::Frame::Sequence sequence;
            Math::Frame::Index currentFrame = Math::Frame::invalidIndex;
            AV::IO::InOutPoints inOutPoints;
            Playback playbackPrev = Playback::Count;
            bool audioEnabled = false;
            float audioVolume = 0.F;
            bool audioMute = false;

            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::ActionGroup> playbackActionGroup;
            std::shared_ptr<UI::PopupButton> speedPopupButton;
            std::shared_ptr<UI::Label> realSpeedLabel;
            std::shared_ptr<UI::MultiStateButton> playbackModeButton;
            std::shared_ptr<FrameWidget> currentFrameWidget;
            std::shared_ptr<FrameWidget> inPointWidget;
            std::shared_ptr<UI::ToolButton> inPointSetButton;
            std::shared_ptr<UI::ToolButton> inPointResetButton;
            std::shared_ptr<FrameWidget> outPointWidget;
            std::shared_ptr<UI::ToolButton> outPointSetButton;
            std::shared_ptr<UI::ToolButton> outPointResetButton;
            std::shared_ptr<UI::Label> durationLabel;
            std::shared_ptr<TimelineSlider> timelineSlider;
            std::shared_ptr<UI::PopupButton> audioPopupButton;
            std::shared_ptr<UI::GridLayout> layout;

            std::shared_ptr<Observer::Value<AV::Time::Units> > timeUnitsObserver;
            std::shared_ptr<Observer::Value<std::shared_ptr<Media> > > currentMediaObserver;
            std::shared_ptr<Observer::Value<AV::IO::Info> > ioInfoObserver;
            std::shared_ptr<Observer::Value<Math::Rational> > speedObserver;
            std::shared_ptr<Observer::Value<Math::Rational> > defaultSpeedObserver;
            std::shared_ptr<Observer::Value<float> > realSpeedObserver;
            std::shared_ptr<Observer::Value<PlaybackMode> > playbackModeObserver;
            std::shared_ptr<Observer::Value<Math::Frame::Sequence> > sequenceObserver;
            std::shared_ptr<Observer::Value<Math::Frame::Index> > currentFrameObserver;
            std::shared_ptr<Observer::Value<AV::IO::InOutPoints> > inOutPointsObserver;
            std::shared_ptr<Observer::Value<Playback> > playbackObserver;
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

            p.actions["Forward"] = UI::Action::create();
            p.actions["Forward"]->setIcon("djvIconPlaybackForward");
            p.actions["Forward"]->setCheckedIcon("djvIconPlaybackStop");
            p.actions["Reverse"] = UI::Action::create();
            p.actions["Reverse"]->setIcon("djvIconPlaybackReverse");
            p.actions["Reverse"]->setCheckedIcon("djvIconPlaybackStop");
            p.playbackActionGroup = UI::ActionGroup::create(UI::ButtonType::Exclusive);
            p.playbackActionGroup->setActions({
                p.actions["Forward"],
                p.actions["Reverse"] });

            p.actions["InPoint"] = UI::Action::create();
            p.actions["InPoint"]->setIcon("djvIconFrameStart");
            p.actions["PrevFrame"] = UI::Action::create();
            p.actions["PrevFrame"]->setIcon("djvIconFramePrev");
            p.actions["PrevFrame"]->setAutoRepeat(true);
            p.actions["NextFrame"] = UI::Action::create();
            p.actions["NextFrame"]->setIcon("djvIconFrameNext");
            p.actions["NextFrame"]->setAutoRepeat(true);
            p.actions["OutPoint"] = UI::Action::create();
            p.actions["OutPoint"]->setIcon("djvIconFrameEnd");

            p.speedPopupButton = UI::PopupButton::create(UI::MenuButtonStyle::Tool, context);
            p.speedPopupButton->setPopupIcon("djvIconPopupMenu");
            p.speedPopupButton->setPopupDefault(UI::Popup::AboveRight);
            p.speedPopupButton->setFontFamily(Render2D::Font::familyMono);
            p.speedPopupButton->setFontSizeRole(UI::MetricsRole::FontSmall);
            p.realSpeedLabel = UI::Label::create(context);
            p.realSpeedLabel->setFontFamily(Render2D::Font::familyMono);
            p.realSpeedLabel->setFontSizeRole(UI::MetricsRole::FontSmall);
            p.realSpeedLabel->setMargin(UI::MetricsRole::MarginSmall);
            p.playbackModeButton = UI::MultiStateButton::create(context);
            p.playbackModeButton->setInsideMargin(UI::MetricsRole::None);
            p.playbackModeButton->addIcon("djvIconPlayOnce");
            p.playbackModeButton->addIcon("djvIconPlayLoop");
            p.playbackModeButton->addIcon("djvIconPlayPingPong");

            p.currentFrameWidget = FrameWidget::create(context);
            p.inPointWidget = FrameWidget::create(context);
            p.inPointSetButton = UI::ToolButton::create(context);
            p.inPointSetButton->setIcon("djvIconFrameSetStartSmall");
            p.inPointSetButton->setInsideMargin(UI::MetricsRole::None);
            p.inPointSetButton->setVAlign(UI::VAlign::Center);
            p.inPointResetButton = UI::ToolButton::create(context);
            p.inPointResetButton->setIcon("djvIconClearSmall");
            p.inPointResetButton->setInsideMargin(UI::MetricsRole::None);
            p.inPointResetButton->setVAlign(UI::VAlign::Center);
            p.outPointWidget = FrameWidget::create(context);
            p.outPointSetButton = UI::ToolButton::create(context);
            p.outPointSetButton->setIcon("djvIconFrameSetEndSmall");
            p.outPointSetButton->setInsideMargin(UI::MetricsRole::None);
            p.outPointSetButton->setVAlign(UI::VAlign::Center);
            p.outPointResetButton = UI::ToolButton::create(context);
            p.outPointResetButton->setIcon("djvIconClearSmall");
            p.outPointResetButton->setInsideMargin(UI::MetricsRole::None);
            p.outPointResetButton->setVAlign(UI::VAlign::Center);

            p.durationLabel = UI::Label::create(context);
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
            toolBar->setBackgroundRole(UI::ColorRole::None);
            toolBar->addAction(p.actions["InPoint"]);
            toolBar->addAction(p.actions["PrevFrame"]);
            toolBar->addAction(p.actions["Reverse"]);
            toolBar->addAction(p.actions["Forward"]);
            toolBar->addAction(p.actions["NextFrame"]);
            toolBar->addAction(p.actions["OutPoint"]);

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
            hLayout2->addChild(p.inPointSetButton);
            hLayout2->addChild(p.inPointResetButton);
            hLayout->addChild(hLayout2);
            hLayout->addExpander();
            hLayout2 = UI::HorizontalLayout::create(context);
            hLayout2->setSpacing(UI::MetricsRole::None);
            hLayout2->addChild(p.outPointResetButton);
            hLayout2->addChild(p.outPointSetButton);
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
            p.actions["InPoint"]->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto media = widget->_p->media)
                        {
                            media->inPoint();
                        }
                    }
                });

            p.actions["PrevFrame"]->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto media = widget->_p->media)
                        {
                            media->prevFrame();
                        }
                    }
                });

            p.actions["NextFrame"]->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto media = widget->_p->media)
                        {
                            media->nextFrame();
                        }
                    }
                });

            p.actions["OutPoint"]->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto media = widget->_p->media)
                        {
                            media->outPoint();
                        }
                    }
                });

            p.playbackActionGroup->setExclusiveCallback(
                [weak](int index)
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto media = widget->_p->media)
                        {
                            Playback playback = Playback::Stop;
                            switch (index)
                            {
                            case 0: playback = Playback::Forward; break;
                            case 1: playback = Playback::Reverse; break;
                            }
                            media->setPlayback(playback);
                        }
                    }
                });

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

            p.playbackModeButton->setCallback(
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
                            const auto& inOutPoints = media->observeInOutPoints()->get();
                            const size_t sequenceFrameCount = widget->_p->sequence.getFrameCount();
                            media->setInOutPoints(AV::IO::InOutPoints(
                                inOutPoints.isEnabled(),
                                Math::clamp(value, static_cast<Math::Frame::Index>(0), static_cast<Math::Frame::Index>(sequenceFrameCount > 0 ? (sequenceFrameCount - 1) : 0)),
                                inOutPoints.getOut()));
                            widget->_widgetUpdate();
                        }
                    }
                });

            p.inPointSetButton->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto media = widget->_p->media)
                        {
                            media->setInPoint();
                        }
                    }
                });

            p.inPointResetButton->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto media = widget->_p->media)
                        {
                            media->resetInPoint();
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
                            const auto& inOutPoints = media->observeInOutPoints()->get();
                            const size_t sequenceFrameCount = widget->_p->sequence.getFrameCount();
                            media->setInOutPoints(AV::IO::InOutPoints(
                                inOutPoints.isEnabled(),
                                inOutPoints.getIn(),
                                Math::clamp(value, static_cast<Math::Frame::Index>(0), static_cast<Math::Frame::Index>(sequenceFrameCount > 0 ? (sequenceFrameCount - 1) : 0))));
                            widget->_widgetUpdate();
                        }
                    }
                });

            p.outPointSetButton->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto media = widget->_p->media)
                        {
                            media->setOutPoint();
                        }
                    }
                });

            p.outPointResetButton->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto media = widget->_p->media)
                        {
                            media->resetOutPoint();
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

                                widget->_p->speedObserver = Observer::Value<Math::Rational>::create(
                                    widget->_p->media->observeSpeed(),
                                    [weak](const Math::Rational& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->speed = value;
                                            widget->_widgetUpdate();
                                            widget->_speedUpdate();
                                        }
                                    });

                                widget->_p->defaultSpeedObserver = Observer::Value<Math::Rational>::create(
                                    widget->_p->media->observeDefaultSpeed(),
                                    [weak](const Math::Rational& value)
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

                                widget->_p->playbackObserver = Observer::Value<Playback>::create(
                                    widget->_p->media->observePlayback(),
                                    [weak](Playback value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            int index = -1;
                                            switch (value)
                                            {
                                            case Playback::Forward: index = 0; break;
                                            case Playback::Reverse: index = 1; break;
                                            default: break;
                                            }
                                            widget->_p->playbackActionGroup->setChecked(index);
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
                                widget->_p->speed = Math::Rational();
                                widget->_p->defaultSpeed = Math::Rational();
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
                                widget->_p->playbackObserver.reset();
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
                p.actions["Forward"]->setTooltip(_getText(DJV_TEXT("playback_forward_tooltip")));
                p.actions["Reverse"]->setTooltip(_getText(DJV_TEXT("playback_reverse_tooltip")));
                p.actions["InPoint"]->setTooltip(_getText(DJV_TEXT("playback_go_to_in_point_tooltip")));
                p.actions["NextFrame"]->setTooltip(_getText(DJV_TEXT("playback_next_frame_tooltip")));
                p.actions["PrevFrame"]->setTooltip(_getText(DJV_TEXT("playback_previous_frame_tooltip")));
                p.actions["OutPoint"]->setTooltip(_getText(DJV_TEXT("playback_go_to_out_point_tooltip")));

                p.speedPopupButton->setTooltip(_getText(DJV_TEXT("playback_speed_popup_tooltip")));
                p.realSpeedLabel->setTooltip(_getText(DJV_TEXT("playback_real_speed_tooltip")));
                p.currentFrameWidget->setTooltip(_getText(DJV_TEXT("playback_current_frame_tooltip")));
                p.inPointWidget->setTooltip(_getText(DJV_TEXT("playback_in_point_tooltip")));
                p.inPointSetButton->setTooltip(_getText(DJV_TEXT("playback_set_in_point_tooltip")));
                p.inPointResetButton->setTooltip(_getText(DJV_TEXT("playback_reset_in_point_tooltip")));
                p.outPointWidget->setTooltip(_getText(DJV_TEXT("playback_out_point_tooltip")));
                p.outPointSetButton->setTooltip(_getText(DJV_TEXT("playback_set_out_point_tooltip")));
                p.outPointResetButton->setTooltip(_getText(DJV_TEXT("playback_reset_out_point_tooltip")));
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

                auto playback = Playback::Stop;
                if (p.media)
                {
                    playback = p.media->observePlayback()->get();
                }
                switch (playback)
                {
                case Playback::Stop:    p.playbackActionGroup->setChecked(-1); break;
                case Playback::Forward: p.playbackActionGroup->setChecked(0); break;
                case Playback::Reverse: p.playbackActionGroup->setChecked(1); break;
                default: break;
                }

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

                p.inPointResetButton->setEnabled(
                    p.inOutPoints.isEnabled() &&
                    p.inOutPoints.getIn() != 0);

                p.outPointWidget->setSequence(p.sequence);
                p.outPointWidget->setSpeed(p.defaultSpeed);
                p.outPointWidget->setFrame(p.inOutPoints.getOut());
                p.outPointWidget->setEnabled(p.inOutPoints.isEnabled());

                p.outPointResetButton->setEnabled(
                    p.inOutPoints.isEnabled() &&
                    p.inOutPoints.getOut() != p.sequence.getLastIndex());

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
                ss << _getText(DJV_TEXT("playback_fps")) << ": " << std::fixed << p.speed.toFloat();
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

