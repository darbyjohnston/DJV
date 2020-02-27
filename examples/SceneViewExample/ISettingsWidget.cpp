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
