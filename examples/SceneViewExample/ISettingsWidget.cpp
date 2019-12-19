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

#include "ISettingsWidget.h"

#include <djvUI/ScrollWidget.h>

#include <djvCore/Context.h>

using namespace djv;

void ISettingsWidget::_init(const std::shared_ptr<Core::Context>& context)
{
    Widget::_init(context);

    _title = Core::ValueSubject<std::string>::create();

    _titleLabel = UI::Label::create(context);
    _titleLabel->setTextHAlign(UI::TextHAlign::Left);
    _titleLabel->setMargin(UI::Layout::Margin(UI::MetricsRole::Margin, UI::MetricsRole::Margin, UI::MetricsRole::None, UI::MetricsRole::None));

    _backButton = UI::ToolButton::create(context);
    _backButton->setIcon("djvIconArrowLeft");
    _backButton->setInsideMargin(UI::MetricsRole::MarginSmall);
    
    _titleBar = UI::HorizontalLayout::create(context);
    _titleBar->setBackgroundRole(UI::ColorRole::BackgroundHeader);
    _titleBar->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
    _titleBar->addChild(_titleLabel);
    _titleBar->setStretch(_titleLabel, UI::RowStretch::Expand);
    _titleBar->addChild(_backButton);

    _childLayout = UI::VerticalLayout::create(context);
    _childLayout->setPointerEnabled(true);

    _layout = UI::VerticalLayout::create(context);
    _layout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
    _layout->addChild(_titleBar);
    _layout->addSeparator();
    auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
    scrollWidget->setShadowOverlay({ UI::Side::Top });
    scrollWidget->setBorder(false);
    scrollWidget->addChild(_childLayout);
    _layout->addChild(scrollWidget);
    _layout->setStretch(scrollWidget, UI::RowStretch::Expand);
    Widget::addChild(_layout);

    auto weak = std::weak_ptr<ISettingsWidget>(std::dynamic_pointer_cast<ISettingsWidget>(shared_from_this()));
    _backButton->setClickedCallback(
        [weak]
        {
            if (auto widget = weak.lock())
            {
                if (widget->_backCallback)
                {
                    widget->_backCallback();
                }
            }
        });
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
        _titleLabel->setText(text);
    }
}

void ISettingsWidget::setBackCallback(const std::function<void(void)>& value)
{
    _backCallback = value;
}

float ISettingsWidget::getHeightForWidth(float value) const
{
    return _layout->getHeightForWidth(value);
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
    _setMinimumSize(_layout->getMinimumSize());
}

void ISettingsWidget::_layoutEvent(Core::Event::Layout&)
{
    _layout->setGeometry(getGeometry());
}

void ISettingsWidget::_initEvent(Core::Event::Init&)
{
    _backButton->setTooltip(_getText(DJV_TEXT("Go to the previous page")));
}
