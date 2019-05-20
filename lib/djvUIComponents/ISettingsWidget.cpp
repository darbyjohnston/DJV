//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvUIComponents/ISettingsWidget.h>

#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/ToolButton.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct ISettingsWidget::Private
        {
            std::shared_ptr<Label> titleLabel;
            std::shared_ptr<VerticalLayout> childLayout;
            std::shared_ptr<VerticalLayout> layout;
            std::function<void(void)> backCallback;
        };

        void ISettingsWidget::_init(Context * context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::ISettingsWidget");

            auto backButton = ToolButton::create(context);
            backButton->setIcon("djvIconArrowLeft");
            p.titleLabel = Label::create(context);
            p.titleLabel->setTextHAlign(TextHAlign::Left);
            p.titleLabel->setFontSizeRole(MetricsRole::FontHeader);
            p.titleLabel->setMargin(MetricsRole::Margin);

            p.layout = VerticalLayout::create(context);
            p.layout->setSpacing(MetricsRole::None);
            auto hLayout = HorizontalLayout::create(context);
            hLayout->setSpacing(MetricsRole::None);
            hLayout->setBackgroundRole(ColorRole::BackgroundHeader);
            hLayout->addChild(backButton);
            hLayout->addChild(p.titleLabel);
            hLayout->setStretch(p.titleLabel, RowStretch::Expand);
            p.layout->addChild(hLayout);
            p.childLayout = VerticalLayout::create(context);
            p.childLayout->setMargin(MetricsRole::MarginLarge);
            p.childLayout->setSpacing(MetricsRole::SpacingLarge);
            auto scrollWidget = ScrollWidget::create(ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->setShadowOverlay({ UI::Side::Top });
            scrollWidget->addChild(p.childLayout);
            p.layout->addChild(scrollWidget);
            p.layout->setStretch(scrollWidget, RowStretch::Expand);
            Widget::addChild(p.layout);

            auto weak = std::weak_ptr<ISettingsWidget>(std::dynamic_pointer_cast<ISettingsWidget>(shared_from_this()));
            backButton->setClickedCallback(
                [weak]
            {
                if (auto widget = weak.lock())
                {
                    if (widget->_p->backCallback)
                    {
                        widget->_p->backCallback();
                    }
                }
            });
        }

        ISettingsWidget::ISettingsWidget() :
            _p(new Private)
        {}

        ISettingsWidget::~ISettingsWidget()
        {}

        void ISettingsWidget::setBackCallback(const std::function<void(void)>& value)
        {
            _p->backCallback = value;
        }

        float ISettingsWidget::getHeightForWidth(float value) const
        {
            return _p->layout->getHeightForWidth(value);
        }

        void ISettingsWidget::addChild(const std::shared_ptr<IObject> & value)
        {
            _p->childLayout->addChild(value);
        }

        void ISettingsWidget::removeChild(const std::shared_ptr<IObject> & value)
        {
            _p->childLayout->removeChild(value);
        }

        void ISettingsWidget::_preLayoutEvent(Event::PreLayout & event)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ISettingsWidget::_layoutEvent(Event::Layout &)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ISettingsWidget::_localeEvent(Event::Locale&)
        {
            DJV_PRIVATE_PTR();
            p.titleLabel->setText(_getText(getSettingsName()));
        }

    } // namespace UI
} // namespace djv

