// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

class TimelineWidget : public djv::UI::Widget
{
    DJV_NON_COPYABLE(TimelineWidget);

protected:
    void _init(const std::shared_ptr<djv::System::Context>&);
    TimelineWidget();

public:
    ~TimelineWidget() override;

    static std::shared_ptr<TimelineWidget> create(const std::shared_ptr<djv::System::Context>&);

protected:
    void _preLayoutEvent(djv::System::Event::PreLayout&) override;
    void _paintEvent(djv::System::Event::Paint&) override;
};