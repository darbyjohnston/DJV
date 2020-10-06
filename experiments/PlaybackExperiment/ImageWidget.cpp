// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include "ImageWidget.h"

#include <djvUI/Style.h>

#include <djvRender2D/Render.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>

using namespace djv;

void ImageWidget::_init(const std::shared_ptr<System::Context>& context)
{
    Widget::_init(context);
}

ImageWidget::ImageWidget()
{}

ImageWidget::~ImageWidget()
{}

std::shared_ptr<ImageWidget> ImageWidget::create(const std::shared_ptr<System::Context>& context)
{
    auto out = std::shared_ptr<ImageWidget>(new ImageWidget);
    out->_init(context);
    return out;
}

void ImageWidget::setImage(const std::shared_ptr<djv::Image::Image>& value)
{
    if (_image == value)
        return;
    _image = value;
    _redraw();
}

void ImageWidget::_preLayoutEvent(System::Event::PreLayout&)
{
    const auto& style = _getStyle();
    const float sa = style->getMetric(UI::MetricsRole::ScrollArea);
    _setMinimumSize(glm::vec2(sa, sa));
}

void ImageWidget::_paintEvent(System::Event::Paint&)
{
    const auto& g = getGeometry();
    const auto& render = _getRender();
    render->setFillColor(Image::Color(0.F, 0.F, 0.F));
    render->drawRect(g);
    if (_image)
    {
        render->setFillColor(Image::Color(1.F, 1.F, 1.F));
        glm::mat3x3 m(1.F);
        m = glm::scale(m, glm::vec2(g.w() / _image->getWidth(), g.h() / _image->getHeight()));
        render->pushTransform(m);
        Render2D::ImageOptions options;
        options.cache = Render2D::ImageCache::Dynamic;
        render->drawImage(_image, g.min, options);
        render->popTransform();
    }
}
