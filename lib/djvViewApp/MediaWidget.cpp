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

#include <djvViewApp/MediaWidget.h>

#include <djvViewApp/FileSettings.h>
#include <djvViewApp/FileSystem.h>
#include <djvViewApp/ImageSystem.h>
#include <djvViewApp/ImageView.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/MediaWidgetPrivate.h>
#include <djvViewApp/TimelineSlider.h>
#include <djvViewApp/WindowSettings.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/Border.h>
#include <djvUI/ButtonGroup.h>
#include <djvUI/FloatSlider.h>
#include <djvUI/GridLayout.h>
#include <djvUI/IntSlider.h>
#include <djvUI/Label.h>
#include <djvUI/ListButton.h>
#include <djvUI/MDICanvas.h>
#include <djvUI/MDIWidget.h>
#include <djvUI/PopupWidget.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/StackLayout.h>
#include <djvUI/ToggleButton.h>
#include <djvUI/ToolBar.h>
#include <djvUI/ToolButton.h>

#include <djvAV/AVSystem.h>

#include <djvCore/Context.h>
#include <djvCore/NumericValueModels.h>
#include <djvCore/Path.h>
#include <djvCore/Timer.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        PointerData::PointerData()
        {}

        PointerData::PointerData(PointerState state, const glm::vec2& pos, const std::map<int, bool>& buttons) :
            state(state),
            pos(pos),
            buttons(buttons)
        {}

        bool PointerData::operator == (const PointerData& other) const
        {
            return state == other.state && pos == other.pos;
        }

        struct MediaWidget::Private
        {
            std::shared_ptr<ValueSubject<PointerData> > hover;
            std::shared_ptr<ValueSubject<PointerData> > drag;

            std::shared_ptr<Media> media;
            AV::IO::Info ioInfo;
            std::shared_ptr<AV::Image::Image> image;
            std::vector<Time::Speed> speeds;
            Time::Speed defaultSpeed;
            Time::Speed speed;
            float realSpeed = 0.f;
            bool playEveryFrame = false;
            Frame::Sequence sequence;
            Frame::Number currentFrame = Frame::invalid;
            Playback playbackPrev = Playback::Stop;
            AV::TimeUnits timeUnits = AV::TimeUnits::First;
            bool frameStoreEnabled = false;
            std::shared_ptr<AV::Image::Image> frameStore;
            bool active = false;
            float fade = 1.f;

            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::ActionGroup> playbackActionGroup;

            std::shared_ptr<UI::Label> titleLabel;
            std::shared_ptr<UI::ToolButton> maximizeButton;
            std::shared_ptr<UI::ToolButton> closeButton;
            std::shared_ptr<UI::HorizontalLayout> titleBar;
            std::shared_ptr<PointerWidget> pointerWidget;
            std::shared_ptr<ImageView> imageView;
            std::shared_ptr<UI::ButtonGroup> speedButtonGroup;
            std::shared_ptr<UI::VerticalLayout> speedButtonLayout;
            std::shared_ptr<UI::ToggleButton> playEveryFrameButton;
            std::shared_ptr<UI::Label> playEveryFrameLabel;
            std::shared_ptr<UI::PopupWidget> speedPopupWidget;
            std::shared_ptr<UI::Label> realSpeedLabel;
            std::shared_ptr<UI::Label> currentFrameLabel;
            std::shared_ptr<UI::Label> durationLabel;
            std::shared_ptr<TimelineSlider> timelineSlider;
            std::shared_ptr<UI::BasicFloatSlider> volumeSlider;
            std::shared_ptr<UI::ToolButton> muteButton;
            std::shared_ptr<UI::PopupWidget> audioPopupWidget;
            std::shared_ptr<UI::IntSlider> cacheMaxSlider;
            std::shared_ptr<UI::ToggleButton> cacheEnabledButton;
            std::shared_ptr<UI::PopupWidget> memoryCachePopupWidget;
            std::shared_ptr<UI::GridLayout> playbackLayout;
            std::shared_ptr<UI::StackLayout> layout;

            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > actionObservers;
            std::shared_ptr<ValueObserver<AV::IO::Info> > ioInfoObserver;
            std::shared_ptr<ValueObserver<Frame::Number> > currentFrameObserver;
            std::shared_ptr<ValueObserver<bool> > currentFrameChangeObserver;
            std::shared_ptr<ValueObserver<AV::TimeUnits> > timeUnitsObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<AV::Image::Image> > > imageObserver;
            std::shared_ptr<ValueObserver<Time::Speed> > speedObserver;
            std::shared_ptr<ValueObserver<Time::Speed> > defaultSpeedObserver;
            std::shared_ptr<ValueObserver<float> > realSpeedObserver;
            std::shared_ptr<ValueObserver<bool> > playEveryFrameObserver;
            std::shared_ptr<ValueObserver<Frame::Sequence> > sequenceObserver;
            std::shared_ptr<ValueObserver<Frame::Number> > currentFrameObserver2;
            std::shared_ptr<ValueObserver<Playback> > playbackObserver;
            std::shared_ptr<ValueObserver<float> > volumeObserver;
            std::shared_ptr<ValueObserver<bool> > muteObserver;
            std::shared_ptr<ValueObserver<bool> > cacheEnabledObserver;
            std::shared_ptr<ValueObserver<int> > cacheMaxObserver;
            std::shared_ptr<ListObserver<Frame::Range> > cachedFramesObserver;
            std::shared_ptr<ValueObserver<float> > fadeObserver;
            std::shared_ptr<ValueObserver<bool> > frameStoreEnabledObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<AV::Image::Image> > > frameStoreObserver;
        };

        void MediaWidget::_init(const std::shared_ptr<Media>& media, Context* context)
        {
            IWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::MediaWidget");

            p.hover = ValueSubject<PointerData>::create();
            p.drag = ValueSubject<PointerData>::create();

            p.media = media;

            p.actions["Forward"] = UI::Action::create();
            p.actions["Forward"]->setIcon("djvIconPlaybackForward");
            p.actions["Reverse"] = UI::Action::create();
            p.actions["Reverse"]->setIcon("djvIconPlaybackReverse");
            p.playbackActionGroup = UI::ActionGroup::create(UI::ButtonType::Exclusive);
            p.playbackActionGroup->addAction(p.actions["Forward"]);
            p.playbackActionGroup->addAction(p.actions["Reverse"]);

            p.actions["InPoint"] = UI::Action::create();
            p.actions["InPoint"]->setIcon("djvIconFrameStart");
            p.actions["PrevFrame"] = UI::Action::create();
            p.actions["PrevFrame"]->setIcon("djvIconFramePrev");
            p.actions["NextFrame"] = UI::Action::create();
            p.actions["NextFrame"]->setIcon("djvIconFrameNext");
            p.actions["OutPoint"] = UI::Action::create();
            p.actions["OutPoint"]->setIcon("djvIconFrameEnd");

            p.titleLabel = UI::Label::create(context);
            p.titleLabel->setText(media->getFileInfo().getFileName(Frame::invalid, false));
            p.titleLabel->setTextHAlign(UI::TextHAlign::Left);
            p.titleLabel->setMargin(UI::Layout::Margin(UI::MetricsRole::Margin, UI::MetricsRole::Margin, UI::MetricsRole::None, UI::MetricsRole::None));
            p.titleLabel->setTooltip(media->getFileInfo());

            p.maximizeButton = UI::ToolButton::create(context);
            p.maximizeButton->setIcon("djvIconSDI");
            p.maximizeButton->setInsideMargin(UI::MetricsRole::MarginSmall);

            p.closeButton = UI::ToolButton::create(context);
            p.closeButton->setIcon("djvIconClose");
            p.closeButton->setInsideMargin(UI::MetricsRole::MarginSmall);

            p.titleBar = UI::HorizontalLayout::create(context);
            p.titleBar->setBackgroundRole(UI::ColorRole::OverlayLight);
            p.titleBar->addChild(p.titleLabel);
            p.titleBar->setStretch(p.titleLabel, UI::RowStretch::Expand);
            p.titleBar->addChild(p.maximizeButton);
            p.titleBar->addChild(p.closeButton);

            p.pointerWidget = PointerWidget::create(context);

            p.imageView = ImageView::create(context);

            p.speedButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Push);
            p.speedButtonLayout = UI::VerticalLayout::create(context);
            p.speedButtonLayout->setSpacing(UI::MetricsRole::None);
            //! \todo Implement me!
            p.playEveryFrameButton = UI::ToggleButton::create(context);
            p.playEveryFrameButton->setEnabled(false);
            p.playEveryFrameLabel = UI::Label::create(context);
            p.playEveryFrameLabel->setEnabled(false);
            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::MetricsRole::None);
            vLayout->addChild(p.speedButtonLayout);
            vLayout->addSeparator();
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setMargin(UI::MetricsRole::MarginSmall);
            hLayout->setSpacing(UI::MetricsRole::None);
            hLayout->addChild(p.playEveryFrameLabel);
            hLayout->addChild(p.playEveryFrameButton);
            vLayout->addChild(hLayout);
            p.speedPopupWidget = UI::PopupWidget::create(context);
            p.speedPopupWidget->addChild(vLayout);
            p.realSpeedLabel = UI::Label::create(context);

            p.currentFrameLabel = UI::Label::create(context);
            p.currentFrameLabel->setMargin(UI::MetricsRole::MarginSmall);

            p.durationLabel = UI::Label::create(context);
            p.durationLabel->setMargin(UI::MetricsRole::MarginSmall);

            p.timelineSlider = TimelineSlider::create(context);
            p.timelineSlider->setMedia(p.media);

            p.volumeSlider = UI::BasicFloatSlider::create(UI::Orientation::Horizontal, context);
            p.volumeSlider->setMargin(UI::MetricsRole::MarginSmall);
            p.muteButton = UI::ToolButton::create(context);
            p.muteButton->setIcon("djvIconAudioMute");
            p.muteButton->setButtonType(UI::ButtonType::Toggle);
            hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::None);
            hLayout->addChild(p.volumeSlider);
            hLayout->addChild(p.muteButton);
            p.audioPopupWidget = UI::PopupWidget::create(context);
            p.audioPopupWidget->setIcon("djvIconAudio");
            p.audioPopupWidget->addChild(hLayout);

            p.cacheMaxSlider = UI::IntSlider::create(context);
            p.cacheMaxSlider->setRange(IntRange(1, 64));
            p.cacheEnabledButton = UI::ToggleButton::create(context);
            p.cacheEnabledButton->setHAlign(UI::HAlign::Center);
            p.cacheEnabledButton->setMargin(UI::MetricsRole::None);
            hLayout = UI::HorizontalLayout::create(context);
            hLayout->setMargin(UI::MetricsRole::MarginSmall);
            hLayout->setSpacing(UI::MetricsRole::SpacingSmall);
            hLayout->addChild(p.cacheMaxSlider);
            hLayout->addChild(p.cacheEnabledButton);
            p.memoryCachePopupWidget = UI::PopupWidget::create(context);
            p.memoryCachePopupWidget->setIcon("djvIconMemory");
            p.memoryCachePopupWidget->setMargin(UI::MetricsRole::MarginSmall);
            p.memoryCachePopupWidget->addChild(hLayout);

            auto toolbar = UI::ToolBar::create(context);
            toolbar->setBackgroundRole(UI::ColorRole::None);
            toolbar->addAction(p.actions["Reverse"]);
            toolbar->addAction(p.actions["Forward"]);
            toolbar->addAction(p.actions["InPoint"]);
            toolbar->addAction(p.actions["PrevFrame"]);
            toolbar->addAction(p.actions["NextFrame"]);
            toolbar->addAction(p.actions["OutPoint"]);

            p.playbackLayout = UI::GridLayout::create(context);
            p.playbackLayout->setBackgroundRole(UI::ColorRole::OverlayLight);
            p.playbackLayout->setSpacing(UI::MetricsRole::None);
            p.playbackLayout->addChild(toolbar);
            p.playbackLayout->setGridPos(toolbar, 0, 0);
            p.playbackLayout->addChild(p.timelineSlider);
            p.playbackLayout->setGridPos(p.timelineSlider, 1, 0);
            p.playbackLayout->setStretch(p.timelineSlider, UI::GridStretch::Horizontal);
            p.playbackLayout->addChild(p.audioPopupWidget);
            p.playbackLayout->setGridPos(p.audioPopupWidget, 2, 0);
            hLayout = UI::HorizontalLayout::create(context);
            hLayout->addChild(p.speedPopupWidget);
            hLayout->addChild(p.realSpeedLabel);
            p.playbackLayout->addChild(hLayout);
            p.playbackLayout->setGridPos(hLayout, 0, 1);
            hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::None);
            hLayout->addChild(p.currentFrameLabel);
            hLayout->addExpander();
            hLayout->addChild(p.durationLabel);
            p.playbackLayout->addChild(hLayout);
            p.playbackLayout->setGridPos(hLayout, 1, 1);
            p.playbackLayout->addChild(p.memoryCachePopupWidget);
            p.playbackLayout->setGridPos(p.memoryCachePopupWidget, 2, 1);

            p.layout = UI::StackLayout::create(context);
            auto stackLayout = UI::StackLayout::create(context);
            stackLayout->setBackgroundRole(UI::ColorRole::OverlayLight);
            stackLayout->addChild(p.imageView);
            stackLayout->addChild(p.pointerWidget);
            p.layout->addChild(stackLayout);
            vLayout = UI::VerticalLayout::create(context);
            vLayout->addChild(p.titleBar);
            vLayout->addExpander();
            vLayout->addChild(p.playbackLayout);
            p.layout->addChild(vLayout);
            addChild(p.layout);

            _widgetUpdate();
            _speedUpdate();
            _realSpeedUpdate();
            _opacityUpdate();

            auto weak = std::weak_ptr<MediaWidget>(std::dynamic_pointer_cast<MediaWidget>(shared_from_this()));
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

            p.pointerWidget->setHoverCallback(
                [weak](const PointerData& data)
                {
                    if (auto widget = weak.lock())
                    {
                        const BBox2f& g = widget->_p->imageView->getGeometry();
                        widget->_p->hover->setIfChanged(PointerData(data.state, data.pos - g.min, data.buttons));
                    }
                });

            p.pointerWidget->setDragCallback(
                [weak](const PointerData& data)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->moveToFront();
                        const BBox2f& g = widget->_p->imageView->getGeometry();
                        widget->_p->drag->setIfChanged(PointerData(data.state, data.pos - g.min, data.buttons));
                    }
                });

            p.speedButtonGroup->setPushCallback(
                [weak](int value)
            {
                if (auto widget = weak.lock())
                {
                    if (auto media = widget->_p->media)
                    {
                        const auto speed =
                            value >= 0 && value < widget->_p->speeds.size() ?
                            widget->_p->speeds[value] :
                            Time::Speed();
                        media->setSpeed(speed);
                    }
                    widget->_p->speedPopupWidget->close();
                }
            });

            p.playEveryFrameButton->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto media = widget->_p->media)
                        {
                            media->setPlayEveryFrame(value);
                        }
                    }
                });

            p.volumeSlider->setValueCallback(
                [weak](float value)
            {
                if (auto widget = weak.lock())
                {
                    if (auto media = widget->_p->media)
                    {
                        media->setVolume(value);
                    }
                }
            });

            p.muteButton->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto media = widget->_p->media)
                        {
                            media->setMute(value);
                        }
                    }
                });

            p.cacheMaxSlider->setValueCallback(
                [context](int value)
                {
                    auto settingsSystem = context->getSystemT<UI::Settings::System>();
                    if (auto fileSettings = settingsSystem->getSettingsT<FileSettings>())
                    {
                        fileSettings->setCacheMax(value);
                    }
                });

            p.cacheEnabledButton->setCheckedCallback(
                [context](bool value)
                {
                    auto settingsSystem = context->getSystemT<UI::Settings::System>();
                    if (auto fileSettings = settingsSystem->getSettingsT<FileSettings>())
                    {
                        fileSettings->setCacheEnabled(value);
                    }
                });

            p.maximizeButton->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto canvas = std::dynamic_pointer_cast<UI::MDI::Canvas>(widget->getParent().lock()))
                        {
                            widget->moveToFront();
                            canvas->setMaximized(!canvas->isMaximized());
                        }
                    }
                });

            p.closeButton->setClickedCallback(
                [media, context]
                {
                    auto fileSystem = context->getSystemT<FileSystem>();
                    fileSystem->close(media);
                });

            p.actionObservers["InPoint"] = ValueObserver<bool>::create(
                p.actions["InPoint"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto widget = weak.lock())
                        {
                            if (auto media = widget->_p->media)
                            {
                                media->inPoint();
                            }
                        }
                    }
                });

            p.actionObservers["PrevFrame"] = ValueObserver<bool>::create(
                p.actions["PrevFrame"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto widget = weak.lock())
                        {
                            if (auto media = widget->_p->media)
                            {
                                media->prevFrame();
                            }
                        }
                    }
                });

            p.actionObservers["NextFrame"] = ValueObserver<bool>::create(
                p.actions["NextFrame"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto widget = weak.lock())
                        {
                            if (auto media = widget->_p->media)
                            {
                                media->nextFrame();
                            }
                        }
                    }
                });

            p.actionObservers["OutPoint"] = ValueObserver<bool>::create(
                p.actions["OutPoint"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto widget = weak.lock())
                        {
                            if (auto media = widget->_p->media)
                            {
                                media->outPoint();
                            }
                        }
                    }
                });

            p.currentFrameObserver = ValueObserver<Frame::Number>::create(
                p.timelineSlider->observeCurrentFrame(),
                [weak](Frame::Number value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto media = widget->_p->media)
                        {
                            media->setCurrentFrame(value);
                        }
                    }
                });

            p.currentFrameChangeObserver = ValueObserver<bool>::create(
                p.timelineSlider->observeCurrentFrameChange(),
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
                            else
                            {
                                media->setPlayback(widget->_p->playbackPrev);
                            }
                        }
                    }
                });

            auto avSystem = context->getSystemT<AV::AVSystem>();
            p.timeUnitsObserver = ValueObserver<AV::TimeUnits>::create(
                avSystem->observeTimeUnits(),
                [weak](AV::TimeUnits value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->timeUnits = value;
                        widget->_widgetUpdate();
                    }
                });

            p.ioInfoObserver = ValueObserver<AV::IO::Info>::create(
                p.media->observeInfo(),
                [weak](const AV::IO::Info& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->ioInfo = value;
                    }
                });

            p.imageObserver = ValueObserver<std::shared_ptr<AV::Image::Image> >::create(
                p.media->observeCurrentImage(),
                [weak](const std::shared_ptr<AV::Image::Image>& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->image = value;
                        widget->_imageUpdate();
                    }
                });

            p.speedObserver = ValueObserver<Time::Speed>::create(
                p.media->observeSpeed(),
                [weak](const Time::Speed& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->speed = value;
                        widget->_widgetUpdate();
                        widget->_speedUpdate();
                    }
                });

            p.defaultSpeedObserver = ValueObserver<Time::Speed>::create(
                p.media->observeSpeed(),
                [weak](const Time::Speed& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->defaultSpeed = value;
                        widget->_widgetUpdate();
                        widget->_speedUpdate();
                    }
                });

            p.realSpeedObserver = ValueObserver<float>::create(
                p.media->observeRealSpeed(),
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->realSpeed = value;
                        widget->_realSpeedUpdate();
                    }
                });

            p.playEveryFrameObserver = ValueObserver<bool>::create(
                p.media->observePlayEveryFrame(),
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->playEveryFrame = value;
                        widget->_widgetUpdate();
                        widget->_speedUpdate();
                    }
                });

            p.sequenceObserver = ValueObserver<Frame::Sequence>::create(
                p.media->observeSequence(),
                [weak](const Frame::Sequence& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->sequence = value;
                        widget->_widgetUpdate();
                    }
                });

            p.currentFrameObserver2 = ValueObserver<Frame::Number>::create(
                p.media->observeCurrentFrame(),
                [weak](Frame::Number value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->currentFrame = value;
                        widget->_widgetUpdate();
                    }
                });

            p.playbackObserver = ValueObserver<Playback>::create(
                p.media->observePlayback(),
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

            p.volumeObserver = ValueObserver<float>::create(
                p.media->observeVolume(),
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->volumeSlider->setValue(value);
                    }
                });

            p.muteObserver = ValueObserver<bool>::create(
                p.media->observeMute(),
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->muteButton->setChecked(value);
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            if (auto fileSettings = settingsSystem->getSettingsT<FileSettings>())
            {
                p.cacheEnabledObserver = ValueObserver<bool>::create(
                    fileSettings->observeCacheEnabled(),
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->cacheEnabledButton->setChecked(value);
                        }
                    });

                p.cacheMaxObserver = ValueObserver<int>::create(
                    fileSettings->observeCacheMax(),
                    [weak](int value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->cacheMaxSlider->setValue(value);
                        }
                    });
            }

            p.cachedFramesObserver = ListObserver<Frame::Range>::create(
                p.media->observeCachedFrames(),
                [weak](const std::vector<Frame::Range>& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->timelineSlider->setCachedFrames(value);
                    }
                });

            if (auto imageSystem = context->getSystemT<ImageSystem>())
            {
                p.frameStoreEnabledObserver = ValueObserver<bool>::create(
                    imageSystem->observeFrameStoreEnabled(),
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->frameStoreEnabled = value;
                            widget->_imageUpdate();
                        }
                    });
                p.frameStoreObserver = ValueObserver<std::shared_ptr<AV::Image::Image> >::create(
                    imageSystem->observeFrameStore(),
                    [weak](const std::shared_ptr<AV::Image::Image>& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->frameStore = value;
                            widget->_imageUpdate();
                        }
                    });
            }

            if (auto windowSystem = context->getSystemT<WindowSystem>())
            {
                p.fadeObserver = ValueObserver<float>::create(
                    windowSystem->observeFade(),
                    [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->fade = value;
                        widget->_opacityUpdate();
                    }
                });
            }
        }

        MediaWidget::MediaWidget() :
            _p(new Private)
        {}

        MediaWidget::~MediaWidget()
        {}

        std::shared_ptr<MediaWidget> MediaWidget::create(const std::shared_ptr<Media>& media, Context* context)
        {
            auto out = std::shared_ptr<MediaWidget>(new MediaWidget);
            out->_init(media, context);
            return out;
        }

        const std::shared_ptr<Media>& MediaWidget::getMedia() const
        {
            return _p->media;
        }

        const std::shared_ptr<ImageView>& MediaWidget::getImageView() const
        {
            return _p->imageView;
        }

        std::shared_ptr<Core::IValueSubject<PointerData> > MediaWidget::observeHover() const
        {
            return _p->hover;
        }

        std::shared_ptr<Core::IValueSubject<PointerData> > MediaWidget::observeDrag() const
        {
            return _p->drag;
        }

        void MediaWidget::_setMaximized(float value)
        {
            IWidget::_setMaximized(value);
            _opacityUpdate();
            _resize();
        }

        void MediaWidget::_setActiveWidget(bool value)
        {
            IWidget::_setActiveWidget(value);
            DJV_PRIVATE_PTR();
            p.active = value;
            p.titleLabel->setTextColorRole(p.active ? UI::ColorRole::Foreground : UI::ColorRole::ForegroundDim);
            p.maximizeButton->setForegroundColorRole(p.active ? UI::ColorRole::Foreground : UI::ColorRole::ForegroundDim);
            p.closeButton->setForegroundColorRole(p.active ? UI::ColorRole::Foreground : UI::ColorRole::ForegroundDim);
            _imageUpdate();
        }

        void MediaWidget::_preLayoutEvent(Event::PreLayout&)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const float sh = style->getMetric(UI::MetricsRole::Shadow);

            const glm::vec2& minimumSize = p.layout->getMinimumSize();
            _setMinimumSize(minimumSize + sh * 2.f);

            glm::vec2 imageSize = p.imageView->getMinimumSize();
            imageSize.x = std::max(imageSize.x * 2.f, minimumSize.x);
            if (p.ioInfo.video.size())
            {
                const float aspectRatio = p.ioInfo.video[0].info.getAspectRatio();
                if (aspectRatio > 0.f)
                {
                    imageSize.y = imageSize.x / aspectRatio;
                }
            }
            _setDesiredSize(imageSize + sh * 2.f);
        }

        void MediaWidget::_layoutEvent(Event::Layout&)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const float sh = style->getMetric(UI::MetricsRole::Shadow);
            const BBox2f g = getGeometry().margin(-sh);
            p.layout->setGeometry(g);
            const glm::vec2 titleBarSize = p.titleBar->getMinimumSize();
            const glm::vec2 playbackSize = p.playbackLayout->isVisible() ? p.playbackLayout->getMinimumSize() : glm::vec2(0.f, 0.f);
            const BBox2f imageFrame = BBox2f(
                glm::vec2(
                    g.min.x,
                    g.min.y + titleBarSize.y),
                glm::vec2(
                    g.max.x,
                    g.max.y - playbackSize.y));
            p.imageView->setImageFrame(imageFrame);
        }

        void MediaWidget::_localeEvent(Event::Locale&)
        {
            DJV_PRIVATE_PTR();
            p.actions["Forward"]->setTooltip(_getText(DJV_TEXT("Forward tooltip")));
            p.actions["Reverse"]->setTooltip(_getText(DJV_TEXT("Reverse tooltip")));
            p.actions["InPoint"]->setTooltip(_getText(DJV_TEXT("Go to in point tooltip")));
            p.actions["NextFrame"]->setTooltip(_getText(DJV_TEXT("Next frame tooltip")));
            p.actions["PrevFrame"]->setTooltip(_getText(DJV_TEXT("Previous frame tooltip")));
            p.actions["OutPoint"]->setTooltip(_getText(DJV_TEXT("Go to out point tooltip")));

            p.maximizeButton->setTooltip(_getText(DJV_TEXT("Maximize tooltip")));
            p.closeButton->setTooltip(_getText(DJV_TEXT("Close tooltip")));

            p.playEveryFrameButton->setTooltip(_getText(DJV_TEXT("Play every frame tooltip")));
            p.speedPopupWidget->setTooltip(_getText(DJV_TEXT("Speed popup tooltip")));
            p.realSpeedLabel->setTooltip(_getText(DJV_TEXT("Real speed tooltip")));
            p.currentFrameLabel->setTooltip(_getText(DJV_TEXT("Current time tooltip")));
            p.durationLabel->setTooltip(_getText(DJV_TEXT("Duration tooltip")));

            p.volumeSlider->setTooltip(_getText(DJV_TEXT("Volume tooltip")));
            p.muteButton->setTooltip(_getText(DJV_TEXT("Mute tooltip")));
            p.audioPopupWidget->setTooltip(_getText(DJV_TEXT("Audio popup tooltip")));

            p.cacheMaxSlider->setTooltip(_getText(DJV_TEXT("Memory cache maximum tooltip")));
            p.cacheEnabledButton->setTooltip(_getText(DJV_TEXT("Memory cache enabled tooltip")));
            p.memoryCachePopupWidget->setTooltip(_getText(DJV_TEXT("Memory cache tooltip")));

            _speedUpdate();
        }

        void MediaWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.actions["Forward"]->setEnabled(p.media.get());
            p.actions["Reverse"]->setEnabled(p.media.get());
            p.actions["InPoint"]->setEnabled(p.media.get());
            p.actions["PrevFrame"]->setEnabled(p.media.get());
            p.actions["NextFrame"]->setEnabled(p.media.get());
            p.actions["OutPoint"]->setEnabled(p.media.get());

            auto playback = Playback::Stop;
            if (p.media)
            {
                playback = p.media->observePlayback()->get();
            }
            switch (playback)
            {
            case Playback::Stop:    p.playbackActionGroup->setChecked(-1); break;
            case Playback::Forward: p.playbackActionGroup->setChecked( 0); break;
            case Playback::Reverse: p.playbackActionGroup->setChecked( 1); break;
            default: break;
            }

            auto avSystem = getContext()->getSystemT<AV::AVSystem>();
            p.currentFrameLabel->setText(avSystem->getLabel(p.sequence.getFrame(p.currentFrame), p.defaultSpeed));
            p.currentFrameLabel->setEnabled(p.media.get());
            p.durationLabel->setText(avSystem->getLabel(p.sequence.getSize(), p.defaultSpeed));
            p.durationLabel->setEnabled(p.media.get());

            p.playbackLayout->setVisible(p.sequence.getSize() > 1);

            p.audioPopupWidget->setEnabled(p.media.get());
            p.memoryCachePopupWidget->setEnabled(p.media.get());
        }

        void MediaWidget::_imageUpdate()
        {
            DJV_PRIVATE_PTR();
            p.imageView->setImage(p.active && p.frameStoreEnabled && p.frameStore ? p.frameStore : p.image);
        }

        void MediaWidget::_speedUpdate()
        {
            DJV_PRIVATE_PTR();
            auto context = getContext();
            p.speedButtonGroup->clearButtons();
            p.speedButtonLayout->clearChildren();
            p.speeds =
            {
                Time::Speed(240),
                Time::Speed(120),
                Time::Speed(60),
                Time::Speed(48),
                Time::Speed(30),
                Time::Speed(30000, 1001),
                Time::Speed(25),
                Time::Speed(24),
                Time::Speed(24000, 1001),
                Time::Speed(16)
            };
            for (const auto& i : p.speeds)
            {
                auto button = UI::ListButton::create(context);
                std::stringstream ss;
                ss.precision(3);
                ss << std::fixed << i.toFloat();
                button->setText(ss.str());
                p.speedButtonGroup->addButton(button);
                p.speedButtonLayout->addChild(button);
            }
            p.speedButtonLayout->addSeparator();
            auto button = UI::ListButton::create(context);
            std::stringstream ss;
            ss << _getText(DJV_TEXT("Default")) << ": ";
            ss.precision(3);
            ss << std::fixed << p.defaultSpeed.toFloat();
            button->setText(ss.str());
            p.speedButtonGroup->addButton(button);
            p.speedButtonLayout->addChild(button);

            p.playEveryFrameLabel->setText(_getText(DJV_TEXT("Play every frame")) + ":");
            {
                std::stringstream ss;
                ss.precision(3);
                ss << _getText(DJV_TEXT("FPS")) << ": " << std::fixed << p.speed.toFloat();
                p.speedPopupWidget->setText(ss.str());
            }
            p.speedPopupWidget->setEnabled(p.media.get());
        }

        void MediaWidget::_realSpeedUpdate()
        {
            DJV_PRIVATE_PTR();
            {
                std::stringstream ss;
                ss.precision(3);
                ss << std::fixed << p.realSpeed;
                p.realSpeedLabel->setText(ss.str());
            }
        }

        void MediaWidget::_opacityUpdate()
        {
            DJV_PRIVATE_PTR();
            p.titleBar->setOpacity(p.fade * (1.f - _getMaximized()));
            p.playbackLayout->setOpacity(p.fade);
        }

    } // namespace ViewApp
} // namespace djv

