// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include "ImageWidget.h"

#include <djvUI/Style.h>

#include <djvRender2D/Render.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>

using namespace djv;

struct ImageWidget::Private
{
    std::shared_ptr<Image::Image> image;
};

void ImageWidget::_init(const std::shared_ptr<System::Context>& context)
{
    Widget::_init(context);
}

ImageWidget::ImageWidget() :
    _p(new Private)
{}

ImageWidget::~ImageWidget()
{}

std::shared_ptr<ImageWidget> ImageWidget::create(const std::shared_ptr<System::Context>& context)
{
    auto out = std::shared_ptr<ImageWidget>(new ImageWidget);
    out->_init(context);
    return out;
}

void ImageWidget::setImage(const std::shared_ptr<Image::Image>& value)
{
    DJV_PRIVATE_PTR();
    if (p.image == value)
        return;
    p.image = value;
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
    DJV_PRIVATE_PTR();
    const auto& g = getGeometry();
    const auto& render = _getRender();
    render->setFillColor(Image::Color(0.F, 0.F, 0.F));
    render->drawRect(g);
    if (p.image)
    {
        render->setFillColor(Image::Color(1.F, 1.F, 1.F));
        glm::mat3x3 m(1.F);
        m = glm::scale(m, glm::vec2(g.w() / p.image->getWidth(), g.h() / p.image->getHeight()));
        render->pushTransform(m);
        Render2D::ImageOptions options;
        options.cache = Render2D::ImageCache::Dynamic;
        render->drawImage(p.image, g.min, options);
        render->popTransform();
    }
}
