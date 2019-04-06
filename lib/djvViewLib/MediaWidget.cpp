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

#include <djvViewLib/MediaWidget.h>

#include <djvViewLib/ImageView.h>
#include <djvViewLib/Media.h>
#include <djvViewLib/PlaybackSystem.h>
#include <djvViewLib/TimelineSlider.h>

#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>
#include <djvUI/ToolBar.h>

#include <djvAV/AVSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct MediaWidget::Private
        {
            std::shared_ptr<Media> media;
            Time::Timestamp duration = 0;
            Time::Timestamp currentTime = 0;
            Time::Speed speed;
            AV::TimeUnits timeUnits = AV::TimeUnits::First;

            std::shared_ptr<ImageView> imageView;
            std::shared_ptr<UI::Label> currentTimeLabel;
            std::shared_ptr<TimelineSlider> timelineSlider;
            std::shared_ptr<UI::ToolBar> toolbar;
            std::shared_ptr<UI::StackLayout> layout;
            std::shared_ptr<ValueObserver<AV::IO::Info> > infoObserver;
            std::shared_ptr<ValueObserver<Time::Timestamp> > durationObserver;
            std::shared_ptr<ValueObserver<Time::Timestamp> > currentTimeObserver;
            std::shared_ptr<ValueObserver<Time::Timestamp> > currentTimeObserver2;
            std::shared_ptr<ValueObserver<AV::TimeUnits> > timeUnitsObserver;
        };
        
        void MediaWidget::_init(Context * context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            p.imageView = ImageView::create(context);

            p.currentTimeLabel = UI::Label::create(context);
            p.currentTimeLabel->setFontSizeRole(UI::MetricsRole::FontLarge);
            p.currentTimeLabel->setMargin(UI::Layout::Margin(UI::MetricsRole::Margin, UI::MetricsRole::Margin, UI::MetricsRole::MarginSmall, UI::MetricsRole::MarginSmall));

            p.timelineSlider = TimelineSlider::create(context);

            p.toolbar = UI::ToolBar::create(context);
            p.toolbar->setBackgroundRole(UI::ColorRole::Overlay);
            if (auto playbackSystem = context->getSystemT<PlaybackSystem>())
            {
                auto actions = playbackSystem->getActions();
                p.toolbar->addAction(actions["Reverse"]);
                p.toolbar->addAction(actions["Forward"]);
                p.toolbar->addAction(actions["InPoint"]);
                p.toolbar->addAction(actions["PrevFrame"]);
                p.toolbar->addAction(actions["NextFrame"]);
                p.toolbar->addAction(actions["OutPoint"]);
            }
            p.toolbar->addChild(p.currentTimeLabel);
            p.toolbar->addChild(p.timelineSlider);
            p.toolbar->setStretch(p.timelineSlider, UI::RowStretch::Expand);

            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::MetricsRole::None);
            vLayout->addExpander();
            vLayout->addChild(p.toolbar);

            p.layout = UI::StackLayout::create(context);
            p.layout->addChild(p.imageView);
            p.layout->addChild(vLayout);
            addChild(p.layout);

            _widgetUpdate();

            auto weak = std::weak_ptr<MediaWidget>(std::dynamic_pointer_cast<MediaWidget>(shared_from_this()));
            p.currentTimeObserver = ValueObserver<Time::Timestamp>::create(
                p.timelineSlider->observeCurrentTime(),
                [weak](Time::Timestamp value)
            {
                if (auto widget = weak.lock())
                {
                    if (widget->_p->media)
                    {
                        widget->_p->media->setCurrentTime(value);
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
        }

        MediaWidget::MediaWidget() :
            _p(new Private)
        {}

        MediaWidget::~MediaWidget()
        {}

        std::shared_ptr<MediaWidget> MediaWidget::create(Context * context)
        {
            auto out = std::shared_ptr<MediaWidget>(new MediaWidget);
            out->_init(context);
            return out;
        }

        const std::shared_ptr<Media> & MediaWidget::getMedia() const
        {
            return _p->media;
        }

        void MediaWidget::setMedia(const std::shared_ptr<Media> & value)
        {
            if (value == _p->media)
                return;
            _p->media = value;
            _p->imageView->setMedia(value);
            _widgetUpdate();
            if (_p->media)
            {
                auto weak = std::weak_ptr<MediaWidget>(std::dynamic_pointer_cast<MediaWidget>(shared_from_this()));
                _p->infoObserver = ValueObserver<AV::IO::Info>::create(
                    _p->media->observeInfo(),
                    [weak](const AV::IO::Info & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->speed = value.video.size() ? value.video[0].speed : Time::Speed();
                        widget->_widgetUpdate();
                    }
                });
                _p->durationObserver = ValueObserver<Time::Timestamp>::create(
                    _p->media->observeDuration(),
                    [weak](Time::Timestamp value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->duration = value;
                        widget->_widgetUpdate();
                    }
                });
                _p->currentTimeObserver2 = ValueObserver<Time::Timestamp>::create(
                    _p->media->observeCurrentTime(),
                    [weak](Time::Timestamp value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->currentTime = value;
                        widget->_widgetUpdate();
                    }
                });
            }
            else
            {
                _p->duration = 0;
                _p->currentTime = 0;
                _p->speed = Time::Speed();
                _p->timelineSlider->setDuration(0);
                _p->timelineSlider->setCurrentTime(0);
                _p->durationObserver.reset();
                _p->currentTimeObserver2.reset();
                _widgetUpdate();
            }
        }

        void MediaWidget::_preLayoutEvent(Event::PreLayout & event)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void MediaWidget::_layoutEvent(Event::Layout &)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void MediaWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            auto avSystem = getContext()->getSystemT<AV::AVSystem>();
            p.currentTimeLabel->setText(avSystem->getLabel(p.currentTime, p.speed));
            std::string currentTimeSizeString;
            switch (p.timeUnits)
            {
            case AV::TimeUnits::Timecode:
                currentTimeSizeString = "00:00:00:00";
                break;
            case AV::TimeUnits::Frames:
                currentTimeSizeString = std::string(Math::getNumDigits(Time::timestampToFrame(p.duration, p.speed)), '0');
                break;
            default: break;
            }
            p.currentTimeLabel->setSizeString(currentTimeSizeString);
            p.timelineSlider->setCurrentTime(p.currentTime);
            p.timelineSlider->setDuration(p.duration);
            p.timelineSlider->setSpeed(p.speed);
            p.timelineSlider->setEnabled(p.media.get());
        }
        
    } // namespace ViewLib
} // namespace djv

