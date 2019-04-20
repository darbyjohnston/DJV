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
            std::shared_ptr<HorizontalLayout> widgetLayout;
            std::shared_ptr<HorizontalLayout> layout;
            std::map<std::shared_ptr<Menu>, std::shared_ptr<Button::Menu> > menusToButtons;
            std::map<std::shared_ptr<Menu>, std::shared_ptr<ValueObserver<std::string> > > iconObservers;
            std::map<std::shared_ptr<Menu>, std::shared_ptr<ValueObserver<std::string> > > textObservers;
            std::shared_ptr<ValueObserver<bool> > closeObserver;

            void closeMenus();
        };

        void MenuBar::_init(Context * context)
        {
            Widget::_init(context);
            
            setClassName("djv::UI::MenuBar");

            auto closeAction = Action::create();
            closeAction->setShortcut(GLFW_KEY_ESCAPE);
            addAction(closeAction);

            DJV_PRIVATE_PTR();
            p.menuLayout = HorizontalLayout::create(context);
            p.menuLayout->setSpacing(MetricsRole::None);

            p.widgetLayout = HorizontalLayout::create(context);
            p.widgetLayout->setSpacing(MetricsRole::None);

            p.layout = HorizontalLayout::create(context);
            p.layout->setSpacing(MetricsRole::None);
            p.layout->addChild(p.menuLayout);
            p.layout->addChild(p.widgetLayout);
            p.layout->setStretch(p.widgetLayout, RowStretch::Expand);
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

        void MenuBar::setStretch(const std::shared_ptr<Widget> & widget, RowStretch value)
        {
            _p->widgetLayout->setStretch(widget, value);
        }

        void MenuBar::addSeparator()
        {
            _p->widgetLayout->addSeparator();
        }

        void MenuBar::addSpacer()
        {
            _p->widgetLayout->addSpacer();
        }

        void MenuBar::addExpander()
        {
            _p->widgetLayout->addExpander();
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
                p.menus.push_back(menu);

                auto button = Button::Menu::create(getContext());
                button->setInsideMargin(Layout::Margin(MetricsRole::Margin, MetricsRole::Margin, MetricsRole::MarginSmall, MetricsRole::MarginSmall));
                button->installEventFilter(shared_from_this());

                p.menuLayout->addChild(button);

                p.menusToButtons[menu] = button;

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
                                menu->popup(i->second, widget->_p->menuLayout);
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
                    }
                });

                _p->iconObservers[menu] = ValueObserver<std::string>::create(
                    menu->observeIcon(),
                    [button](const std::string & value)
                {
                    button->setIcon(value);
                });

                _p->textObservers[menu] = ValueObserver<std::string>::create(
                    menu->observeText(),
                    [button](const std::string & value)
                {
                    button->setText(value);
                });
            }
            else if (auto widget = std::dynamic_pointer_cast<Widget>(value))
            {
                _p->widgetLayout->addChild(widget);
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
                        _p->menuLayout->removeChild(i->second);
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
                _p->widgetLayout->removeChild(widget);
            }
        }

        void MenuBar::_preLayoutEvent(Event::PreLayout & event)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void MenuBar::_layoutEvent(Event::Layout & event)
        {
            const BBox2f & g = getGeometry();
            auto style = _getStyle();
            _p->layout->setGeometry(getMargin().bbox(g, style));
        }

        bool MenuBar::_eventFilter(const std::shared_ptr<IObject> & object, Event::IEvent & event)
        {
            switch (event.getEventType())
            {
            case Event::Type::PointerEnter:
            {
                bool checked = false;
                DJV_PRIVATE_PTR();
                for (const auto & i : p.menus)
                {
                    auto j = p.menusToButtons.find(i);
                    if (j != p.menusToButtons.end())
                    {
                        checked |= j->second->isChecked();
                    }
                }
                if (checked)
                {
                    if (auto button = std::dynamic_pointer_cast<Button::Menu>(object))
                    {
                        button->setChecked(true);
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
        }

    } // namespace UI
} // namespace djv
