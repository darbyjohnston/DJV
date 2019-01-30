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
#include <djvUI/IconSystem.h>
#include <djvUI/Overlay.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/Shortcut.h>
#include <djvUI/Window.h>

#include <djvAV/Image.h>
#include <djvAV/Render2D.h>

#include <djvCore/TextSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace
        {
            class MenuWidget : public Widget
            {
                DJV_NON_COPYABLE(MenuWidget);

            protected:
                void _init(
                    const std::map<size_t, std::shared_ptr<Action> > & actions,
                    const std::map<size_t, std::shared_ptr<Menu> > & menus,
                    Context *);
                MenuWidget();

            public:
                static std::shared_ptr<MenuWidget> create(
                    const std::map<size_t, std::shared_ptr<Action> > & actions,
                    const std::map<size_t, std::shared_ptr<Menu> > & menus,
                    Context *);

                void setCloseCallback(const std::function<void(void)> &);

            protected:
                void _styleEvent(Event::Style&) override;
                void _preLayoutEvent(Event::PreLayout&) override;
                void _layoutEvent(Event::Layout&) override;
                void _paintEvent(Event::Paint&) override;
                void _pointerEnterEvent(Event::PointerEnter&) override;
                void _pointerLeaveEvent(Event::PointerLeave&) override;
                void _pointerMoveEvent(Event::PointerMove&) override;
                void _buttonPressEvent(Event::ButtonPress&) override;
                void _buttonReleaseEvent(Event::ButtonRelease&) override;

                void _updateEvent(Event::Update&) override;

            private:
                struct Item
                {
                    ButtonType buttonType = ButtonType::First;
                    bool checked = false;
                    std::string iconName;
                    std::shared_ptr<AV::Image::Image> icon;
                    std::string text;
                    glm::vec2 textSize;
                    std::string shortcutLabel;
                    glm::vec2 shortcutSize;
                    bool enabled = true;
                    glm::vec2 size;
                    BBox2f geom;
                };

                std::shared_ptr<Item> _getItem(const glm::vec2 &) const;
                void _itemsUpdate();
                void _iconUpdate();
                void _textUpdate();

                std::map<size_t, std::shared_ptr<Action> > _actions;
                std::map<size_t, std::shared_ptr<Menu> > _menus;
                AV::Font::Metrics _fontMetrics;
                std::future<AV::Font::Metrics> _fontMetricsFuture;
                bool _fontMetricsRequest = false;
                bool _hasTextIndent = false;
                bool _hasShortcuts = false;
                std::map<size_t, std::shared_ptr<Item> > _items;
                std::map<std::shared_ptr<Action>, std::shared_ptr<Item> > _actionToItem;
                std::map<std::shared_ptr<Item>, std::shared_ptr<Action> > _itemToAction;
                std::map<std::shared_ptr<Menu>, std::shared_ptr<Item> > _menuToItem;
                std::map<std::shared_ptr<Item>, std::shared_ptr<Menu> > _itemToMenu;
                std::map<std::shared_ptr<Item>, std::future<glm::vec2> > _textSizeFutures;
                std::map<std::shared_ptr<Item>, std::future<glm::vec2> > _shortcutSizeFutures;
                std::map<std::shared_ptr<Item>, std::future<std::shared_ptr<AV::Image::Image> > > _iconFutures;
                std::map<Event::PointerID, std::shared_ptr<Item> > _hoveredItems;
                std::pair<Event::PointerID, std::shared_ptr<Item> > _pressed;
                glm::vec2 _pressedPos = glm::vec2(0.f, 0.f);
                std::function<void(void)> _closeCallback;
                std::map<std::shared_ptr<Item>, std::shared_ptr<ValueObserver<ButtonType> > > _buttonTypeObservers;
                std::map<std::shared_ptr<Item>, std::shared_ptr<ValueObserver<bool> > > _checkedObservers;
                std::map<std::shared_ptr<Item>, std::shared_ptr<ValueObserver<std::string> > > _iconObservers;
                std::map<std::shared_ptr<Item>, std::shared_ptr<ValueObserver<std::string> > > _textObservers;
                std::map<std::shared_ptr<Item>, std::shared_ptr<ListObserver<std::shared_ptr<Shortcut> > > > _shortcutsObservers;
                std::map<std::shared_ptr<Item>, std::shared_ptr<ValueObserver<bool> > > _enabledObservers;
                bool _iconUpdateRequest = false;
                bool _textUpdateRequest = false;
            };

            void MenuWidget::_init(
                const std::map<size_t, std::shared_ptr<Action> > & actions,
                const std::map<size_t, std::shared_ptr<Menu> > & menus,
                Context * context)
            {
                Widget::_init(context);
                _actions = actions;
                _menus = menus;
                setBackgroundRole(Style::ColorRole::Background);
                setPointerEnabled(true);
            }

            MenuWidget::MenuWidget()
            {}

            std::shared_ptr<MenuWidget> MenuWidget::create(
                const std::map<size_t, std::shared_ptr<Action> > & actions,
                const std::map<size_t, std::shared_ptr<Menu> > & menus,
                Context * context)
            {
                auto out = std::shared_ptr<MenuWidget>(new MenuWidget);
                out->_init(actions, menus, context);
                return out;
            }

            void MenuWidget::setCloseCallback(const std::function<void(void)> & value)
            {
                _closeCallback = value;
            }

            void MenuWidget::_styleEvent(Event::Style&)
            {
                _itemsUpdate();
            }

            void MenuWidget::_preLayoutEvent(Event::PreLayout&)
            {
                if (auto style = _getStyle().lock())
                {
                    const float m = style->getMetric(Style::MetricsRole::MarginSmall);
                    const float s = style->getMetric(Style::MetricsRole::Spacing);
                    const float b = style->getMetric(Style::MetricsRole::Border);
                    const float iconSize = style->getMetric(Style::MetricsRole::Icon);

                    if (_fontMetricsRequest)
                    {
                        _fontMetricsRequest = false;
                        _fontMetrics = _fontMetricsFuture.get();
                    }
                    for (auto & i : _textSizeFutures)
                    {
                        i.first->textSize = i.second.get();
                    }
                    _textSizeFutures.clear();
                    for (auto & i : _shortcutSizeFutures)
                    {
                        i.first->shortcutSize = i.second.get();
                    }
                    _shortcutSizeFutures.clear();

                    glm::vec2 textSize(0.f, 0.f);
                    glm::vec2 shortcutSize(0.f, 0.f);
                    _hasTextIndent = false;
                    for (const auto & i : _items)
                    {
                        if (!i.second->text.empty())
                        {
                            textSize.x = std::max(textSize.x, i.second->textSize.x);
                            textSize.y = std::max(textSize.y, i.second->textSize.y);
                            shortcutSize.x = std::max(shortcutSize.x, i.second->shortcutSize.x);
                            shortcutSize.y = std::max(shortcutSize.y, i.second->shortcutSize.y);
                        }
                        if (!i.second->iconName.empty())
                        {
                            _hasTextIndent = true;
                        }
                    }
                    glm::vec2 itemSize(0.f, 0.f);
                    if (_hasTextIndent)
                    {
                        itemSize.x = iconSize;
                        itemSize.y = iconSize;
                        itemSize.x += s;
                    }
                    itemSize.x += textSize.x;
                    itemSize.y = std::max(itemSize.y, textSize.y);
                    if (_hasShortcuts)
                    {
                        itemSize.x += s;
                        itemSize.x += shortcutSize.x;
                        itemSize.y = std::max(itemSize.y, shortcutSize.y);
                    }

                    std::map<size_t, glm::vec2> sizes;
                    for (auto & i : _items)
                    {
                        glm::vec2 size(0.f, 0.f);
                        if (!i.second->text.empty())
                        {
                            size = itemSize + m * 2.f;
                        }
                        else
                        {
                            size = glm::vec2(b, b);
                        }
                        i.second->size = size;
                        sizes[i.first] = size;
                    }

                    glm::vec2 size(0.f, 0.f);
                    for (const auto & i : sizes)
                    {
                        size.x = std::max(size.x, i.second.x);
                        size.y += i.second.y;
                    }
                    _setMinimumSize(size);
                }
            }

            void MenuWidget::_layoutEvent(Event::Layout&)
            {
                if (auto style = _getStyle().lock())
                {
                    const BBox2f & g = getGeometry();
                    const float m = style->getMetric(Style::MetricsRole::MarginSmall);
                    const float s = style->getMetric(Style::MetricsRole::Spacing);
                    const float b = style->getMetric(Style::MetricsRole::Border);
                    const float iconSize = style->getMetric(Style::MetricsRole::Icon);

                    float y = g.min.y;
                    for (auto & i : _items)
                    {
                        i.second->geom.min.x = g.min.x;
                        i.second->geom.min.y = y;
                        i.second->geom.max.x = g.max.x;
                        i.second->geom.max.y = y + i.second->size.y;
                        y += i.second->size.y;
                    }
                }
            }

            void MenuWidget::_paintEvent(Event::Paint& event)
            {
                Widget::_paintEvent(event);
                if (auto render = _getRender().lock())
                {
                    if (auto style = _getStyle().lock())
                    {
                        const BBox2f & g = getGeometry();
                        const float m = style->getMetric(Style::MetricsRole::MarginSmall);
                        const float s = style->getMetric(Style::MetricsRole::Spacing);
                        const float b = style->getMetric(Style::MetricsRole::Border);
                        const float iconSize = style->getMetric(Style::MetricsRole::Icon);

                        for (auto & i : _iconFutures)
                        {
                            i.first->icon = i.second.get();
                        }
                        _iconFutures.clear();

                        for (const auto & i : _items)
                        {
                            if (i.second->checked)
                            {
                                render->setFillColor(_getColorWithOpacity(style->getColor(Style::ColorRole::Checked)));
                                render->drawRect(i.second->geom);
                            }
                            if (i.second->enabled)
                            {
                                if (i.second == _pressed.second)
                                {
                                    render->setFillColor(_getColorWithOpacity(style->getColor(Style::ColorRole::Pressed)));
                                    render->drawRect(i.second->geom);
                                }
                                else
                                {
                                    for (const auto & hovered : _hoveredItems)
                                    {
                                        if (i.second == hovered.second)
                                        {
                                            render->setFillColor(_getColorWithOpacity(style->getColor(Style::ColorRole::Hovered)));
                                            render->drawRect(i.second->geom);
                                            break;
                                        }
                                    }
                                }
                            }
                        }

                        render->setCurrentFont(style->getFontInfo(AV::Font::Info::faceDefault, Style::MetricsRole::FontMedium));
                        for (const auto & i : _items)
                        {
                            const Style::ColorRole colorRole = i.second->enabled ? Style::ColorRole::Foreground : Style::ColorRole::Disabled;
                            float x = i.second->geom.min.x + m;
                            float y = 0.f;

                            if (i.second->icon)
                            {
                                y = i.second->geom.min.y + ceilf(i.second->size.y / 2.f - iconSize / 2.f);
                                render->setFillColor(_getColorWithOpacity(style->getColor(colorRole)));
                                render->drawFilledImage(i.second->icon, BBox2f(x, y, iconSize, iconSize), AV::Render::ImageCache::Static);
                            }
                            if (_hasTextIndent)
                            {
                                x += iconSize + s;
                            }

                            if (!i.second->text.empty())
                            {
                                y = i.second->geom.min.y + ceilf(i.second->size.y / 2.f) - ceilf(_fontMetrics.lineHeight / 2.f) + _fontMetrics.ascender;
                                render->setFillColor(_getColorWithOpacity(style->getColor(colorRole)));
                                render->drawText(i.second->text, glm::vec2(x, y));
                                x += i.second->textSize.x;

                                if (!i.second->shortcutLabel.empty())
                                {
                                    x = g.max.x - i.second->shortcutSize.x - m;
                                    render->drawText(i.second->shortcutLabel, glm::vec2(x, y));
                                }
                            }
                            else
                            {
                                render->setFillColor(_getColorWithOpacity(style->getColor(Style::ColorRole::Border)));
                                render->drawRect(i.second->geom);
                            }
                        }
                    }
                }
            }

            void MenuWidget::_pointerEnterEvent(Event::PointerEnter& event)
            {
                event.accept();
                const auto & pointerInfo = event.getPointerInfo();
                if (auto item = _getItem(pointerInfo.projectedPos))
                {
                    _hoveredItems[pointerInfo.id] = item;
                }
                _redraw();
            }

            void MenuWidget::_pointerLeaveEvent(Event::PointerLeave& event)
            {
                event.accept();
                const auto & pointerInfo = event.getPointerInfo();
                const auto i = _hoveredItems.find(pointerInfo.id);
                if (i != _hoveredItems.end())
                {
                    _hoveredItems.erase(i);
                }
                _redraw();
            }

            void MenuWidget::_pointerMoveEvent(Event::PointerMove& event)
            {
                event.accept();
                const auto & pointerInfo = event.getPointerInfo();
                const auto id = pointerInfo.id;
                const auto & pos = pointerInfo.projectedPos;
                if (id == _pressed.first)
                {
                    if (auto style = _getStyle().lock())
                    {
                        const float distance = glm::length(pos - _pressedPos);
                        const bool accepted = distance < style->getMetric(Style::MetricsRole::Drag);
                        event.setAccepted(accepted);
                        if (!accepted)
                        {
                            _pressed.first = 0;
                            _pressed.second = nullptr;
                            _redraw();
                        }
                    }
                }
                else
                {
                    if (auto item = _getItem(pos))
                    {
                        if (_hoveredItems[id] != item)
                        {
                            _hoveredItems[id] = item;
                            _redraw();
                        }
                    }
                }
            }

            void MenuWidget::_buttonPressEvent(Event::ButtonPress& event)
            {
                if (!isEnabled(true) || _pressed.second)
                    return;
                const auto & pointerInfo = event.getPointerInfo();
                const auto id = pointerInfo.id;
                const auto & pos = pointerInfo.projectedPos;
                if (auto item = _getItem(pos))
                {
                    if (item->enabled)
                    {
                        event.accept();
                        _pressed.first = id;
                        _pressed.second = item;
                        _pressedPos = pos;
                        _redraw();
                    }
                }
            }

            void MenuWidget::_buttonReleaseEvent(Event::ButtonRelease& event)
            {
                const auto & pointerInfo = event.getPointerInfo();
                const auto id = pointerInfo.id;
                const auto & pos = pointerInfo.projectedPos;
                if (_pressed.second)
                {
                    const auto i = _itemToAction.find(_pressed.second);
                    if (i != _itemToAction.end())
                    {
                        switch (i->second->observeButtonType()->get())
                        {
                        case ButtonType::Push:
                            i->second->doClicked();
                            break;
                        case ButtonType::Toggle:
                            i->second->setChecked(!i->second->observeChecked()->get());
                            i->second->doChecked();
                            break;
                        case ButtonType::Radio:
                            if (!i->second->observeChecked()->get())
                            {
                                i->second->setChecked(true);
                                i->second->doChecked();
                            }
                            break;
                        default: break;
                        }
                        if (_closeCallback)
                        {
                            _closeCallback();
                        }
                    }
                    _pressed.first = 0;
                    _pressed.second = nullptr;
                    if (auto item = _getItem(pos))
                    {
                        _hoveredItems[id] = item;
                        _redraw();
                    }
                }
            }

            void MenuWidget::_updateEvent(Event::Update&)
            {
                if (_iconUpdateRequest)
                {
                    _iconUpdate();
                }
                if (_textUpdateRequest)
                {
                    _textUpdate();
                }
            }

            std::shared_ptr<MenuWidget::Item> MenuWidget::_getItem(const glm::vec2 & pos) const
            {
                std::shared_ptr<MenuWidget::Item> out;
                for (const auto & i : _items)
                {
                    if (i.second->geom.contains(pos) && !i.second->text.empty())
                    {
                        out = i.second;
                        break;
                    }
                }
                return out;
            }

            void MenuWidget::_itemsUpdate()
            {
                auto style = _getStyle().lock();
                auto fontSystem = _getFontSystem().lock();
                auto textSystem = _getTextSystem().lock();
                if (style && fontSystem && textSystem)
                {
                    const auto fontInfo = style->getFontInfo(AV::Font::Info::faceDefault, Style::MetricsRole::FontMedium);
                    _fontMetricsFuture = fontSystem->getMetrics(fontInfo);
                    _fontMetricsRequest = true;
                    _hasShortcuts = false;
                    _items.clear();
                    _actionToItem.clear();
                    _itemToAction.clear();
                    _menuToItem.clear();
                    _itemToMenu.clear();
                    _buttonTypeObservers.clear();
                    _checkedObservers.clear();
                    _iconObservers.clear();
                    _textObservers.clear();
                    _shortcutsObservers.clear();
                    _enabledObservers.clear();
                    auto weak = std::weak_ptr<MenuWidget>(std::dynamic_pointer_cast<MenuWidget>(shared_from_this()));
                    for (const auto & i : _actions)
                    {
                        auto item = std::shared_ptr<Item>(new Item);
                        if (i.second)
                        {
                            _buttonTypeObservers[item] = ValueObserver<ButtonType>::create(
                                i.second->observeButtonType(),
                                [weak, item](ButtonType value)
                            {
                                if (auto widget = weak.lock())
                                {
                                    item->buttonType = value;
                                    widget->_resize();
                                }
                            });
                            _checkedObservers[item] = ValueObserver<bool>::create(
                                i.second->observeChecked(),
                                [weak, item](bool value)
                            {
                                if (auto widget = weak.lock())
                                {
                                    item->checked = value;
                                    widget->_redraw();
                                }
                            });
                            _iconObservers[item] = ValueObserver<std::string>::create(
                                i.second->observeIcon(),
                                [weak, item](std::string value)
                            {
                                if (auto widget = weak.lock())
                                {
                                    item->iconName = value;
                                    widget->_iconUpdateRequest = true;
                                }
                            });
                            _textObservers[item] = ValueObserver<std::string>::create(
                                i.second->observeText(),
                                [weak, item](std::string value)
                            {
                                if (auto widget = weak.lock())
                                {
                                    item->text = value;
                                    widget->_textUpdateRequest = true;
                                }
                            });
                            _shortcutsObservers[item] = ListObserver<std::shared_ptr<Shortcut> >::create(
                                i.second->observeShortcuts(),
                                [weak, item](const std::vector<std::shared_ptr<Shortcut> > & value)
                            {
                                if (auto widget = weak.lock())
                                {
                                    if (auto textSystem = widget->getContext()->getSystemT<TextSystem>().lock())
                                    {
                                        std::vector<std::string> labels;
                                        for (const auto & i : value)
                                        {
                                            labels.push_back(Shortcut::getText(
                                                i->observeShortcutKey()->get(),
                                                i->observeShortcutModifiers()->get(),
                                                textSystem));
                                        }
                                        item->shortcutLabel = String::join(labels, ", ");
                                        widget->_textUpdateRequest = true;
                                    }
                                }
                            });
                            _enabledObservers[item] = ValueObserver<bool>::create(
                                i.second->observeEnabled(),
                                [weak, item](bool value)
                            {
                                if (auto widget = weak.lock())
                                {
                                    item->enabled = value;
                                    widget->_redraw();
                                }
                            });
                        }
                        _items[i.first] = item;
                        _actionToItem[i.second] = item;
                        _itemToAction[item] = i.second;
                    }
                    for (const auto & i : _menus)
                    {
                        auto item = std::shared_ptr<Item>(new Item);
                        if (i.second)
                        {
                            _textObservers[item] = ValueObserver<std::string>::create(
                                i.second->observeMenuName(),
                                [weak, item](std::string value)
                            {
                                if (auto widget = weak.lock())
                                {
                                    item->text = value;
                                    widget->_textUpdateRequest = true;
                                }
                            });
                        }
                        _items[i.first] = item;
                        _menuToItem[i.second] = item;
                        _itemToMenu[item] = i.second;
                    }
                }
                _iconUpdate();
                _textUpdate();
            }

            void MenuWidget::_iconUpdate()
            {
                _iconUpdateRequest = false;
                auto iconSystem = _getIconSystem().lock();
                auto style = _getStyle().lock();
                if (style && iconSystem)
                {
                    for (const auto & i : _items)
                    {
                        if (!i.second->iconName.empty())
                        {
                            _iconFutures[i.second] = iconSystem->getIcon(
                                i.second->iconName,
                                static_cast<int>(style->getMetric(Style::MetricsRole::Icon)));
                        }
                    }
                }
            }

            void MenuWidget::_textUpdate()
            {
                _textUpdateRequest = false;
                auto style = _getStyle().lock();
                auto fontSystem = _getFontSystem().lock();
                auto textSystem = _getTextSystem().lock();
                if (style && fontSystem && textSystem)
                {
                    _hasShortcuts = false;
                    for (const auto & i : _items)
                    {
                        const auto fontInfo = style->getFontInfo(AV::Font::Info::faceDefault, Style::MetricsRole::FontMedium);
                        _textSizeFutures[i.second] = fontSystem->measure(i.second->text, fontInfo);
                        _shortcutSizeFutures[i.second] = fontSystem->measure(i.second->shortcutLabel, fontInfo);
                        _hasShortcuts |= i.second->shortcutLabel.size() > 0;
                    }
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
                            const auto & geom = geomCandidates.front();
                            i.first->move(geom.min);
                            i.first->resize(geom.getSize());
                        }
                    }
                }
            }

            void MenuOverlayLayout::_paintEvent(Event::Paint& event)
            {
                Widget::_paintEvent(event);
                if (auto render = _getRender().lock())
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
                            render->drawRect(g);
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

            std::shared_ptr<Layout::Overlay> createOverlay();
        };

        void Menu::_init(Context * context)
        {
            DJV_PRIVATE_PTR();
            p.context = context;
            p.menuIcon = ValueSubject<std::string>::create();
            p.menuName = ValueSubject<std::string>::create();
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

        std::shared_ptr<IValueSubject<std::string> > Menu::observeMenuIcon() const
        {
            return _p->menuIcon;
        }

        std::shared_ptr<IValueSubject<std::string> > Menu::observeMenuName() const
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

        void Menu::popup(const std::shared_ptr<Window> & window, const glm::vec2 & pos)
        {
            hide();

            DJV_PRIVATE_PTR();
            auto menuOverlayLayout = MenuOverlayLayout::create(p.context);
            auto weak = std::weak_ptr<Menu>(shared_from_this());
            if (_p->count > 0)
            {
                auto menuWidget = MenuWidget::create(_p->actions, _p->menus, p.context);
                auto scrollWidget = ScrollWidget::create(ScrollType::Vertical, p.context);
                scrollWidget->addWidget(menuWidget);
                menuOverlayLayout->addWidget(scrollWidget, pos, MenuOverlayLayoutType::TopLevel);
                menuWidget->setCloseCallback(
                    [weak]
                {
                    if (auto menu = weak.lock())
                    {
                        menu->hide();
                    }
                });
            }

            p.overlay = p.createOverlay();
            p.overlay->addWidget(menuOverlayLayout);

            p.overlay->setCloseCallback(
                [weak]
            {
                if (auto menu = weak.lock())
                {
                    menu->hide();
                }
            });

            window->addWidget(p.overlay);
            p.overlay->show();
        }

        void Menu::popup(const std::shared_ptr<Window> & window, const std::weak_ptr<Widget> & button)
        {
            hide();

            DJV_PRIVATE_PTR();
            auto menuOverlayLayout = MenuOverlayLayout::create(p.context);
            auto weak = std::weak_ptr<Menu>(shared_from_this());
            if (_p->count > 0)
            {
                auto menuWidget = MenuWidget::create(_p->actions, _p->menus, p.context);
                auto scrollWidget = ScrollWidget::create(ScrollType::Vertical, p.context);
                scrollWidget->addWidget(menuWidget);
                menuOverlayLayout->addWidget(scrollWidget, button, MenuOverlayLayoutType::TopLevel);
                menuWidget->setCloseCallback(
                    [weak]
                {
                    if (auto menu = weak.lock())
                    {
                        menu->hide();
                    }
                });
            }

            p.overlay = p.createOverlay();
            p.overlay->setAnchor(button);
            p.overlay->addWidget(menuOverlayLayout);

            p.overlay->setCloseCallback(
                [weak]
            {
                if (auto menu = weak.lock())
                {
                    menu->hide();
                }
            });

            window->addWidget(p.overlay);
            p.overlay->show();
        }

        void Menu::popup(const std::shared_ptr<Window> & window, const std::weak_ptr<Widget> & button, const std::weak_ptr<Widget> & anchor)
        {
            hide();

            DJV_PRIVATE_PTR();
            auto menuOverlayLayout = MenuOverlayLayout::create(p.context);
            auto weak = std::weak_ptr<Menu>(shared_from_this());
            if (_p->count > 0)
            {
                auto menuWidget = MenuWidget::create(_p->actions, _p->menus, p.context);
                auto scrollWidget = ScrollWidget::create(ScrollType::Vertical, p.context);
                scrollWidget->addWidget(menuWidget);
                menuOverlayLayout->addWidget(scrollWidget, button, MenuOverlayLayoutType::TopLevel);
                menuWidget->setCloseCallback(
                    [weak]
                {
                    if (auto menu = weak.lock())
                    {
                        menu->hide();
                    }
                });
            }

            p.overlay = p.createOverlay();
            p.overlay->setAnchor(anchor);
            p.overlay->addWidget(menuOverlayLayout);

            p.overlay->setCloseCallback(
                [weak]
            {
                if (auto menu = weak.lock())
                {
                    menu->hide();
                }
            });

            window->addWidget(p.overlay);
            p.overlay->show();
        }

        void Menu::hide()
        {
            DJV_PRIVATE_PTR();
            if (p.overlay)
            {
                bool visible = p.overlay->isVisible();
                p.overlay->hide();
                p.overlay->setParent(nullptr);
                p.overlay = nullptr;
                if (visible && p.closeCallback)
                {
                    p.closeCallback();
                }
            }
        }

        void Menu::setCloseCallback(const std::function<void(void)> & callback)
        {
            _p->closeCallback = callback;
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
