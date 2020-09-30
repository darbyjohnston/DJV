// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/ComboBox.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/Menu.h>
#include <djvUI/MenuButton.h>
#include <djvUI/RowLayout.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct ComboBox::Private
        {
            std::vector<std::shared_ptr<Action> > items;
            int currentItem = -1;
            std::shared_ptr<ActionGroup> actionGroup;
            std::shared_ptr<Menu> menu;
            std::shared_ptr<Button::Menu> button;
            std::function<void(int)> callback;
            std::shared_ptr<Observer::Value<bool> > closeObserver;
        };

        void ComboBox::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::ComboBox");
            setHAlign(HAlign::Fill);
            setVAlign(VAlign::Center);

            p.actionGroup = ActionGroup::create(ButtonType::Radio);

            p.menu = Menu::create(context);
            p.menu->setMinimumSizeRole(MetricsRole::None);
            addChild(p.menu);

            p.button = Button::Menu::create(MenuButtonStyle::ComboBox, context);
            p.button->setPopupIcon("djvIconPopupMenu");
            p.button->setTextFocusEnabled(true);
            p.button->setBackgroundRole(ColorRole::Button);
            addChild(p.button);

            _currentItemUpdate();

            auto weak = std::weak_ptr<ComboBox>(std::dynamic_pointer_cast<ComboBox>(shared_from_this()));
            p.actionGroup->setRadioCallback(
                [weak](int value)
            {
                if (auto widget = weak.lock())
                {
                    widget->close();
                    widget->setCurrentItem(value);
                    if (widget->_p->callback)
                    {
                        widget->_p->callback(value);
                    }
                }
            });

            p.menu->setCloseCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->button->setOpen(false);
                        widget->_p->button->takeTextFocus();
                    }
                });

            p.button->setOpenCallback(
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (value)
                        {
                            widget->open();
                        }
                        else
                        {
                            widget->close();
                        }
                    }
                });
        }

        ComboBox::ComboBox() :
            _p(new Private)
        {}

        ComboBox::~ComboBox()
        {}

        std::shared_ptr<ComboBox> ComboBox::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ComboBox>(new ComboBox);
            out->_init(context);
            return out;
        }

        void ComboBox::setItems(const std::vector<std::string>& value)
        {
            DJV_PRIVATE_PTR();
            p.items.clear();
            for (const auto& i : value)
            {
                auto action = Action::create();
                action->setText(i);
                p.items.push_back(action);
            }
            p.currentItem = p.items.size() ? 0 : -1;
            _itemsUpdate();
            _currentItemUpdate();
        }

        void ComboBox::setItems(const std::vector<std::shared_ptr<Action> >& value)
        {
            DJV_PRIVATE_PTR();
            p.items = value;
            p.currentItem = p.items.size() ? 0 : -1;
            _itemsUpdate();
            _currentItemUpdate();
        }

        void ComboBox::clearItems()
        {
            DJV_PRIVATE_PTR();
            if (!p.items.empty())
            {
                p.items.clear();
                _itemsUpdate();
                _currentItemUpdate();
            }
        }

        int ComboBox::getCurrentItem() const
        {
            return _p->currentItem;
        }

        void ComboBox::setCurrentItem(int value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.currentItem)
                return;
            p.currentItem = value;
            _p->actionGroup->setChecked(value);
            _currentItemUpdate();
        }

        void ComboBox::firstItem()
        {
            DJV_PRIVATE_PTR();
            const size_t size = p.items.size();
            if (size)
            {
                setCurrentItem(0);
            }
        }

        void ComboBox::lastItem()
        {
            DJV_PRIVATE_PTR();
            const size_t size = p.items.size();
            if (size)
            {
                setCurrentItem(size - 1);
            }
        }

        void ComboBox::prevItem()
        {
            DJV_PRIVATE_PTR();
            const size_t size = p.items.size();
            if (size && p.currentItem > 0)
            {
                setCurrentItem(p.currentItem - 1);
            }
        }

        void ComboBox::nextItem()
        {
            DJV_PRIVATE_PTR();
            const size_t size = p.items.size();
            if (size && p.currentItem >= 0 && p.currentItem < static_cast<int>(size) - 1)
            {
                setCurrentItem(p.currentItem + 1);
            }
        }

        bool ComboBox::isOpen() const
        {
            return _p->menu->isOpen();
        }

        void ComboBox::open()
        {
            DJV_PRIVATE_PTR();
            if (!p.items.empty())
            {
                p.menu->popup(p.button);
            }
        }

        void ComboBox::close()
        {
            DJV_PRIVATE_PTR();
            if (p.menu->isOpen())
            {
                p.menu->close();
                takeTextFocus();
            }
        }

        void ComboBox::setFont(int index, const std::string& font)
        {
            DJV_PRIVATE_PTR();
            const auto& actions = p.actionGroup->getActions();
            if (index >= 0 && index < actions.size())
            {
                actions[index]->setFont(font);
            }
        }

        void ComboBox::setFontSizeRole(MetricsRole value)
        {
            DJV_PRIVATE_PTR();
            p.button->setFontSizeRole(value);
        }

        void ComboBox::setCallback(const std::function<void(int)>& value)
        {
            _p->callback = value;
        }

        std::shared_ptr<Widget> ComboBox::getFocusWidget()
        {
            return _p->button->getFocusWidget();
        }

        void ComboBox::_preLayoutEvent(System::Event::PreLayout& event)
        {
            _setMinimumSize(_p->button->getMinimumSize());
        }

        void ComboBox::_layoutEvent(System::Event::Layout& event)
        {
            _p->button->setGeometry(getGeometry());
        }

        void ComboBox::_keyPressEvent(System::Event::KeyPress& event)
        {
            Widget::_keyPressEvent(event);
            DJV_PRIVATE_PTR();
            if (!event.isAccepted() && hasTextFocus())
            {
                const int currentItem = p.currentItem;
                switch (event.getKey())
                {
                case GLFW_KEY_HOME:
                    event.accept();
                    firstItem();
                    break;
                case GLFW_KEY_END:
                    event.accept();
                    lastItem();
                    break;
                case GLFW_KEY_UP:
                    event.accept();
                    prevItem();
                    break;
                case GLFW_KEY_DOWN:
                    event.accept();
                    nextItem();
                    break;
                default: break;
                }
                if (currentItem != p.currentItem && p.callback)
                {
                    p.callback(p.currentItem);
                }
            }
        }

        void ComboBox::_itemsUpdate()
        {
            DJV_PRIVATE_PTR();
            p.menu->clearActions();
            for (const auto& i : p.items)
            {
                p.menu->addAction(i);
            }
            p.actionGroup->setActions(p.items);
            p.actionGroup->setChecked(p.currentItem);
        }

        void ComboBox::_currentItemUpdate()
        {
            DJV_PRIVATE_PTR();
            p.actionGroup->setChecked(p.currentItem);
            if (p.currentItem >= 0 && p.currentItem < static_cast<int>(p.items.size()))
            {
                p.button->setText(p.items[p.currentItem]->observeText()->get());
                p.button->setEnabled(true);
            }
            else
            {
                p.button->setText(std::string());
                p.button->setEnabled(false);
            }
        }

    } // namespace UI
} // namespace djv
