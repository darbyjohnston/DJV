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
#include <djvViewApp/PlaybackSettings.h>
#include <djvViewApp/PlaybackSpeedWidget.h>
#include <djvViewApp/TimelineSlider.h>
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
#include <djvUI/PopupWidget.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/StackLayout.h>
#include <djvUI/ToolBar.h>
#include <djvUI/ToolButton.h>

#include <djvAV/AVSystem.h>
#include <djvAV/Render2D.h>

#include <djvCore/Context.h>
#include <djvCore/NumericValueModels.h>
#include <djvCore/Path.h>
#include <djvCore/Timer.h>

#include <iomanip>

//#define DJV_DEMO_THREADS

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
            std::shared_ptr<ValueSubject<glm::vec2> > scroll;

            std::shared_ptr<Media> media;
            AV::IO::Info ioInfo;
            std::shared_ptr<AV::Image::Image> image;
            Time::Speed defaultSpeed;
            Time::Speed speed;
            float realSpeed = 0.F;
            bool playEveryFrame = false;
            PlaybackMode playbackMode = PlaybackMode::First;
            Frame::Sequence sequence;
            Frame::Index currentFrame = Frame::invalidIndex;
            AV::IO::InOutPoints inOutPoints;
            Playback playbackPrev = Playback::Count;
            AV::TimeUnits timeUnits = AV::TimeUnits::First;
            ImageViewLock viewLock = ImageViewLock::First;
            bool frameStoreEnabled = false;
            std::shared_ptr<AV::Image::Image> frameStore;
            bool audioEnabled = false;
            float audioVolume = 0.F;
            bool audioMute = false;
            bool active = false;
            float fade = 1.F;

            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::ActionGroup> playbackActionGroup;

            std::shared_ptr<UI::Label> titleLabel;
            std::shared_ptr<UI::ToolButton> maximizeButton;
            std::shared_ptr<UI::ToolButton> closeButton;
            std::shared_ptr<UI::HorizontalLayout> titleBar;
            std::shared_ptr<PointerWidget> pointerWidget;
            std::shared_ptr<ImageView> imageView;
            std::shared_ptr<PlaybackSpeedWidget> speedWidget;
            std::shared_ptr<UI::PopupWidget> speedPopupWidget;
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
            std::shared_ptr<UI::Label> audioLabel;
            std::shared_ptr<UI::FloatSlider> audioVolumeSlider;
            std::shared_ptr<UI::ToolButton> audioMuteButton;
            std::shared_ptr<UI::PopupWidget> audioPopupWidget;
#ifdef DJV_DEMO_THREADS
            std::shared_ptr<UI::IntSlider> ioThreadsSlider;
            std::shared_ptr<UI::PopupWidget> ioThreadsPopupWidget;
            std::shared_ptr<UI::Label> ioThreadsLabel;
            std::shared_ptr<UI::Label> cachePercentageLabel;
#endif // DJV_DEMO_THREADS
            std::shared_ptr<UI::GridLayout> playbackLayout;
            std::shared_ptr<UI::StackLayout> layout;

            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > actionObservers;
            std::shared_ptr<ValueObserver<AV::IO::Info> > ioInfoObserver;
            std::shared_ptr<ValueObserver<Frame::Index> > currentFrameObserver;
            std::shared_ptr<ValueObserver<bool> > currentFrameChangeObserver;
            std::shared_ptr<ValueObserver<AV::TimeUnits> > timeUnitsObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<AV::Image::Image> > > imageObserver;
            std::shared_ptr<ValueObserver<Time::Speed> > speedObserver;
            std::shared_ptr<ValueObserver<Time::Speed> > defaultSpeedObserver;
            std::shared_ptr<ValueObserver<float> > realSpeedObserver;
            std::shared_ptr<ValueObserver<bool> > playEveryFrameObserver;
            std::shared_ptr<ValueObserver<PlaybackMode> > playbackModeObserver;
            std::shared_ptr<ValueObserver<Frame::Sequence> > sequenceObserver;
            std::shared_ptr<ValueObserver<Frame::Index> > currentFrameObserver2;
            std::shared_ptr<ValueObserver<AV::IO::InOutPoints> > inOutPointsObserver;
            std::shared_ptr<ValueObserver<Playback> > playbackObserver;
            std::shared_ptr<ValueObserver<bool> > audioEnabledObserver;
            std::shared_ptr<ValueObserver<float> > volumeObserver;
            std::shared_ptr<ValueObserver<bool> > muteObserver;
#ifdef DJV_DEMO_THREADS
            std::shared_ptr<ValueObserver<size_t> > ioThreadsObserver;
#endif // DJV_DEMO_THREADS
            std::shared_ptr<ValueObserver<bool> > cacheEnabledObserver;
            std::shared_ptr<ValueObserver<Frame::Sequence> > cacheSequenceObserver;
            std::shared_ptr<ValueObserver<Frame::Sequence> > cachedFramesObserver;
            std::shared_ptr<ListObserver<std::shared_ptr<AnnotatePrimitive> > > annotationsObserver;
            std::shared_ptr<ValueObserver<float> > fadeObserver;
            std::shared_ptr<ValueObserver<ImageViewLock> > viewLockObserver;
            std::shared_ptr<ValueObserver<bool> > frameStoreEnabledObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<AV::Image::Image> > > frameStoreObserver;
            std::shared_ptr<ValueObserver<UI::ImageAspectRatio> > imageAspectRatioObserver;
        };

        void MediaWidget::_init(const std::shared_ptr<Media>& media, const std::shared_ptr<Context>& context)
        {
            IWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::MediaWidget");

            p.hover = ValueSubject<PointerData>::create();
            p.drag = ValueSubject<PointerData>::create();
            p.scroll = ValueSubject<glm::vec2>::create();

            p.media = media;

            p.actions["Forward"] = UI::Action::create();
            p.actions["Forward"]->setIcon("djvIconPlaybackForward");
            p.actions["Forward"]->setCheckedIcon("djvIconPlaybackStop");
            p.actions["Reverse"] = UI::Action::create();
            p.actions["Reverse"]->setIcon("djvIconPlaybackReverse");
            p.actions["Reverse"]->setCheckedIcon("djvIconPlaybackStop");
            p.playbackActionGroup = UI::ActionGroup::create(UI::ButtonType::Exclusive);
            p.playbackActionGroup->addAction(p.actions["Forward"]);
            p.playbackActionGroup->addAction(p.actions["Reverse"]);

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
            p.titleLabel->setText(media->getFileInfo().getFileName(Frame::invalid, false));
            p.titleLabel->setTextHAlign(UI::TextHAlign::Left);
            p.titleLabel->setMargin(UI::Layout::Margin(UI::MetricsRole::Margin, UI::MetricsRole::Margin, UI::MetricsRole::None, UI::MetricsRole::None));
            p.titleLabel->setTooltip(std::string(media->getFileInfo()));

            p.maximizeButton = UI::ToolButton::create(context);
            p.maximizeButton->setIcon("djvIconSDI");
            p.maximizeButton->setInsideMargin(UI::MetricsRole::MarginSmall);

            p.closeButton = UI::ToolButton::create(context);
            p.closeButton->setIcon("djvIconClose");
            p.closeButton->setInsideMargin(UI::MetricsRole::MarginSmall);

            p.titleBar = UI::HorizontalLayout::create(context);
            p.titleBar->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            p.titleBar->setBackgroundRole(UI::ColorRole::OverlayLight);
            p.titleBar->addChild(p.titleLabel);
            p.titleBar->setStretch(p.titleLabel, UI::RowStretch::Expand);
            p.titleBar->addChild(p.maximizeButton);
            p.titleBar->addChild(p.closeButton);

            p.pointerWidget = PointerWidget::create(context);

            p.imageView = ImageView::create(context);

            p.speedWidget = PlaybackSpeedWidget::create(context);
            p.speedPopupWidget = UI::PopupWidget::create(context);
            p.speedPopupWidget->setPopupIcon("djvIconPopupMenu");
            p.speedPopupWidget->setFont(AV::Font::familyMono);
            p.speedPopupWidget->setFontSizeRole(UI::MetricsRole::FontSmall);
            p.speedPopupWidget->addChild(p.speedWidget);
            p.realSpeedLabel = UI::Label::create(context);
            p.realSpeedLabel->setFont(AV::Font::familyMono);
            p.realSpeedLabel->setFontSizeRole(UI::MetricsRole::FontSmall);

            p.playbackModeButton = UI::MultiStateButton::create(context);
            p.playbackModeButton->addIcon("djvIconPlayOnce");
            p.playbackModeButton->addIcon("djvIconPlayLoop");
            p.playbackModeButton->addIcon("djvIconPlayPingPong");

            p.currentFrameWidget = FrameWidget::create(context);
            p.inPointWidget = FrameWidget::create(context);
            p.inPointSetButton = UI::ToolButton::create(context);
            p.inPointSetButton->setIcon("djvIconFrameStartSmall");
            p.inPointSetButton->setInsideMargin(UI::MetricsRole::None);
            p.inPointSetButton->setVAlign(UI::VAlign::Center);
            p.inPointResetButton = UI::ToolButton::create(context);
            p.inPointResetButton->setIcon("djvIconCloseSmall");
            p.inPointResetButton->setInsideMargin(UI::MetricsRole::None);
            p.inPointResetButton->setVAlign(UI::VAlign::Center);
            p.outPointWidget = FrameWidget::create(context);
            p.outPointSetButton = UI::ToolButton::create(context);
            p.outPointSetButton->setIcon("djvIconFrameEndSmall");
            p.outPointSetButton->setInsideMargin(UI::MetricsRole::None);
            p.outPointSetButton->setVAlign(UI::VAlign::Center);
            p.outPointResetButton = UI::ToolButton::create(context);
            p.outPointResetButton->setIcon("djvIconCloseSmall");
            p.outPointResetButton->setInsideMargin(UI::MetricsRole::None);
            p.outPointResetButton->setVAlign(UI::VAlign::Center);

            p.durationLabel = UI::Label::create(context);
            p.durationLabel->setFont(AV::Font::familyMono);
            p.durationLabel->setFontSizeRole(UI::MetricsRole::FontSmall);
            p.durationLabel->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));

            p.timelineSlider = TimelineSlider::create(context);
            p.timelineSlider->setMedia(p.media);

            p.audioLabel = UI::Label::create(context);
            p.audioLabel->setTextHAlign(UI::TextHAlign::Left);
            p.audioLabel->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
            p.audioLabel->setBackgroundRole(UI::ColorRole::Trough);
            p.audioVolumeSlider = UI::FloatSlider::create(context);
            p.audioVolumeSlider->setRange(FloatRange(0.F, 100.F));
            p.audioVolumeSlider->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
            p.audioMuteButton = UI::ToolButton::create(context);
            p.audioMuteButton->setIcon("djvIconAudioMute");
            p.audioMuteButton->setButtonType(UI::ButtonType::Toggle);
            p.audioMuteButton->setVAlign(UI::VAlign::Fill);
            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            vLayout->addChild(p.audioLabel);
            vLayout->addSeparator();
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            hLayout->addChild(p.audioVolumeSlider);
            hLayout->addChild(p.audioMuteButton);
            vLayout->addChild(hLayout);
            p.audioPopupWidget = UI::PopupWidget::create(context);
            p.audioPopupWidget->setVAlign(UI::VAlign::Center);
            p.audioPopupWidget->addChild(vLayout);

            auto toolBar = UI::ToolBar::create(context);
            toolBar->setVAlign(UI::VAlign::Center);
            toolBar->setBackgroundRole(UI::ColorRole::None);
            toolBar->addAction(p.actions["InPoint"]);
            toolBar->addAction(p.actions["PrevFrame"]);
            toolBar->addAction(p.actions["Reverse"]);
            toolBar->addAction(p.actions["Forward"]);
            toolBar->addAction(p.actions["NextFrame"]);
            toolBar->addAction(p.actions["OutPoint"]);
            
#ifdef DJV_DEMO_THREADS
            p.ioThreadsSlider = UI::IntSlider::create(context);
            p.ioThreadsSlider->setRange(IntRange(2, 64));
            p.ioThreadsPopupWidget = UI::PopupWidget::create(context);
            p.ioThreadsPopupWidget->setIcon("djvIconPopupMenu");
            p.ioThreadsPopupWidget->setVAlign(UI::VAlign::Center);
            p.ioThreadsPopupWidget->addChild(p.ioThreadsSlider);

            p.ioThreadsLabel = UI::Label::create(context);
            p.ioThreadsLabel->setFontSizeRole(UI::MetricsRole::Swatch);
            p.ioThreadsLabel->setTextHAlign(UI::TextHAlign::Left);

            p.cachePercentageLabel = UI::Label::create(context);
            p.cachePercentageLabel->setTextHAlign(UI::TextHAlign::Right);
            p.cachePercentageLabel->setFont(AV::Font::familyMono);
            p.cachePercentageLabel->setFontSizeRole(UI::MetricsRole::Slider);
#endif // DJV_DEMO_THREADS
            
            p.playbackLayout = UI::GridLayout::create(context);
            p.playbackLayout->setBackgroundRole(UI::ColorRole::OverlayLight);
            p.playbackLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall, UI::MetricsRole::None));
            p.playbackLayout->addChild(toolBar);
            p.playbackLayout->setGridPos(toolBar, 0, 0);
            p.playbackLayout->addChild(p.timelineSlider);
            p.playbackLayout->setGridPos(p.timelineSlider, 1, 0);
            p.playbackLayout->setStretch(p.timelineSlider, UI::GridStretch::Horizontal);
            p.playbackLayout->addChild(p.audioPopupWidget);
            p.playbackLayout->setGridPos(p.audioPopupWidget, 2, 0);
#ifdef DJV_DEMO_THREADS
            p.playbackLayout->addChild(p.ioThreadsPopupWidget);
            p.playbackLayout->setGridPos(p.ioThreadsPopupWidget, 2, 1);
#endif // DJV_DEMO_THREADS
            hLayout = UI::HorizontalLayout::create(context);
            hLayout->addChild(p.speedPopupWidget);
            hLayout->addChild(p.realSpeedLabel);
            hLayout->addChild(p.playbackModeButton);
            p.playbackLayout->addChild(hLayout);
            p.playbackLayout->setGridPos(hLayout, 0, 1);
            hLayout = UI::HorizontalLayout::create(context);
            hLayout->addChild(p.currentFrameWidget);
            auto hLayout2 = UI::HorizontalLayout::create(context);
            hLayout2->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            hLayout2->addChild(p.inPointWidget);
            hLayout2->addChild(p.inPointSetButton);
            hLayout2->addChild(p.inPointResetButton);
            hLayout->addChild(hLayout2);
            hLayout->addExpander();
            hLayout2 = UI::HorizontalLayout::create(context);
            hLayout2->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            hLayout2->addChild(p.outPointResetButton);
            hLayout2->addChild(p.outPointSetButton);
            hLayout2->addChild(p.outPointWidget);
            hLayout->addChild(hLayout2);
            hLayout->addChild(p.durationLabel);
            p.playbackLayout->addChild(hLayout);
            p.playbackLayout->setGridPos(hLayout, 1, 1);

            p.layout = UI::StackLayout::create(context);
            p.layout->setBackgroundRole(UI::ColorRole::OverlayLight);
            p.layout->addChild(p.imageView);
            p.layout->addChild(p.pointerWidget);
            vLayout = UI::VerticalLayout::create(context);
            vLayout->addChild(p.titleBar);
            vLayout->addExpander();
            vLayout->addChild(p.playbackLayout);
            p.layout->addChild(vLayout);
#ifdef DJV_DEMO_THREADS
            vLayout = UI::VerticalLayout::create(context);
            vLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginLarge));
            vLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingLarge));
            vLayout->setBackgroundRole(UI::ColorRole::OverlayLight);
            vLayout->setHAlign(UI::HAlign::Center);
            vLayout->setVAlign(UI::VAlign::Center);
            vLayout->addChild(p.ioThreadsLabel);
            vLayout->addChild(p.cachePercentageLabel);
            p.layout->addChild(vLayout);
#endif // DJV_DEMO_THREADS
            addChild(p.layout);

            _widgetUpdate();
            _speedUpdate();
            _realSpeedUpdate();
            _audioUpdate();
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
            p.pointerWidget->setScrollCallback(
                [weak](const glm::vec2& data)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->moveToFront();
                        const BBox2f& g = widget->_p->imageView->getGeometry();
                        widget->_p->scroll->setAlways(data);
                    }
                });

            auto contextWeak = std::weak_ptr<Context>(context);
            p.speedWidget->setSpeedCallback(
                [weak, contextWeak](const Time::Speed& value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            if (auto media = widget->_p->media)
                            {
                                media->setSpeed(value);
                            }
                            auto avSystem = context->getSystemT<AV::AVSystem>();
                            avSystem->setDefaultSpeed(Time::fromRational(value));
                            widget->_p->speedPopupWidget->close();
                        }
                    }
                });

            p.speedWidget->setPlayEveryFrameCallback(
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            if (auto media = widget->_p->media)
                            {
                                media->setPlayEveryFrame(value);
                            }
                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                            if (auto playbackSettings = settingsSystem->getSettingsT<PlaybackSettings>())
                            {
                                playbackSettings->setPlayEveryFrame(value);
                            }
                        }
                    }
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
                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                            if (auto playbackSettings = settingsSystem->getSettingsT<PlaybackSettings>())
                            {
                                playbackSettings->setPlaybackMode(playbackMode);
                            }
                            widget->_widgetUpdate();
                        }
                    }
                });

            p.currentFrameWidget->setCallback(
                [weak](Frame::Index value)
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
                [weak](Frame::Index value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto media = widget->_p->media)
                        {
                            const auto& inOutPoints = media->observeInOutPoints()->get();
                            const size_t sequenceSize = widget->_p->sequence.getSize();
                            media->setInOutPoints(AV::IO::InOutPoints(
                                inOutPoints.isEnabled(),
                                Math::clamp(value, static_cast<Frame::Index>(0), static_cast<Frame::Index>(sequenceSize > 0 ? (sequenceSize - 1) : 0)),
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
                [weak](Frame::Index value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto media = widget->_p->media)
                        {
                            const auto& inOutPoints = media->observeInOutPoints()->get();
                            const size_t sequenceSize = widget->_p->sequence.getSize();
                            media->setInOutPoints(AV::IO::InOutPoints(
                                inOutPoints.isEnabled(),
                                inOutPoints.getIn(),
                                Math::clamp(value, static_cast<Frame::Index>(0), static_cast<Frame::Index>(sequenceSize > 0 ? (sequenceSize - 1) : 0))));
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
                [weak](Frame::Index value)
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

            p.audioVolumeSlider->setValueCallback(
                [weak](float value)
            {
                if (auto widget = weak.lock())
                {
                    if (auto media = widget->_p->media)
                    {
                        media->setVolume(value / 100.F);
                    }
                }
            });

            p.audioMuteButton->setCheckedCallback(
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

#ifdef DJV_DEMO_THREADS
            p.ioThreadsSlider->setValueCallback(
                [weak](int value)
            {
                if (auto widget = weak.lock())
                {
                    if (auto media = widget->_p->media)
                    {
                        media->setThreadCount(value);
                    }
                }
            });
#endif // DJV_DEMO_THREADS
            
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
                        widget->_widgetUpdate();
                        widget->_audioUpdate();
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
                p.media->observeDefaultSpeed(),
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

            p.currentFrameObserver2 = ValueObserver<Frame::Index>::create(
                p.media->observeCurrentFrame(),
                [weak](Frame::Index value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->currentFrame = value;
                        widget->_widgetUpdate();
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

#ifdef DJV_DEMO_THREADS
            p.ioThreadsObserver = ValueObserver<size_t>::create(
                p.media->observeThreadCount(),
                [weak](size_t value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->ioThreadsSlider->setValue(value);
                        
                        std::stringstream ss;
                        ss << value << " " << "threads";
                        widget->_p->ioThreadsLabel->setText(ss.str());
                    }
                });
#endif // DJV_DEMO_THREADS

            auto settingsSystem = context->getSystemT<UI::Settings::System>();
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

            p.cacheSequenceObserver = ValueObserver<Frame::Sequence>::create(
                p.media->observeCacheSequence(),
                [weak](const Frame::Sequence& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->timelineSlider->setCacheSequence(value);
                    }
                });

            p.cachedFramesObserver = ValueObserver<Frame::Sequence>::create(
                p.media->observeCachedFrames(),
                [weak](const Frame::Sequence& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->timelineSlider->setCachedFrames(value);
                        
#ifdef DJV_DEMO_THREADS
                        const size_t sequenceSize = widget->_p->sequence.getSize();
                        const size_t cacheSize = value.getSize();
                        const int cachePercentage = sequenceSize > 0 ?
                            static_cast<int>(cacheSize / static_cast<float>(sequenceSize) * 100.F) :
                            0;
                        std::stringstream ss;
                        ss << std::setw(3) << cachePercentage << "%";
                        widget->_p->cachePercentageLabel->setText(ss.str());
#endif // DJV_DEMO_THREADS
                    }
                });

            p.annotationsObserver = ListObserver<std::shared_ptr<AnnotatePrimitive> >::create(
                p.media->observeAnnotations(),
                [weak](const std::vector<std::shared_ptr<AnnotatePrimitive> >& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageView->setAnnotations(value);
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
                p.viewLockObserver = ValueObserver<ImageViewLock>::create(
                    viewSettings->observeLock(),
                    [weak](ImageViewLock value)
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

            p.imageAspectRatioObserver = ValueObserver<UI::ImageAspectRatio>::create(
                p.imageView->observeImageAspectRatio(),
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

        std::shared_ptr<MediaWidget> MediaWidget::create(const std::shared_ptr<Media>& media, const std::shared_ptr<Context>& context)
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

        void MediaWidget::fitWindow()
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const float sh = style->getMetric(UI::MetricsRole::Shadow);
            const BBox2f imageBBox = p.imageView->getImageBBox();
            const float zoom = p.imageView->observeImageZoom()->get();
            const glm::vec2 imageSize = imageBBox.getSize() * zoom;
            glm::vec2 size(ceilf(imageSize.x), ceilf(imageSize.y));
            switch (p.viewLock)
            {
            case ImageViewLock::Frame:
                size.y += _getTitleBarHeight() + _getPlaybackHeight();
                break;
            default: break;
            }
            resize(size + sh * 2.F);
        }

        std::shared_ptr<IValueSubject<PointerData> > MediaWidget::observeHover() const
        {
            return _p->hover;
        }

        std::shared_ptr<IValueSubject<PointerData> > MediaWidget::observeDrag() const
        {
            return _p->drag;
        }

        std::shared_ptr<IValueSubject<glm::vec2> > MediaWidget::observeScroll() const
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


        std::map<UI::MDI::Handle, std::vector<BBox2f> > MediaWidget::_getHandles() const
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

            glm::vec2 imageSize = p.imageView->getMinimumSize();
            const float ar = p.imageView->getImageBBox().getAspect();
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
            case ImageViewLock::Frame:
                size.y += _getTitleBarHeight() + _getPlaybackHeight();
                break;
            default: break;
            }

            _setMinimumSize(size + sh * 2.F);
        }

        void MediaWidget::_layoutEvent(Event::Layout&)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const float sh = style->getMetric(UI::MetricsRole::Shadow);
            const BBox2f g = getGeometry().margin(-sh);
            p.layout->setGeometry(g);
            const BBox2f imageFrame = BBox2f(
                glm::vec2(
                    g.min.x,
                    g.min.y + _getTitleBarHeight()),
                glm::vec2(
                    g.max.x,
                    g.max.y - _getPlaybackHeight()));
            p.imageView->setImageFrame(imageFrame);
        }

        void MediaWidget::_initEvent(Event::Init& event)
        {
            IWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            
            p.actions["Forward"]->setTooltip(_getText(DJV_TEXT("Forward tooltip")));
            p.actions["Reverse"]->setTooltip(_getText(DJV_TEXT("Reverse tooltip")));
            p.actions["InPoint"]->setTooltip(_getText(DJV_TEXT("Go to in point tooltip")));
            p.actions["NextFrame"]->setTooltip(_getText(DJV_TEXT("Next frame tooltip")));
            p.actions["PrevFrame"]->setTooltip(_getText(DJV_TEXT("Previous frame tooltip")));
            p.actions["OutPoint"]->setTooltip(_getText(DJV_TEXT("Go to out point tooltip")));

            p.maximizeButton->setTooltip(_getText(DJV_TEXT("Maximize tooltip")));
            p.closeButton->setTooltip(_getText(DJV_TEXT("Close tooltip")));

            p.speedPopupWidget->setTooltip(_getText(DJV_TEXT("Speed popup tooltip")));
            p.realSpeedLabel->setTooltip(_getText(DJV_TEXT("Real speed tooltip")));
            p.currentFrameWidget->setTooltip(_getText(DJV_TEXT("Current frame tooltip")));
            p.inPointWidget->setTooltip(_getText(DJV_TEXT("In point tooltip")));
            p.inPointSetButton->setTooltip(_getText(DJV_TEXT("Set in point tooltip")));
            p.inPointResetButton->setTooltip(_getText(DJV_TEXT("Reset in point tooltip")));
            p.outPointWidget->setTooltip(_getText(DJV_TEXT("Out point tooltip")));
            p.outPointSetButton->setTooltip(_getText(DJV_TEXT("Set out point tooltip")));
            p.outPointResetButton->setTooltip(_getText(DJV_TEXT("Reset out point tooltip")));
            p.durationLabel->setTooltip(_getText(DJV_TEXT("Duration tooltip")));

            p.audioLabel->setText(_getText(DJV_TEXT("Audio")));
            p.audioVolumeSlider->setTooltip(_getText(DJV_TEXT("Volume tooltip")));
            p.audioMuteButton->setTooltip(_getText(DJV_TEXT("Mute tooltip")));
            p.audioPopupWidget->setTooltip(_getText(DJV_TEXT("Audio popup tooltip")));
            
#ifdef DJV_DEMO_THREADS
            p.ioThreadsPopupWidget->setTooltip(_getText(DJV_TEXT("I/O threads popup tooltip")));
#endif // DJV_DEMO_THREADS

            _widgetUpdate();
            _speedUpdate();
        }

        void MediaWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
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

                p.playbackModeButton->setCurrentIndex(static_cast<int>(p.playbackMode));
                switch (p.playbackMode)
                {
                case PlaybackMode::Once:
                    p.playbackModeButton->setTooltip(_getText(DJV_TEXT("Playback mode once tooltip")));
                    break;
                case PlaybackMode::Loop:
                    p.playbackModeButton->setTooltip(_getText(DJV_TEXT("Playback mode loop tooltip")));
                    break;
                case PlaybackMode::PingPong:
                    p.playbackModeButton->setTooltip(_getText(DJV_TEXT("Playback mode ping-pong tooltip")));
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
                const size_t sequenceSize = p.sequence.getSize();
                p.outPointResetButton->setEnabled(
                    p.inOutPoints.isEnabled() &&
                    sequenceSize > 0 &&
                    p.inOutPoints.getOut() != (sequenceSize - 1));

                auto avSystem = context->getSystemT<AV::AVSystem>();
                p.durationLabel->setText(avSystem->getLabel(p.sequence.getSize(), p.defaultSpeed));

                p.timelineSlider->setInOutPointsEnabled(p.inOutPoints.isEnabled());
                p.timelineSlider->setInPoint(p.inOutPoints.getIn());
                p.timelineSlider->setOutPoint(p.inOutPoints.getOut());

                p.playbackLayout->setVisible(p.sequence.getSize() > 1);
            }
        }

        void MediaWidget::_imageUpdate()
        {
            DJV_PRIVATE_PTR();
            p.imageView->setImage(p.active && p.frameStoreEnabled && p.frameStore ? p.frameStore : p.image);
        }

        void MediaWidget::_speedUpdate()
        {
            DJV_PRIVATE_PTR();
            p.speedWidget->setSpeed(p.speed);
            p.speedWidget->setDefaultSpeed(p.defaultSpeed);
            p.speedWidget->setPlayEveryFrame(p.playEveryFrame);

            std::stringstream ss;
            ss.precision(2);
            ss << _getText(DJV_TEXT("FPS")) << ": " << std::fixed << p.speed.toFloat();
            p.speedPopupWidget->setText(ss.str());
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
            p.audioVolumeSlider->setValue(p.audioVolume * 100.F);
            p.audioMuteButton->setChecked(p.audioMute);
            if (!p.audioEnabled || p.audioMute)
            {
                p.audioPopupWidget->setIcon("djvIconAudioMute");
            }
            else if (p.audioVolume < 1.F / 4.F)
            {
                p.audioPopupWidget->setIcon("djvIconAudio0");
            }
            else if (p.audioVolume < 2.F / 4.F)
            {
                p.audioPopupWidget->setIcon("djvIconAudio1");
            }
            else if (p.audioVolume < 3.F / 4.F)
            {
                p.audioPopupWidget->setIcon("djvIconAudio2");
            }
            else
            {
                p.audioPopupWidget->setIcon("djvIconAudio3");
            }
        }

        void MediaWidget::_opacityUpdate()
        {
            DJV_PRIVATE_PTR();
            const float maximize = 1.F - _getMaximize();
            p.titleBar->setOpacity(p.fade * maximize);
            p.playbackLayout->setOpacity(p.fade);
        }

    } // namespace ViewApp
} // namespace djv

