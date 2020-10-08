// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

class Media;

class DrawerWidget : public djv::UI::Widget
{
    DJV_NON_COPYABLE(DrawerWidget);

protected:
    void _init(
        const std::shared_ptr<Media>&,
        const std::shared_ptr<djv::System::Context>&);
    DrawerWidget();

public:
    ~DrawerWidget() override;

    static std::shared_ptr<DrawerWidget> create(
        const std::shared_ptr<Media>&,
        const std::shared_ptr<djv::System::Context>&);

protected:
    void _preLayoutEvent(djv::System::Event::PreLayout&) override;
    void _layoutEvent(djv::System::Event::Layout&) override;

private:
    DJV_PRIVATE();
};