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

        float MessagesWidget::getHeightForWidth(float value) const
        {
            return _p->textBlock->getHeightForWidth(value);
        }

        void MessagesWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->textBlock->getMinimumSize());
        }

        void MessagesWidget::_layoutEvent(System::Event::Layout&)
        {
            _p->textBlock->setGeometry(getGeometry());
        }

        struct MessagesToolBar::Private
        {
            std::shared_ptr<UI::CheckBox> popupCheckBox;
            std::shared_ptr<UI::ToolButton> copyButton;
            std::shared_ptr<UI::ToolButton> clearButton;
            std::function<void(bool)> popupCallback;
            std::function<void(void)> copyCallback;
            std::function<void(void)> clearCallback;
        };

        void MessagesToolBar::_init(const std::shared_ptr<System::Context>& context)
        {
            ToolBar::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::MessagesToolBar");

            p.popupCheckBox = UI::CheckBox::create(context);
            p.copyButton = UI::ToolButton::create(context);
            p.copyButton->setIcon("djvIconShare");
            p.clearButton = UI::ToolButton::create(context);
            p.clearButton->setIcon("djvIconClear");

            addExpander();
            addChild(p.popupCheckBox);
            addChild(p.copyButton);
            addChild(p.clearButton);

            auto weak = std::weak_ptr<MessagesToolBar>(std::dynamic_pointer_cast<MessagesToolBar>(shared_from_this()));
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

        MessagesToolBar::MessagesToolBar() :
            _p(new Private)
        {}

        MessagesToolBar::~MessagesToolBar()
        {}

        std::shared_ptr<MessagesToolBar> MessagesToolBar::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<MessagesToolBar>(new MessagesToolBar);
            out->_init(context);
            return out;
        }

        void MessagesToolBar::setPopup(bool value)
        {
            _p->popupCheckBox->setChecked(value);
        }

        void MessagesToolBar::setPopupCallback(const std::function<void(bool)>& value)
        {
            _p->popupCallback = value;
        }

        void MessagesToolBar::setCopyCallback(const std::function<void(void)>& value)
        {
            _p->copyCallback = value;
        }

        void MessagesToolBar::setClearCallback(const std::function<void(void)>& value)
        {
            _p->clearCallback = value;
        }

        void MessagesToolBar::_initEvent(System::Event::Init& event)
        {
            ToolBar::_initEvent(event);
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

