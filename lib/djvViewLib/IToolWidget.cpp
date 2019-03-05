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

#include <djvViewLib/IToolWidget.h>

#include <djvUI/Border.h>
#include <djvUI/FlatButton.h>
#include <djvUI/Label.h>
#include <djvUI/MDICanvas.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct IToolWidget::Private
        {
            std::shared_ptr<UI::Label> titleLabel;
            std::shared_ptr<UI::HorizontalLayout> titleBar;
            std::shared_ptr<UI::FlatButton> closeButton;
            std::shared_ptr<UI::VerticalLayout> childLayout;
            std::shared_ptr<UI::VerticalLayout> layout;
            std::shared_ptr<UI::Border> border;
            std::function<void(void)> closeCallback;
        };

        void IToolWidget::_init(Context * context)
        {
            IWidget::_init(context);

            DJV_PRIVATE_PTR();

            p.titleLabel = UI::Label::create(context);
            p.titleLabel->setFontSizeRole(UI::MetricsRole::FontHeader);
            p.titleLabel->setTextHAlign(UI::TextHAlign::Left);
            p.titleLabel->setMargin(UI::MetricsRole::Margin);

            p.closeButton = UI::FlatButton::create(context);
            p.closeButton->setIcon("djvIconClose");
            p.closeButton->setInsideMargin(UI::MetricsRole::MarginSmall);

            p.titleBar = UI::HorizontalLayout::create(context);
            p.titleBar->setSpacing(UI::MetricsRole::None);
            p.titleBar->addChild(p.titleLabel);
            p.titleBar->setStretch(p.titleLabel, UI::RowStretch::Expand);
            p.titleBar->addChild(p.closeButton);

            p.childLayout = UI::VerticalLayout::create(context);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setBackgroundRole(UI::ColorRole::Background);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->addChild(p.titleBar);
            p.layout->addSeparator();
            p.layout->addChild(p.childLayout);
            p.layout->setStretch(p.childLayout, UI::RowStretch::Expand);

            p.border = UI::Border::create(context);
            p.border->setMargin(UI::MetricsRole::Handle);
            p.border->addChild(p.layout);
            IWidget::addChild(p.border);

            auto weak = std::weak_ptr<IToolWidget>(std::dynamic_pointer_cast<IToolWidget>(shared_from_this()));
            p.closeButton->setClickedCallback(
                [weak]
            {
                if (auto widget = weak.lock())
                {
                    widget->close();
                }
            });
        }

        IToolWidget::IToolWidget() :
            _p(new Private)
        {}

        IToolWidget::~IToolWidget()
        {}

        const std::string & IToolWidget::getTitle() const
        {
            return _p->titleLabel->getText();
        }

        void IToolWidget::setTitle(const std::string & text)
        {
            _p->titleLabel->setText(text);
        }

        void IToolWidget::close()
        {
            DJV_PRIVATE_PTR();
            hide();
            if (p.closeCallback)
            {
                p.closeCallback();
            }
        }

        void IToolWidget::setCloseCallback(const std::function<void(void)> & value)
        {
            _p->closeCallback = value;
        }

        float IToolWidget::getHeightForWidth(float value) const
        {
            return _p->border->getHeightForWidth(value);
        }

        void IToolWidget::addChild(const std::shared_ptr<IObject>& value)
        {
            _p->childLayout->addChild(value);
            if (auto widget = std::dynamic_pointer_cast<Widget>(value))
            {
                _p->childLayout->setStretch(widget, UI::RowStretch::Expand);
            }
        }

        void IToolWidget::removeChild(const std::shared_ptr<IObject>& value)
        {
            _p->childLayout->removeChild(value);
        }

        void IToolWidget::_preLayoutEvent(Event::PreLayout& event)
        {
            _setMinimumSize(_p->border->getMinimumSize());
        }

        void IToolWidget::_layoutEvent(Event::Layout&)
        {
            _p->border->setGeometry(getGeometry());
        }

        void IToolWidget::_localeEvent(Event::Locale &)
        {
            DJV_PRIVATE_PTR();
            p.closeButton->setTooltip(_getText(DJV_TEXT("Close tooltip")));
        }

    } // namespace ViewLib
} // namespace djv

