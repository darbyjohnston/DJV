// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Bellows.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>

#include <djvCore/ValueObserver.h>

class ISettingsWidget : public djv::UI::Widget
{
    DJV_NON_COPYABLE(ISettingsWidget);

protected:
    void _init(const std::shared_ptr<djv::Core::Context>&);
    ISettingsWidget();

public:
    virtual ~ISettingsWidget();

    std::shared_ptr<djv::Core::IValueSubject<std::string> > observeTitle() const;
    void setTitle(const std::string&);

    virtual void setSizeGroup(const std::weak_ptr<djv::UI::LabelSizeGroup>&) {}

    float getHeightForWidth(float) const override;

    void addChild(const std::shared_ptr<IObject>&) override;
    void removeChild(const std::shared_ptr<IObject>&) override;
    void clearChildren() override;

protected:
    void _preLayoutEvent(djv::Core::Event::PreLayout&) override;
    void _layoutEvent(djv::Core::Event::Layout&) override;

private:
    std::weak_ptr<djv::UI::LabelSizeGroup> _sizeGroup;
    std::shared_ptr<djv::Core::ValueSubject<std::string> > _title;
    std::shared_ptr<djv::UI::VerticalLayout> _childLayout;
    std::shared_ptr<djv::UI::Layout::Bellows> _bellows;
};
