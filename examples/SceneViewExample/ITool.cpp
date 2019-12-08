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

#include "ITool.h"

#include <djvCore/Context.h>

using namespace djv;

void ITool::_init(const std::shared_ptr<Core::Context>& context)
{
    IWidget::_init(context);

    _titleLabel = UI::Label::create(context);
    _titleLabel->setTextHAlign(UI::TextHAlign::Left);
    _titleLabel->setMargin(UI::Layout::Margin(UI::MetricsRole::Margin, UI::MetricsRole::Margin, UI::MetricsRole::None, UI::MetricsRole::None));

    _closeButton = UI::ToolButton::create(context);
    _closeButton->setIcon("djvIconClose");
    _closeButton->setInsideMargin(UI::MetricsRole::MarginSmall);

    _titleBar = UI::HorizontalLayout::create(context);
    _titleBar->setBackgroundRole(UI::ColorRole::BackgroundHeader);
    _titleBar->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
    _titleBar->addChild(_titleLabel);
    _titleBar->setStretch(_titleLabel, UI::RowStretch::Expand);
    _titleBar->addChild(_closeButton);

    _childLayout = UI::VerticalLayout::create(context);
    _childLayout->setPointerEnabled(true);

    auto layout = UI::VerticalLayout::create(context);
    layout->setBackgroundRole(UI::ColorRole::OverlayLight);
    layout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
    layout->addChild(_titleBar);
    layout->addSeparator();
    layout->addChild(_childLayout);
    layout->setStretch(_childLayout, UI::RowStretch::Expand);

    _layout = UI::VerticalLayout::create(context);
    _layout->setMargin(UI::Layout::Margin(UI::MetricsRole::Shadow));
    _layout->addChild(layout);
    _layout->setStretch(layout, UI::RowStretch::Expand);
    IWidget::addChild(_layout);

    auto weak = std::weak_ptr<ITool>(std::dynamic_pointer_cast<ITool>(shared_from_this()));
    _closeButton->setClickedCallback(
        [weak]
        {
            if (auto widget = weak.lock())
            {
                widget->close();
            }
        });
}

ITool::ITool()
{}

ITool::~ITool()
{}

const std::string& ITool::getTitle() const
{
    return _titleLabel->getText();
}

void ITool::setTitle(const std::string& text)
{
    _titleLabel->setText(text);
}

void ITool::close()
{
    hide();
    if (_closeCallback)
    {
        _closeCallback();
    }
}

void ITool::setCloseCallback(const std::function<void(void)>& value)
{
    _closeCallback = value;
}

float ITool::getHeightForWidth(float value) const
{
    return _layout->getHeightForWidth(value);
}

void ITool::addChild(const std::shared_ptr<IObject>& value)
{
    _childLayout->addChild(value);
    if (auto widget = std::dynamic_pointer_cast<Widget>(value))
    {
        _childLayout->setStretch(widget, UI::RowStretch::Expand);
    }
}

void ITool::removeChild(const std::shared_ptr<IObject>& value)
{
    _childLayout->removeChild(value);
}

void ITool::clearChildren()
{
    _childLayout->clearChildren();
}

std::map<UI::MDI::Handle, std::vector<Core::BBox2f> > ITool::_getHandles() const
{
    auto out = IWidget::_getHandles();
    out[UI::MDI::Handle::Move][0] = _titleBar->getGeometry();
    return out;
}

void ITool::_setActiveWidget(bool value)
{
    IWidget::_setActiveWidget(value);
    _titleLabel->setTextColorRole(value ? UI::ColorRole::Foreground : UI::ColorRole::ForegroundDim);
    _closeButton->setForegroundColorRole(value ? UI::ColorRole::Foreground : UI::ColorRole::ForegroundDim);
}

void ITool::_preLayoutEvent(Core::Event::PreLayout& event)
{
    _setMinimumSize(_layout->getMinimumSize());
}

void ITool::_layoutEvent(Core::Event::Layout&)
{
    _layout->setGeometry(getGeometry());
}

void ITool::_initEvent(Core::Event::Init&)
{
}
