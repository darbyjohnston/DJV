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

    setPointerEnabled(true);
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

void TimelineWidget::setSequence(const Math::Frame::Sequence& value)
{
    if (value == _sequence)
        return;
    _sequence = value;
    _redraw();
}

void TimelineWidget::setFrame(const Math::Frame::Index value)
{
    if (value == _frame)
        return;
    _frame = value;
    _redraw();
}

void TimelineWidget::setFrameCallback(const std::function<void(Math::Frame::Index)>& value)
{
    _frameCallback = value;
}

void TimelineWidget::_preLayoutEvent(System::Event::PreLayout&)
{
    const auto& style = _getStyle();
    _setMinimumSize(glm::vec2(0.F, style->getMetric(UI::MetricsRole::Icon)));
}

void TimelineWidget::_paintEvent(System::Event::Paint&)
{
    const auto& style = _getStyle();
    const float b = style->getMetric(UI::MetricsRole::Border);
    const Math::BBox2f& g = getGeometry();

    const float size0 = floorf(_frameToPos(0));
    const float size1 = ceilf(_frameToPos(1));
    const float x = floorf(_frameToPos(_frame));
    const auto& render = _getRender();
    render->setFillColor(style->getColor(UI::ColorRole::Foreground));
    render->drawRect(Math::BBox2f(x, g.min.y, size1 - size0, g.h()));
}

void TimelineWidget::_pointerEnterEvent(System::Event::PointerEnter& event)
{
    if (!event.isRejected())
    {
        event.accept();
        if (isEnabled(true))
        {
            _redraw();
        }
    }
}

void TimelineWidget::_pointerLeaveEvent(System::Event::PointerLeave& event)
{
    event.accept();
    if (isEnabled(true))
    {
        _redraw();
    }
}

void TimelineWidget::_pointerMoveEvent(System::Event::PointerMove& event)
{
    event.accept();
    const auto& pointerInfo = event.getPointerInfo();
    if (pointerInfo.id == _pressedID)
    {
        _frame = _posToFrame(pointerInfo.projectedPos.x);
        if (_frameCallback)
        {
            _frameCallback(_frame);
        }
    }
}

void TimelineWidget::_buttonPressEvent(System::Event::ButtonPress& event)
{
    if (_pressedID)
        return;
    event.accept();
    takeTextFocus();
    const auto& pointerInfo = event.getPointerInfo();
    _pressedID = pointerInfo.id;
    _pressedPos = pointerInfo.projectedPos;
    _redraw();
}

void TimelineWidget::_buttonReleaseEvent(System::Event::ButtonRelease& event)
{
    const auto& pointerInfo = event.getPointerInfo();
    if (pointerInfo.id == _pressedID)
    {
        event.accept();
        _pressedID = System::Event::invalidID;
        _redraw();
    }
}

float TimelineWidget::_frameToPos(Math::Frame::Index value) const
{
    const Math::BBox2f& g = getGeometry();
    const size_t frameCount = _sequence.getFrameCount();
    return frameCount > 0 ? (g.min.x + value / static_cast<float>(frameCount - 1) * g.w()) : g.min.x;
}

Math::Frame::Index TimelineWidget::_posToFrame(float value) const
{
    const Math::BBox2f& g = getGeometry();
    const size_t frameCount = _sequence.getFrameCount();
    return frameCount > 0 ? ((value - g.min.x) / g.w() * (frameCount - 1)) : 0;
}