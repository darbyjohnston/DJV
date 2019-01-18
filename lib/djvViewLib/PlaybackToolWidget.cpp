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

#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct PlaybackToolWidget::Private
        {
            std::shared_ptr<UI::Label> titleLabel;
            std::shared_ptr<PlaybackWidget> playbackWidget;
            std::shared_ptr<TimelineSlider> timelineSlider;
            std::shared_ptr<UI::Icon> resizeHandle;
            std::shared_ptr<UI::Layout::Vertical> layout;
        };

        void PlaybackToolWidget::_init(Context * context)
        {
            Widget::_init(context);

            setBackgroundRole(UI::Style::ColorRole::Overlay);
            setPointerEnabled(true);

            _p->titleLabel = UI::Label::create(context);
            _p->titleLabel->setTextHAlign(UI::TextHAlign::Left);
            _p->titleLabel->setMargin(UI::Style::MetricsRole::MarginSmall);

            _p->playbackWidget = PlaybackWidget::create(context);
            
            _p->timelineSlider = TimelineSlider::create(context);
            
            _p->resizeHandle = UI::Icon::create("djvIconWindowResizeHandle", context);
            _p->resizeHandle->setVAlign(UI::VAlign::Bottom);

            _p->layout = UI::Layout::Vertical::create(context);
            _p->layout->setSpacing(UI::Style::MetricsRole::None);
            _p->layout->addWidget(_p->titleLabel);
            _p->layout->addSeparator();
            auto hLayout = UI::Layout::Horizontal::create(context);
            hLayout->setSpacing(UI::Style::MetricsRole::None);
            hLayout->addWidget(_p->playbackWidget);
            hLayout->addWidget(_p->timelineSlider, UI::Layout::RowStretch::Expand);
            hLayout->addWidget(_p->resizeHandle);
            _p->layout->addWidget(hLayout, UI::Layout::RowStretch::Expand);
            _p->layout->setParent(shared_from_this());
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

        std::shared_ptr<UI::Widget> PlaybackToolWidget::getResizeHandle()
        {
            return _p->resizeHandle;
        }

        void PlaybackToolWidget::_preLayoutEvent(Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void PlaybackToolWidget::_layoutEvent(Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void PlaybackToolWidget::_localeEvent(Event::Locale &)
        {
            _p->titleLabel->setText(_getText(DJV_TEXT("djv::ViewLib", "Playback")));
        }

    } // namespace ViewLib
} // namespace djv

