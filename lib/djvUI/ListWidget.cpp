// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/ListWidget.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/ListButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct ListWidget::Private
        {
            std::vector<std::string> items;
            int currentItem = -1;
            std::shared_ptr<ButtonGroup> buttonGroup;
            std::shared_ptr<VerticalLayout> layout;
            std::shared_ptr<ScrollWidget> scrollWidget;
            std::function<void(int)> currentItemCallback;
        };

        void ListWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::ListWidget");
            setPointerEnabled(true);

            p.buttonGroup = ButtonGroup::create(ButtonType::Radio);

            p.layout = VerticalLayout::create(context);
            p.layout->setSpacing(MetricsRole::None);

            p.scrollWidget = ScrollWidget::create(ScrollType::Vertical, context);
            p.scrollWidget->addChild(p.layout);
            addChild(p.scrollWidget);

            _updateItems();
            _updateCurrentItem();

            auto weak = std::weak_ptr<ListWidget>(std::dynamic_pointer_cast<ListWidget>(shared_from_this()));
            p.buttonGroup->setRadioCallback(
                [weak](int value)
            {
                if (auto widget = weak.lock())
                {
                    if (widget->_p->currentItemCallback)
                    {
                        widget->_p->currentItemCallback(value);
                    }
                }
            });
        }

        ListWidget::ListWidget() :
            _p(new Private)
        {}

        ListWidget::~ListWidget()
        {}

        std::shared_ptr<ListWidget> ListWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ListWidget>(new ListWidget);
            out->_init(context);
            return out;
        }

        void ListWidget::setItems(const std::vector<std::string>& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.items)
                return;
            p.items = value;
            _updateItems();
        }

        void ListWidget::addItem(const std::string& value)
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                p.items.push_back(value);
                auto button = ListButton::create(context);
                button->setText(value);
                p.buttonGroup->addButton(button);
                p.layout->addChild(button);
            }
        }

        void ListWidget::clearItems()
        {
            DJV_PRIVATE_PTR();
            if (p.items.size())
            {
                p.items.clear();
                _updateItems();
            }
        }

        void ListWidget::setCurrentItem(int value)
        {
            DJV_PRIVATE_PTR();
            p.currentItem = value;
            _updateCurrentItem();
        }

        void ListWidget::setCurrentItemCallback(const std::function<void(int)>& value)
        {
            _p->currentItemCallback = value;
        }

        void ListWidget::setBorder(bool value)
        {
            _p->scrollWidget->setBorder(value);
        }

        void ListWidget::_preLayoutEvent(Event::PreLayout& event)
        {
            const auto& style = _getStyle();
            const float sa = style->getMetric(MetricsRole::ScrollArea);
            glm::vec2 size = _p->scrollWidget->getMinimumSize();
            size.x = std::max(size.x, sa);
            size.y = std::max(size.y, sa);
            _setMinimumSize(size);
        }

        void ListWidget::_layoutEvent(Event::Layout& event)
        {
            _p->scrollWidget->setGeometry(getGeometry());
        }

        void ListWidget::_keyPressEvent(Event::KeyPress& event)
        {
            Widget::_keyPressEvent(event);
            DJV_PRIVATE_PTR();
            if (!event.isAccepted())
            {
                const size_t size = p.items.size();
                if (size > 0)
                {
                    switch (event.getKey())
                    {
                    case GLFW_KEY_HOME:
                        event.accept();
                        if (p.currentItemCallback)
                        {
                            p.currentItemCallback(0);
                        }
                        break;
                    case GLFW_KEY_END:
                        event.accept();
                        if (p.currentItemCallback)
                        {
                            p.currentItemCallback(size - 1);
                        }
                        break;
                    case GLFW_KEY_UP:
                        event.accept();
                        if (p.currentItem > 0 && p.currentItemCallback)
                        {
                            p.currentItemCallback(p.currentItem - 1);
                        }
                        break;
                    case GLFW_KEY_DOWN:
                        event.accept();
                        if (p.currentItem < size - 1 && p.currentItemCallback)
                        {
                            p.currentItemCallback(p.currentItem + 1);
                        }
                        break;
                    }
                }
            }
        }

        void ListWidget::_updateItems()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                p.buttonGroup->clearButtons();
                p.layout->clearChildren();
                for (const auto& i : p.items)
                {
                    auto button = ListButton::create(context);
                    button->setText(i);
                    p.buttonGroup->addButton(button);
                    p.layout->addChild(button);
                }
            }
        }

        void ListWidget::_updateCurrentItem()
        {
            DJV_PRIVATE_PTR();
            if (p.currentItem >= 0 && p.currentItem < p.items.size())
            {
                p.buttonGroup->setChecked(p.currentItem, true);
            }
            else
            {
                p.buttonGroup->setChecked(-1, true);
            }
        }

    } // namespace UI
} // namespace djv
