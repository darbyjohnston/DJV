// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/MediaWidget.h>

#include <djvViewApp/FileSettings.h>
#include <djvViewApp/FileSystem.h>
#include <djvViewApp/HUDWidget.h>
#include <djvViewApp/ImageSystem.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/MediaWidgetPrivate.h>
#include <djvViewApp/PlaybackSettings.h>
#include <djvViewApp/TimelineSlider.h>
#include <djvViewApp/ViewWidget.h>
#include <djvViewApp/ViewSettings.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/Border.h>
#include <djvUI/FloatSlider.h>
#include <djvUI/GridLayout.h>
#include <djvUI/IntEdit.h>
#include <djvUI/IntSlider.h>
#include <djvUI/Label.h>
#include <djvUI/LineEdit.h>
#include <djvUI/MDICanvas.h>
#include <djvUI/MDIWidget.h>
#include <djvUI/MultiStateButton.h>
#include <djvUI/PopupButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/StackLayout.h>
#include <djvUI/ToolBar.h>
#include <djvUI/ToolButton.h>

#include <djvRender2D/Render.h>

#include <djvAV/AVSystem.h>
#include <djvAV/TimeFunc.h>

#include <djvSystem/Context.h>
#include <djvSystem/FileInfo.h>
#include <djvSystem/Path.h>
#include <djvSystem/Timer.h>

#include <djvMath/Math.h>
#include <djvMath/NumericValueModels.h>

#include <iomanip>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        PointerData::PointerData()
        {}

        PointerData::PointerData(
            PointerState state,
            const glm::vec2& pos,
            const std::map<int, bool>& buttons,
            int key,
            int keyModifiers) :
            state(state),
            pos(pos),
            buttons(buttons),
            key(key),
            keyModifiers(keyModifiers)
        {}

        bool PointerData::operator == (const PointerData& other) const
        {
            return
                state        == other.state &&
                pos          == other.pos &&
                buttons      == other.buttons &&
                key          == other.key &&
                keyModifiers == other.keyModifiers;
        }

        ScrollData::ScrollData()
        {}

        ScrollData::ScrollData(
            const glm::vec2& delta,
            int key,
            int keyModifiers) :
            delta(delta),
            key(key),
            keyModifiers(keyModifiers)
        {}

        bool ScrollData::operator == (const ScrollData& other) const
        {
            return
                delta == other.delta &&
                key == other.key &&
                keyModifiers == other.keyModifiers;
        }

        struct MediaWidget::Private
        {
            std::shared_ptr<Media> media;
            AV::IO::Info ioInfo;
            std::vector<Image::Info> layers;
            int currentLayer = -1;
            std::shared_ptr<Image::Image> image;
            Math::Rational defaultSpeed;
            Math::Rational speed;
            float realSpeed = 0.F;
            PlaybackMode playbackMode = PlaybackMode::First;
            Math::Frame::Sequence sequence;
            Math::Frame::Index currentFrame = Math::Frame::invalidIndex;
            AV::IO::InOutPoints inOutPoints;
            Playback playbackPrev = Playback::Count;
            AV::Time::Units timeUnits = AV::Time::Units::First;
            ViewLock viewLock = ViewLock::First;
            std::shared_ptr<ValueSubject<HUDOptions> > hudOptions;
            bool frameStoreEnabled = false;
            std::shared_ptr<Image::Image> frameStore;
            bool audioEnabled = false;
            float audioVolume = 0.F;
            bool audioMute = false;
            bool active = false;
            float fade = 1.F;
            std::shared_ptr<ValueSubject<PointerData> > hover;
            std::shared_ptr<ValueSubject<PointerData> > drag;
            std::shared_ptr<ValueSubject<ScrollData> > scroll;

            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::ActionGroup> playbackActionGroup;

            std::shared_ptr<UI::Label> titleLabel;
            std::shared_ptr<UI::ToolButton> maximizeButton;
            std::shared_ptr<UI::ToolButton> closeButton;
            std::shared_ptr<UI::HorizontalLayout> titleBar;
            std::shared_ptr<PointerWidget> pointerWidget;
            std::shared_ptr<ViewWidget> viewWidget;
            std::shared_ptr<HUDWidget> hud;
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
            std::shared_ptr<UI::GridLayout> playbackLayout;
            std::shared_ptr<UI::StackLayout> layout;

            std::shared_ptr<ValueObserver<AV::Time::Units> > timeUnitsObserver;
            std::shared_ptr<ValueObserver<AV::IO::Info> > ioInfoObserver;
            std::shared_ptr<ValueObserver<std::pair<std::vector<Image::Info>, int> > > layersObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<Image::Image> > > imageObserver;
            std::shared_ptr<ValueObserver<Math::Rational> > speedObserver;
            std::shared_ptr<ValueObserver<Math::Rational> > defaultSpeedObserver;
            std::shared_ptr<ValueObserver<float> > realSpeedObserver;
            std::shared_ptr<ValueObserver<PlaybackMode> > playbackModeObserver;
            std::shared_ptr<ValueObserver<Math::Frame::Sequence> > sequenceObserver;
            std::shared_ptr<ValueObserver<Math::Frame::Index> > currentFrameObserver;
            std::shared_ptr<ValueObserver<AV::IO::InOutPoints> > inOutPointsObserver;
            std::shared_ptr<ValueObserver<Playback> > playbackObserver;
            std::shared_ptr<ValueObserver<bool> > audioEnabledObserver;
            std::shared_ptr<ValueObserver<float> > volumeObserver;
            std::shared_ptr<ValueObserver<bool> > muteObserver;
            std::shared_ptr<ValueObserver<bool> > cacheEnabledObserver;
            std::shared_ptr<ValueObserver<Math::Frame::Sequence> > cacheSequenceObserver;
            std::shared_ptr<ValueObserver<Math::Frame::Sequence> > cachedFramesObserver;
            std::shared_ptr<ListObserver<std::shared_ptr<AnnotatePrimitive> > > annotationsObserver;
            std::shared_ptr<ValueObserver<float> > fadeObserver;
            std::shared_ptr<ValueObserver<ViewLock> > viewLockObserver;
            std::shared_ptr<ValueObserver<bool> > frameStoreEnabledObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<Image::Image> > > frameStoreObserver;
            std::shared_ptr<ValueObserver<Render2D::ImageOptions> > imageOptionsObserver;
            std::shared_ptr<ValueObserver<UI::ImageRotate> > imageRotateObserver;
            std::shared_ptr<ValueObserver<UI::ImageAspectRatio> > imageAspectRatioObserver;
        };

        void MediaWidget::_init(const std::shared_ptr<Media>& media, const std::shared_ptr<System::Context>& context)
        {
            IWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::MediaWidget");

            p.media = media;
            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
            p.hudOptions = ValueSubject<HUDOptions>::create(viewSettings->observeHUDOptions()->get());
            p.hover = ValueSubject<PointerData>::create();
            p.drag = ValueSubject<PointerData>::create();
            p.scroll = ValueSubject<ScrollData>::create();

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

            p.titleLabel = UI::Label::create(context);
            p.titleLabel->setText(media->getFileInfo().getFileName(Math::Frame::invalid, false));
            p.titleLabel->setTextHAlign(UI::TextHAlign::Left);
            p.titleLabel->setMargin(UI::Layout::Margin(UI::MetricsRole::Margin, UI::MetricsRole::Margin, UI::MetricsRole::None, UI::MetricsRole::None));
            p.titleLabel->setTooltip(std::string(media->getFileInfo()));

            p.maximizeButton = UI::ToolButton::create(context);
            p.maximizeButton->setIcon("djvIconSDI");

            p.closeButton = UI::ToolButton::create(context);
            p.closeButton->setIcon("djvIconClose");

            p.titleBar = UI::HorizontalLayout::create(context);
            p.titleBar->setSpacing(UI::MetricsRole::None);
            p.titleBar->setBackgroundRole(UI::ColorRole::OverlayLight);
            p.titleBar->addChild(p.titleLabel);
            p.titleBar->setStretch(p.titleLabel, UI::RowStretch::Expand);
            p.titleBar->addChild(p.maximizeButton);
            p.titleBar->addChild(p.closeButton);

            p.pointerWidget = PointerWidget::create(context);

            p.viewWidget = ViewWidget::create(context);

            p.hud = HUDWidget::create(context);
            p.hud->setHUDOptions(p.hudOptions->get());

            p.speedPopupButton = UI::PopupButton::create(UI::MenuButtonStyle::Tool, context);
            p.speedPopupButton->setPopupIcon("djvIconPopupMenu");
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
            p.timelineSlider->setMargin(UI::MetricsRole::MarginInside);

            p.audioPopupButton = UI::PopupButton::create(UI::MenuButtonStyle::Tool, context);
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
                        
            p.playbackLayout = UI::GridLayout::create(context);
            p.playbackLayout->setBackgroundRole(UI::ColorRole::OverlayLight);
            p.playbackLayout->setSpacing(UI::MetricsRole::None);
            p.playbackLayout->addChild(toolBar);
            p.playbackLayout->setGridPos(toolBar, 0, 0);
            p.playbackLayout->addChild(p.timelineSlider);
            p.playbackLayout->setGridPos(p.timelineSlider, 1, 0);
            p.playbackLayout->setStretch(p.timelineSlider, UI::GridStretch::Horizontal);
            p.playbackLayout->addChild(p.audioPopupButton);
            p.playbackLayout->setGridPos(p.audioPopupButton, 2, 0);
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::None);
            hLayout->addChild(p.speedPopupButton);
            hLayout->addChild(p.realSpeedLabel);
            hLayout->addChild(p.playbackModeButton);
            p.playbackLayout->addChild(hLayout);
            p.playbackLayout->setGridPos(hLayout, 0, 1);
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
            p.playbackLayout->addChild(hLayout);
            p.playbackLayout->setGridPos(hLayout, 1, 1);

            p.layout = UI::StackLayout::create(context);
            p.layout->setBackgroundRole(UI::ColorRole::OverlayLight);
            p.layout->addChild(p.viewWidget);
            p.layout->addChild(p.hud);
            p.layout->addChild(p.pointerWidget);
            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->addChild(p.titleBar);
            vLayout->addExpander();
            vLayout->addChild(p.playbackLayout);
            p.layout->addChild(vLayout);
            addChild(p.layout);

            _widgetUpdate();
            _speedUpdate();
            _realSpeedUpdate();
            _audioUpdate();
            _opacityUpdate();
            _hudUpdate();

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
                        const Math::BBox2f& g = widget->_p->viewWidget->getGeometry();
                        widget->_p->hover->setIfChanged(
                            PointerData(data.state, data.pos - g.min, data.buttons, data.key, data.keyModifiers));
                    }
                });
            p.pointerWidget->setDragCallback(
                [weak](const PointerData& data)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->moveToFront();
                        const Math::BBox2f& g = widget->_p->viewWidget->getGeometry();
                        widget->_p->drag->setIfChanged(
                            PointerData(data.state, data.pos - g.min, data.buttons, data.key, data.keyModifiers));
                    }
                });
            p.pointerWidget->setScrollCallback(
                [weak](const ScrollData& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->moveToFront();
                        widget->_p->scroll->setAlways(value);
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
                            out = PlaybackSpeedWidget::create(widget->_p->media, context);
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
                [media, contextWeak]() -> std::shared_ptr<UI::Widget>
                {
                    std::shared_ptr<UI::Widget> out;
                    if (auto context = contextWeak.lock())
                    {
                        out = AudioWidget::create(media, context);
                    }
                    return out;
                });

            p.maximizeButton->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto canvas = std::dynamic_pointer_cast<UI::MDI::Canvas>(widget->getParent().lock()))
                        {
                            widget->moveToFront();
                            canvas->setMaximize(!canvas->isMaximized());
                        }
                    }
                });

            p.closeButton->setClickedCallback(
                [media, contextWeak]
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto fileSystem = context->getSystemT<FileSystem>();
                        fileSystem->close(media);
                    }
                });

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

            auto avSystem = context->getSystemT<AV::AVSystem>();
            p.timeUnitsObserver = ValueObserver<AV::Time::Units>::create(
                avSystem->observeTimeUnits(),
                [weak](AV::Time::Units value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->timeUnits = value;
                        widget->_widgetUpdate();
                        widget->_hudUpdate();
                    }
                });

            p.ioInfoObserver = ValueObserver<AV::IO::Info>::create(
                p.media->observeInfo(),
                [weak](const AV::IO::Info& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->ioInfo = value;
                        widget->_widgetUpdate();
                        widget->_audioUpdate();
                    }
                });

            p.layersObserver = ValueObserver<std::pair<std::vector<Image::Info>, int> >::create(
                p.media->observeLayers(),
                [weak](const std::pair<std::vector<Image::Info>, int>& value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->layers = value.first;
                    widget->_p->currentLayer = value.second;
                    widget->_hudUpdate();
                }
            });

            p.imageObserver = ValueObserver<std::shared_ptr<Image::Image> >::create(
                p.media->observeCurrentImage(),
                [weak](const std::shared_ptr<Image::Image>& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->image = value;
                        widget->_imageUpdate();
                    }
                });

            p.speedObserver = ValueObserver<Math::Rational>::create(
                p.media->observeSpeed(),
                [weak](const Math::Rational& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->speed = value;
                        widget->_widgetUpdate();
                        widget->_speedUpdate();
                        widget->_hudUpdate();
                    }
                });

            p.defaultSpeedObserver = ValueObserver<Math::Rational>::create(
                p.media->observeDefaultSpeed(),
                [weak](const Math::Rational& value)
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
                        widget->_hudUpdate();
                    }
                });

            p.playbackModeObserver = ValueObserver<PlaybackMode>::create(
                p.media->observePlaybackMode(),
                [weak](PlaybackMode value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->playbackMode = value;
                        widget->_widgetUpdate();
                        widget->_speedUpdate();
                    }
                });

            p.sequenceObserver = ValueObserver<Math::Frame::Sequence>::create(
                p.media->observeSequence(),
                [weak](const Math::Frame::Sequence& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->sequence = value;
                        widget->_widgetUpdate();
                    }
                });

            p.currentFrameObserver = ValueObserver<Math::Frame::Index>::create(
                p.media->observeCurrentFrame(),
                [weak](Math::Frame::Index value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->currentFrame = value;
                        widget->_widgetUpdate();
                        widget->_hudUpdate();
                    }
                });

            p.inOutPointsObserver = ValueObserver<AV::IO::InOutPoints>::create(
                p.media->observeInOutPoints(),
                [weak](const AV::IO::InOutPoints& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->inOutPoints = value;
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

            p.audioEnabledObserver = ValueObserver<bool>::create(
                p.media->observeAudioEnabled(),
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->audioEnabled = value;
                        widget->_audioUpdate();
                    }
                });

            p.volumeObserver = ValueObserver<float>::create(
                p.media->observeVolume(),
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->audioVolume = value;
                        widget->_audioUpdate();
                    }
                });

            p.muteObserver = ValueObserver<bool>::create(
                p.media->observeMute(),
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->audioMute = value;
                        widget->_audioUpdate();
                    }
                });

            if (auto fileSettings = settingsSystem->getSettingsT<FileSettings>())
            {
                p.cacheEnabledObserver = ValueObserver<bool>::create(
                    fileSettings->observeCacheEnabled(),
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->timelineSlider->setCacheEnabled(value);
                        }
                    });
            }

            p.cacheSequenceObserver = ValueObserver<Math::Frame::Sequence>::create(
                p.media->observeCacheSequence(),
                [weak](const Math::Frame::Sequence& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->timelineSlider->setCacheSequence(value);
                    }
                });

            p.cachedFramesObserver = ValueObserver<Math::Frame::Sequence>::create(
                p.media->observeCachedFrames(),
                [weak](const Math::Frame::Sequence& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->timelineSlider->setCachedFrames(value);
                    }
                });

            p.annotationsObserver = ListObserver<std::shared_ptr<AnnotatePrimitive> >::create(
                p.media->observeAnnotations(),
                [weak](const std::vector<std::shared_ptr<AnnotatePrimitive> >& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->viewWidget->setAnnotations(value);
                    }
                });

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

            if (auto viewSettings = settingsSystem->getSettingsT<ViewSettings>())
            {
                p.viewLockObserver = ValueObserver<ViewLock>::create(
                    viewSettings->observeLock(),
                    [weak](ViewLock value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->viewLock = value;
                        }
                    });
            }

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
                p.frameStoreObserver = ValueObserver<std::shared_ptr<Image::Image> >::create(
                    imageSystem->observeFrameStore(),
                    [weak](const std::shared_ptr<Image::Image>& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->frameStore = value;
                            widget->_imageUpdate();
                        }
                    });
            }

            p.imageOptionsObserver = ValueObserver<Render2D::ImageOptions>::create(
                p.viewWidget->observeImageOptions(),
                [weak](const Render2D::ImageOptions& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->timelineSlider->setImageOptions(value);
                    }
                });

            p.imageRotateObserver = ValueObserver<UI::ImageRotate>::create(
                p.viewWidget->observeImageRotate(),
                [weak](UI::ImageRotate value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->timelineSlider->setImageRotate(value);
                    }
                });

            p.imageAspectRatioObserver = ValueObserver<UI::ImageAspectRatio>::create(
                p.viewWidget->observeImageAspectRatio(),
                [weak](UI::ImageAspectRatio value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->timelineSlider->setImageAspectRatio(value);
                    }
                });
        }

        MediaWidget::MediaWidget() :
            _p(new Private)
        {}

        MediaWidget::~MediaWidget()
        {}

        std::shared_ptr<MediaWidget> MediaWidget::create(const std::shared_ptr<Media>& media, const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<MediaWidget>(new MediaWidget);
            out->_init(media, context);
            return out;
        }

        const std::shared_ptr<Media>& MediaWidget::getMedia() const
        {
            return _p->media;
        }

        const std::shared_ptr<ViewWidget>& MediaWidget::getViewWidget() const
        {
            return _p->viewWidget;
        }

        void MediaWidget::fitWindow()
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const float sh = style->getMetric(UI::MetricsRole::Shadow);
            const Math::BBox2f imageBBox = p.viewWidget->getImageBBox();
            const float zoom = p.viewWidget->observeImageZoom()->get();
            const glm::vec2 imageSize = imageBBox.getSize() * zoom;
            glm::vec2 size(ceilf(imageSize.x), ceilf(imageSize.y));
            switch (p.viewLock)
            {
            case ViewLock::Frame:
                size.y += _getTitleBarHeight() + _getPlaybackHeight();
                break;
            default: break;
            }
            resize(size + sh * 2.F);
        }

        std::shared_ptr<Core::IValueSubject<HUDOptions> > MediaWidget::observeHUDOptions() const
        {
            return _p->hudOptions;
        }

        void MediaWidget::setHUDOptions(const HUDOptions& value)
        {
            DJV_PRIVATE_PTR();
            if (p.hudOptions->setIfChanged(value))
            {
                p.hud->setHUDOptions(value);
            }
        }

        std::shared_ptr<IValueSubject<PointerData> > MediaWidget::observeHover() const
        {
            return _p->hover;
        }

        std::shared_ptr<IValueSubject<PointerData> > MediaWidget::observeDrag() const
        {
            return _p->drag;
        }

        std::shared_ptr<IValueSubject<ScrollData> > MediaWidget::observeScroll() const
        {
            return _p->scroll;
        }

        float MediaWidget::_getTitleBarHeight() const
        {
            DJV_PRIVATE_PTR();
            return p.titleBar->getMinimumSize().y;
        }

        float MediaWidget::_getPlaybackHeight() const
        {
            DJV_PRIVATE_PTR();
            return p.playbackLayout->isVisible() ? p.playbackLayout->getMinimumSize().y : 0.F;
        }


        std::map<UI::MDI::Handle, std::vector<Math::BBox2f> > MediaWidget::_getHandles() const
        {
            auto out = IWidget::_getHandles();
            out[UI::MDI::Handle::Move] = { _p->titleBar->getGeometry() };
            return out;
        }

        void MediaWidget::_setMaximize(float value)
        {
            IWidget::_setMaximize(value);
            _opacityUpdate();
            _resize();
        }

        void MediaWidget::_setActiveWidget(bool value)
        {
            IWidget::_setActiveWidget(value);
            DJV_PRIVATE_PTR();
            p.active = value;
            p.scroll->setAlways(ScrollData(glm::vec2(0.F, 0.F), 0, 0));
            p.titleLabel->setTextColorRole(p.active ? UI::ColorRole::Foreground : UI::ColorRole::ForegroundDim);
            p.maximizeButton->setForegroundColorRole(p.active ? UI::ColorRole::Foreground : UI::ColorRole::ForegroundDim);
            p.closeButton->setForegroundColorRole(p.active ? UI::ColorRole::Foreground : UI::ColorRole::ForegroundDim);
            _imageUpdate();
        }

        void MediaWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const float sh = style->getMetric(UI::MetricsRole::Shadow);
            const glm::vec2& minimumSize = p.layout->getMinimumSize();

            glm::vec2 imageSize = p.viewWidget->getMinimumSize();
            const float ar = p.viewWidget->getImageBBox().getAspect();
            if (ar > 1.F)
            {
                imageSize.x = std::max(imageSize.x * 2.F, minimumSize.x);
                imageSize.y = imageSize.x / ar;
            }
            else if (ar > 0.F)
            {
                imageSize.y = std::max(imageSize.y * 2.F, minimumSize.y);
                imageSize.x = imageSize.y * ar;
            }
            glm::vec2 size(ceilf(imageSize.x), ceilf(imageSize.y));
            
            switch (p.viewLock)
            {
            case ViewLock::Frame:
                size.y += _getTitleBarHeight() + _getPlaybackHeight();
                break;
            default: break;
            }

            _setMinimumSize(size + sh * 2.F);
        }

        void MediaWidget::_layoutEvent(System::Event::Layout&)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const float sh = style->getMetric(UI::MetricsRole::Shadow);
            const Math::BBox2f g = getGeometry().margin(-sh);
            p.layout->setGeometry(g);
            const Math::BBox2f frame = Math::BBox2f(
                glm::vec2(
                    g.min.x,
                    g.min.y + _getTitleBarHeight()),
                glm::vec2(
                    g.max.x,
                    g.max.y - _getPlaybackHeight()));
            p.viewWidget->setImageFrame(frame);
            p.hud->setHUDFrame(frame);
        }

        void MediaWidget::_initEvent(System::Event::Init& event)
        {
            IWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.actions["Forward"]->setTooltip(_getText(DJV_TEXT("playback_forward_tooltip")));
                p.actions["Reverse"]->setTooltip(_getText(DJV_TEXT("playback_reverse_tooltip")));
                p.actions["InPoint"]->setTooltip(_getText(DJV_TEXT("playback_go_to_in_point_tooltip")));
                p.actions["NextFrame"]->setTooltip(_getText(DJV_TEXT("playback_next_frame_tooltip")));
                p.actions["PrevFrame"]->setTooltip(_getText(DJV_TEXT("playback_previous_frame_tooltip")));
                p.actions["OutPoint"]->setTooltip(_getText(DJV_TEXT("playback_go_to_out_point_tooltip")));

                p.maximizeButton->setTooltip(_getText(DJV_TEXT("widget_media_maximize_tooltip")));
                p.closeButton->setTooltip(_getText(DJV_TEXT("widget_media_close_tooltip")));

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

        void MediaWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                auto playback = Playback::Stop;
                playback = p.media->observePlayback()->get();
                switch (playback)
                {
                case Playback::Stop:    p.playbackActionGroup->setChecked(-1); break;
                case Playback::Forward: p.playbackActionGroup->setChecked( 0); break;
                case Playback::Reverse: p.playbackActionGroup->setChecked( 1); break;
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

                p.durationLabel->setText(AV::Time::toString(p.sequence.getFrameCount(), p.defaultSpeed, p.timeUnits));

                p.timelineSlider->setInOutPointsEnabled(p.inOutPoints.isEnabled());
                p.timelineSlider->setInPoint(p.inOutPoints.getIn());
                p.timelineSlider->setOutPoint(p.inOutPoints.getOut());

                p.playbackLayout->setVisible(p.sequence.getFrameCount() > 1);
            }
        }

        void MediaWidget::_imageUpdate()
        {
            DJV_PRIVATE_PTR();
            p.viewWidget->setImage(p.active && p.frameStoreEnabled && p.frameStore ? p.frameStore : p.image);
        }

        void MediaWidget::_speedUpdate()
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

        void MediaWidget::_realSpeedUpdate()
        {
            DJV_PRIVATE_PTR();
            std::stringstream ss;
            ss.precision(2);
            ss << std::fixed << p.realSpeed;
            p.realSpeedLabel->setText(ss.str());
        }

        void MediaWidget::_audioUpdate()
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

        void MediaWidget::_opacityUpdate()
        {
            DJV_PRIVATE_PTR();
            const float maximize = 1.F - _getMaximize();
            p.titleBar->setOpacity(p.fade * maximize);
            p.playbackLayout->setOpacity(p.fade);
        }

        void MediaWidget::_hudUpdate()
        {
            DJV_PRIVATE_PTR();
            HUDData data;
            data.fileName = p.media->getFileInfo().getFileName(Math::Frame::invalid, false);
            if (p.currentLayer >= 0 && p.currentLayer < static_cast<int>(p.layers.size()))
            {
                const auto& layer = p.layers[p.currentLayer];
                data.layer = layer.name;
                data.size = layer.size;
                data.type = layer.type;
            }
            data.isSequence = p.sequence.getFrameCount() > 1;
            data.currentFrame = AV::Time::toString(p.sequence.getFrame(p.currentFrame), p.speed, p.timeUnits);
            data.speed = p.speed;
            data.realSpeed = p.realSpeed;
            p.hud->setHUDData(data);
        }

    } // namespace ViewApp
} // namespace djv

