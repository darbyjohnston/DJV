// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/SoloLayout.h>
#include <djvUI/Widget.h>

#include <djvCore/ValueObserver.h>

class ISettingsWidget;

class SettingsWidget : public djv::UI::Widget
{
    DJV_NON_COPYABLE(SettingsWidget);

protected:
    void _init(const std::shared_ptr<djv::System::Context>&);
    SettingsWidget();

public:
    ~SettingsWidget() override;

    static std::shared_ptr<SettingsWidget> create(const std::shared_ptr<djv::System::Context>&);

    float getHeightForWidth(float) const override;

    void addChild(const std::shared_ptr<IObject>&) override;
    void removeChild(const std::shared_ptr<IObject>&) override;
    void clearChildren() override;

protected:
    void _initLayoutEvent(djv::System::Event::InitLayout&) override;
    void _preLayoutEvent(djv::System::Event::PreLayout&) override;
    void _layoutEvent(djv::System::Event::Layout&) override;

private:
    std::shared_ptr<djv::UI::Text::LabelSizeGroup> _sizeGroup;
    std::shared_ptr<djv::UI::VerticalLayout> _childLayout;
    std::shared_ptr<djv::UI::ScrollWidget> _scrollWidget;
};
