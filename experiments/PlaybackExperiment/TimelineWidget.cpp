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
    std::vector<IO::FrameInfo> frameInfo;
    IO::FrameInfo currentFrame;
    std::function<void(const IO::FrameInfo&)> callback;
    System::Event::PointerID pressedID = System::Event::invalidID;
    IO::FrameInfo pointerFrameInfo;
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

void TimelineWidget::setFrameInfo(const std::vector<IO::FrameInfo>& value)
{
    DJV_PRIVATE_PTR();
    if (value == p.frameInfo)
        return;
    p.frameInfo = value;
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

void TimelineWidget::setCallback(const std::function<void(const IO::FrameInfo&)>& value)
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

    const float size0 = floorf(_frameToPos(IO::FrameInfo()));
    const float size1 = ceilf(_frameToPos(IO::FrameInfo(AV::Time::scale(1, p.speed.swap(), IO::timebase))));
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

        const auto i = std::find(p.frameInfo.begin(), p.frameInfo.end(), p.currentFrame);
        if (i != p.frameInfo.end())
        {
            float x = floorf(_frameToPos(*i));
            render->setFillColor(style->getColor(UI::ColorRole::Foreground));
            render->drawRect(Math::BBox2f(x, g.min.y, size1 - size0, g.h()));
        }
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
        p.pointerFrameInfo.timestamp = IO::timestampInvalid;
        _redraw();
    }
}

float TimelineWidget::_frameToPos(const IO::FrameInfo& value) const
{
    float out = 0.F;
    const size_t size = _p->frameInfo.size();
    if (size > 0)
    {
        const Math::BBox2f& g = getGeometry();
        const auto i = std::find(_p->frameInfo.begin(), _p->frameInfo.end(), value);
        const float n = (i - _p->frameInfo.begin()) / static_cast<float>(size - 1);
        out = g.min.x + n * g.w();
    }
    return out;
}

IO::FrameInfo TimelineWidget::_posToFrame(float value) const
{
    IO::FrameInfo out;
    const size_t size = _p->frameInfo.size();
    if (size > 0)
    {
        const Math::BBox2f& g = getGeometry();
        const float n = (value - g.min.x) / g.w();
        if (n >= 0.F && n <= 1.F)
        {
            out = _p->frameInfo[n * (size - 1)];
        }
    }
    return out;
}

void TimelineWidget::_pointerAction(const djv::System::Event::PointerInfo& pointerInfo)
{
    DJV_PRIVATE_PTR();
    const IO::FrameInfo frameInfo = _posToFrame(pointerInfo.projectedPos.x);
    if (frameInfo != p.pointerFrameInfo)
    {
        p.pointerFrameInfo = frameInfo;
        if (p.callback)
        {
            p.callback(p.pointerFrameInfo);
        }
        _redraw();
    }
}

