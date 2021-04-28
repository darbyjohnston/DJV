// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include "TimelineWidget.h"

#include <djvUI/Style.h>

#include <djvRender2D/Render.h>

#include <djvAV/Time.h>

using namespace djv;

struct TimelineWidget::Private
{
    Math::IntRational speed;
    int64_t duration = 0;
    IO::FrameInfo currentFrame;
    std::function<void(const IO::Timestamp&)> callback;
    System::Event::PointerID pressedID = System::Event::invalidID;
    IO::Timestamp pointerTimestamp = IO::timestampInvalid;
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

void TimelineWidget::setSpeed(const Math::IntRational& value)
{
    DJV_PRIVATE_PTR();
    if (value == p.speed)
        return;
    p.speed = value;
    _redraw();
}

void TimelineWidget::setDuration(int64_t value)
{
    DJV_PRIVATE_PTR();
    if (value == p.duration)
        return;
    p.duration = value;
    _redraw();
}

void TimelineWidget::setCurrentFrame(const IO::FrameInfo& value)
{
    DJV_PRIVATE_PTR();
    if (value == p.currentFrame)
        return;
    p.currentFrame = value;
    _redraw();
}

void TimelineWidget::setCallback(const std::function<void(const IO::Timestamp&)>& value)
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

    const float size0 = floorf(_timestampToPos(0));
    const float size1 = ceilf(_timestampToPos(AV::Time::scale(1, p.speed.swap(), IO::timebase)));
    if (size1 > size0)
    {
        const auto& render = _getRender();
        render->setFillColor(style->getColor(UI::ColorRole::ForegroundDim));

        /*for (const auto& i : p.frameInfo)
        {
            float x = floorf(_frameToPos(i));
            if (x >= g.min.x && x < g.max.x)
            {
                render->drawRect(Math::BBox2f(x, g.min.y, size1 - size0, g.h()));
            }
        }*/

        float x = floorf(_timestampToPos(p.currentFrame.timestamp));
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
        p.pointerTimestamp = IO::timestampInvalid;
        _redraw();
    }
}

float TimelineWidget::_timestampToPos(IO::Timestamp value) const
{
    DJV_PRIVATE_PTR();
    float out = 0.F;
    if (p.duration > 0)
    {
        const int64_t frame = AV::Time::scale(1, p.speed.swap(), IO::timebase);
        const float n = value / static_cast<float>(p.duration - frame);
        const Math::BBox2f& g = getGeometry();
        out = g.min.x + n * g.w();
    }
    return out;
}

IO::Timestamp TimelineWidget::_posToTimestamp(float value) const
{
    DJV_PRIVATE_PTR();
    IO::Timestamp out;
    if (p.duration > 0)
    {
        const Math::BBox2f& g = getGeometry();
        const float n = (value - g.min.x) / g.w();
        const int64_t frame = AV::Time::scale(1, p.speed.swap(), IO::timebase);
        out = static_cast<IO::Timestamp>(n * (p.duration - frame));
    }
    return out;
}

void TimelineWidget::_pointerAction(const djv::System::Event::PointerInfo& pointerInfo)
{
    DJV_PRIVATE_PTR();
    const IO::Timestamp timestamp = _posToTimestamp(pointerInfo.projectedPos.x);
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

