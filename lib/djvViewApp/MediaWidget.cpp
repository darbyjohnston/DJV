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

#include <djvViewApp/FileSystem.h>
#include <djvViewApp/ImageSystem.h>
#include <djvViewApp/ImageView.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/TimelineSlider.h>
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

        namespace
        {
            class PointerWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(PointerWidget);

            protected:
                void _init(Context*);
                PointerWidget()
                {}

            public:
                static std::shared_ptr<PointerWidget> create(Context*);

                void setHoverCallback(const std::function<void(PointerData)>&);
                void setDragCallback(const std::function<void(PointerData)>&);

            protected:
                void _pointerEnterEvent(Event::PointerEnter&) override;
                void _pointerLeaveEvent(Event::PointerLeave&) override;
                void _pointerMoveEvent(Event::PointerMove&) override;
                void _buttonPressEvent(Event::ButtonPress&) override;
                void _buttonReleaseEvent(Event::ButtonRelease&) override;

            private:
                uint32_t _pressedID = Event::InvalidID;
                std::map<int, bool> _buttons;
                std::function<void(PointerData)> _hoverCallback;
                std::function<void(PointerData)> _dragCallback;
            };

            void PointerWidget::_init(Context* context)
            {
                Widget::_init(context);
                setClassName("djv::ViewApp::MediaWidget::PointerWidget");
            }

            std::shared_ptr<PointerWidget> PointerWidget::create(Context* context)
            {
                auto out = std::shared_ptr<PointerWidget>(new PointerWidget);
                out->_init(context);
                return out;
            }

            void PointerWidget::setHoverCallback(const std::function<void(PointerData)>& callback)
            {
                _hoverCallback = callback;
            }

            void PointerWidget::setDragCallback(const std::function<void(PointerData)>& callback)
            {
                _dragCallback = callback;
            }

            void PointerWidget::_pointerEnterEvent(Event::PointerEnter& event)
            {
                if (!event.isRejected())
                {
                    event.accept();
                    const auto& pos = event.getPointerInfo().projectedPos;
                    if (_hoverCallback)
                    {
                        _hoverCallback(PointerData(PointerState::Start, pos, std::map<int, bool>()));
                    }
                }
            }

            void PointerWidget::_pointerLeaveEvent(Event::PointerLeave& event)
            {
                event.accept();
                const auto& pos = event.getPointerInfo().projectedPos;
                if (_hoverCallback)
                {
                    _hoverCallback(PointerData(PointerState::End, pos, std::map<int, bool>()));
                }
            }

            void PointerWidget::_pointerMoveEvent(Event::PointerMove& event)
            {
                event.accept();
                const auto& pos = event.getPointerInfo().projectedPos;
                if (_pressedID)
                {
                    if (_dragCallback)
                    {
                        _dragCallback(PointerData(PointerState::Move, pos, _buttons));
                    }
                }
                else
                {
                    if (_hoverCallback)
                    {
                        _hoverCallback(PointerData(PointerState::Move, pos, std::map<int, bool>()));
                    }
                }
            }

            void PointerWidget::_buttonPressEvent(Event::ButtonPress& event)
            {
                if (_pressedID)
                    return;
                event.accept();
                const auto& info = event.getPointerInfo();
                _pressedID = info.id;
                _buttons = info.buttons;
                if (_dragCallback)
                {
                    _dragCallback(PointerData(PointerState::Start, info.pos, info.buttons));
                }
            }

            void PointerWidget::_buttonReleaseEvent(Event::ButtonRelease& event)
            {
                if (event.getPointerInfo().id != _pressedID)
                    return;
                event.accept();
                const auto& info = event.getPointerInfo();
                _pressedID = Event::InvalidID;
                _buttons = std::map<int, bool>();
                if (_dragCallback)
                {
                    _dragCallback(PointerData(PointerState::End, info.pos, info.buttons));
                }
            }

        } // namespace

        struct MediaWidget::Private
        {
            std::shared_ptr<ValueSubject<PointerData> > hover;
            std::shared_ptr<ValueSubject<PointerData> > drag;

            std::shared_ptr<Media> media;
            std::shared_ptr<AV::Image::Image> image;
            std::vector<Time::Speed> speeds;
            Time::Speed defaultSpeed;
            Time::Speed speed;
            float realSpeed = 0.f;
            bool playEveryFrame = false;
            Time::Timestamp duration = 0;
            Time::Timestamp currentTime = 0;
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
            std::shared_ptr<UI::Label> currentTimeLabel;
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

            std::shared_ptr<ValueObserver<Time::Timestamp> > currentTimeObserver;
            std::shared_ptr<ValueObserver<AV::TimeUnits> > timeUnitsObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<AV::Image::Image> > > imageObserver;
            std::shared_ptr<ValueObserver<Time::Speed> > speedObserver;
            std::shared_ptr<ValueObserver<Time::Speed> > defaultSpeedObserver;
            std::shared_ptr<ValueObserver<float> > realSpeedObserver;
            std::shared_ptr<ValueObserver<bool> > playEveryFrameObserver;
            std::shared_ptr<ValueObserver<Time::Timestamp> > durationObserver;
            std::shared_ptr<ValueObserver<Time::Timestamp> > currentTimeObserver2;
            std::shared_ptr<ValueObserver<Playback> > playbackObserver;
            std::shared_ptr<ValueObserver<float> > volumeObserver;
            std::shared_ptr<ValueObserver<bool> > muteObserver;
            std::shared_ptr<ValueObserver<bool> > hasCacheObserver;
            std::shared_ptr<ValueObserver<bool> > cacheEnabledObserver;
            std::shared_ptr<ValueObserver<int> > cacheMaxObserver;
            std::shared_ptr<ListObserver<Time::TimestampRange> > cachedTimestampsObserver;
            std::shared_ptr<ValueObserver<float> > fadeObserver;
            std::shared_ptr<ValueObserver<bool> > frameStoreEnabledObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<AV::Image::Image> > > frameStoreObserver;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > actionObservers;
        };

        void MediaWidget::_init(const std::shared_ptr<Media>& media, Context* context)
        {
            IWidget::_init(context);
            setClassName("djv::ViewApp::MediaWidget");

            DJV_PRIVATE_PTR();
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
            p.titleLabel->setText(media->getFileInfo().getFileName(Frame::Invalid, false));
            p.titleLabel->setTextHAlign(UI::TextHAlign::Left);
            p.titleLabel->setMargin(UI::MetricsRole::Margin);
            p.titleLabel->setTooltip(media->getFileInfo());

            p.maximizeButton = UI::ToolButton::create(context);
            p.maximizeButton->setIcon("djvIconSDI");
            p.maximizeButton->setInsideMargin(UI::MetricsRole::MarginSmall);

            p.closeButton = UI::ToolButton::create(context);
            p.closeButton->setIcon("djvIconClose");
            p.closeButton->setInsideMargin(UI::MetricsRole::MarginSmall);

            p.titleBar = UI::HorizontalLayout::create(context);
            p.titleBar->setBackgroundRole(UI::ColorRole::OverlayLight);
            p.titleBar->setSpacing(UI::MetricsRole::None);
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

            p.currentTimeLabel = UI::Label::create(context);
            p.currentTimeLabel->setMargin(UI::MetricsRole::MarginSmall);

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
            p.playbackLayout->setSpacing(UI::MetricsRole::None);
            p.playbackLayout->setBackgroundRole(UI::ColorRole::OverlayLight);
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
            hLayout->addChild(p.currentTimeLabel);
            hLayout->addExpander();
            hLayout->addChild(p.durationLabel);
            p.playbackLayout->addChild(hLayout);
            p.playbackLayout->setGridPos(hLayout, 1, 1);
            p.playbackLayout->addChild(p.memoryCachePopupWidget);
            p.playbackLayout->setGridPos(p.memoryCachePopupWidget, 2, 1);

            p.layout = UI::StackLayout::create(context);
            p.layout->addChild(p.imageView);
            vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::MetricsRole::None);
            vLayout->addChild(p.titleBar);
            vLayout->addChild(p.pointerWidget);
            vLayout->setStretch(p.pointerWidget, UI::RowStretch::Expand);
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
                [weak](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto media = widget->_p->media)
                        {
                            media->setCacheMax(value);
                        }
                    }
                });

            p.cacheEnabledButton->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto media = widget->_p->media)
                        {
                            media->setCacheEnabled(value);
                        }
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

            p.currentTimeObserver = ValueObserver<Time::Timestamp>::create(
                p.timelineSlider->observeCurrentTime(),
                [weak](Time::Timestamp value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto media = widget->_p->media)
                        {
                            media->setCurrentTime(value);
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

            p.durationObserver = ValueObserver<Time::Timestamp>::create(
                p.media->observeDuration(),
                [weak](Time::Timestamp value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->duration = value;
                        widget->_widgetUpdate();
                    }
                });

            p.currentTimeObserver2 = ValueObserver<Time::Timestamp>::create(
                p.media->observeCurrentTime(),
                [weak](Time::Timestamp value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->currentTime = value;
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

            p.hasCacheObserver = ValueObserver<bool>::create(
                p.media->observeHasCache(),
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->cacheMaxSlider->setEnabled(value);
                        widget->_p->cacheEnabledButton->setEnabled(value);
                    }
                });

            p.cacheEnabledObserver = ValueObserver<bool>::create(
                p.media->observeCacheEnabled(),
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->cacheEnabledButton->setChecked(value);
                    }
                });

            p.cacheMaxObserver = ValueObserver<int>::create(
                p.media->observeCacheMax(),
                [weak](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->cacheMaxSlider->setValue(value);
                    }
                });

            p.cachedTimestampsObserver = ListObserver<Time::TimestampRange>::create(
                p.media->observeCachedTimestamps(),
                [weak](const std::vector<Time::TimestampRange>& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->timelineSlider->setCachedTimestamps(value);
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
            _setMinimumSize(p.layout->getMinimumSize() + sh * 2.f);
        }

        void MediaWidget::_layoutEvent(Event::Layout&)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const float sh = style->getMetric(UI::MetricsRole::Shadow);
            p.layout->setGeometry(getGeometry().margin(-sh));
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
            p.currentTimeLabel->setTooltip(_getText(DJV_TEXT("Current time tooltip")));
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
            p.currentTimeLabel->setText(avSystem->getLabel(p.currentTime, p.defaultSpeed));
            p.currentTimeLabel->setEnabled(p.media.get());
            p.durationLabel->setText(avSystem->getLabel(p.duration, p.defaultSpeed));
            p.durationLabel->setEnabled(p.media.get());

            p.timelineSlider->setEnabled(p.media.get());

            const int64_t f = Time::scale(1, p.defaultSpeed.swap(), Time::getTimebaseRational());
            p.playbackLayout->setVisible(p.duration > f);

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

