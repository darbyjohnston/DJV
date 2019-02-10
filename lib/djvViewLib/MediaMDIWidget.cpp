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

#include <djvViewLib/MediaMDIWidget.h>

#include <djvViewLib/ImageView.h>
#include <djvViewLib/PlaybackWidget.h>
#include <djvViewLib/TimelineSlider.h>

#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ToolButton.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct MediaMDIWidget::Private
        {
			std::shared_ptr<Media> media;
            std::shared_ptr<UI::Label> titleLabel;
            std::shared_ptr<UI::HorizontalLayout> titleBar;
			std::shared_ptr<ImageView> imageView;
			std::shared_ptr<PlaybackWidget> playbackWidget;
			std::shared_ptr<TimelineSlider> timelineSlider;
            std::shared_ptr<UI::VerticalLayout> layout;
            std::function<void(void)> maximizeCallback;
            std::function<void(void)> closeCallback;
			std::shared_ptr<ValueObserver<Time::Duration> > durationObserver;
			std::shared_ptr<ValueObserver<Time::Timestamp> > currentTimeObserver;
			std::shared_ptr<ValueObserver<Time::Timestamp> > currentTimeObserver2;
			std::shared_ptr<ValueObserver<Playback> > playbackObserver;
			std::shared_ptr<ValueObserver<Playback> > playbackObserver2;
        };
        
        void MediaMDIWidget::_init(Context * context)
        {
            IWidget::_init(context);

            DJV_PRIVATE_PTR();
            p.titleLabel = UI::Label::create(context);
			p.titleLabel->setFontSizeRole(UI::MetricsRole::FontHeader);
            p.titleLabel->setTextHAlign(UI::TextHAlign::Left);
            p.titleLabel->setMargin(UI::MetricsRole::Margin);

            auto maximizeButton = UI::ToolButton::create(context);
            maximizeButton->setIcon("djvIconViewLibSDI");

            auto closeButton = UI::ToolButton::create(context);
            closeButton->setIcon("djvIconClose");

            p.titleBar = UI::HorizontalLayout::create(context);
            p.titleBar->addWidget(p.titleLabel, UI::RowStretch::Expand);
			p.titleBar->setBackgroundRole(UI::ColorRole::Trough);
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::None);
            hLayout->addWidget(maximizeButton);
            hLayout->addWidget(closeButton);
            p.titleBar->addWidget(hLayout);

			p.imageView = ImageView::create(context);

			_p->playbackWidget = PlaybackWidget::create(context);
			_p->timelineSlider = TimelineSlider::create(context);
			auto playbackSettingsButton = UI::ToolButton::create(context);
			playbackSettingsButton->setIcon("djvIconPopupMenu");

			p.layout = UI::VerticalLayout::create(context);
			p.layout->setMargin(UI::MetricsRole::Handle);
			p.layout->setSpacing(UI::MetricsRole::None);
			p.layout->addWidget(p.titleBar);
			p.layout->addWidget(p.imageView, UI::RowStretch::Expand);
			hLayout = UI::HorizontalLayout::create(context);
			hLayout->setSpacing(UI::MetricsRole::None);
			hLayout->setBackgroundRole(UI::ColorRole::Trough);
			hLayout->addWidget(_p->playbackWidget);
			hLayout->addWidget(_p->timelineSlider, UI::RowStretch::Expand);
			hLayout->addWidget(playbackSettingsButton);
			p.layout->addWidget(hLayout);
            IContainer::addWidget(p.layout);

			_widgetUpdate();

            auto weak = std::weak_ptr<MediaMDIWidget>(std::dynamic_pointer_cast<MediaMDIWidget>(shared_from_this()));
            maximizeButton->setClickedCallback(
                [weak]
            {
                if (auto widget = weak.lock())
                {
                    if (widget->_p->maximizeCallback)
                    {
                        widget->_p->maximizeCallback();
                    }
                }
            });

            closeButton->setClickedCallback(
                [weak]
            {
                if (auto widget = weak.lock())
                {
                    if (widget->_p->closeCallback)
                    {
                        widget->_p->closeCallback();
                    }
                }
            });

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

			p.playbackObserver = ValueObserver<Playback>::create(
				p.playbackWidget->observePlayback(),
				[weak](const Playback & value)
			{
				if (auto widget = weak.lock())
				{
					if (widget->_p->media)
					{
						widget->_p->media->setPlayback(value);
					}
				}
			});
        }

		MediaMDIWidget::MediaMDIWidget() :
            _p(new Private)
        {}

		MediaMDIWidget::~MediaMDIWidget()
        {}

        std::shared_ptr<MediaMDIWidget> MediaMDIWidget::create(Context * context)
        {
            auto out = std::shared_ptr<MediaMDIWidget>(new MediaMDIWidget);
            out->_init(context);
            return out;
        }

        const std::string & MediaMDIWidget::getTitle() const
        {
            return _p->titleLabel->getText();
        }

        void MediaMDIWidget::setTitle(const std::string & text)
        {
            _p->titleLabel->setText(text);
        }

        const std::shared_ptr<Media> & MediaMDIWidget::getMedia() const
        {
            return _p->media;
        }

        void MediaMDIWidget::setMedia(const std::shared_ptr<Media> & value)
        {
			if (value == _p->media)
				return;
			_p->media = value;
			_p->imageView->setMedia(value);
			_widgetUpdate();
			if (_p->media)
			{
				auto weak = std::weak_ptr<MediaMDIWidget>(std::dynamic_pointer_cast<MediaMDIWidget>(shared_from_this()));
				_p->durationObserver = ValueObserver<Time::Duration>::create(
					_p->media->observeDuration(),
					[weak](Time::Duration value)
				{
					if (auto system = weak.lock())
					{
						system->_p->timelineSlider->setDuration(value);
					}
				});
				_p->currentTimeObserver2 = ValueObserver<Time::Timestamp>::create(
					_p->media->observeCurrentTime(),
					[weak](Time::Timestamp value)
				{
					if (auto system = weak.lock())
					{
						system->_p->timelineSlider->setCurrentTime(value);
					}
				});
				_p->playbackObserver2 = ValueObserver<Playback>::create(
					_p->media->observePlayback(),
					[weak](Playback value)
				{
					if (auto system = weak.lock())
					{
						system->_p->playbackWidget->setPlayback(value);
					}
				});
			}
			else
			{
				_p->playbackWidget->setPlayback(Playback());
				_p->timelineSlider->setDuration(0);
				_p->timelineSlider->setCurrentTime(0);
				_p->durationObserver.reset();
				_p->currentTimeObserver2.reset();
				_p->playbackObserver2.reset();
			}
        }

        void MediaMDIWidget::setCloseCallback(const std::function<void(void)> & value)
        {
            _p->closeCallback = value;
        }

        void MediaMDIWidget::setMaximizeCallback(const std::function<void(void)> & value)
        {
            _p->maximizeCallback = value;
        }

        float MediaMDIWidget::getHeightForWidth(float value) const
        {
            return _p->layout->getHeightForWidth(value);
        }

        void MediaMDIWidget::_preLayoutEvent(Event::PreLayout& event)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void MediaMDIWidget::_layoutEvent(Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

		void MediaMDIWidget::_widgetUpdate()
		{
			DJV_PRIVATE_PTR();
			p.playbackWidget->setEnabled(p.media.get());
			p.timelineSlider->setEnabled(p.media.get());
		}
        
    } // namespace ViewLib
} // namespace djv

