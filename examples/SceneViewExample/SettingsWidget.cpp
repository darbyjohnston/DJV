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

    setBackgroundRole(UI::ColorRole::OverlayLight);

    _buttonGroup = UI::ButtonGroup::create(UI::ButtonType::Push);
    _buttonLayout = UI::VerticalLayout::create(context);
    _buttonLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
    _scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
    _scrollWidget->setBorder(false);
    _scrollWidget->addChild(_buttonLayout);

    _layout = UI::SoloLayout::create(context);
    _layout->addChild(_scrollWidget);
    _layout->setCurrentWidget(_scrollWidget);
    Widget::addChild(_layout);

    auto weak = std::weak_ptr<SettingsWidget>(std::dynamic_pointer_cast<SettingsWidget>(shared_from_this()));
    _buttonGroup->setPushCallback(
        [weak](int index)
        {
            if (auto widget = weak.lock())
            {
                widget->_layout->setCurrentIndex(index + 1);
            }
        });
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
    return _layout->getHeightForWidth(value);
}

void SettingsWidget::addChild(const std::shared_ptr<IObject>& value)
{
    if (auto context = getContext().lock())
    {
        if (auto widget = std::dynamic_pointer_cast<ISettingsWidget>(value))
        {
            _layout->addChild(value);
            
            auto button = UI::ListButton::create(context);
            button->setInsideMargin(UI::Layout::Margin(UI::MetricsRole::Margin));
            _buttons[widget] = button;
            _buttonGroup->addButton(button);
            _buttonLayout->addChild(button);
            _titleObservers[widget] = Core::ValueObserver<std::string>::create(
                widget->observeTitle(),
                [button](const std::string& value)
                {
                    button->setText(value);
                });

            auto weak = std::weak_ptr<SettingsWidget>(std::dynamic_pointer_cast<SettingsWidget>(shared_from_this()));
            widget->setBackCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_layout->setCurrentWidget(widget->_scrollWidget);
                    }
                });
        }
    }
}

void SettingsWidget::removeChild(const std::shared_ptr<IObject>& value)
{
    _layout->removeChild(value);
    auto i = _buttons.find(std::dynamic_pointer_cast<ISettingsWidget>(value));
    if (i != _buttons.end())
    {
        _buttonLayout->removeChild(i->second);
    }
}

void SettingsWidget::clearChildren()
{
    _layout->clearChildren();
    _buttonLayout->clearChildren();
}

void SettingsWidget::_preLayoutEvent(Core::Event::PreLayout& event)
{
    _setMinimumSize(_layout->getMinimumSize());
}

void SettingsWidget::_layoutEvent(Core::Event::Layout&)
{
    _layout->setGeometry(getGeometry());
}
