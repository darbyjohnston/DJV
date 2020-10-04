// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include "Window.h"

#include "ImageWidget.h"
#include "Media.h"
#include "TimelineWidget.h"

using namespace djv;

void Window::_init(
    const std::shared_ptr<Media>& media,
    const std::shared_ptr<System::Context>& context)
{
    UI::Window::_init(context);

    _media = media;

    _imageWidget = ImageWidget::create(context);

    _timelineWidget = TimelineWidget::create(context);

    _layout = UI::VerticalLayout::create(context);
    _layout->setSpacing(UI::MetricsRole::None);
    _layout->addChild(_imageWidget);
    _layout->setStretch(_imageWidget, UI::RowStretch::Expand);
    _layout->addChild(_timelineWidget);
    addChild(_layout);

    auto weak = std::weak_ptr<Window>(std::dynamic_pointer_cast<Window>(shared_from_this()));
    _imageObserver = Core::Observer::Value<std::shared_ptr<Image::Image> >::create(
        media->observeCurrentImage(),
        [weak](const std::shared_ptr<Image::Image>& value)
        {
            if (auto widget = weak.lock())
            {
                widget->_imageWidget->setImage(value);
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
