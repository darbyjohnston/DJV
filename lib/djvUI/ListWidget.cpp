// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/ListWidget.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/ListButton.h>
#include <djvUI/RowLayout.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        ListItem::ListItem()
        {}
        
        ListItem::ListItem(const std::string& text) :
            text(text)
        {}
        
        bool ListItem::operator == (const ListItem& other) const
        {
            return icon == other.icon &&
                rightIcon == other.rightIcon &&
                text == other.text &&
                rightText == other.rightText &&
                colorRole == other.colorRole &&
                tooltip == other.tooltip;
        }

        struct ListWidget::Private
        {
            std::vector<ListItem> items;
            std::string filter;
            std::vector<ColorRole> alternateRowsRoles = { ColorRole::None, ColorRole::None };
            std::vector<std::shared_ptr<ListButton> > buttons;
            std::shared_ptr<ButtonGroup> buttonGroup;
            std::shared_ptr<VerticalLayout> layout;

            static void initButton(const std::shared_ptr<ListButton>&, const ListItem&);
        };

        void ListWidget::_init(ButtonType buttonType, const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();
            
            setClassName("djv::UI::ListWidget");

            p.buttonGroup = ButtonGroup::create(buttonType);

            p.layout = VerticalLayout::create(context);
            p.layout->setSpacing(MetricsRole::None);
            addChild(p.layout);
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

        void ListWidget::setItems(const std::vector<std::string>& value, int checked)
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
            _updateItems(checked);
            _updateFilter();
        }

        void ListWidget::setItems(const std::vector<ListItem>& value, int checked)
        {
            DJV_PRIVATE_PTR();
            if (value == p.items && checked == _p->buttonGroup->getChecked())
                return;
            p.items = value;
            _updateItems(checked);
            _updateFilter();
        }

        void ListWidget::addItem(const std::string& value)
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                p.items.push_back({ value });
                auto button = ListButton::create(context);
                p.initButton(button, ListItem(value));
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
                p.initButton(button, value);
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
                _updateItems(-1);
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
            _updateFilter();
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
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ListWidget::_layoutEvent(Event::Layout& event)
        {
            _p->layout->setGeometry(getGeometry());
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

        void ListWidget::_updateItems(int checked)
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                p.buttonGroup->clearButtons();
                const size_t buttonsSize = p.buttons.size();
                size_t i = 0;
                for (; i < p.items.size() && i < buttonsSize; ++i)
                {
                    p.initButton(p.buttons[i], p.items[i]);
                }
                for (; i < p.items.size(); ++i)
                {
                    auto button = ListButton::create(context);
                    p.initButton(button, p.items[i]);
                    p.buttons.push_back(button);
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
                DJV_ASSERT(p.buttons.size() == p.layout->getChildWidgets().size());
                std::vector<std::shared_ptr<Button::IButton> > iButtons;
                for (const auto& i : p.buttons)
                {
                    iButtons.push_back(i);
                }
                p.buttonGroup->setButtons(iButtons, checked);
            }
        }

        void ListWidget::_updateFilter()
        {
            DJV_PRIVATE_PTR();
            const auto& buttons = p.buttonGroup->getButtons();
            DJV_ASSERT(buttons.size() == p.items.size());
            std::vector<int> indices;
            for (int i = 0; i < static_cast<int>(p.items.size()); ++i)
            {
                const auto& item = p.items[i];
                const bool match = String::match(item.text + " " + item.rightText, p.filter);
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
        }
        
        void ListWidget::Private::initButton(const std::shared_ptr<ListButton>& button, const ListItem& item)
        {
            button->setIcon(item.icon);
            button->setRightIcon(item.rightIcon);
            button->setText(item.text);
            button->setRightText(item.rightText);
            button->setBackgroundRole(item.colorRole);
            button->setTooltip(item.tooltip);
        }

    } // namespace UI
} // namespace djv
