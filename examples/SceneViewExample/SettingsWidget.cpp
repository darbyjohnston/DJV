//------------------------------------------------------------------------------
// Copyright (c) 2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

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
    _childLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
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
        widget->setSizeGroup(_sizeGroup);
    }
    _childLayout->addChild(value);
}

void SettingsWidget::removeChild(const std::shared_ptr<IObject>& value)
{
    if (auto widget = std::dynamic_pointer_cast<ISettingsWidget>(value))
    {
        widget->setSizeGroup(std::weak_ptr<UI::LabelSizeGroup>());
    }
    _childLayout->removeChild(value);
}

void SettingsWidget::clearChildren()
{
    for (auto i : getChildWidgets())
    {
        if (auto widget = std::dynamic_pointer_cast<ISettingsWidget>(i))
        {
            widget->setSizeGroup(std::weak_ptr<UI::LabelSizeGroup>());
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
