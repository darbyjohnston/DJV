// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include "TimelineWidget.h"

#include <djvUI/Style.h>

#include <djvRender2D/Render.h>

#include <djvAV/TimeFunc.h>

using namespace djv;

struct TimelineWidget::Private
{
    Math::Rational speed;
    Math::Frame::Sequence sequence;
    Timestamp timestamp = 0;
    std::function<void(Timestamp)> callback;
    System::Event::PointerID pressedID = System::Event::invalidID;
    Timestamp pointerTimestamp = timestampInvalid;
};

void TimelineWidget::_init(const std::shared_ptr<System::Context>& context)
{
    Widget::_init(context);

    setPointerEnabled(true);
}

TimelineWidget::TimelineWidget() :
    _p(new Private)
{}

TimelineWidget::~TimelineWidget()
{}

std::shared_ptr<TimelineWidget> TimelineWidget::create(const std::shared_ptr<System::Context>& context)
{
    auto out = std::shared_ptr<TimelineWidget>(new TimelineWidget);
    out->_init(context);
    return out;
}

void TimelineWidget::setSpeed(const Math::Rational& value)
{
    DJV_PRIVATE_PTR();
    if (value == p.speed)
        return;
    p.speed = value;
    _redraw();
}

void TimelineWidget::setSequence(const Math::Frame::Sequence& value)
{
    DJV_PRIVATE_PTR();
    if (value == p.sequence)
        return;
    p.sequence = value;
    _redraw();
}

void TimelineWidget::setTime(Timestamp value)
{
    DJV_PRIVATE_PTR();
    if (value == p.timestamp)
        return;
    p.timestamp = value;
    _redraw();
}

void TimelineWidget::setCallback(const std::function<void(Timestamp)>& value)
{
   _p->callback = value;
}

void TimelineWidget::_preLayoutEvent(System::Event::PreLayout&)
{
    const auto& style = _getStyle();
    _setMinimumSize(glm::vec2(0.F, style->getMetric(UI::MetricsRole::Icon)));
}

void TimelineWidget::_paintEvent(System::Event::Paint&)
{
    DJV_PRIVATE_PTR();

    const auto& style = _getStyle();
    const float b = style->getMetric(UI::MetricsRole::Border);
    const Math::BBox2f& g = getGeometry();

    const float size0 = floorf(_timeToPos(0));
    const float size1 = ceilf(_timeToPos(AV::Time::scale(1, p.speed.swap(), timebase)));
    if (size1 > size0)
    {
        const auto& render = _getRender();
        render->setFillColor(style->getColor(UI::ColorRole::ForegroundDim));
        Timestamp ts = 0;
        float x = floorf(_timeToPos(ts));
        while (x < g.max.x)
        {
            render->drawRect(Math::BBox2f(x, g.min.y, size1 - size0, g.h()));
            ts += timebase.swap().toFloat();
            x = floorf(_timeToPos(ts));
        }

        x = floorf(_timeToPos(p.timestamp));
        render->setFillColor(style->getColor(UI::ColorRole::Foreground));
        render->drawRect(Math::BBox2f(x, g.min.y, size1 - size0, g.h()));
    }
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
    DJV_PRIVATE_PTR();
    event.accept();
    const auto& pointerInfo = event.getPointerInfo();
    if (pointerInfo.id == p.pressedID)
    {
        _pointerAction(pointerInfo);
    }
}

void TimelineWidget::_buttonPressEvent(System::Event::ButtonPress& event)
{
    DJV_PRIVATE_PTR();
    if (p.pressedID)
        return;
    event.accept();
    takeTextFocus();
    const auto& pointerInfo = event.getPointerInfo();
    p.pressedID = pointerInfo.id;
    _pointerAction(pointerInfo);
}

void TimelineWidget::_buttonReleaseEvent(System::Event::ButtonRelease& event)
{
    DJV_PRIVATE_PTR();
    const auto& pointerInfo = event.getPointerInfo();
    if (pointerInfo.id == p.pressedID)
    {
        event.accept();
        p.pressedID = System::Event::invalidID;
        p.pointerTimestamp = timestampInvalid;
        _redraw();
    }
}

float TimelineWidget::_timeToPos(Timestamp value) const
{
    const Math::BBox2f& g = getGeometry();
    const int64_t length = AV::Time::scale(_p->sequence.getFrameCount() - 1, _p->speed.swap(), timebase);
    const float out = length > 0 ? static_cast<float>(g.min.x + value / static_cast<float>(length) * g.w()) : g.min.x;
    return out;
}

Timestamp TimelineWidget::_posToTime(float value) const
{
    const Math::BBox2f& g = getGeometry();
    const int64_t length = AV::Time::scale(_p->sequence.getFrameCount() - 1, _p->speed.swap(), timebase);
    Math::Frame::Index out = length > 0 ? ((value - g.min.x) / g.w() * length) : 0;
    return out;
}

void TimelineWidget::_pointerAction(const djv::System::Event::PointerInfo& pointerInfo)
{
    DJV_PRIVATE_PTR();
    const Timestamp timestamp = _posToTime(pointerInfo.projectedPos.x);
    if (timestamp != p.pointerTimestamp)
    {
        p.pointerTimestamp = timestamp;
        if (p.callback)
        {
            p.callback(p.pointerTimestamp);
        }
        _redraw();
    }
}

