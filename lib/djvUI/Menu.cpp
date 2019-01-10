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

#include <djvUI/Menu.h>

#include <djvUI/Action.h>
#include <djvUI/Border.h>
#include <djvUI/IButton.h>
#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/Overlay.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/Separator.h>
#include <djvUI/Shortcut.h>
#include <djvUI/StackLayout.h>
#include <djvUI/Window.h>

#include <djvAV/Render2DSystem.h>

#include <djvCore/TextSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace
        {
            class MenuLayout;

            class MenuSpacer : public Widget
            {
                DJV_NON_COPYABLE(MenuSpacer);

            protected:
                void _init(Context *);
                MenuSpacer();

            public:
                static std::shared_ptr<MenuSpacer> create(Context *);

                void setSpacerSize(float);

            protected:
                void _preLayoutEvent(Event::PreLayout&) override;

            private:
                float _size = 0.f;
            };

            void MenuSpacer::_init(Context * context)
            {
                Widget::_init(context);

                setClassName("djv::UI::MenuSpacer");
            };

            MenuSpacer::MenuSpacer()
            {}

            std::shared_ptr<MenuSpacer> MenuSpacer::create(Context * context)
            {
                auto out = std::shared_ptr<MenuSpacer>(new MenuSpacer);
                out->_init(context);
                return out;
            }

            void MenuSpacer::setSpacerSize(float value)
            {
                if (value == _size)
                    return;
                _size = value;
                _setMinimumSize(glm::vec2(_size, 0.f));
                _resize();
            }

            void MenuSpacer::_preLayoutEvent(Event::PreLayout&)
            {
                _setMinimumSize(glm::vec2(_size, 0.f));
            }

            class MenuButton : public Button::IButton
            {
                DJV_NON_COPYABLE(MenuButton);

            protected:
                void _init(Context *);
                MenuButton();

            public:
                static std::shared_ptr<MenuButton> create(Context *);

                void setAction(const std::shared_ptr<Action> &);
                void setMenu(const std::shared_ptr<Menu> &);

                float getHeightForWidth(float) const override;

            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
                void _layoutEvent(Event::Layout&) override;

                void _updateEvent(Event::Update&) override;

            private:
                std::shared_ptr<Icon> _icon;
                std::shared_ptr<MenuSpacer> _iconSpacer;
                std::shared_ptr<Label> _textLabel;
                std::shared_ptr<Label> _shortcutsLabel;
                std::shared_ptr<Layout::Horizontal> _layout;
                std::shared_ptr<ValueObserver<ButtonType> > _buttonTypeObserver;
                std::shared_ptr<ValueObserver<bool> > _checkedObserver;
                std::shared_ptr<ValueObserver<std::string> > _iconObserver;
                std::shared_ptr<ValueObserver<std::string> > _textObserver;
                std::shared_ptr<ListObserver<std::shared_ptr<Shortcut> > > _shortcutsObserver;
                std::shared_ptr<ValueObserver<bool> > _enabledObserver;

                friend class MenuLayout;
            };

            void MenuButton::_init(Context * context)
            {
                IButton::_init(context);

                setClassName("djv::UI::MenuButton");

                _icon = Icon::create(context);
                _iconSpacer = MenuSpacer::create(context);
                _iconSpacer->hide();

                _textLabel = Label::create(context);
                _textLabel->setHAlign(HAlign::Left);

                _shortcutsLabel = Label::create(context);
                _shortcutsLabel->hide();

                _layout = Layout::Horizontal::create(context);
                _layout->setMargin(Layout::Margin(Style::MetricsRole::Margin, Style::MetricsRole::Margin, Style::MetricsRole::MarginSmall, Style::MetricsRole::MarginSmall));
                _layout->addWidget(_icon);
                _layout->addWidget(_iconSpacer);
                _layout->addWidget(_textLabel, Layout::RowStretch::Expand);
                _layout->addWidget(_shortcutsLabel);
                _layout->setParent(shared_from_this());
            }

            MenuButton::MenuButton()
            {}

            std::shared_ptr<MenuButton> MenuButton::create(Context * context)
            {
                auto out = std::shared_ptr<MenuButton>(new MenuButton);
                out->_init(context);
                return out;
            }

            void MenuButton::setAction(const std::shared_ptr<Action> & action)
            {
                auto weak = std::weak_ptr<MenuButton>(std::dynamic_pointer_cast<MenuButton>(shared_from_this()));
                _buttonTypeObserver = ValueObserver<ButtonType>::create(
                    action->getButtonType(),
                    [weak](ButtonType value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->setButtonType(value);
                    }
                });
                _checkedObserver = ValueObserver<bool>::create(
                    action->isChecked(),
                    [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->setChecked(value);
                    }
                });
                _iconObserver = ValueObserver<std::string>::create(
                    action->getIcon(),
                    [weak](const std::string & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_icon->setIcon(value);
                        widget->_icon->setVisible(!value.empty());
                    }
                });
                _textObserver = ValueObserver<std::string>::create(
                    action->getText(),
                    [weak](const std::string & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_textLabel->setText(value);
                    }
                });
                auto context = getContext();
                _shortcutsObserver = ListObserver<std::shared_ptr<Shortcut> >::create(
                    action->getShortcuts(),
                    [weak, context](const std::vector<std::shared_ptr<Shortcut> > & value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto textSystem = context->getSystemT<TextSystem>().lock())
                        {
                            widget->_shortcutsLabel->setVisible(value.size());
                            std::vector<std::string> labels;
                            for (const auto & i : value)
                            {
                                labels.push_back(Shortcut::getText(
                                    i->getShortcutKey()->get(),
                                    i->getShortcutModifiers()->get(),
                                    textSystem));
                            }
                            widget->_shortcutsLabel->setText(String::join(labels, ", "));
                        }
                    }
                });
                _enabledObserver = ValueObserver<bool>::create(
                    action->isEnabled(),
                    [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->setEnabled(value);
                    }
                });
            }

            void MenuButton::setMenu(const std::shared_ptr<Menu> & menu)
            {
                auto weak = std::weak_ptr<MenuButton>(std::dynamic_pointer_cast<MenuButton>(shared_from_this()));
                _iconObserver = ValueObserver<std::string>::create(
                    menu->getMenuIcon(),
                    [weak](const std::string & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_icon->setIcon(value);
                        widget->_icon->setVisible(!value.empty());
                    }
                });
                _textObserver = ValueObserver<std::string>::create(
                    menu->getMenuName(),
                    [weak](const std::string & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_textLabel->setText(value);
                    }
                });
            }

            float MenuButton::getHeightForWidth(float value) const
            {
                return _layout->getHeightForWidth(value);
            }

            void MenuButton::_preLayoutEvent(Event::PreLayout& event)
            {
                _setMinimumSize(_layout->getMinimumSize());
            }

            void MenuButton::_layoutEvent(Event::Layout&)
            {
                _layout->setGeometry(getGeometry());
            }

            void MenuButton::_updateEvent(Event::Update& event)
            {
                IButton::_updateEvent(event);
                const Style::ColorRole colorRole = _getForegroundColorRole();
                _icon->setIconColorRole(colorRole);
                _textLabel->setTextColorRole(colorRole);
                _shortcutsLabel->setTextColorRole(colorRole);
            }

            class MenuLayout : public Widget
            {
                DJV_NON_COPYABLE(MenuLayout);

            protected:
                void _init(Context *);
                MenuLayout();

            public:
                static std::shared_ptr<MenuLayout> create(Context *);

                void addWidget(const std::shared_ptr<Widget>&);
                void clearWidgets();

                void addSeparator();

            protected:
                void _preLayoutEvent(Core::Event::PreLayout&) override;
                void _layoutEvent(Core::Event::Layout&) override;
            };

            void MenuLayout::_init(Context * context)
            {
                Widget::_init(context);
                setClassName("djv::UI::MenuLayout");
                setBackgroundRole(Style::ColorRole::Background);
            }

            MenuLayout::MenuLayout()
            {}

            std::shared_ptr<MenuLayout> MenuLayout::create(Context * context)
            {
                auto out = std::shared_ptr<MenuLayout>(new MenuLayout);
                out->_init(context);
                return out;
            }

            void MenuLayout::addWidget(const std::shared_ptr<Widget>& value)
            {
                value->setParent(shared_from_this());
                _resize();
            }

            void MenuLayout::clearWidgets()
            {
                auto children = getChildren();
                for (auto& child : children)
                {
                    child->setParent(nullptr);
                }
                _resize();
            }

            void MenuLayout::addSeparator()
            {
                addWidget(Layout::Separator::create(getContext()));
            }

            void MenuLayout::_preLayoutEvent(Event::PreLayout&)
            {
                if (auto style = _getStyle().lock())
                {
                    float iconWidth = 0.f;
                    float textWidth = 0.f;
                    float shortcutsWidth = 0.f;
                    float spacing = 0.f;
                    float margin = 0.f;
                    const auto & children = getChildrenT<Widget>();
                    for (const auto& child : children)
                    {
                        if (auto button = std::dynamic_pointer_cast<MenuButton>(child))
                        {
                            const float s = button->_layout->getSpacing().get(style).x;
                            float childSpacing = 0.f;
                            if (button->_icon->isVisible())
                            {
                                const auto & iconSize = button->_icon->getMinimumSize();
                                iconWidth = std::max(iconSize.x, iconWidth);
                                childSpacing += s;
                            }
                            const auto & textSize = button->_textLabel->getMinimumSize();
                            textWidth = std::max(textSize.x, textWidth);
                            if (button->_shortcutsLabel->isVisible())
                            {
                                const auto & shortcutsSize = button->_shortcutsLabel->getMinimumSize();
                                shortcutsWidth = std::max(shortcutsSize.x, shortcutsWidth);
                                childSpacing += s;
                            }
                            spacing = std::max(childSpacing, spacing);
                            margin = std::max(button->_layout->getMargin().getWidth(style), margin);
                        }
                    }
                    glm::vec2 minimumSize(0.f, 0.f);
                    for (const auto& child : children)
                    {
                        auto size = child->getMinimumSize();
                        if (auto button = std::dynamic_pointer_cast<MenuButton>(child))
                        {
                            size.x = iconWidth + textWidth + shortcutsWidth + spacing + margin;
                            if (iconWidth && button->_icon->getIcon().empty())
                            {
                                button->_iconSpacer->setVisible(true);
                                button->_iconSpacer->setSpacerSize(iconWidth);
                            }
                        }
                        minimumSize.x = std::max(size.x, minimumSize.x);
                        minimumSize.y += size.y;
                    }
                    _setMinimumSize(minimumSize);
                }
            }

            void MenuLayout::_layoutEvent(Event::Layout&)
            {
                const BBox2f & g = getGeometry();
                const float w = g.w();
                glm::vec2 pos = g.min;
                for (auto& child : getChildrenT<Widget>())
                {
                    const auto & size = child->getMinimumSize();
                    child->setGeometry(BBox2f(pos.x, pos.y, w, size.y));
                    pos.y += size.y;
                }
            }

            enum class MenuOverlayLayoutType
            {
                TopLevel,
                SubMenu
            };

            class MenuOverlayLayout : public Widget
            {
                DJV_NON_COPYABLE(MenuOverlayLayout);

            protected:
                void _init(Context *);
                MenuOverlayLayout();

            public:
                static std::shared_ptr<MenuOverlayLayout> create(Context *);

                void addWidget(const std::shared_ptr<Widget>&, const glm::vec2 &, MenuOverlayLayoutType);
                void addWidget(const std::shared_ptr<Widget>&, const std::weak_ptr<Widget> &, MenuOverlayLayoutType);
                void removeWidget(const std::shared_ptr<Widget>&);
                void clearWidgets();

            protected:
                void _layoutEvent(Event::Layout&) override;
                void _paintEvent(Event::Paint&) override;

            private:
                std::map<std::shared_ptr<Widget>, std::pair<glm::vec2, MenuOverlayLayoutType> > _widgetToPos;
                std::map<std::shared_ptr<Widget>, std::pair<std::weak_ptr<Widget>, MenuOverlayLayoutType> > _widgetToButton;
            };

            void MenuOverlayLayout::_init(Context * context)
            {
                Widget::_init(context);
            }

            MenuOverlayLayout::MenuOverlayLayout()
            {}

            std::shared_ptr<MenuOverlayLayout> MenuOverlayLayout::create(Context * context)
            {
                auto out = std::shared_ptr<MenuOverlayLayout>(new MenuOverlayLayout);
                out->_init(context);
                return out;
            }

            void MenuOverlayLayout::addWidget(const std::shared_ptr<Widget>& widget, const glm::vec2 & pos, MenuOverlayLayoutType type)
            {
                widget->setParent(shared_from_this());
                _widgetToPos[widget] = std::make_pair(pos, type);
            }

            void MenuOverlayLayout::addWidget(const std::shared_ptr<Widget>& widget, const std::weak_ptr<Widget> & button, MenuOverlayLayoutType type)
            {
                widget->setParent(shared_from_this());
                _widgetToButton[widget] = std::make_pair(button, type);
            }

            void MenuOverlayLayout::removeWidget(const std::shared_ptr<Widget>& value)
            {
                value->setParent(nullptr);
                const auto i = _widgetToPos.find(value);
                if (i != _widgetToPos.end())
                {
                    _widgetToPos.erase(i);
                }
                const auto j = _widgetToButton.find(value);
                if (j != _widgetToButton.end())
                {
                    _widgetToButton.erase(j);
                }
            }

            void MenuOverlayLayout::clearWidgets()
            {
                auto children = getChildren();
                for (auto& child : children)
                {
                    child->setParent(nullptr);
                }
                _widgetToPos.clear();
                _widgetToButton.clear();
            }

            void MenuOverlayLayout::_layoutEvent(Event::Layout&)
            {
                const BBox2f & g = getGeometry();
                for (const auto & i : _widgetToPos)
                {
                    const auto & pos = i.second.first;
                    const auto & minimumSize = i.first->getMinimumSize();
                    std::vector<BBox2f> geomCandidates;
                    switch (i.second.second)
                    {
                    case MenuOverlayLayoutType::TopLevel:
                    {
                        const BBox2f aboveLeft(
                            glm::vec2(pos.x - minimumSize.x, pos.y - minimumSize.y),
                            glm::vec2(pos.x, pos.y));
                        const BBox2f aboveRight(
                            glm::vec2(pos.x, pos.y - minimumSize.y),
                            glm::vec2(pos.x + minimumSize.x, pos.y));
                        const BBox2f belowLeft(
                            glm::vec2(pos.x - minimumSize.x, pos.y),
                            glm::vec2(pos.x, pos.y + minimumSize.y));
                        const BBox2f belowRight(
                            glm::vec2(pos.x, pos.y),
                            glm::vec2(pos.x + minimumSize.x, pos.y + minimumSize.y));
                        geomCandidates.push_back(belowRight.intersect(g));
                        geomCandidates.push_back(belowLeft.intersect(g));
                        geomCandidates.push_back(aboveRight.intersect(g));
                        geomCandidates.push_back(aboveLeft.intersect(g));
                        break;
                    }
                    case MenuOverlayLayoutType::SubMenu:
                    {
                        break;
                    }
                    default: break;
                    }
                    if (geomCandidates.size())
                    {
                        std::sort(geomCandidates.begin(), geomCandidates.end(),
                            [](const BBox2f & a, const BBox2f & b) -> bool
                        {
                            return a.getArea() > b.getArea();
                        });
                        i.first->move(geomCandidates.front().min);
                        i.first->resize(geomCandidates.front().getSize());
                    }
                }
                for (const auto & i : _widgetToButton)
                {
                    if (auto button = i.second.first.lock())
                    {
                        const auto & buttonBBox = button->getGeometry();
                        const auto & minimumSize = i.first->getMinimumSize();
                        std::vector<BBox2f> geomCandidates;
                        switch (i.second.second)
                        {
                        case MenuOverlayLayoutType::TopLevel:
                        {
                            const BBox2f aboveLeft(
                                glm::vec2(std::min(buttonBBox.max.x - minimumSize.x, buttonBBox.min.x), buttonBBox.min.y + 1 - minimumSize.y),
                                glm::vec2(buttonBBox.max.x, buttonBBox.min.y + 1));
                            const BBox2f aboveRight(
                                glm::vec2(buttonBBox.min.x, buttonBBox.min.y + 1 - minimumSize.y),
                                glm::vec2(std::max(buttonBBox.min.x + minimumSize.x, buttonBBox.max.x), buttonBBox.min.y + 1));
                            const BBox2f belowLeft(
                                glm::vec2(std::min(buttonBBox.max.x - minimumSize.x, buttonBBox.min.x), buttonBBox.max.y - 1),
                                glm::vec2(buttonBBox.max.x, buttonBBox.max.y - 1 + minimumSize.y));
                            const BBox2f belowRight(
                                glm::vec2(buttonBBox.min.x, buttonBBox.max.y - 1),
                                glm::vec2(std::max(buttonBBox.min.x + minimumSize.x, buttonBBox.max.x), buttonBBox.max.y - 1 + minimumSize.y));
                            geomCandidates.push_back(belowRight.intersect(g));
                            geomCandidates.push_back(belowLeft.intersect(g));
                            geomCandidates.push_back(aboveRight.intersect(g));
                            geomCandidates.push_back(aboveLeft.intersect(g));
                            break;
                        }
                        case MenuOverlayLayoutType::SubMenu:
                        {
                            break;
                        }
                        default: break;
                        }
                        if (geomCandidates.size())
                        {
                            std::sort(geomCandidates.begin(), geomCandidates.end(),
                                [](const BBox2f & a, const BBox2f & b) -> bool
                            {
                                return a.getArea() > b.getArea();
                            });
                            i.first->move(geomCandidates.front().min);
                            i.first->resize(geomCandidates.front().getSize());
                        }
                    }
                }
            }

            void MenuOverlayLayout::_paintEvent(Event::Paint& event)
            {
                Widget::_paintEvent(event);
                if (auto render = _getRenderSystem().lock())
                {
                    if (auto style = _getStyle().lock())
                    {
                        const float s = style->getMetric(Style::MetricsRole::Shadow);
                        render->setFillColor(_getColorWithOpacity(style->getColor(Style::ColorRole::Shadow)));
                        for (const auto & i : getChildrenT<Widget>())
                        {
                            BBox2f g = i->getGeometry();
                            g.min.x += s;
                            g.min.y += s;
                            g.max.x += s;
                            g.max.y += s;
                            render->drawRectangle(g);
                        }
                    }
                }
            }

        } // namespace

        struct Menu::Private
        {
            Context * context = nullptr;
            std::shared_ptr<ValueSubject<std::string> > menuIcon;
            std::shared_ptr<ValueSubject<std::string> > menuName;
            std::map<size_t, std::shared_ptr<Action> > actions;
            std::map<size_t, std::shared_ptr<Menu> > menus;
            size_t count = 0;
            std::shared_ptr<Layout::Overlay> overlay;
            std::function<void(void)> closeCallback;

            std::shared_ptr<Widget> createMenu(const std::weak_ptr<Menu> &, MenuType);
            std::shared_ptr<Layout::Overlay> createOverlay();
        };

        void Menu::_init(Context * context)
        {
            _p->context = context;
            _p->menuIcon = ValueSubject<std::string>::create();
            _p->menuName = ValueSubject<std::string>::create();
        }

        Menu::Menu() :
            _p(new Private)
        {}

        Menu::~Menu()
        {}

        std::shared_ptr<Menu> Menu::create(Context * context)
        {
            auto out = std::shared_ptr<Menu>(new Menu);
            out->_init(context);
            return out;
        }

        std::shared_ptr<Menu> Menu::create(const std::string & name, Context * context)
        {
            auto out = std::shared_ptr<Menu>(new Menu);
            out->_init(context);
            out->setMenuName(name);
            return out;
        }

        std::shared_ptr<IValueSubject<std::string> > Menu::getMenuIcon() const
        {
            return _p->menuIcon;
        }

        std::shared_ptr<IValueSubject<std::string> > Menu::getMenuName() const
        {
            return _p->menuName;
        }

        void Menu::setMenuIcon(const std::string & value)
        {
            _p->menuIcon->setIfChanged(value);
        }

        void Menu::setMenuName(const std::string & value)
        {
            _p->menuName->setIfChanged(value);
        }

        void Menu::addAction(const std::shared_ptr<Action> & action)
        {
            _p->actions[_p->count++] = action;
        }

        void Menu::addMenu(const std::shared_ptr<Menu> & menu)
        {
            _p->menus[_p->count++] = menu;
        }

        void Menu::addSeparator()
        {
            _p->actions[_p->count++] = nullptr;
        }

        void Menu::clearActions()
        {
            _p->actions.clear();
        }

        void Menu::popup(const std::shared_ptr<Window> & window, const glm::vec2 & pos, MenuType type)
        {
            hide();

            auto menuOverlayLayout = MenuOverlayLayout::create(_p->context);
            menuOverlayLayout->addWidget(_p->createMenu(shared_from_this(), type), pos, MenuOverlayLayoutType::TopLevel);

            _p->overlay = _p->createOverlay();
            _p->overlay->addWidget(menuOverlayLayout);

            auto weak = std::weak_ptr<Menu>(shared_from_this());
            _p->overlay->setCloseCallback(
                [weak]
            {
                if (auto menu = weak.lock())
                {
                    menu->hide();
                }
            });

            window->addWidget(_p->overlay);
            _p->overlay->show();
        }

        void Menu::popup(const std::shared_ptr<Window> & window, const std::weak_ptr<Widget> & button, MenuType type)
        {
            hide();

            auto menuOverlayLayout = MenuOverlayLayout::create(_p->context);
            menuOverlayLayout->addWidget(_p->createMenu(shared_from_this(), type), button, MenuOverlayLayoutType::TopLevel);

            _p->overlay = _p->createOverlay();
            _p->overlay->setAnchor(button);
            _p->overlay->addWidget(menuOverlayLayout);

            auto weak = std::weak_ptr<Menu>(shared_from_this());
            _p->overlay->setCloseCallback(
                [weak]
            {
                if (auto menu = weak.lock())
                {
                    menu->hide();
                }
            });

            window->addWidget(_p->overlay);
            _p->overlay->show();
        }

        void Menu::popup(const std::shared_ptr<Window> & window, const std::weak_ptr<Widget> & button, const std::weak_ptr<Widget> & anchor, MenuType type)
        {
            hide();

            auto menuOverlayLayout = MenuOverlayLayout::create(_p->context);
            menuOverlayLayout->addWidget(_p->createMenu(shared_from_this(), type), button, MenuOverlayLayoutType::TopLevel);

            _p->overlay = _p->createOverlay();
            _p->overlay->setAnchor(anchor);
            _p->overlay->addWidget(menuOverlayLayout);

            auto weak = std::weak_ptr<Menu>(shared_from_this());
            _p->overlay->setCloseCallback(
                [weak]
            {
                if (auto menu = weak.lock())
                {
                    menu->hide();
                }
            });

            window->addWidget(_p->overlay);
            _p->overlay->show();
        }

        void Menu::hide()
        {
            if (_p->overlay)
            {
                bool visible = _p->overlay->isVisible();
                _p->overlay->hide();
                _p->overlay->setParent(nullptr);
                _p->overlay = nullptr;
                if (visible && _p->closeCallback)
                {
                    _p->closeCallback();
                }
            }
        }

        void Menu::setCloseCallback(const std::function<void(void)> & callback)
        {
            _p->closeCallback = callback;
        }

        std::shared_ptr<Widget> Menu::Private::createMenu(const std::weak_ptr<Menu> & menuWeak, MenuType type)
        {
            auto layout = MenuLayout::create(context);
            std::map<size_t, std::shared_ptr<Widget> > widgets;
            for (const auto & i : actions)
            {
                if (auto & action = i.second)
                {
                    auto button = MenuButton::create(context);
                    button->setAction(action);
                    widgets[i.first] = button;
                    button->setClickedCallback(
                        [menuWeak, action]
                    {
                        action->doClicked();
                        if (auto menu = menuWeak.lock())
                        {
                            menu->hide();
                        }
                    });
                    button->setCheckedCallback(
                        [menuWeak, action](bool value)
                    {
                        action->setChecked(value);
                        action->doChecked();
                        if (auto menu = menuWeak.lock())
                        {
                            menu->hide();
                        }
                    });
                }
                else
                {
                    widgets[i.first] = Layout::Separator::create(context);
                }
            }
            for (const auto & i : menus)
            {
                auto button = MenuButton::create(context);
                button->setMenu(i.second);
                widgets[i.first] = button;
            }

            for (const auto & i : widgets)
            {
                layout->addWidget(i.second);
            }

            std::shared_ptr<Widget> out;
            switch (type)
            {
            case MenuType::Default:
            {
                auto border = Layout::Border::create(context);
                border->addWidget(layout);
                out = border;
                break;
            }
            case MenuType::ComboBox:
            {
                auto scrollWidget = ScrollWidget::create(ScrollType::Vertical, context);
                scrollWidget->addWidget(layout);
                out = scrollWidget;
                break;
            }
            }
            return out;
        }

        std::shared_ptr<Layout::Overlay> Menu::Private::createOverlay()
        {
            auto overlay = Layout::Overlay::create(context);
            overlay->setCaptureKeyboard(false);
            overlay->setFadeIn(false);
            overlay->setBackgroundRole(Style::ColorRole::None);
            return overlay;
        }

    } // namespace UI
} // namespace djv
