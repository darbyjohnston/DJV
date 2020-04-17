// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include "ISettingsWidget.h"

#include <djvCore/Context.h>

using namespace djv;

void ISettingsWidget::_init(const std::shared_ptr<Core::Context>& context)
{
    Widget::_init(context);

    _title = Core::ValueSubject<std::string>::create();
    
    _childLayout = UI::VerticalLayout::create(context);
    //_childLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::Margin));

    _bellows = UI::Bellows::create(context);
    _bellows->close();
    _bellows->addChild(_childLayout);
    Widget::addChild(_bellows);
}

ISettingsWidget::ISettingsWidget()
{}

ISettingsWidget::~ISettingsWidget()
{}

std::shared_ptr<djv::Core::IValueSubject<std::string> > ISettingsWidget::observeTitle() const
{
    return _title;
}

void ISettingsWidget::setTitle(const std::string& text)
{
    if (_title->setIfChanged(text))
    {
        _bellows->setText(text);
    }
}

float ISettingsWidget::getHeightForWidth(float value) const
{
    return _bellows->getHeightForWidth(value);
}

void ISettingsWidget::addChild(const std::shared_ptr<IObject>& value)
{
    _childLayout->addChild(value);
    if (auto widget = std::dynamic_pointer_cast<Widget>(value))
    {
        _childLayout->setStretch(widget, UI::RowStretch::Expand);
    }
}

void ISettingsWidget::removeChild(const std::shared_ptr<IObject>& value)
{
    _childLayout->removeChild(value);
}

void ISettingsWidget::clearChildren()
{
    _childLayout->clearChildren();
}

void ISettingsWidget::_preLayoutEvent(Core::Event::PreLayout& event)
{
    _setMinimumSize(_bellows->getMinimumSize());
}

void ISettingsWidget::_layoutEvent(Core::Event::Layout&)
{
    _bellows->setGeometry(getGeometry());
}
