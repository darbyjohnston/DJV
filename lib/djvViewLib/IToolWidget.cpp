//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>
#include <djvUI/ToolButton.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct IToolWidget::Private
        {
            std::shared_ptr<UI::Label> titleLabel;
            std::shared_ptr<UI::Layout::Horizontal> titleBar;
            std::shared_ptr<UI::Layout::Vertical> childLayout;
            std::shared_ptr<UI::Layout::Vertical> layout;
            std::shared_ptr<UI::Layout::Border> border;
            std::function<void(void)> closedCallback;
        };

        void IToolWidget::_init(Context * context)
        {
            IWidget::_init(context);

            DJV_PRIVATE_PTR();
            p.titleLabel = UI::Label::create(context);
            p.titleLabel->setTextHAlign(UI::TextHAlign::Left);
            p.titleLabel->setMargin(UI::Style::MetricsRole::Margin);

            auto closeButton = UI::Button::Tool::create(context);
            closeButton->setIcon("djvIconCloseSmall");

            p.titleBar = UI::Layout::Horizontal::create(context);
            p.titleBar->addWidget(p.titleLabel, UI::Layout::RowStretch::Expand);
            auto hLayout = UI::Layout::Horizontal::create(context);
            hLayout->setSpacing(UI::Style::MetricsRole::None);
            hLayout->addWidget(closeButton);
            p.titleBar->addWidget(hLayout);

            p.childLayout = UI::Layout::Vertical::create(context);

            p.layout = UI::Layout::Vertical::create(context);
            p.layout->setBackgroundRole(UI::Style::ColorRole::Background);
            p.layout->setSpacing(UI::Style::MetricsRole::None);
            p.layout->addWidget(p.titleBar);
            p.layout->addSeparator();
            p.layout->addWidget(p.childLayout);

            p.border = UI::Layout::Border::create(context);
            p.border->setMargin(UI::Style::MetricsRole::Handle);
            p.border->addWidget(p.layout);
            IContainer::addWidget(p.border);

            auto weak = std::weak_ptr<IToolWidget>(std::dynamic_pointer_cast<IToolWidget>(shared_from_this()));
            closeButton->setClickedCallback(
                [weak]
            {
                if (auto widget = weak.lock())
                {
                    if (widget->_p->closedCallback)
                    {
                        widget->_p->closedCallback();
                    }
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

        void IToolWidget::setClosedCallback(const std::function<void(void)> & value)
        {
            _p->closedCallback = value;
        }

        void IToolWidget::addWidget(const std::shared_ptr<Widget>& widget)
        {
            _p->childLayout->addWidget(widget);
        }

        void IToolWidget::removeWidget(const std::shared_ptr<Widget>& widget)
        {
            _p->childLayout->addWidget(widget);
        }

        void IToolWidget::clearWidgets()
        {
            _p->childLayout->clearWidgets();
        }

        float IToolWidget::getHeightForWidth(float value) const
        {
            return _p->border->getHeightForWidth(value);
        }

        void IToolWidget::_preLayoutEvent(Event::PreLayout& event)
        {
            _setMinimumSize(_p->border->getMinimumSize());
        }

        void IToolWidget::_layoutEvent(Event::Layout&)
        {
            _p->border->setGeometry(getGeometry());
        }

    } // namespace ViewLib
} // namespace djv

