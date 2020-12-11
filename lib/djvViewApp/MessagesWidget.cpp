// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/MessagesWidget.h>

#include <djvUI/CheckBox.h>
#include <djvUI/RowLayout.h>
#include <djvUI/TextBlock.h>
#include <djvUI/ToolButton.h>

#include <djvSystem/Context.h>
#include <djvSystem/LogSystem.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct MessagesWidget::Private
        {
            std::shared_ptr<UI::Text::Block> textBlock;
        };

        void MessagesWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::MessagesWidget");

            p.textBlock = UI::Text::Block::create(context);
            p.textBlock->setFontFamily(Render2D::Font::familyMono);
            p.textBlock->setFontSizeRole(UI::MetricsRole::FontSmall);
            p.textBlock->setMargin(UI::MetricsRole::Margin);
            addChild(p.textBlock);
        }

        MessagesWidget::MessagesWidget() :
            _p(new Private)
        {}

        MessagesWidget::~MessagesWidget()
        {}

        std::shared_ptr<MessagesWidget> MessagesWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<MessagesWidget>(new MessagesWidget);
            out->_init(context);
            return out;
        }

        void MessagesWidget::setText(const std::string& value)
        {
            _p->textBlock->setText(value);
        }

        void MessagesWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->textBlock->getMinimumSize());
        }

        void MessagesWidget::_layoutEvent(System::Event::Layout&)
        {
            _p->textBlock->setGeometry(getGeometry());
        }

        struct MessagesFooterWidget::Private
        {
            std::shared_ptr<UI::CheckBox> popupCheckBox;
            std::shared_ptr<UI::ToolButton> copyButton;
            std::shared_ptr<UI::ToolButton> clearButton;
            std::shared_ptr<UI::HorizontalLayout> layout;
            std::function<void(bool)> popupCallback;
            std::function<void(void)> copyCallback;
            std::function<void(void)> clearCallback;
        };

        void MessagesFooterWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::MessagesFooterWidget");

            p.popupCheckBox = UI::CheckBox::create(context);
            p.copyButton = UI::ToolButton::create(context);
            p.copyButton->setIcon("djvIconShare");
            p.clearButton = UI::ToolButton::create(context);
            p.clearButton->setIcon("djvIconClear");

            p.layout = UI::HorizontalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->addChild(p.popupCheckBox);
            p.layout->addExpander();
            p.layout->addChild(p.copyButton);
            p.layout->addChild(p.clearButton);
            addChild(p.layout);

            auto weak = std::weak_ptr<MessagesFooterWidget>(std::dynamic_pointer_cast<MessagesFooterWidget>(shared_from_this()));
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

        MessagesFooterWidget::MessagesFooterWidget() :
            _p(new Private)
        {}

        MessagesFooterWidget::~MessagesFooterWidget()
        {}

        std::shared_ptr<MessagesFooterWidget> MessagesFooterWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<MessagesFooterWidget>(new MessagesFooterWidget);
            out->_init(context);
            return out;
        }

        void MessagesFooterWidget::setPopup(bool value)
        {
            _p->popupCheckBox->setChecked(value);
        }

        void MessagesFooterWidget::setPopupCallback(const std::function<void(bool)>& value)
        {
            _p->popupCallback = value;
        }

        void MessagesFooterWidget::setCopyCallback(const std::function<void(void)>& value)
        {
            _p->copyCallback = value;
        }

        void MessagesFooterWidget::setClearCallback(const std::function<void(void)>& value)
        {
            _p->clearCallback = value;
        }

        void MessagesFooterWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void MessagesFooterWidget::_layoutEvent(System::Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void MessagesFooterWidget::_initEvent(System::Event::Init& event)
        {
            Widget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.popupCheckBox->setText(_getText(DJV_TEXT("widget_messages_popup")));
                p.popupCheckBox->setTooltip(_getText(DJV_TEXT("widget_messages_popup_tooltip")));
                p.copyButton->setTooltip(_getText(DJV_TEXT("widget_messages_copy_tooltip")));
                p.clearButton->setTooltip(_getText(DJV_TEXT("widget_messages_clear_tooltip")));
            }
        }

    } // namespace ViewApp
} // namespace djv

