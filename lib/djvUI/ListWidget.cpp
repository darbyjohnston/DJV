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
        bool ListItem::operator == (const ListItem& other) const
        {
            return icon == other.icon &&
                text == other.text &&
                rightsideText == other.rightsideText &&
                colorRole == other.colorRole;
        }

        struct ListWidget::Private
        {
            std::vector<ListItem> items;
            std::string filter;
            std::vector<ColorRole> alternateRowsRoles = { ColorRole::None, ColorRole::None };
            std::vector<std::shared_ptr<ListButton> > buttons;
            std::shared_ptr<ButtonGroup> buttonGroup;
            std::shared_ptr<VerticalLayout> layout;
            std::shared_ptr<ScrollWidget> scrollWidget;
        };

        void ListWidget::_init(ButtonType buttonType, const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();
            
            setClassName("djv::UI::ListWidget");
            setPointerEnabled(true);

            p.buttonGroup = ButtonGroup::create(buttonType);

            p.layout = VerticalLayout::create(context);
            p.layout->setSpacing(MetricsRole::None);

            p.scrollWidget = ScrollWidget::create(ScrollType::Vertical, context);
            p.scrollWidget->addChild(p.layout);
            addChild(p.scrollWidget);

            _updateItems();
        }

        ListWidget::ListWidget() :
            _p(new Private)
        {}

        ListWidget::~ListWidget()
        {}

        std::shared_ptr<ListWidget> ListWidget::create(ButtonType buttonType, const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ListWidget>(new ListWidget);
            out->_init(buttonType, context);
            return out;
        }

        void ListWidget::setItems(const std::vector<std::string>& value)
        {
            DJV_PRIVATE_PTR();
            std::vector<ListItem> tmp;
            for (const auto& i : value)
            {
                tmp.push_back({ i });
            }
            if (tmp == p.items)
                return;
            p.items = tmp;
            _updateItems();
            _updateFilter();
        }

        void ListWidget::setItems(const std::vector<ListItem>& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.items)
                return;
            p.items = value;
            _updateItems();
            _updateFilter();
        }

        void ListWidget::addItem(const std::string& value)
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                p.items.push_back({ value });
                auto button = ListButton::create(context);
                button->setText(value);
                p.buttons.push_back(button);
                p.buttonGroup->addButton(button);
                p.layout->addChild(button);
                _updateFilter();
            }
        }

        void ListWidget::addItem(const ListItem& value)
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                p.items.push_back(value);
                auto button = ListButton::create(context);
                button->setIcon(value.icon);
                button->setText(value.text);
                button->setRightsideText(value.rightsideText);
                p.buttons.push_back(button);
                p.buttonGroup->addButton(button);
                p.layout->addChild(button);
                _updateFilter();
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

        void ListWidget::setButtonType(ButtonType value)
        {
            _p->buttonGroup->setButtonType(value);
            _updateFilter();
        }

        int ListWidget::getChecked() const
        {
            return _p->buttonGroup->getChecked();
        }

        void ListWidget::setChecked(int index, bool value)
        {
            _p->buttonGroup->setChecked(index, value);
        }

        void ListWidget::setPushCallback(const std::function<void(int)>& value)
        {
            _p->buttonGroup->setPushCallback(value);
        }

        void ListWidget::setToggleCallback(const std::function<void(int, bool)>& value)
        {
            _p->buttonGroup->setToggleCallback(value);
        }

        void ListWidget::setRadioCallback(const std::function<void(int)>& value)
        {
            _p->buttonGroup->setRadioCallback(value);
        }

        void ListWidget::setExclusiveCallback(const std::function<void(int)>& value)
        {
            _p->buttonGroup->setExclusiveCallback(value);
        }

        void ListWidget::setFilter(const std::string& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.filter)
                return;
            p.filter = value;
            _updateFilter(Callback::Trigger);
        }

        void ListWidget::setBorder(bool value)
        {
            _p->scrollWidget->setBorder(value);
        }

        void ListWidget::setAlternateRowsRoles(ColorRole value0, ColorRole value1)
        {
            DJV_PRIVATE_PTR();
            if (value0 == p.alternateRowsRoles[0] && value1 == p.alternateRowsRoles[1])
                return;
            p.alternateRowsRoles = { value0, value1 };
            _updateFilter();
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
                        p.buttonGroup->setChecked(0);
                        break;
                    case GLFW_KEY_END:
                        event.accept();
                        p.buttonGroup->setChecked(size - 1);
                        break;
                    case GLFW_KEY_UP:
                        event.accept();
                        p.buttonGroup->setChecked(p.buttonGroup->getChecked() - 1);
                        break;
                    case GLFW_KEY_DOWN:
                        event.accept();
                        p.buttonGroup->setChecked(p.buttonGroup->getChecked() + 1);
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
                const int currentItem = p.buttonGroup->getChecked();
                const size_t buttonsSize = p.buttonGroup->getButtonCount();
                size_t i = 0;
                for (; i < buttonsSize && i < p.items.size(); ++i)
                {
                    const auto& item = p.items[i];
                    p.buttons[i]->setIcon(item.icon);
                    p.buttons[i]->setText(item.text);
                    p.buttons[i]->setRightsideText(item.rightsideText);
                }
                for (; i < p.items.size(); ++i)
                {
                    const auto& item = p.items[i];
                    auto button = ListButton::create(context);
                    button->setIcon(item.icon);
                    button->setText(item.text);
                    button->setRightsideText(item.rightsideText);
                    p.buttons.push_back(button);
                    p.buttonGroup->addButton(button);
                    p.layout->addChild(button);
                }
                for (; i < buttonsSize; ++i)
                {
                    auto button = p.buttons.begin() + p.items.size();
                    p.buttonGroup->removeButton(*button);
                    p.layout->removeChild(*button);
                    p.buttons.erase(button);
                }
                DJV_ASSERT(p.buttons.size() == p.items.size());
                DJV_ASSERT(p.buttons.size() == p.buttonGroup->getButtonCount());
                DJV_ASSERT(p.buttons.size() == p.layout->getChildWidgets().size());
                p.buttonGroup->setChecked(currentItem);
            }
        }

        void ListWidget::_updateFilter(Callback callback)
        {
            DJV_PRIVATE_PTR();
            const auto& buttons = p.buttonGroup->getButtons();
            DJV_ASSERT(buttons.size() == p.items.size());
            std::vector<int> indices;
            for (int i = 0; i < static_cast<int>(p.items.size()); ++i)
            {
                const auto& item = p.items[i];
                const bool match = String::match(item.text + " " + item.rightsideText, p.filter);
                buttons[i]->setVisible(match);
                if (match)
                {
                    if (item.colorRole != ColorRole::None)
                    {
                        buttons[i]->setBackgroundRole(item.colorRole);
                    }
                    else
                    {
                        buttons[i]->setBackgroundRole(
                            (0 == indices.size() % 2) ?
                            p.alternateRowsRoles[0] :
                            p.alternateRowsRoles[1]);
                    }
                    indices.push_back(i);
                }
            }
            switch (p.buttonGroup->getButtonType())
            {
            case ButtonType::Radio:
            {
                const int index = Math::closest(p.buttonGroup->getChecked(), indices);
                p.buttonGroup->setChecked(index >= 0 && index < indices.size() ? indices[index] : -1, true, callback);
                break;
            }
            default: break;
            }
        }

    } // namespace UI
} // namespace djv
