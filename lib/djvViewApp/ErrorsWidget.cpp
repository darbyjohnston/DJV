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

#include <djvViewApp/ErrorsWidget.h>

#include <djvUI/CheckBox.h>
#include <djvUI/PushButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/StackLayout.h>
#include <djvUI/TextBlock.h>

#include <djvCore/Context.h>
#include <djvCore/LogSystem.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        namespace
        {
            class SizeWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(SizeWidget);

            protected:
                SizeWidget();

            public:
                static std::shared_ptr<SizeWidget> create(const std::shared_ptr<Context>&);

            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
            };

            SizeWidget::SizeWidget()
            {}

            std::shared_ptr<SizeWidget> SizeWidget::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<SizeWidget>(new SizeWidget);
                out->_init(context);
                return out;
            }

            void SizeWidget::_preLayoutEvent(Event::PreLayout&)
            {
                const auto& style = _getStyle();
                const float s = style->getMetric(UI::MetricsRole::Dialog);
                _setMinimumSize(glm::vec2(s, s * .5F));
            }

        } // namespace

        struct ErrorsWidget::Private
        {
            std::shared_ptr<UI::TextBlock> textBlock;
            std::shared_ptr<UI::CheckBox> popupCheckBox;
            std::shared_ptr<UI::PushButton> copyButton;
            std::shared_ptr<UI::PushButton> clearButton;
            std::function<void(bool)> popupCallback;
            std::function<void(void)> copyCallback;
            std::function<void(void)> clearCallback;
        };

        void ErrorsWidget::_init(const std::shared_ptr<Core::Context>& context)
        {
            MDIWidget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::ErrorsWidget");

            p.textBlock = UI::TextBlock::create(context);
            p.textBlock->setFontFamily(AV::Font::familyMono);
            p.textBlock->setFontSizeRole(UI::MetricsRole::FontSmall);
            p.textBlock->setMargin(UI::Layout::Margin(UI::MetricsRole::Margin));

            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->setShadowOverlay({ UI::Side::Top });
            scrollWidget->addChild(p.textBlock);

            p.popupCheckBox = UI::CheckBox::create(context);
            p.copyButton = UI::PushButton::create(context);
            p.clearButton = UI::PushButton::create(context);

            auto layout = UI::VerticalLayout::create(context);
            layout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            layout->addChild(scrollWidget);
            layout->setStretch(scrollWidget, UI::RowStretch::Expand);
            layout->addSeparator();
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
            hLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));
            hLayout->addChild(p.popupCheckBox);
            hLayout->addExpander();
            hLayout->addChild(p.copyButton);
            hLayout->addChild(p.clearButton);
            layout->addChild(hLayout);
            auto stackLayout = UI::StackLayout::create(context);
            stackLayout->setBackgroundRole(UI::ColorRole::Background);
            stackLayout->addChild(SizeWidget::create(context));
            stackLayout->addChild(layout);
            addChild(stackLayout);

            auto weak = std::weak_ptr<ErrorsWidget>(std::dynamic_pointer_cast<ErrorsWidget>(shared_from_this()));
            p.popupCheckBox->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (widget->_p->popupCallback)
                        {
                            widget->_p->popupCallback(value);
                        }
                    }
                });

            p.copyButton->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        if (widget->_p->copyCallback)
                        {
                            widget->_p->copyCallback();
                        }
                    }
                });

            p.clearButton->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        if (widget->_p->clearCallback)
                        {
                            widget->_p->clearCallback();
                        }
                    }
                });
        }

        ErrorsWidget::ErrorsWidget() :
            _p(new Private)
        {}

        ErrorsWidget::~ErrorsWidget()
        {}

        std::shared_ptr<ErrorsWidget> ErrorsWidget::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<ErrorsWidget>(new ErrorsWidget);
            out->_init(context);
            return out;
        }

        void ErrorsWidget::_initEvent(Event::Init & event)
        {
            MDIWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            setTitle(_getText(DJV_TEXT("Errors")));
            p.popupCheckBox->setText(_getText(DJV_TEXT("Popup")));
            p.popupCheckBox->setTooltip(_getText(DJV_TEXT("Errors widget popup tooltip")));
            p.copyButton->setText(_getText(DJV_TEXT("Copy")));
            p.copyButton->setTooltip(_getText(DJV_TEXT("Errors widget copy tooltip")));
            p.clearButton->setText(_getText(DJV_TEXT("Clear")));
            p.clearButton->setTooltip(_getText(DJV_TEXT("Errors widget clear tooltip")));
        }

        void ErrorsWidget::setText(const std::string& value)
        {
            _p->textBlock->setText(value);
        }

        void ErrorsWidget::setPopup(bool value)
        {
            _p->popupCheckBox->setChecked(value);
        }

        void ErrorsWidget::setPopupCallback(const std::function<void(bool)>& value)
        {
            _p->popupCallback = value;
        }

        void ErrorsWidget::setCopyCallback(const std::function<void(void)>& value)
        {
            _p->copyCallback = value;
        }

        void ErrorsWidget::setClearCallback(const std::function<void(void)>& value)
        {
            _p->clearCallback = value;
        }

    } // namespace ViewApp
} // namespace djv

