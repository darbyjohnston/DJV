// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include "SettingsWidget.h"

#include "ISettingsWidget.h"

#include <djvUI/ScrollWidget.h>

#include <djvCore/Context.h>

using namespace djv;

void SettingsWidget::_init(const std::shared_ptr<Core::Context>& context)
{
    Widget::_init(context);

    setBackgroundRole(UI::ColorRole::Background);

    _sizeGroup = UI::LabelSizeGroup::create();

    _childLayout = UI::VerticalLayout::create(context);
    _childLayout->setSpacing(UI::MetricsRole::None);
    _scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
    _scrollWidget->setBorder(false);
    _scrollWidget->addChild(_childLayout);
    Widget::addChild(_scrollWidget);
}

SettingsWidget::SettingsWidget()
{}

SettingsWidget::~SettingsWidget()
{}

std::shared_ptr<SettingsWidget> SettingsWidget::create(const std::shared_ptr<Core::Context>& context)
{
    auto out = std::shared_ptr<SettingsWidget>(new SettingsWidget);
    out->_init(context);
    return out;
}

float SettingsWidget::getHeightForWidth(float value) const
{
    return _scrollWidget->getHeightForWidth(value);
}

void SettingsWidget::addChild(const std::shared_ptr<IObject>& value)
{
    if (auto widget = std::dynamic_pointer_cast<ISettingsWidget>(value))
    {
        widget->setLabelSizeGroup(_sizeGroup);
    }
    _childLayout->addChild(value);
}

void SettingsWidget::removeChild(const std::shared_ptr<IObject>& value)
{
    if (auto widget = std::dynamic_pointer_cast<ISettingsWidget>(value))
    {
        widget->setLabelSizeGroup(std::weak_ptr<UI::LabelSizeGroup>());
    }
    _childLayout->removeChild(value);
}

void SettingsWidget::clearChildren()
{
    for (auto i : getChildWidgets())
    {
        if (auto widget = std::dynamic_pointer_cast<ISettingsWidget>(i))
        {
            widget->setLabelSizeGroup(std::weak_ptr<UI::LabelSizeGroup>());
        }
    }
    _childLayout->clearChildren();
}

void SettingsWidget::_initLayoutEvent(Core::Event::InitLayout& event)
{
    _sizeGroup->calcMinimumSize();
}

void SettingsWidget::_preLayoutEvent(Core::Event::PreLayout& event)
{
    _setMinimumSize(_scrollWidget->getMinimumSize());
}

void SettingsWidget::_layoutEvent(Core::Event::Layout&)
{
    _scrollWidget->setGeometry(getGeometry());
}
