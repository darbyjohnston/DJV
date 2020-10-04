// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include "TimelineWidget.h"

#include <djvUI/Style.h>

#include <djvRender2D/Render.h>

using namespace djv;

void TimelineWidget::_init(const std::shared_ptr<System::Context>& context)
{
    Widget::_init(context);
}

TimelineWidget::TimelineWidget()
{}

TimelineWidget::~TimelineWidget()
{}

std::shared_ptr<TimelineWidget> TimelineWidget::create(const std::shared_ptr<System::Context>& context)
{
    auto out = std::shared_ptr<TimelineWidget>(new TimelineWidget);
    out->_init(context);
    return out;
}

void TimelineWidget::_preLayoutEvent(System::Event::PreLayout&)
{
    const auto& style = _getStyle();
    _setMinimumSize(glm::vec2(0.F, style->getMetric(UI::MetricsRole::Icon)));
}

void TimelineWidget::_paintEvent(System::Event::Paint&)
{
}
