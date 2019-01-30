//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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
#include <djvViewLib/PlaybackWidget.h>
#include <djvViewLib/TimelineSlider.h>

#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct MediaWidget::Private
        {
            std::shared_ptr<Media> media;
            std::shared_ptr<ImageView> imageView;
            std::shared_ptr<PlaybackWidget> playbackWidget;
            std::shared_ptr<TimelineSlider> timelineSlider;
            std::shared_ptr<UI::Layout::Stack> layout;
            std::shared_ptr<ValueObserver<Time::Duration> > durationObserver;
            std::shared_ptr<ValueObserver<Time::Timestamp> > currentTimeObserver;
            std::shared_ptr<ValueObserver<Time::Timestamp> > currentTimeObserver2;
            std::shared_ptr<ValueObserver<Playback> > playbackObserver;
            std::shared_ptr<ValueObserver<Playback> > playbackObserver2;
        };
        
        void MediaWidget::_init(Context * context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            p.imageView = ImageView::create(context);

            _p->playbackWidget = PlaybackWidget::create(context);
            _p->timelineSlider = TimelineSlider::create(context);

            auto hLayout = UI::Layout::Horizontal::create(context);
            hLayout->setSpacing(UI::Style::MetricsRole::None);
            hLayout->setBackgroundRole(UI::Style::ColorRole::Overlay);
            hLayout->addWidget(_p->playbackWidget);
            hLayout->addWidget(_p->timelineSlider, UI::Layout::RowStretch::Expand);
            auto vLayout = UI::Layout::Vertical::create(context);
            vLayout->setSpacing(UI::Style::MetricsRole::None);
            vLayout->addExpander();
            vLayout->addWidget(hLayout);

            p.layout = UI::Layout::Stack::create(context);
            p.layout->addWidget(p.imageView);
            p.layout->addWidget(vLayout);
            p.layout->setParent(shared_from_this());

            auto weak = std::weak_ptr<MediaWidget>(std::dynamic_pointer_cast<MediaWidget>(shared_from_this()));
            p.currentTimeObserver = ValueObserver<Time::Timestamp>::create(
                p.timelineSlider->observeCurrentTime(),
                [weak](Time::Timestamp value)
            {
                if (auto system = weak.lock())
                {
                    if (system->_p->media)
                    {
                        system->_p->media->setCurrentTime(value);
                    }
                }
            });

            p.playbackObserver = ValueObserver<Playback>::create(
                p.playbackWidget->observePlayback(),
                [weak](Playback value)
            {
                if (auto system = weak.lock())
                {
                    if (system->_p->media)
                    {
                        system->_p->media->setPlayback(value);
                    }
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

            auto weak = std::weak_ptr<MediaWidget>(std::dynamic_pointer_cast<MediaWidget>(shared_from_this()));
            _p->durationObserver = ValueObserver<Time::Duration>::create(
                value->observeDuration(),
                [weak](Time::Duration value)
            {
                if (auto system = weak.lock())
                {
                    system->_p->timelineSlider->setDuration(value);
                }
            });
            _p->currentTimeObserver2 = ValueObserver<Time::Timestamp>::create(
                value->observeCurrentTime(),
                [weak](Time::Timestamp value)
            {
                if (auto system = weak.lock())
                {
                    system->_p->timelineSlider->setCurrentTime(value);
                }
            });
            _p->playbackObserver2 = ValueObserver<Playback>::create(
                value->observePlayback(),
                [weak](Playback value)
            {
                if (auto system = weak.lock())
                {
                    system->_p->playbackWidget->setPlayback(value);
                }
            });
        }

        void MediaWidget::_preLayoutEvent(Event::PreLayout& event)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void MediaWidget::_layoutEvent(Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }
        
    } // namespace ViewLib
} // namespace djv

