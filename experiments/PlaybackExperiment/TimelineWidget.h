// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

#include <djvMath/FrameNumber.h>

class TimelineWidget : public djv::UI::Widget
{
    DJV_NON_COPYABLE(TimelineWidget);

protected:
    void _init(const std::shared_ptr<djv::System::Context>&);
    TimelineWidget();

public:
    ~TimelineWidget() override;

    static std::shared_ptr<TimelineWidget> create(const std::shared_ptr<djv::System::Context>&);

    void setSequence(const djv::Math::Frame::Sequence&);

    void setFrame(const djv::Math::Frame::Index);
    void setFrameCallback(const std::function<void(djv::Math::Frame::Index)>&);

protected:
    void _preLayoutEvent(djv::System::Event::PreLayout&) override;
    void _paintEvent(djv::System::Event::Paint&) override;
    void _pointerEnterEvent(djv::System::Event::PointerEnter&) override;
    void _pointerLeaveEvent(djv::System::Event::PointerLeave&) override;
    void _pointerMoveEvent(djv::System::Event::PointerMove&) override;
    void _buttonPressEvent(djv::System::Event::ButtonPress&) override;
    void _buttonReleaseEvent(djv::System::Event::ButtonRelease&) override;

private:
    float _frameToPos(djv::Math::Frame::Index) const;
    djv::Math::Frame::Index _posToFrame(float) const;

    DJV_PRIVATE();
};