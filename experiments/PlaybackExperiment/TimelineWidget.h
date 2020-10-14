// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include "IO.h"

#include <djvUI/Widget.h>

#include <djvAV/Time.h>

#include <djvMath/FrameNumber.h>
#include <djvMath/Rational.h>

class TimelineWidget : public djv::UI::Widget
{
    DJV_NON_COPYABLE(TimelineWidget);

protected:
    void _init(const std::shared_ptr<djv::System::Context>&);
    TimelineWidget();

public:
    ~TimelineWidget() override;

    static std::shared_ptr<TimelineWidget> create(const std::shared_ptr<djv::System::Context>&);

    void setSpeed(const djv::Math::Rational&);
    void setSequence(const djv::Math::Frame::Sequence&);
    void setTime(Timestamp);

    void setCallback(const std::function<void(Timestamp)>&);

protected:
    void _preLayoutEvent(djv::System::Event::PreLayout&) override;
    void _paintEvent(djv::System::Event::Paint&) override;
    void _pointerEnterEvent(djv::System::Event::PointerEnter&) override;
    void _pointerLeaveEvent(djv::System::Event::PointerLeave&) override;
    void _pointerMoveEvent(djv::System::Event::PointerMove&) override;
    void _buttonPressEvent(djv::System::Event::ButtonPress&) override;
    void _buttonReleaseEvent(djv::System::Event::ButtonRelease&) override;

private:
    float _timeToPos(Timestamp) const;
    Timestamp _posToTime(float) const;

    DJV_PRIVATE();
};