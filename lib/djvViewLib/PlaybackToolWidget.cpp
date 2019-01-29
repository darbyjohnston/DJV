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

#include <djvViewLib/PlaybackToolWidget.h>

#include <djvViewLib/PlaybackWidget.h>
#include <djvViewLib/TimelineSlider.h>

#include <djvUI/Border.h>
#include <djvUI/RowLayout.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct PlaybackToolWidget::Private
        {
            std::shared_ptr<PlaybackWidget> playbackWidget;
            std::shared_ptr<TimelineSlider> timelineSlider;
            std::shared_ptr<UI::Layout::Horizontal> layout;
            std::shared_ptr<UI::Layout::Border> border;
        };

        void PlaybackToolWidget::_init(Context * context)
        {
            IToolWidget::_init(context);

            setBackgroundRole(UI::Style::ColorRole::Background);
            setPointerEnabled(true);

            _p->playbackWidget = PlaybackWidget::create(context);
            
            _p->timelineSlider = TimelineSlider::create(context);

            _p->layout = UI::Layout::Horizontal::create(context);
            _p->layout->setSpacing(UI::Style::MetricsRole::None);
            _p->layout->addSpacer();
            _p->layout->addSeparator();
            _p->layout->addWidget(_p->playbackWidget);
            _p->layout->addSeparator();
            _p->layout->addWidget(_p->timelineSlider, UI::Layout::RowStretch::Expand);
            _p->layout->addSeparator();
            _p->layout->addSpacer();

            _p->border = UI::Layout::Border::create(context);
            _p->border->addWidget(_p->layout);
            _p->border->setParent(shared_from_this());
        }

        PlaybackToolWidget::PlaybackToolWidget() :
            _p(new Private)
        {}

        PlaybackToolWidget::~PlaybackToolWidget()
        {}
        
        std::shared_ptr<PlaybackToolWidget> PlaybackToolWidget::create(Context * context)
        {
            auto out = std::shared_ptr<PlaybackToolWidget>(new PlaybackToolWidget);
            out->_init(context);
            return out;
        }

        std::shared_ptr<IValueSubject<Time::Timestamp> > PlaybackToolWidget::observeCurrentTime() const
        {
            return _p->timelineSlider->observeCurrentTime();
        }

        std::shared_ptr<IValueSubject<Playback> > PlaybackToolWidget::observePlayback() const
        {
            return _p->playbackWidget->observePlayback();
        }

        void PlaybackToolWidget::setDuration(Time::Duration value)
        {
            _p->timelineSlider->setDuration(value);
        }

        void PlaybackToolWidget::setCurrentTime(Time::Timestamp value)
        {
            _p->timelineSlider->setCurrentTime(value);
        }

        void PlaybackToolWidget::setPlayback(Playback value)
        {
            _p->playbackWidget->setPlayback(value);
        }

        std::shared_ptr<UI::Widget> PlaybackToolWidget::getMoveHandle()
        {
            return std::dynamic_pointer_cast<UI::Widget>(shared_from_this());
        }

        void PlaybackToolWidget::_preLayoutEvent(Event::PreLayout&)
        {
            _setMinimumSize(_p->border->getMinimumSize());
        }

        void PlaybackToolWidget::_layoutEvent(Event::Layout&)
        {
            _p->border->setGeometry(getGeometry());
        }

    } // namespace ViewLib
} // namespace djv

