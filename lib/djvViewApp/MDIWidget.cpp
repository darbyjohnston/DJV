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

#include <djvViewApp/MDIWidget.h>

#include <djvViewApp/FileSystem.h>
#include <djvViewApp/ImageSystem.h>
#include <djvViewApp/ImageView.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/TimelineSlider.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/Border.h>
#include <djvUI/FloatEdit.h>
#include <djvUI/FloatSlider.h>
#include <djvUI/GridLayout.h>
#include <djvUI/Label.h>
#include <djvUI/MDICanvas.h>
#include <djvUI/MDIWidget.h>
#include <djvUI/PopupWidget.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>
#include <djvUI/ToolBar.h>
#include <djvUI/ToolButton.h>

#include <djvAV/AVSystem.h>

#include <djvCore/Context.h>
#include <djvCore/Path.h>
#include <djvCore/Timer.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
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

                void setHoverCallback(const std::function<void(Hover, const glm::vec2&)>&);
                void setDragCallback(const std::function<void(Drag, const glm::vec2&)>&);

            protected:
                void _pointerEnterEvent(Event::PointerEnter&) override;
                void _pointerLeaveEvent(Event::PointerLeave&) override;
                void _pointerMoveEvent(Event::PointerMove&) override;
                void _buttonPressEvent(Event::ButtonPress&) override;
                void _buttonReleaseEvent(Event::ButtonRelease&) override;

            private:
                uint32_t _pressedID = Event::InvalidID;
                std::function<void(Hover, const glm::vec2&)> _hoverCallback;
                std::function<void(Drag, const glm::vec2&)> _dragCallback;
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

            void PointerWidget::setHoverCallback(const std::function<void(Hover, const glm::vec2&)>& callback)
            {
                _hoverCallback = callback;
            }

            void PointerWidget::setDragCallback(const std::function<void(Drag, const glm::vec2&)>& callback)
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
                        _hoverCallback(Hover::Start, pos);
                    }
                }
            }

            void PointerWidget::_pointerLeaveEvent(Event::PointerLeave& event)
            {
                event.accept();
                const auto& pos = event.getPointerInfo().projectedPos;
                if (_hoverCallback)
                {
                    _hoverCallback(Hover::End, pos);
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
                        _dragCallback(Drag::Move, pos);
                    }
                }
                else
                {
                    if (_hoverCallback)
                    {
                        _hoverCallback(Hover::Move, pos);
                    }
                }
            }

            void PointerWidget::_buttonPressEvent(Event::ButtonPress& event)
            {
                if (_pressedID)
                    return;
                const auto id = event.getPointerInfo().id;
                const auto& pos = event.getPointerInfo().projectedPos;
                event.accept();
                _pressedID = id;
                if (_dragCallback)
                {
                    _dragCallback(Drag::Start, pos);
                }
            }

            void PointerWidget::_buttonReleaseEvent(Event::ButtonRelease& event)
            {
                if (event.getPointerInfo().id != _pressedID)
                    return;
                event.accept();
                const auto& pos = event.getPointerInfo().projectedPos;
                _pressedID = Event::InvalidID;
                if (_dragCallback)
                {
                    _dragCallback(Drag::End, pos);
                }
            }

        } // namespace

        struct MDIWidget::Private
        {
            std::shared_ptr<Media> media;
            std::shared_ptr<AV::Image::Image> image;
            Time::Speed speed;
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
            //std::shared_ptr<UI::FloatEdit> speedEdit;
            std::shared_ptr<UI::ToolButton> speedButton;
            std::shared_ptr<UI::Label> currentTimeLabel;
            std::shared_ptr<UI::Label> durationLabel;
            std::shared_ptr<TimelineSlider> timelineSlider;
            std::shared_ptr<UI::PopupWidget> audioPopupWidget;
            std::shared_ptr<UI::BasicFloatSlider> volumeSlider;
            std::shared_ptr<UI::ToolButton> muteButton;
            std::shared_ptr<UI::GridLayout> playbackLayout;
            std::shared_ptr<UI::StackLayout> layout;

            std::function<void(Hover, const glm::vec2&)> hoverCallback;
            std::function<void(Drag, const glm::vec2&)> dragCallback;

            std::shared_ptr<ValueObserver<Time::Timestamp> > currentTimeObserver;
            std::shared_ptr<ValueObserver<AV::TimeUnits> > timeUnitsObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<AV::Image::Image> > > imageObserver;
            std::shared_ptr<ValueObserver<AV::IO::Info> > infoObserver;
            std::shared_ptr<ValueObserver<Time::Speed> > speedObserver;
            std::shared_ptr<ValueObserver<Time::Timestamp> > durationObserver;
            std::shared_ptr<ValueObserver<Time::Timestamp> > currentTimeObserver2;
            std::shared_ptr<ValueObserver<Playback> > playbackObserver;
            std::shared_ptr<ValueObserver<float> > volumeObserver;
            std::shared_ptr<ValueObserver<bool> > muteObserver;
            std::shared_ptr<ValueObserver<float> > fadeObserver;
            std::shared_ptr<ValueObserver<bool> > frameStoreEnabledObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<AV::Image::Image> > > frameStoreObserver;
        };

        void MDIWidget::_init(const std::shared_ptr<Media>& media, Context* context)
        {
            IWidget::_init(context);
            setClassName("djv::ViewApp::MDIWidget");

            DJV_PRIVATE_PTR();

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
            p.titleLabel->setText(Core::FileSystem::Path(media->getFileName()).getFileName());
            p.titleLabel->setTextHAlign(UI::TextHAlign::Left);
            p.titleLabel->setMargin(UI::MetricsRole::Margin);
            p.titleLabel->setTooltip(media->getFileName());

            p.maximizeButton = UI::ToolButton::create(context);
            p.maximizeButton->setIcon("djvIconSDI");
            p.maximizeButton->setInsideMargin(UI::MetricsRole::MarginSmall);

            p.closeButton = UI::ToolButton::create(context);
            p.closeButton->setIcon("djvIconClose");
            p.closeButton->setInsideMargin(UI::MetricsRole::MarginSmall);

            p.titleBar = UI::HorizontalLayout::create(context);
            p.titleBar->setBackgroundRole(UI::ColorRole::Overlay);
            p.titleBar->setSpacing(UI::MetricsRole::None);
            p.titleBar->addChild(p.titleLabel);
            p.titleBar->setStretch(p.titleLabel, UI::RowStretch::Expand);
            p.titleBar->addChild(p.maximizeButton);
            p.titleBar->addChild(p.closeButton);

            p.pointerWidget = PointerWidget::create(context);

            p.imageView = ImageView::create(context);

            //p.speedEdit = UI::FloatEdit::create(context);
            //p.speedEdit->setRange(FloatRange(0.f, 1000.f));
            p.speedButton = UI::ToolButton::create(context);

            p.currentTimeLabel = UI::Label::create(context);
            p.currentTimeLabel->setMargin(UI::MetricsRole::MarginSmall);

            p.durationLabel = UI::Label::create(context);
            p.durationLabel->setMargin(UI::MetricsRole::MarginSmall);

            p.timelineSlider = TimelineSlider::create(context);
            p.timelineSlider->setMedia(p.media);

            p.volumeSlider = UI::BasicFloatSlider::create(UI::Orientation::Vertical, context);
            p.volumeSlider->setMargin(UI::MetricsRole::MarginSmall);
            p.muteButton = UI::ToolButton::create(context);
            p.muteButton->setIcon("djvIconAudioMute");
            p.muteButton->setButtonType(UI::ButtonType::Toggle);
            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::MetricsRole::None);
            vLayout->addChild(p.volumeSlider);
            vLayout->addChild(p.muteButton);
            p.audioPopupWidget = UI::PopupWidget::create(context);
            p.audioPopupWidget->setIcon("djvIconAudio");
            p.audioPopupWidget->addChild(vLayout);

            auto toolbar = UI::ToolBar::create(context);
            toolbar->addAction(p.actions["Reverse"]);
            toolbar->addAction(p.actions["Forward"]);
            toolbar->addAction(p.actions["InPoint"]);
            toolbar->addAction(p.actions["PrevFrame"]);
            toolbar->addAction(p.actions["NextFrame"]);
            toolbar->addAction(p.actions["OutPoint"]);

            p.playbackLayout = UI::GridLayout::create(context);
            p.playbackLayout->setSpacing(UI::MetricsRole::None);
            p.playbackLayout->setBackgroundRole(UI::ColorRole::Overlay);
            p.playbackLayout->addChild(toolbar);
            p.playbackLayout->setGridPos(toolbar, 0, 0);
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::None);
            hLayout->addChild(p.timelineSlider);
            hLayout->setStretch(p.timelineSlider, UI::RowStretch::Expand);
            hLayout->addChild(p.audioPopupWidget);
            p.playbackLayout->addChild(hLayout);
            p.playbackLayout->setGridPos(hLayout, 1, 0);
            p.playbackLayout->setStretch(hLayout, UI::GridStretch::Horizontal);
            hLayout = UI::HorizontalLayout::create(context);
            hLayout->addChild(p.speedButton);
            p.playbackLayout->addChild(hLayout);
            p.playbackLayout->setGridPos(hLayout, 0, 1);
            hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::None);
            hLayout->addChild(p.currentTimeLabel);
            hLayout->addExpander();
            hLayout->addChild(p.durationLabel);
            p.playbackLayout->addChild(hLayout);
            p.playbackLayout->setGridPos(hLayout, 1, 1);

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
            _opacityUpdate();

            auto weak = std::weak_ptr<MDIWidget>(std::dynamic_pointer_cast<MDIWidget>(shared_from_this()));
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
                [weak](Hover hover, const glm::vec2& value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (widget->_p->hoverCallback)
                        {
                            const BBox2f& g = widget->_p->imageView->getGeometry();
                            widget->_p->hoverCallback(hover, value - g.min);
                        }
                    }
                });

            p.pointerWidget->setDragCallback(
                [weak](Drag drag, const glm::vec2& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->moveToFront();
                        if (widget->_p->dragCallback)
                        {
                            const BBox2f& g = widget->_p->imageView->getGeometry();
                            widget->_p->dragCallback(drag, value - g.min);
                        }
                    }
                });

            /*p.speedEdit->setValueCallback(
                [weak](float value)
            {
                if (auto widget = weak.lock())
                {
                    if (auto media = widget->_p->media)
                    {
                        media->setSpeed(Time::Speed::fromFloat(value));
                    }
                }
            });*/

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

            p.infoObserver = ValueObserver<AV::IO::Info>::create(
                p.media->observeInfo(),
                [weak](const AV::IO::Info& value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (value.video.size())
                        {
                            widget->_p->speed = value.video[0].speed;
                            const int64_t f = Time::scale(1, value.video[0].speed.swap(), Time::getTimebaseRational());
                            widget->_p->playbackLayout->setVisible(value.video[0].duration > f);
                        }
                        else
                        {
                            widget->_p->speed = Time::Speed();
                            widget->_p->playbackLayout->hide();
                        }
                        widget->_widgetUpdate();
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

        MDIWidget::MDIWidget() :
            _p(new Private)
        {}

        MDIWidget::~MDIWidget()
        {}

        std::shared_ptr<MDIWidget> MDIWidget::create(const std::shared_ptr<Media>& media, Context* context)
        {
            auto out = std::shared_ptr<MDIWidget>(new MDIWidget);
            out->_init(media, context);
            return out;
        }

        const std::shared_ptr<Media>& MDIWidget::getMedia() const
        {
            return _p->media;
        }

        const std::shared_ptr<ImageView>& MDIWidget::getImageView() const
        {
            return _p->imageView;
        }

        void MDIWidget::setHoverCallback(const std::function<void(Hover hover, const glm::vec2&)>& value)
        {
            _p->hoverCallback = value;
        }

        void MDIWidget::setDragCallback(const std::function<void(Drag drag, const glm::vec2&)>& value)
        {
            _p->dragCallback = value;
        }

        void MDIWidget::_setMaximized(float value)
        {
            IWidget::_setMaximized(value);
            _opacityUpdate();
            _resize();
        }

        void MDIWidget::_activeWidget(bool value)
        {
            DJV_PRIVATE_PTR();
            p.active = value;
            _imageUpdate();
        }

        void MDIWidget::_preLayoutEvent(Event::PreLayout&)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const float m = style->getMetric(UI::MetricsRole::Handle);
            _setMinimumSize(p.layout->getMinimumSize() + m * (1.f - _getMaximized()));
        }

        void MDIWidget::_layoutEvent(Event::Layout&)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const float m = style->getMetric(UI::MetricsRole::Handle);
            p.layout->setGeometry(getGeometry().margin(-m * (1.f - _getMaximized())));
        }

        void MDIWidget::_localeEvent(Event::Locale&)
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

            p.speedButton->setTooltip(_getText(DJV_TEXT("Speed tooltip")));
            p.currentTimeLabel->setTooltip(_getText(DJV_TEXT("Current time tooltip")));
            p.durationLabel->setTooltip(_getText(DJV_TEXT("Duration tooltip")));

            p.audioPopupWidget->setTooltip(_getText(DJV_TEXT("Audio popup tooltip")));
            p.volumeSlider->setTooltip(_getText(DJV_TEXT("Volume tooltip")));
            p.muteButton->setTooltip(_getText(DJV_TEXT("Mute tooltip")));

            _speedUpdate();
        }

        void MDIWidget::_widgetUpdate()
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

            p.speedButton->setEnabled(p.media.get());

            auto avSystem = getContext()->getSystemT<AV::AVSystem>();
            p.currentTimeLabel->setText(avSystem->getLabel(p.currentTime, p.speed));
            p.currentTimeLabel->setEnabled(p.media.get());
            p.durationLabel->setText(avSystem->getLabel(p.duration, p.speed));
            p.durationLabel->setEnabled(p.media.get());

            p.timelineSlider->setEnabled(p.media.get());

            p.audioPopupWidget->setEnabled(p.media.get());
        }

        void MDIWidget::_imageUpdate()
        {
            DJV_PRIVATE_PTR();
            p.imageView->setImage(p.active && p.frameStoreEnabled && p.frameStore ? p.frameStore : p.image);
        }

        void MDIWidget::_speedUpdate()
        {
            DJV_PRIVATE_PTR();
            //p.speedEdit->setValue(Time::Speed::toFloat(p.speed));
            {
                std::stringstream ss;
                ss.precision(2);
                ss << DJV_TEXT("FPS") << ": " << std::fixed << Time::Speed::toFloat(p.speed);
                p.speedButton->setText(ss.str());
            }
        }

        void MDIWidget::_opacityUpdate()
        {
            DJV_PRIVATE_PTR();
            p.titleBar->setOpacity(p.fade * (1.f - _getMaximized()));
            p.playbackLayout->setOpacity(p.fade);
        }

    } // namespace ViewApp
} // namespace djv

