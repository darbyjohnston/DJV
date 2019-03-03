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
                void _init(Context *);
                MenuWidget();

            public:
                static std::shared_ptr<MenuWidget> create(Context *);

                void setActions(const std::map<size_t, std::shared_ptr<Action> > &);

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

                std::shared_ptr<Widget> _createTooltip(const glm::vec2 & pos) override;

                void _updateEvent(Event::Update&) override;

            private:
                struct Item
                {
                    ButtonType buttonType = ButtonType::First;
                    bool checked = false;
                    std::shared_ptr<AV::Image::Image> icon;
                    std::string title;
                    glm::vec2 titleSize;
                    std::string shortcutLabel;
                    glm::vec2 shortcutSize;
                    bool enabled = true;
                    glm::vec2 size;
                    BBox2f geom;
                };

                std::shared_ptr<Item> _getItem(const glm::vec2 &) const;
                void _itemsUpdate();
                void _textUpdate();

                std::map<size_t, std::shared_ptr<Action> > _actions;
                AV::Font::Metrics _fontMetrics;
                std::future<AV::Font::Metrics> _fontMetricsFuture;
                bool _hasShortcuts = false;
                std::map<size_t, std::shared_ptr<Item> > _items;
                std::map<std::shared_ptr<Action>, std::shared_ptr<Item> > _actionToItem;
                std::map<std::shared_ptr<Item>, std::shared_ptr<Action> > _itemToAction;
                std::map<std::shared_ptr<Item>, std::future<glm::vec2> > _titleSizeFutures;
                std::map<std::shared_ptr<Item>, std::future<glm::vec2> > _shortcutSizeFutures;
                std::map<Event::PointerID, std::shared_ptr<Item> > _hoveredItems;
                std::pair<Event::PointerID, std::shared_ptr<Item> > _pressed;
                glm::vec2 _pressedPos = glm::vec2(0.f, 0.f);
                std::function<void(void)> _closeCallback;
                std::map<std::shared_ptr<Item>, std::shared_ptr<ValueObserver<ButtonType> > > _buttonTypeObservers;
                std::map<std::shared_ptr<Item>, std::shared_ptr<ValueObserver<bool> > > _checkedObservers;
                std::map<std::shared_ptr<Item>, std::shared_ptr<ValueObserver<std::string> > > _titleObservers;
                std::map<std::shared_ptr<Item>, std::shared_ptr<ListObserver<std::shared_ptr<Shortcut> > > > _shortcutsObservers;
                std::map<std::shared_ptr<Item>, std::shared_ptr<ValueObserver<bool> > > _enabledObservers;
                bool _textUpdateRequest = false;
            };

            void MenuWidget::_init(Context * context)
            {
                Widget::_init(context);
            }

            MenuWidget::MenuWidget()
            {}

            std::shared_ptr<MenuWidget> MenuWidget::create(Context * context)
            {
                auto out = std::shared_ptr<MenuWidget>(new MenuWidget);
                out->_init(context);
                return out;
            }

            void MenuWidget::setActions(const std::map<size_t, std::shared_ptr<Action> > & actions)
            {
                _actions = actions;
                _itemsUpdate();
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
                    const float m = style->getMetric(MetricsRole::MarginSmall);
                    const float s = style->getMetric(MetricsRole::Spacing);
                    const float b = style->getMetric(MetricsRole::Border);

                    if (_fontMetricsFuture.valid())
                    {
                        try
                        {
                            _fontMetrics = _fontMetricsFuture.get();
                            _resize();
                        }
                        catch (const std::exception& e)
                        {
                            _log(e.what(), LogLevel::Error);
                        }
                    }
                    for (auto & i : _titleSizeFutures)
                    {
                        if (i.second.valid())
                        {
                            try
                            {
                                i.first->titleSize = i.second.get();
                                _resize();
                            }
                            catch (const std::exception& e)
                            {
                                _log(e.what(), LogLevel::Error);
                            }
                        }
                    }
                    for (auto & i : _shortcutSizeFutures)
                    {
                        if (i.second.valid())
                        {
                            try
                            {
                                i.first->shortcutSize = i.second.get();
                                _resize();
                            }
                            catch (const std::exception& e)
                            {
                                _log(e.what(), LogLevel::Error);
                            }
                        }
                    }

                    glm::vec2 titleSize(0.f, 0.f);
                    glm::vec2 shortcutSize(0.f, 0.f);
                    for (const auto & i : _items)
                    {
                        if (!i.second->title.empty())
                        {
                            titleSize.x = std::max(titleSize.x, i.second->titleSize.x);
                            titleSize.y = std::max(titleSize.y, i.second->titleSize.y);
                        }
                        if (!i.second->shortcutLabel.empty())
                        {
                            shortcutSize.x = std::max(shortcutSize.x, i.second->shortcutSize.x);
                            shortcutSize.y = std::max(shortcutSize.y, i.second->shortcutSize.y);
                        }
                    }

                    glm::vec2 itemSize(0.f, 0.f);
                    itemSize.x = titleSize.x;
                    itemSize.y = std::max(itemSize.y, titleSize.y);
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
                        if (!i.second->title.empty())
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
                        const float m = style->getMetric(MetricsRole::MarginSmall);
                        const float s = style->getMetric(MetricsRole::Spacing);
                        const float iconSize = style->getMetric(MetricsRole::Icon);

                        for (const auto & i : _items)
                        {
                            if (i.second->checked)
                            {
                                render->setFillColor(_getColorWithOpacity(style->getColor(ColorRole::Checked)));
                                render->drawRect(i.second->geom);
                            }
                            if (i.second->enabled)
                            {
                                if (i.second == _pressed.second)
                                {
                                    render->setFillColor(_getColorWithOpacity(style->getColor(ColorRole::Pressed)));
                                    render->drawRect(i.second->geom);
                                }
                                else
                                {
                                    for (const auto & hovered : _hoveredItems)
                                    {
                                        if (i.second == hovered.second)
                                        {
                                            render->setFillColor(_getColorWithOpacity(style->getColor(ColorRole::Hovered)));
                                            render->drawRect(i.second->geom);
                                            break;
                                        }
                                    }
                                }
                            }
                        }

                        render->setCurrentFont(style->getFontInfo(AV::Font::Info::faceDefault, MetricsRole::FontMedium));
                        for (const auto & i : _items)
                        {
                            const ColorRole colorRole = i.second->enabled ? ColorRole::Foreground : ColorRole::Disabled;
                            float x = i.second->geom.min.x + m;
                            float y = 0.f;

                            if (i.second->icon)
                            {
                                y = i.second->geom.min.y + ceilf(i.second->size.y / 2.f - iconSize / 2.f);
                                render->setFillColor(_getColorWithOpacity(style->getColor(colorRole)));
                                render->drawFilledImage(i.second->icon, BBox2f(x, y, iconSize, iconSize));
                            }

                            if (!i.second->title.empty())
                            {
                                y = i.second->geom.min.y + ceilf(i.second->size.y / 2.f) - ceilf(_fontMetrics.lineHeight / 2.f) + _fontMetrics.ascender;
                                render->setFillColor(_getColorWithOpacity(style->getColor(colorRole)));
                                render->drawText(i.second->title, glm::vec2(x, y));
                                x += i.second->titleSize.x;

                                if (!i.second->shortcutLabel.empty())
                                {
                                    x = g.max.x - i.second->shortcutSize.x - m;
                                    render->drawText(i.second->shortcutLabel, glm::vec2(x, y));
                                }
                            }
                            else
                            {
                                render->setFillColor(_getColorWithOpacity(style->getColor(ColorRole::Border)));
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
                        const bool accepted = distance < style->getMetric(MetricsRole::Drag);
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
                        case ButtonType::Exclusive:
                            i->second->setChecked(!i->second->observeChecked()->get());
                            i->second->doChecked();
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

            std::shared_ptr<Widget> MenuWidget::_createTooltip(const glm::vec2 & pos)
            {
                std::string text;
                for (const auto & i : _items)
                {
                    if (i.second->geom.contains(pos))
                    {
                        const auto j = _itemToAction.find(i.second);
                        if (j != _itemToAction.end())
                        {
                            text = j->second->observeTooltip()->get();
                            break;
                        }
                    }
                }
                return !text.empty() ? _createTooltipDefault(text) : nullptr;
            }

            void MenuWidget::_updateEvent(Event::Update&)
            {
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
                    if (i.second->geom.contains(pos) && !i.second->title.empty())
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
                    const auto fontInfo = style->getFontInfo(AV::Font::Info::faceDefault, MetricsRole::FontMedium);
                    _fontMetricsFuture = fontSystem->getMetrics(fontInfo);
                    _hasShortcuts = false;
                    _items.clear();
                    _actionToItem.clear();
                    _itemToAction.clear();
                    _buttonTypeObservers.clear();
                    _checkedObservers.clear();
                    _titleObservers.clear();
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
                            _titleObservers[item] = ValueObserver<std::string>::create(
                                i.second->observeTitle(),
                                [weak, item](std::string value)
                            {
                                if (auto widget = weak.lock())
                                {
                                    item->title = value;
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
                }
                _textUpdate();
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
                        const auto fontInfo = style->getFontInfo(AV::Font::Info::faceDefault, MetricsRole::FontMedium);
                        _titleSizeFutures[i.second] = fontSystem->measure(i.second->title, fontInfo);
                        _shortcutSizeFutures[i.second] = fontSystem->measure(i.second->shortcutLabel, fontInfo);
                        _hasShortcuts |= i.second->shortcutLabel.size() > 0;
                    }
                }
            }

            class MenuPopupWidget : public Widget
            {
                DJV_NON_COPYABLE(MenuPopupWidget);

            protected:
                void _init(Context *);
                MenuPopupWidget();

            public:
                static std::shared_ptr<MenuPopupWidget> create(Context *);

                void setActions(const std::map<size_t, std::shared_ptr<Action> > &);
                
                void setCloseCallback(const std::function<void(void)> &);

            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
                void _layoutEvent(Event::Layout&) override;
                void _buttonPressEvent(Event::ButtonPress &) override;
                void _buttonReleaseEvent(Event::ButtonRelease &) override;

            private:
                std::shared_ptr<ScrollWidget> _scrollWidget;
                std::shared_ptr<MenuWidget> _menuWidget;
            };

            void MenuPopupWidget::_init(Context * context)
            {
                Widget::_init(context);
                setBackgroundRole(ColorRole::Background);
                setPointerEnabled(true);

                _menuWidget = MenuWidget::create(context);

                _scrollWidget = ScrollWidget::create(ScrollType::Vertical, context);
                _scrollWidget->setMinimumSizeRole(MetricsRole::None);
                _scrollWidget->addWidget(_menuWidget);
                _scrollWidget->setParent(shared_from_this());
            }

            MenuPopupWidget::MenuPopupWidget()
            {}

            std::shared_ptr<MenuPopupWidget> MenuPopupWidget::create(Context * context)
            {
                auto out = std::shared_ptr< MenuPopupWidget>(new MenuPopupWidget);
                out->_init(context);
                return out;
            }

            void MenuPopupWidget::setActions(const std::map<size_t, std::shared_ptr<Action> > & actions)
            {
                _menuWidget->setActions(actions);
            }

            void MenuPopupWidget::setCloseCallback(const std::function<void(void)> & callback)
            {
                _menuWidget->setCloseCallback(callback);
            }

            void MenuPopupWidget::_buttonPressEvent(Event::ButtonPress & event)
            {
                event.accept();
            }

            void MenuPopupWidget::_buttonReleaseEvent(Event::ButtonRelease & event)
            {
                event.accept();
            }

            void MenuPopupWidget::_preLayoutEvent(Event::PreLayout &)
            {
                _setMinimumSize(_scrollWidget->getMinimumSize());
            }

            void MenuPopupWidget::_layoutEvent(Event::Layout &)
            {
                _scrollWidget->setGeometry(getGeometry());
            }

            class MenuOverlayLayout : public Widget
            {
                DJV_NON_COPYABLE(MenuOverlayLayout);

            protected:
                void _init(Context *);
                MenuOverlayLayout();

            public:
                static std::shared_ptr<MenuOverlayLayout> create(Context *);

                void addWidget(const std::shared_ptr<Widget>&);
                void removeWidget(const std::shared_ptr<Widget>&);
                void clearWidgets();

                void setPos(const std::shared_ptr<Widget>&, const glm::vec2 &);
                void setButton(const std::shared_ptr<Widget>&, const std::weak_ptr<Widget> &);

            protected:
                void _layoutEvent(Event::Layout&) override;
                void _paintEvent(Event::Paint&) override;

            private:
                std::map<std::shared_ptr<Widget>, glm::vec2> _widgetToPos;
                std::map<std::shared_ptr<Widget>, std::weak_ptr<Widget> > _widgetToButton;
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

            void MenuOverlayLayout::addWidget(const std::shared_ptr<Widget>& widget)
            {
                widget->setParent(shared_from_this());
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

            void MenuOverlayLayout::setPos(const std::shared_ptr<Widget>& widget, const glm::vec2 & pos)
            {
                _widgetToPos[widget] = pos;
            }

            void MenuOverlayLayout::setButton(const std::shared_ptr<Widget>& widget, const std::weak_ptr<Widget> & button)
            {
                _widgetToButton[widget] = button;
            }

            void MenuOverlayLayout::_layoutEvent(Event::Layout&)
            {
                const BBox2f & g = getGeometry();
                for (const auto & i : _widgetToPos)
                {
                    const auto & pos = i.second;
                    const auto & minimumSize = i.first->getMinimumSize();
                    std::vector<BBox2f> geomCandidates;
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
                    std::sort(geomCandidates.begin(), geomCandidates.end(),
                        [](const BBox2f & a, const BBox2f & b) -> bool
                    {
                        return a.getArea() > b.getArea();
                    });
                    i.first->move(geomCandidates.front().min);
                    i.first->resize(geomCandidates.front().getSize());
                }
                for (const auto & i : _widgetToButton)
                {
                    if (auto button = i.second.lock())
                    {
                        const auto & buttonBBox = button->getGeometry();
                        const auto & minimumSize = i.first->getMinimumSize();
                        std::vector<BBox2f> geomCandidates;
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

            void MenuOverlayLayout::_paintEvent(Event::Paint& event)
            {
                Widget::_paintEvent(event);
                /*if (auto render = _getRender().lock())
                {
                    if (auto style = _getStyle().lock())
                    {
                        const float s = style->getMetric(MetricsRole::Shadow);
                        render->setFillColor(_getColorWithOpacity(style->getColor(ColorRole::Shadow)));
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
                }*/
            }

        } // namespace

        struct Menu::Private
        {
            std::shared_ptr<ValueSubject<std::string> > icon;
            std::shared_ptr<ValueSubject<std::string> > text;
            std::map<size_t, std::shared_ptr<Action> > actions;
            size_t count = 0;
            std::shared_ptr<MenuPopupWidget> popupWidget;
            std::shared_ptr< MenuOverlayLayout> overlayLayout;
            std::shared_ptr<Layout::Overlay> overlay;
            std::function<void(void)> closeCallback;

            std::shared_ptr<Layout::Overlay> createOverlay();
        };

        void Menu::_init(Context * context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            p.icon = ValueSubject<std::string>::create();
            p.text = ValueSubject<std::string>::create();

            p.popupWidget = MenuPopupWidget::create(context);

            p.overlayLayout = MenuOverlayLayout::create(context);
            p.overlayLayout->addWidget(p.popupWidget);

            p.overlay = Layout::Overlay::create(context);
            p.overlay->setCaptureKeyboard(false);
            p.overlay->setFadeIn(false);
            p.overlay->setVisible(true);
            p.overlay->setBackgroundRole(ColorRole::None);
            p.overlay->addWidget(p.overlayLayout);
            p.overlay->setParent(shared_from_this());

            auto weak = std::weak_ptr<Menu>(std::dynamic_pointer_cast<Menu>(shared_from_this()));
            p.popupWidget->setCloseCallback(
                [weak]
            {
                if (auto menu = weak.lock())
                {
                    menu->hide();
                }
            });

            p.overlay->setCloseCallback(
                [weak]
            {
                if (auto menu = weak.lock())
                {
                    menu->hide();
                }
            });
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

        std::shared_ptr<Menu> Menu::create(const std::string & text, Context * context)
        {
            auto out = std::shared_ptr<Menu>(new Menu);
            out->_init(context);
            out->setText(text);
            return out;
        }

        std::shared_ptr<IValueSubject<std::string> > Menu::observeIcon() const
        {
            return _p->icon;
        }

        std::shared_ptr<IValueSubject<std::string> > Menu::observeText() const
        {
            return _p->text;
        }

        void Menu::setIcon(const std::string & value)
        {
            _p->icon->setIfChanged(value);
        }

        void Menu::setText(const std::string & value)
        {
            _p->text->setIfChanged(value);
        }

        void Menu::addSeparator()
        {
            DJV_PRIVATE_PTR();
            p.actions[p.count++] = nullptr;
        }

        void Menu::popup(const glm::vec2 & pos)
        {
            DJV_PRIVATE_PTR();
            if (_p->count > 0)
            {
                if (auto window = getWindow().lock())
                {
                    p.overlayLayout->setPos(p.popupWidget, pos);
                    window->addWidget(std::dynamic_pointer_cast<Widget>(shared_from_this()));
                    show();
                }
            }
        }

        void Menu::popup(const std::weak_ptr<Widget> & button)
        {
            DJV_PRIVATE_PTR();
            if (_p->count > 0)
            {
                if (auto window = getWindow().lock())
                {
                    p.overlayLayout->setButton(p.popupWidget, button);
                    p.overlay->setAnchor(button);
                    window->addWidget(std::dynamic_pointer_cast<Widget>(shared_from_this()));
                    show();
                }
            }
        }

        void Menu::popup(const std::weak_ptr<Widget> & button, const std::weak_ptr<Widget> & anchor)
        {
            DJV_PRIVATE_PTR();
            if (_p->count > 0)
            {
                if (auto window = getWindow().lock())
                {
                    p.overlayLayout->setButton(p.popupWidget, button);
                    p.overlay->setAnchor(anchor);
                    window->addWidget(std::dynamic_pointer_cast<Widget>(shared_from_this()));
                    show();
                }
            }
        }

        void Menu::setCloseCallback(const std::function<void(void)> & callback)
        {
            _p->closeCallback = callback;
        }

        void Menu::setVisible(bool value)
        {
            const bool visible = isVisible();
            Widget::setVisible(value);
            DJV_PRIVATE_PTR();
            if (value != visible && visible && p.closeCallback)
            {
                p.closeCallback();
            }
        }

        void Menu::addAction(const std::shared_ptr<Action> & action)
        {
            Widget::addAction(action);
            DJV_PRIVATE_PTR();
            p.actions[p.count++] = action;
            p.popupWidget->setActions(p.actions);
        }

        void Menu::removeAction(const std::shared_ptr<Action> & action)
        {
            Widget::removeAction(action);
            DJV_PRIVATE_PTR();
            auto i = p.actions.begin();
            while (i != p.actions.end())
            {
                if (i->second == action)
                {
                    i = p.actions.erase(i);
                }
                else
                {
                    ++i;
                }
            }
            p.popupWidget->setActions(p.actions);
        }

        void Menu::clearActions()
        {
            Widget::clearActions();
            DJV_PRIVATE_PTR();
            p.actions.clear();
            p.popupWidget->setActions(p.actions);
        }

        void Menu::_preLayoutEvent(Event::PreLayout &)
        {
            _setMinimumSize(_p->overlay->getMinimumSize());
        }

        void Menu::_layoutEvent(Event::Layout &)
        {
            _p->overlay->setGeometry(getGeometry());
        }

    } // namespace UI
} // namespace djv
