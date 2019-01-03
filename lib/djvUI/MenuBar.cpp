//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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
#include <djvUI/IButton.h>
#include <djvUI/Label.h>
#include <djvUI/Menu.h>
#include <djvUI/Overlay.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Shortcut.h>
#include <djvUI/StackLayout.h>
#include <djvUI/Window.h>

#include <djvAV/Render2DSystem.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace
        {
            class MenuBarButton : public Widget
            {
                DJV_NON_COPYABLE(MenuBarButton);

            protected:
                void _init(Context *);
                MenuBarButton();

            public:
                static std::shared_ptr<MenuBarButton> create(Context *);

                void setText(const std::string&);

                bool isChecked() const;
                void setChecked(bool);
                void setCheckedCallback(const std::function<void(bool)>&);

            protected:
                void _preLayoutEvent(Event::PreLayout &) override;
                void _layoutEvent(Event::Layout &) override;
                void _paintEvent(Event::Paint&) override;
                void _pointerEnterEvent(Event::PointerEnter&) override;
                void _pointerLeaveEvent(Event::PointerLeave&) override;
                void _pointerMoveEvent(Event::PointerMove&) override;
                void _buttonPressEvent(Event::ButtonPress&) override;

            private:
                bool _isHovered() const;

                bool _checked = false;
                std::shared_ptr<Label> _label;
                std::shared_ptr<Layout::HorizontalLayout> _layout;
                std::map<Event::PointerID, bool> _pointerHover;
                std::function<void(bool)> _checkedCallback;
            };

            void MenuBarButton::_init(Context * context)
            {
                Widget::_init(context);

                setClassName("djv::UI::MenuBarButton");
                setPointerEnabled(true);

                _label = Label::create(context);

                _layout = Layout::HorizontalLayout::create(context);
                _layout->setMargin(Layout::Margin(Style::MetricsRole::Margin, Style::MetricsRole::Margin, Style::MetricsRole::MarginSmall, Style::MetricsRole::MarginSmall));
                _layout->addWidget(_label);
                _layout->setParent(shared_from_this());
            }

            MenuBarButton::MenuBarButton()
            {}

            std::shared_ptr<MenuBarButton> MenuBarButton::create(Context * context)
            {
                auto out = std::shared_ptr<MenuBarButton>(new MenuBarButton);
                out->_init(context);
                return out;
            }

            void MenuBarButton::setText(const std::string& value)
            {
                _label->setText(value);
            }

            bool MenuBarButton::isChecked() const
            {
                return _checked;
            }

            void MenuBarButton::setChecked(bool value)
            {
                if (value == _checked)
                    return;
                _checked = value;
                _redraw();
                if (_checkedCallback)
                {
                    _checkedCallback(_checked);
                }
            }

            void MenuBarButton::setCheckedCallback(const std::function<void(bool)> & callback)
            {
                _checkedCallback = callback;
            }

            void MenuBarButton::_preLayoutEvent(Event::PreLayout &)
            {
                _setMinimumSize(_layout->getMinimumSize());
            }

            void MenuBarButton::_layoutEvent(Event::Layout &)
            {
                _layout->setGeometry(getGeometry());
            }

            void MenuBarButton::_paintEvent(Event::Paint& event)
            {
                Widget::_paintEvent(event);
                if (auto render = _getRenderSystem().lock())
                {
                    if (auto style = _getStyle().lock())
                    {
                        const BBox2f& g = getGeometry();
                        if (_isHovered() || _checked)
                        {
                            render->setFillColor(_getColorWithOpacity(style->getColor(Style::ColorRole::Hover)));
                            render->drawRectangle(g);
                        }
                    }
                }
            }

            void MenuBarButton::_pointerEnterEvent(Event::PointerEnter& event)
            {
                event.accept();
                const auto id = event.getPointerInfo().id;
                _pointerHover[id] = true;
                _redraw();
            }

            void MenuBarButton::_pointerLeaveEvent(Event::PointerLeave& event)
            {
                event.accept();
                const auto id = event.getPointerInfo().id;
                const auto i = _pointerHover.find(id);
                if (i != _pointerHover.end())
                {
                    _pointerHover.erase(i);
                    _redraw();
                }
            }

            void MenuBarButton::_pointerMoveEvent(Event::PointerMove& event)
            {
                event.accept();
            }

            void MenuBarButton::_buttonPressEvent(Event::ButtonPress& event)
            {
                event.accept();
                setChecked(!_checked);
            }

            bool MenuBarButton::_isHovered() const
            {
                bool out = false;
                for (const auto& i : _pointerHover)
                {
                    out |= i.second;
                }
                return out;
            }

        } // namespace

        struct MenuBar::Private
        {
            std::vector<std::shared_ptr<Menu> > menus;
            std::shared_ptr<Layout::HorizontalLayout> buttonLayout;
            std::shared_ptr<Layout::HorizontalLayout> layout;
            std::map<std::shared_ptr<Menu>, std::shared_ptr<MenuBarButton> > menusToButtons;
            std::map<std::shared_ptr<Menu>, std::shared_ptr<ValueObserver<std::string> > > menuNameObservers;
            std::shared_ptr<ValueObserver<bool> > closeObserver;

            void closeMenus();
        };

        void MenuBar::_init(Context * context)
        {
            Widget::_init(context);
            
            setClassName("djv::UI::MenuBar");

            auto closeShortcut = Shortcut::create(GLFW_KEY_ESCAPE);
            auto closeAction = Action::create();
            closeAction->setShortcut(closeShortcut);
            addAction(closeAction);

            _p->buttonLayout = Layout::HorizontalLayout::create(context);
            _p->buttonLayout->setSpacing(Style::MetricsRole::None);

            _p->layout = Layout::HorizontalLayout::create(context);
            _p->layout->setSpacing(Style::MetricsRole::None);
            _p->layout->addWidget(_p->buttonLayout);
            _p->layout->addExpander();
            _p->layout->setParent(shared_from_this());

            auto weak = std::weak_ptr<MenuBar>(std::dynamic_pointer_cast<MenuBar>(shared_from_this()));
            _p->closeObserver = ValueObserver<bool>::create(
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

        void MenuBar::addMenu(const std::shared_ptr<Menu> & menu)
        {
            _p->menus.push_back(menu);
            
            auto button = MenuBarButton::create(getContext());
            button->installEventFilter(shared_from_this());
            _p->buttonLayout->addWidget(button);

            _p->menusToButtons[menu] = button;

            auto weak = std::weak_ptr<MenuBar>(std::dynamic_pointer_cast<MenuBar>(shared_from_this()));
            button->setCheckedCallback(
                [weak, menu, button](bool value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->closeMenus();
                    if (value)
                    {
                        if (auto window = widget->getWindow().lock())
                        {
                            menu->popup(window, button, widget->_p->buttonLayout);
                        }
                    }
                }
            });

            menu->setCloseCallback(
                [weak, menu]
            {
                if (auto widget = weak.lock())
                {
                    const auto i = widget->_p->menusToButtons.find(menu);
                    if (i != widget->_p->menusToButtons.end())
                    {
                        i->second->setChecked(false);
                    }
                }
            });

            _p->menuNameObservers[menu] = ValueObserver<std::string>::create(
                menu->getMenuName(),
                [button](const std::string & value)
            {
                button->setText(value);
            });
        }

        float MenuBar::getHeightForWidth(float value) const
        {
            return _p->layout->getHeightForWidth(value);
        }

        void MenuBar::_preLayoutEvent(Event::PreLayout& event)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void MenuBar::_layoutEvent(Event::Layout& event)
        {
            if (auto style = _getStyle().lock())
            {
                const BBox2f & g = getGeometry();
                _p->layout->setGeometry(getMargin().bbox(g, style));
            }
        }

        bool MenuBar::_eventFilter(const std::shared_ptr<IObject>& object, Event::IEvent& event)
        {
            switch (event.getEventType())
            {
            case Event::Type::PointerEnter:
            {
                bool checked = false;
                for (const auto & i : _p->menus)
                {
                    auto j = _p->menusToButtons.find(i);
                    if (j != _p->menusToButtons.end())
                    {
                        checked |= j->second->isChecked();
                    }
                }
                if (checked)
                {
                    if (auto button = std::dynamic_pointer_cast<MenuBarButton>(object))
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
