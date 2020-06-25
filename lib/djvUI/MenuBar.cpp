// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/MenuBar.h>

#include <djvUI/Action.h>
#include <djvUI/Label.h>
#include <djvUI/Menu.h>
#include <djvUI/MenuButton.h>
#include <djvUI/Overlay.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Shortcut.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct MenuBar::Private
        {
            std::vector<std::shared_ptr<Menu> > menus;
            std::shared_ptr<HorizontalLayout> menuLayout;
            std::map<Side, std::shared_ptr<HorizontalLayout> > widgetLayout;
            std::shared_ptr<HorizontalLayout> layout;
            std::shared_ptr<Button::Menu> menuOpen;
            std::map<std::shared_ptr<Menu>, std::shared_ptr<Button::Menu> > menusToButtons;
            std::map<std::shared_ptr<Button::Menu>, std::shared_ptr<Menu> > buttonsToMenus;
            std::map<std::shared_ptr<Menu>, std::shared_ptr<ValueObserver<std::string> > > iconObservers;
            std::map<std::shared_ptr<Menu>, std::shared_ptr<ValueObserver<std::string> > > textObservers;

            void closeMenus();
        };

        void MenuBar::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);            
            DJV_PRIVATE_PTR();
            
            setClassName("djv::UI::MenuBar");

            p.menuLayout = HorizontalLayout::create(context);
            p.menuLayout->setSpacing(MetricsRole::None);

            p.widgetLayout[Side::Left] = HorizontalLayout::create(context);
            p.widgetLayout[Side::Left]->setSpacing(MetricsRole::None);
            p.widgetLayout[Side::Right] = HorizontalLayout::create(context);
            p.widgetLayout[Side::Right]->setSpacing(MetricsRole::None);

            p.layout = HorizontalLayout::create(context);
            p.layout->setSpacing(MetricsRole::None);
            p.layout->addChild(p.widgetLayout[Side::Left]);
            p.layout->addChild(p.menuLayout);
            p.layout->addChild(p.widgetLayout[Side::Right]);
            p.layout->setStretch(p.widgetLayout[Side::Right], RowStretch::Expand);
            Widget::addChild(p.layout);
        }

        MenuBar::MenuBar() :
            _p(new Private)
        {}

        MenuBar::~MenuBar()
        {}

        std::shared_ptr<MenuBar> MenuBar::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<MenuBar>(new MenuBar);
            out->_init(context);
            return out;
        }

        void MenuBar::setSide(const std::shared_ptr<Widget>& widget, Side side)
        {
            const auto i = _p->widgetLayout.find(side);
            if (i != _p->widgetLayout.end())
            {
                i->second->addChild(widget);
            }
        }

        void MenuBar::setStretch(const std::shared_ptr<Widget>& widget, RowStretch value, Side side)
        {
            const auto i = _p->widgetLayout.find(side);
            if (i != _p->widgetLayout.end())
            {
                i->second->setStretch(widget, value);
            }
        }

        void MenuBar::addSeparator(Side side)
        {
            const auto i = _p->widgetLayout.find(side);
            if (i != _p->widgetLayout.end())
            {
                i->second->addSeparator();
            }
        }

        void MenuBar::addSpacer(Side side, MetricsRole role)
        {
            const auto i = _p->widgetLayout.find(side);
            if (i != _p->widgetLayout.end())
            {
                i->second->addSpacer(role);
            }
        }

        void MenuBar::addExpander(Side side)
        {
            const auto i = _p->widgetLayout.find(side);
            if (i != _p->widgetLayout.end())
            {
                i->second->addExpander();
            }
        }

        float MenuBar::getHeightForWidth(float value) const
        {
            return _p->layout->getHeightForWidth(value);
        }

        void MenuBar::addChild(const std::shared_ptr<IObject>& value)
        {
            Widget::addChild(value);
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                if (auto menu = std::dynamic_pointer_cast<Menu>(value))
                {
                    menu->close();
                    menu->setMinimumSizeRole(MetricsRole::None);
                    p.menus.push_back(menu);

                    auto button = Button::Menu::create(Button::MenuStyle::Flat, context);
                    button->setTextFocusEnabled(false);

                    p.menuLayout->addChild(button);

                    p.menusToButtons[menu] = button;
                    p.buttonsToMenus[button] = menu;

                    auto weak = std::weak_ptr<MenuBar>(std::dynamic_pointer_cast<MenuBar>(shared_from_this()));
                    auto menuWeak = std::weak_ptr<Menu>(std::dynamic_pointer_cast<Menu>(menu));
                    auto contextWeak = std::weak_ptr<Context>(context);
                    button->setOpenCallback(
                        [weak, menuWeak](bool open)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_openCallback(open, menuWeak);
                            }
                        });

                    menu->setCloseCallback(
                        [weak, menuWeak]
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_closeCallback(menuWeak);
                            }
                        });

                    p.iconObservers[menu] = ValueObserver<std::string>::create(
                        menu->observeIcon(),
                        [button](const std::string& value)
                        {
                            button->setIcon(value);
                        });

                    p.textObservers[menu] = ValueObserver<std::string>::create(
                        menu->observeText(),
                        [button](const std::string& value)
                        {
                            button->setText(value);
                        });
                }
                else if (auto widget = std::dynamic_pointer_cast<Widget>(value))
                {
                    p.widgetLayout[Side::Right]->addChild(widget);
                }
            }
        }

        void MenuBar::removeChild(const std::shared_ptr<IObject>& value)
        {
            Widget::removeChild(value);
            DJV_PRIVATE_PTR();
            if (auto menu = std::dynamic_pointer_cast<Menu>(value))
            {
                {
                    const auto i = p.menusToButtons.find(menu);
                    if (i != p.menusToButtons.end())
                    {
                        const auto j = p.buttonsToMenus.find(i->second);
                        if (j != p.buttonsToMenus.end())
                        {
                            p.buttonsToMenus.erase(j);
                        }
                        p.menuLayout->removeChild(i->second);
                        p.menusToButtons.erase(i);
                    }
                }
                menu->setCloseCallback(nullptr);
                {
                    const auto i = p.iconObservers.find(menu);
                    if (i != p.iconObservers.end())
                    {
                        p.iconObservers.erase(i);
                    }
                }
                {
                    const auto i = p.textObservers.find(menu);
                    if (i != p.textObservers.end())
                    {
                        p.textObservers.erase(i);
                    }
                }
            }
            else if (auto widget = std::dynamic_pointer_cast<Widget>(value))
            {
                p.widgetLayout[Side::Left]->removeChild(widget);
                p.widgetLayout[Side::Right]->removeChild(widget);
            }
        }

        void MenuBar::_preLayoutEvent(Event::PreLayout& event)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void MenuBar::_layoutEvent(Event::Layout& event)
        {
            const BBox2f& g = getGeometry();
            const auto& style = _getStyle();
            _p->layout->setGeometry(getMargin().bbox(g, style));
        }

        bool MenuBar::_eventFilter(const std::shared_ptr<IObject>&, Event::Event& event)
        {
            DJV_PRIVATE_PTR();
            switch (event.getEventType())
            {
            case Event::Type::PointerMove:
            {
                Event::PointerMove& moveEvent = static_cast<Event::PointerMove&>(event);
                for (const auto& i : p.buttonsToMenus)
                {
                    if (i.first->getGeometry().contains(moveEvent.getPointerInfo().projectedPos))
                    {
                        if (p.menuOpen && p.menuOpen != i.first)
                        {
                            p.menuOpen->setOpen(false);
                            p.closeMenus();
                            auto popup = i.second->popup(i.first, p.menuLayout);
                            popup->installEventFilter(shared_from_this());
                            p.menuOpen = i.first;
                            p.menuOpen->setOpen(true);
                        }
                        break;
                    }
                }
                break;
            }
            default: break;
            }
            return false;
        }
        
        void MenuBar::_openCallback(bool open, const std::weak_ptr<Menu>& menuWeak)
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                if (auto menu = menuWeak.lock())
                {
                    p.closeMenus();
                    if (open)
                    {
                        const auto i = p.menusToButtons.find(menu);
                        if (i != p.menusToButtons.end())
                        {
                            auto popup = menu->popup(i->second, p.menuLayout);
                            popup->installEventFilter(shared_from_this());
                            p.menuOpen = i->second;
                        }
                    }
                }
            }
        }
        
        void MenuBar::_closeCallback(const std::weak_ptr<Menu>& menuWeak)
        {
            DJV_PRIVATE_PTR();
            if (auto menu = menuWeak.lock())
            {
                const auto i = p.menusToButtons.find(menu);
                if (i != p.menusToButtons.end())
                {
                    i->second->setOpen(false);
                }
            }
            p.menuOpen.reset();
        }

        void MenuBar::Private::closeMenus()
        {
            for (auto& i : menus)
            {
                i->close();
            }
            menuOpen.reset();
        }

    } // namespace UI
} // namespace djv
