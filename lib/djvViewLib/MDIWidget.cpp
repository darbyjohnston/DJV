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

#include <djvViewLib/MDIWidget.h>

#include <djvViewLib/Media.h>
#include <djvViewLib/PlaybackWidget.h>
#include <djvViewLib/TimelineSlider.h>

#include <djvUI/ImageWidget.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>

#include <djvCore/Memory.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct MDIWidget::Private
        {
            std::shared_ptr<Media> media;
            std::shared_ptr<UI::ImageWidget> imageWidget;
            std::shared_ptr<PlaybackWidget> playbackWidget;
            std::shared_ptr<TimelineSlider> timelineSlider;
            std::shared_ptr<UI::StackLayout> layout;
            std::shared_ptr<ValueObserver<std::shared_ptr<AV::Image> > > imageObserver;
            std::shared_ptr<ValueObserver<AV::Duration> > durationObserver;
            std::shared_ptr<ValueObserver<AV::Timestamp> > currentTimeObserver;
            std::shared_ptr<ValueObserver<Playback> > playbackObserver;
            std::shared_ptr<ValueObserver<Playback> > playbackObserver2;
            std::shared_ptr<ValueObserver<AV::Timestamp> > currentTimeObserver2;
        };
        
        void MDIWidget::_init(const std::shared_ptr<Media> & media, Context * context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            p.media = media;

            p.imageWidget = UI::ImageWidget::create(context);
            p.playbackWidget = PlaybackWidget::create(context);
            p.timelineSlider = TimelineSlider::create(context);

            p.layout = UI::StackLayout::create(context);
            p.layout->addWidget(p.imageWidget);
            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::MetricsRole::None);
            vLayout->addExpander();
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setBackgroundRole(UI::ColorRole::Overlay);
            hLayout->addWidget(p.playbackWidget);
            hLayout->addWidget(p.timelineSlider, UI::RowLayoutStretch::Expand);
            vLayout->addWidget(hLayout);
            p.layout->addWidget(vLayout);
            p.layout->setParent(shared_from_this());

            p.imageObserver = ValueObserver<std::shared_ptr<AV::Image> >::create(
                media->getCurrentImage(),
                [this](const std::shared_ptr<AV::Image> & image)
            {
                DJV_PRIVATE_PTR();
                size_t hash = 0;
                Memory::hashCombine(hash, p.media->getFileName());
                Memory::hashCombine(hash, p.media->getCurrentTime()->get());
                p.imageWidget->setImage(image, hash);
            });

            p.durationObserver = ValueObserver<AV::Duration>::create(
                media->getDuration(),
                [this](AV::Duration value)
            {
                DJV_PRIVATE_PTR();
                p.timelineSlider->setDuration(value);
            });

            p.currentTimeObserver = ValueObserver<AV::Timestamp>::create(
                media->getCurrentTime(),
                [this](AV::Timestamp value)
            {
                DJV_PRIVATE_PTR();
                p.timelineSlider->setCurrentTime(value);
            });

            p.playbackObserver = ValueObserver<Playback>::create(
                media->getPlayback(),
                [this](Playback value)
            {
                DJV_PRIVATE_PTR();
                p.playbackWidget->setPlayback(value);
            });

            p.playbackObserver2 = ValueObserver<Playback>::create(
                p.playbackWidget->getPlayback(),
                [this](Playback value)
            {
                DJV_PRIVATE_PTR();
                p.media->setPlayback(value);
            });

            p.currentTimeObserver2 = ValueObserver<AV::Timestamp>::create(
                p.timelineSlider->getCurrentTime(),
                [this](AV::Timestamp value)
            {
                DJV_PRIVATE_PTR();
                p.media->setCurrentTime(value);
            });
        }

        MDIWidget::MDIWidget() :
            _p(new Private)
        {}

        MDIWidget::~MDIWidget()
        {}

        std::shared_ptr<MDIWidget> MDIWidget::create(const std::shared_ptr<Media> & media, Context * context)
        {
            auto out = std::shared_ptr<MDIWidget>(new MDIWidget);
            out->_init(media, context);
            return out;
        }

        void MDIWidget::preLayoutEvent(PreLayoutEvent& event)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void MDIWidget::layoutEvent(LayoutEvent&)
        {
            _p->layout->setGeometry(getGeometry());
        }
        
    } // namespace ViewLib
} // namespace djv

