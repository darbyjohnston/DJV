// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/PopupMenu.h>

#include <djvUI/Action.h>
#include <djvUI/Menu.h>
#include <djvUI/MenuButton.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct PopupMenu::Private
        {
            std::shared_ptr<Menu> menu;
            std::shared_ptr<Button::Menu> button;
            std::shared_ptr<Observer::ValueObserver<std::string> > iconObserver;
            std::shared_ptr<Observer::ValueObserver<std::string> > textObserver;
            std::shared_ptr<Observer::ValueObserver<bool> > closeObserver;
        };

        void PopupMenu::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();
            
            setClassName("djv::UI::PopupMenu");

            p.button = Button::Menu::create(MenuButtonStyle::Flat, context);
            addChild(p.button);

            auto weak = std::weak_ptr<PopupMenu>(std::dynamic_pointer_cast<PopupMenu>(shared_from_this()));
            p.button->setOpenCallback(
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (widget->_p->menu)
                        {
                            if (value)
                            {
                                widget->_p->menu->popup(widget->_p->button);
                            }
                            else
                            {
                                widget->_p->menu->close();
                            }
                        }
                    }
                });
        }

        PopupMenu::PopupMenu() :
            _p(new Private)
        {}

        PopupMenu::~PopupMenu()
        {}

        std::shared_ptr<PopupMenu> PopupMenu::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<PopupMenu>(new PopupMenu);
            out->_init(context);
            return out;
        }

        void PopupMenu::setMenu(const std::shared_ptr<Menu>& menu)
        {
            DJV_PRIVATE_PTR();
            if (p.menu)
            {
                if (auto parent = p.menu->getParent().lock())
                {
                    parent->removeChild(p.menu);
                }
                p.menu->setCloseCallback(nullptr);
                p.iconObserver.reset();
                p.textObserver.reset();
            }

            p.menu = menu;

            if (p.menu)
            {
                addChild(p.menu);

                auto weak = std::weak_ptr<PopupMenu>(std::dynamic_pointer_cast<PopupMenu>(shared_from_this()));
                menu->setCloseCallback(
                    [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->button->setOpen(false);
                    }
                });

                p.iconObserver = Observer::ValueObserver<std::string>::create(
                    p.menu->observeIcon(),
                    [weak](const std::string& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->button->setIcon(value);
                    }
                });

                p.textObserver = Observer::ValueObserver<std::string>::create(
                    p.menu->observeText(),
                    [weak](const std::string& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->button->setText(value);
                    }
                });
            }
        }

        MetricsRole PopupMenu::getInsideMargin() const
        {
            return _p->button->getInsideMargin();
        }

        void PopupMenu::setInsideMargin(MetricsRole value)
        {
            _p->button->setInsideMargin(value);
        }

        void PopupMenu::_preLayoutEvent(System::Event::PreLayout& event)
        {
            _setMinimumSize(_p->button->getMinimumSize());
        }

        void PopupMenu::_layoutEvent(System::Event::Layout& event)
        {
            _p->button->setGeometry(getGeometry());
        }

    } // namespace UI
} // namespace djv
