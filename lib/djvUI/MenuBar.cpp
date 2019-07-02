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

#include <djvUI/MenuBar.h>

#include <djvUI/Action.h>
#include <djvUI/Label.h>
#include <djvUI/Menu.h>
#include <djvUI/MenuButton.h>
#include <djvUI/Overlay.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Shortcut.h>
#include <djvUI/Window.h>

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
            std::shared_ptr<ValueObserver<bool> > closeObserver;

            void closeMenus();
        };

        void MenuBar::_init(Context * context)
        {
            Widget::_init(context);
            
            DJV_PRIVATE_PTR();
            setClassName("djv::UI::MenuBar");

            auto closeAction = Action::create();
            closeAction->setShortcut(GLFW_KEY_ESCAPE);
            addAction(closeAction);

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

            auto weak = std::weak_ptr<MenuBar>(std::dynamic_pointer_cast<MenuBar>(shared_from_this()));
            p.closeObserver = ValueObserver<bool>::create(
                closeAction->observeClicked(),
                [weak](bool value)
            {
                if (value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->closeMenus();
                    }
                }
            });
        }

        MenuBar::MenuBar() :
            _p(new Private)
        {}

        MenuBar::~MenuBar()
        {}

        std::shared_ptr<MenuBar> MenuBar::create(Context * context)
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

        void MenuBar::setStretch(const std::shared_ptr<Widget> & widget, RowStretch value, Side side)
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

        void MenuBar::addSpacer(Side side)
        {
            const auto i = _p->widgetLayout.find(side);
            if (i != _p->widgetLayout.end())
            {
                i->second->addSpacer();
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

        void MenuBar::addChild(const std::shared_ptr<IObject> & value)
        {
            Widget::addChild(value);
            DJV_PRIVATE_PTR();
            if (auto menu = std::dynamic_pointer_cast<Menu>(value))
            {
                menu->hide();
                menu->setMinimumSizeRole(MetricsRole::None);
                p.menus.push_back(menu);

                auto button = Button::Menu::create(Button::MenuStyle::Flat, getContext());
                button->installEventFilter(shared_from_this());

                p.menuLayout->addChild(button);

                p.menusToButtons[menu] = button;
                p.buttonsToMenus[button] = menu;

                auto weak = std::weak_ptr<MenuBar>(std::dynamic_pointer_cast<MenuBar>(shared_from_this()));
                button->setCheckedCallback(
                    [weak, menu](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->closeMenus();
                        if (value)
                        {
                            const auto i = widget->_p->menusToButtons.find(menu);
                            if (i != widget->_p->menusToButtons.end())
                            {
                                i->second->setChecked(true);
                                menu->popup(i->second, widget->_p->menuLayout);
                                widget->_p->menuOpen = i->second;
                            }
                        }
                    }
                });

                auto weakMenu = std::weak_ptr<Menu>(std::dynamic_pointer_cast<Menu>(menu));
                menu->setCloseCallback(
                    [weak, weakMenu]
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto menu = weakMenu.lock())
                        {
                            const auto i = widget->_p->menusToButtons.find(menu);
                            if (i != widget->_p->menusToButtons.end())
                            {
                                i->second->setChecked(false);
                            }
                        }
                        widget->_p->menuOpen.reset();
                    }
                });

                p.iconObservers[menu] = ValueObserver<std::string>::create(
                    menu->observeIcon(),
                    [button](const std::string & value)
                {
                    button->setIcon(value);
                });

                p.textObservers[menu] = ValueObserver<std::string>::create(
                    menu->observeText(),
                    [button](const std::string & value)
                {
                    button->setText(value);
                });
            }
            else if (auto widget = std::dynamic_pointer_cast<Widget>(value))
            {
                p.widgetLayout[Side::Right]->addChild(widget);
            }
        }

        void MenuBar::removeChild(const std::shared_ptr<IObject> & value)
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

        void MenuBar::_preLayoutEvent(Event::PreLayout & event)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void MenuBar::_layoutEvent(Event::Layout & event)
        {
            const BBox2f & g = getGeometry();
            const auto& style = _getStyle();
            _p->layout->setGeometry(getMargin().bbox(g, style));
        }

        bool MenuBar::_eventFilter(const std::shared_ptr<IObject> & object, Event::Event & event)
        {
            DJV_PRIVATE_PTR();
            switch (event.getEventType())
            {
            case Event::Type::PointerEnter:
            {
                if (auto button = std::dynamic_pointer_cast<Button::Menu>(object))
                {
                    if (p.menuOpen && p.menuOpen != button)
                    {
                        p.menuOpen->setChecked(false);                        
                        p.closeMenus();
                        const auto i = p.buttonsToMenus.find(button);
                        if (i != p.buttonsToMenus.end())
                        {
                            button->setChecked(true);
                            i->second->popup(button, p.menuLayout);
                            p.menuOpen = button;
                        }
                    }
                }
                break;
            }
            default: break;
            }
            return false;
        }

        void MenuBar::Private::closeMenus()
        {
            for (auto & i : menus)
            {
                i->hide();
            }
            menuOpen.reset();
        }

    } // namespace UI
} // namespace djv
