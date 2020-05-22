// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/MessagesWidget.h>

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

        struct MessagesWidget::Private
        {
            std::shared_ptr<UI::TextBlock> textBlock;
            std::shared_ptr<UI::CheckBox> popupCheckBox;
            std::shared_ptr<UI::PushButton> copyButton;
            std::shared_ptr<UI::PushButton> clearButton;
            std::function<void(bool)> popupCallback;
            std::function<void(void)> copyCallback;
            std::function<void(void)> clearCallback;
        };

        void MessagesWidget::_init(const std::shared_ptr<Core::Context>& context)
        {
            MDIWidget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::MessagesWidget");

            p.textBlock = UI::TextBlock::create(context);
            p.textBlock->setFontFamily(AV::Font::familyMono);
            p.textBlock->setFontSizeRole(UI::MetricsRole::FontSmall);
            p.textBlock->setMargin(UI::MetricsRole::Margin);

            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->setShadowOverlay({ UI::Side::Top });
            scrollWidget->addChild(p.textBlock);

            p.popupCheckBox = UI::CheckBox::create(context);
            p.copyButton = UI::PushButton::create(context);
            p.clearButton = UI::PushButton::create(context);

            auto layout = UI::VerticalLayout::create(context);
            layout->setSpacing(UI::MetricsRole::None);
            layout->addChild(scrollWidget);
            layout->setStretch(scrollWidget, UI::RowStretch::Expand);
            layout->addSeparator();
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setMargin(UI::MetricsRole::MarginSmall);
            hLayout->setSpacing(UI::MetricsRole::SpacingSmall);
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

            auto weak = std::weak_ptr<MessagesWidget>(std::dynamic_pointer_cast<MessagesWidget>(shared_from_this()));
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

        MessagesWidget::MessagesWidget() :
            _p(new Private)
        {}

        MessagesWidget::~MessagesWidget()
        {}

        std::shared_ptr<MessagesWidget> MessagesWidget::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<MessagesWidget>(new MessagesWidget);
            out->_init(context);
            return out;
        }

        void MessagesWidget::_initEvent(Event::Init & event)
        {
            MDIWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            setTitle(_getText(DJV_TEXT("widget_messages")));
            p.popupCheckBox->setText(_getText(DJV_TEXT("widget_messages_popup")));
            p.popupCheckBox->setTooltip(_getText(DJV_TEXT("widget_messages_popup_tooltip")));
            p.copyButton->setText(_getText(DJV_TEXT("widget_messages_copy")));
            p.copyButton->setTooltip(_getText(DJV_TEXT("widget_messages_copy_tooltip")));
            p.clearButton->setText(_getText(DJV_TEXT("widget_messages_clear")));
            p.clearButton->setTooltip(_getText(DJV_TEXT("widget_messages_clear_tooltip")));
        }

        void MessagesWidget::setText(const std::string& value)
        {
            _p->textBlock->setText(value);
        }

        void MessagesWidget::setPopup(bool value)
        {
            _p->popupCheckBox->setChecked(value);
        }

        void MessagesWidget::setPopupCallback(const std::function<void(bool)>& value)
        {
            _p->popupCallback = value;
        }

        void MessagesWidget::setCopyCallback(const std::function<void(void)>& value)
        {
            _p->copyCallback = value;
        }

        void MessagesWidget::setClearCallback(const std::function<void(void)>& value)
        {
            _p->clearCallback = value;
        }

    } // namespace ViewApp
} // namespace djv

