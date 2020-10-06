// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include "Window.h"

#include "ImageWidget.h"
#include "Media.h"
#include "PlaybackWidget.h"
#include "TimelineWidget.h"

using namespace djv;

void Window::_init(
    const std::shared_ptr<Media>& media,
    const std::shared_ptr<System::Context>& context)
{
    UI::Window::_init(context);

    _media = media;

    _imageWidget = ImageWidget::create(context);

    _playbackWidget = PlaybackWidget::create(context);

    _currentTimeLabel = UI::Text::Label::create(context);
    _currentTimeLabel->setSizeString("000000");

    _timelineWidget = TimelineWidget::create(context);

    _videoQueueGraphWidget = UIComponents::LineGraphWidget::create(context);
    _audioQueueGraphWidget = UIComponents::LineGraphWidget::create(context);

    _layout = UI::VerticalLayout::create(context);
    _layout->setSpacing(UI::MetricsRole::None);
    auto hLayout = UI::HorizontalLayout::create(context);
    hLayout->setSpacing(UI::MetricsRole::None);
    hLayout->addChild(_imageWidget);
    hLayout->setStretch(_imageWidget, UI::RowStretch::Expand);
    auto vLayout = UI::VerticalLayout::create(context);
    vLayout->setMargin(UI::MetricsRole::MarginSmall);
    vLayout->setSpacing(UI::MetricsRole::SpacingSmall);
    auto label = UI::Text::Label::create(context);
    label->setText("Video Queue");
    label->setTextHAlign(UI::TextHAlign::Left);
    vLayout->addChild(label);
    vLayout->addChild(_videoQueueGraphWidget);
    label = UI::Text::Label::create(context);
    label->setText("Audio Queue");
    label->setTextHAlign(UI::TextHAlign::Left);
    vLayout->addChild(label);
    vLayout->addChild(_audioQueueGraphWidget);
    hLayout->addChild(vLayout);
    _layout->addChild(hLayout);
    _layout->setStretch(hLayout, UI::RowStretch::Expand);
    hLayout = UI::HorizontalLayout::create(context);
    hLayout->setSpacing(UI::MetricsRole::SpacingSmall);
    hLayout->addChild(_playbackWidget);
    hLayout->addChild(_currentTimeLabel);
    hLayout->addChild(_timelineWidget);
    hLayout->setStretch(_timelineWidget, UI::RowStretch::Expand);
    _layout->addChild(hLayout);
    addChild(_layout);

    _widgetUpdate();

    auto weak = std::weak_ptr<Window>(std::dynamic_pointer_cast<Window>(shared_from_this()));
    _playbackWidget->setPlaybackCallback(
        [weak](Playback value)
        {
            if (auto widget = weak.lock())
            {
                widget->_media->setPlayback(value);
            }
        });

    _timelineWidget->setFrameCallback(
        [weak](Math::Frame::Index value)
        {
            if (auto widget = weak.lock())
            {
                widget->_media->seek(value);
            }
        });

    _infoObserver = Core::Observer::Value<IOInfo>::create(
        media->observeInfo(),
        [weak](const IOInfo& value)
        {
            if (auto widget = weak.lock())
            {
                widget->_info = value;
                widget->_widgetUpdate();
            }
        });

    _imageObserver = Core::Observer::Value<std::shared_ptr<Image::Image> >::create(
        media->observeCurrentImage(),
        [weak](const std::shared_ptr<Image::Image>& value)
        {
            if (auto widget = weak.lock())
            {
                widget->_imageWidget->setImage(value);
            }
        });

    _videoQueueSizeObserver = Core::Observer::Value<size_t>::create(
        media->observeVideoQueueSize(),
        [weak](size_t value)
        {
            if (auto widget = weak.lock())
            {
                widget->_videoQueueGraphWidget->addSample(value);
            }
        });

    _audioQueueSizeObserver = Core::Observer::Value<size_t>::create(
        media->observeAudioQueueSize(),
        [weak](size_t value)
        {
            if (auto widget = weak.lock())
            {
                std::stringstream ss;
                ss << value;
                widget->_audioQueueGraphWidget->addSample(value);
            }
        });

    _playbackObserver = Core::Observer::Value<Playback>::create(
        media->observePlayback(),
        [weak](Playback value)
        {
            if (auto widget = weak.lock())
            {
                widget->_playbackWidget->setPlayback(value);
            }
        });

    _currentFrameObserver = Core::Observer::Value<Math::Frame::Index>::create(
        media->observeCurrentFrame(),
        [weak](Math::Frame::Index value)
        {
            if (auto widget = weak.lock())
            {
                widget->_currentFrame = value;
                widget->_widgetUpdate();
            }
        });
}

Window::Window()
{}

Window::~Window()
{}

std::shared_ptr<Window> Window::create(
    const std::shared_ptr<Media>& media,
    const std::shared_ptr<System::Context>& context)
{
    auto out = std::shared_ptr<Window>(new Window);
    out->_init(media, context);
    return out;
}

void Window::_widgetUpdate()
{
    std::stringstream ss;
    ss << _currentFrame;
    _currentTimeLabel->setText(ss.str());

    _timelineWidget->setSequence(_info.videoSequence);
    _timelineWidget->setFrame(_currentFrame);
}
