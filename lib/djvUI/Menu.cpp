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
#include <djvUI/DrawUtil.h>
#include <djvUI/EventSystem.h>
#include <djvUI/IconSystem.h>
#include <djvUI/LayoutUtil.h>
#include <djvUI/MenuButton.h>
#include <djvUI/Overlay.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/Shortcut.h>
#include <djvUI/Window.h>

#include <djvAV/FontSystem.h>
#include <djvAV/Image.h>
#include <djvAV/Render2D.h>

#include <djvCore/Context.h>
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
                void _init(const std::shared_ptr<Context>&);
                MenuWidget();

            public:
                static std::shared_ptr<MenuWidget> create(const std::shared_ptr<Context>&);

                void setActions(const std::map<size_t, std::shared_ptr<Action> > &);

                void setCloseCallback(const std::function<void(void)> &);

            protected:
                void _styleEvent(Event::Style &) override;
                void _preLayoutEvent(Event::PreLayout &) override;
                void _layoutEvent(Event::Layout &) override;
                void _paintEvent(Event::Paint &) override;
                void _pointerEnterEvent(Event::PointerEnter &) override;
                void _pointerLeaveEvent(Event::PointerLeave &) override;
                void _pointerMoveEvent(Event::PointerMove &) override;
                void _buttonPressEvent(Event::ButtonPress &) override;
                void _buttonReleaseEvent(Event::ButtonRelease &) override;

                std::shared_ptr<Widget> _createTooltip(const glm::vec2 & pos) override;

                void _updateEvent(Event::Update &) override;

            private:
                struct Item
                {
                    ButtonType buttonType = ButtonType::First;
                    bool checked = false;
                    std::shared_ptr<AV::Image::Image> icon;
                    std::string text;
                    std::string font;
                    AV::Font::Metrics fontMetrics;
                    glm::vec2 textSize = glm::vec2(0.F, 0.F);
                    std::string shortcutLabel;
                    glm::vec2 shortcutSize = glm::vec2(0.F, 0.F);
                    bool enabled = true;
                    glm::vec2 size = glm::vec2(0.F, 0.F);
                    BBox2f geom = BBox2f(0.F, 0.F, 0.F, 0.F);
                };

                std::shared_ptr<Item> _getItem(const glm::vec2 &) const;
                void _itemsUpdate();
                void _textUpdate();

                std::shared_ptr<AV::Font::System> _fontSystem;
                std::map<size_t, std::shared_ptr<Action> > _actions;
                bool _hasIcons = false;
                bool _hasShortcuts = false;
                std::map<size_t, std::shared_ptr<Item> > _items;
                std::map<std::shared_ptr<Action>, std::shared_ptr<Item> > _actionToItem;
                std::map<std::shared_ptr<Item>, std::shared_ptr<Action> > _itemToAction;
                std::map<std::shared_ptr<Item>, bool> _hasIcon;
                std::map<std::shared_ptr<Item>, std::future<std::shared_ptr<AV::Image::Image> > > _iconFutures;
                std::map<std::shared_ptr<Item>, std::future<AV::Font::Metrics> > _fontMetricsFutures;
                std::map<std::shared_ptr<Item>, std::future<glm::vec2> > _textSizeFutures;
                std::map<std::shared_ptr<Item>, std::future<glm::vec2> > _shortcutSizeFutures;
                std::map<Event::PointerID, std::shared_ptr<Item> > _hoveredItems;
                std::pair<Event::PointerID, std::shared_ptr<Item> > _pressed;
                glm::vec2 _pressedPos = glm::vec2(0.F, 0.F);
                std::function<void(void)> _closeCallback;
                std::map<std::shared_ptr<Item>, std::shared_ptr<ValueObserver<ButtonType> > > _buttonTypeObservers;
                std::map<std::shared_ptr<Item>, std::shared_ptr<ValueObserver<bool> > > _checkedObservers;
                std::map<std::shared_ptr<Item>, std::shared_ptr<ValueObserver<std::string> > > _iconObservers;
                std::map<std::shared_ptr<Item>, std::shared_ptr<ValueObserver<std::string> > > _textObservers;
                std::map<std::shared_ptr<Item>, std::shared_ptr<ValueObserver<std::string> > > _fontObservers;
                std::map<std::shared_ptr<Item>, std::shared_ptr<ListObserver<std::shared_ptr<Shortcut> > > > _shortcutsObservers;
                std::map<std::shared_ptr<Item>, std::shared_ptr<ValueObserver<bool> > > _enabledObservers;
                bool _textUpdateRequest = false;
            };

            void MenuWidget::_init(const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);
                setClassName("djv::UI::MenuWidget");
                _fontSystem = context->getSystemT<AV::Font::System>();
            }

            MenuWidget::MenuWidget()
            {}

            std::shared_ptr<MenuWidget> MenuWidget::create(const std::shared_ptr<Context>& context)
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

            void MenuWidget::_styleEvent(Event::Style &)
            {
                _itemsUpdate();
            }

            void MenuWidget::_preLayoutEvent(Event::PreLayout &)
            {
                const auto& style = _getStyle();
                const float m = style->getMetric(MetricsRole::MarginSmall);
                const float s = style->getMetric(MetricsRole::Spacing);
                const float b = style->getMetric(MetricsRole::Border);
                const float is = style->getMetric(MetricsRole::Icon);
                const float iss = style->getMetric(MetricsRole::IconSmall);

                for (auto& i : _iconFutures)
                {
                    if (i.second.valid())
                    {
                        try
                        {
                            i.first->icon = i.second.get();
                        }
                        catch (const std::exception& e)
                        {
                            _log(e.what(), LogLevel::Error);
                        }
                    }
                }
                for (auto& i : _fontMetricsFutures)
                {
                    if (i.second.valid())
                    {
                        try
                        {
                            i.first->fontMetrics = i.second.get();
                        }
                        catch (const std::exception& e)
                        {
                            _log(e.what(), LogLevel::Error);
                        }
                    }
                }
                for (auto & i : _textSizeFutures)
                {
                    if (i.second.valid())
                    {
                        try
                        {
                            i.first->textSize = i.second.get();
                        }
                        catch (const std::exception & e)
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
                        }
                        catch (const std::exception & e)
                        {
                            _log(e.what(), LogLevel::Error);
                        }
                    }
                }

                glm::vec2 textSize(0.F, 0.F);
                glm::vec2 shortcutSize(0.F, 0.F);
                for (const auto & i : _items)
                {
                    const auto j = _itemToAction.find(i.second);
                    if (j != _itemToAction.end() && j->second)
                    {
                        textSize.x = std::max(textSize.x, i.second->textSize.x);
                        textSize.y = std::max(textSize.y, i.second->textSize.y);
                    }
                    if (!i.second->shortcutLabel.empty())
                    {
                        shortcutSize.x = std::max(shortcutSize.x, i.second->shortcutSize.x);
                        shortcutSize.y = std::max(shortcutSize.y, i.second->shortcutSize.y);
                    }
                }

                glm::vec2 itemSize(0.F, 0.F);
                itemSize.x += iss + m * 2.F;
                itemSize.y = std::max(itemSize.y, iss + m * 2.F);
                if (_hasIcons)
                {
                    itemSize.x += is;
                    itemSize.y = std::max(itemSize.y, is);
                }
                itemSize.x += textSize.x + m * 2.F;
                itemSize.y = std::max(itemSize.y, textSize.y + m * 2.F);
                if (_hasShortcuts)
                {
                    itemSize.x += shortcutSize.x + m * 2.F;
                    itemSize.y = std::max(itemSize.y, shortcutSize.y + m * 2.F);
                }

                std::map<size_t, glm::vec2> sizes;
                for (auto & i : _items)
                {
                    glm::vec2 size(0.F, 0.F);
                    const auto j = _itemToAction.find(i.second);
                    if (j != _itemToAction.end() && j->second)
                    {
                        size = itemSize + m * 2.F;
                    }
                    else
                    {
                        size = glm::vec2(b, b);
                    }
                    i.second->size = size;
                    sizes[i.first] = size;
                }

                glm::vec2 size(0.F, 0.F);
                for (const auto & i : sizes)
                {
                    size.x = std::max(size.x, i.second.x);
                    size.y += i.second.y;
                }
                _setMinimumSize(size);
            }

            void MenuWidget::_layoutEvent(Event::Layout &)
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

            void MenuWidget::_paintEvent(Event::Paint & event)
            {
                Widget::_paintEvent(event);

                const BBox2f & g = getGeometry();
                const auto& style = _getStyle();
                const float m = style->getMetric(MetricsRole::MarginSmall);
                const float s = style->getMetric(MetricsRole::Spacing);
                const float b = style->getMetric(MetricsRole::Border);
                const float is = style->getMetric(MetricsRole::Icon);
                const float iss = style->getMetric(MetricsRole::IconSmall);

                auto render = _getRender();
                for (const auto & i : _items)
                {
                    if (i.second->enabled)
                    {
                        if (i.second == _pressed.second)
                        {
                            render->setFillColor(style->getColor(ColorRole::Pressed));
                            render->drawRect(i.second->geom);
                        }
                        else
                        {
                            for (const auto & hovered : _hoveredItems)
                            {
                                if (i.second == hovered.second)
                                {
                                    render->setFillColor(style->getColor(ColorRole::Hovered));
                                    render->drawRect(i.second->geom);
                                    break;
                                }
                            }
                        }
                    }
                }

                for (const auto & i : _items)
                {
                    float x = i.second->geom.min.x + m;
                    float y = 0.F;

                    render->setAlphaMult(i.second->enabled ? 1.F : style->getPalette().getDisabledMult());

                    switch (i.second->buttonType)
                    {
                    case ButtonType::Toggle:
                    case ButtonType::Radio:
                    case ButtonType::Exclusive:
                    {
                        const BBox2f checkBoxGeometry(
                            x + m,
                            floorf(i.second->geom.min.y + ceilf(i.second->size.y / 2.F - iss / 2.F)),
                            iss,
                            iss);
                        render->setFillColor(style->getColor(ColorRole::Border));
                        drawBorder(render, checkBoxGeometry, b);
                        render->setFillColor(style->getColor(i.second->checked ? ColorRole::Checked : ColorRole::Trough));
                        render->drawRect(checkBoxGeometry.margin(-b));
                        break;
                    }
                    default: break;
                    }
                    x += iss + m * 2.F;

                    if (_hasIcons)
                    {
                        render->setFillColor(style->getColor(ColorRole::Foreground));
                        if (i.second->icon)
                        {
                            y = i.second->geom.min.y + ceilf(i.second->size.y / 2.F - is / 2.F);
                            render->drawFilledImage(i.second->icon, glm::vec2(x, y));
                        }
                        x += is;
                    }

                    render->setFillColor(style->getColor(ColorRole::Foreground));
                    const auto j = _itemToAction.find(i.second);
                    if (j != _itemToAction.end() && j->second)
                    {
                        y = i.second->geom.min.y + ceilf(i.second->size.y / 2.F) - ceilf(i.second->fontMetrics.lineHeight / 2.F) + i.second->fontMetrics.ascender;
                        const auto fontInfo = i.second->font.empty() ?
                            style->getFontInfo(AV::Font::faceDefault, MetricsRole::FontMedium) :
                            style->getFontInfo(i.second->font, AV::Font::faceDefault, MetricsRole::FontMedium);
                        render->setCurrentFont(fontInfo);
                        render->drawText(i.second->text, glm::vec2(x + m, y));
                        x += i.second->textSize.x + m * 2.F;

                        if (!i.second->shortcutLabel.empty())
                        {
                            x = g.max.x - i.second->shortcutSize.x - m;
                            render->drawText(i.second->shortcutLabel, glm::vec2(x, y));
                        }
                    }
                    else
                    {
                        render->setFillColor(style->getColor(ColorRole::Border));
                        render->drawRect(i.second->geom);
                    }
                }
            }

            void MenuWidget::_pointerEnterEvent(Event::PointerEnter & event)
            {
                event.accept();
                const auto & pointerInfo = event.getPointerInfo();
                if (auto item = _getItem(pointerInfo.projectedPos))
                {
                    _hoveredItems[pointerInfo.id] = item;
                }
                _redraw();
            }

            void MenuWidget::_pointerLeaveEvent(Event::PointerLeave & event)
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

            void MenuWidget::_pointerMoveEvent(Event::PointerMove & event)
            {
                event.accept();
                const auto & pointerInfo = event.getPointerInfo();
                const auto id = pointerInfo.id;
                const auto & pos = pointerInfo.projectedPos;
                if (id == _pressed.first)
                {
                    const float distance = glm::length(pos - _pressedPos);
                    const auto& style = _getStyle();
                    const bool accepted = distance < style->getMetric(MetricsRole::Drag);
                    event.setAccepted(accepted);
                    if (!accepted)
                    {
                        _pressed.first = 0;
                        _pressed.second = nullptr;
                        _redraw();
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

            void MenuWidget::_buttonPressEvent(Event::ButtonPress & event)
            {
                if (_pressed.second)
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

            void MenuWidget::_buttonReleaseEvent(Event::ButtonRelease & event)
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
                            break;
                        case ButtonType::Radio:
                            if (!i->second->observeChecked()->get())
                            {
                                i->second->setChecked(true);
                            }
                            break;
                        case ButtonType::Exclusive:
                            i->second->setChecked(!i->second->observeChecked()->get());
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

            void MenuWidget::_updateEvent(Event::Update &)
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
                    const auto j = _itemToAction.find(i.second);
                    if (i.second->geom.contains(pos) && j != _itemToAction.end() && j->second)
                    {
                        out = i.second;
                        break;
                    }
                }
                return out;
            }

            void MenuWidget::_itemsUpdate()
            {
                _hasIcons = false;
                _hasShortcuts = false;
                _items.clear();
                _actionToItem.clear();
                _itemToAction.clear();
                _hasIcon.clear();
                _iconFutures.clear();
                _fontMetricsFutures.clear();
                _textSizeFutures.clear();
                _shortcutSizeFutures.clear();
                _buttonTypeObservers.clear();
                _checkedObservers.clear();
                _iconObservers.clear();
                _textObservers.clear();
                _fontObservers.clear();
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
                                    widget->_redraw();
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
                            [weak, item](const std::string& value)
                            {
                                if (auto widget = weak.lock())
                                {
                                    widget->_hasIcon[item] = !value.empty();
                                    widget->_hasIcons = false;
                                    for (const auto& i : widget->_hasIcon)
                                    {
                                        widget->_hasIcons |= i.second;
                                    }
                                    if (widget->_hasIcon[item])
                                    {
                                        if (auto context = widget->getContext().lock())
                                        {
                                            auto iconSystem = context->getSystemT<IconSystem>();
                                            auto style = widget->_getStyle();
                                            widget->_iconFutures[item] = iconSystem->getIcon(value, static_cast<int>(style->getMetric(MetricsRole::Icon)));
                                            widget->_resize();
                                        }
                                    }
                                }
                            });
                        _textObservers[item] = ValueObserver<std::string>::create(
                            i.second->observeText(),
                            [weak, item](const std::string& value)
                            {
                                if (auto widget = weak.lock())
                                {
                                    item->text = value;
                                    widget->_textUpdateRequest = true;
                                }
                            });
                        _fontObservers[item] = ValueObserver<std::string>::create(
                            i.second->observeFont(),
                            [weak, item](const std::string& value)
                        {
                            if (auto widget = weak.lock())
                            {
                                item->font = value;
                                widget->_textUpdateRequest = true;
                            }
                        });
                        _shortcutsObservers[item] = ListObserver<std::shared_ptr<Shortcut> >::create(
                            i.second->observeShortcuts(),
                            [weak, item](const std::vector<std::shared_ptr<Shortcut> > & value)
                        {
                            if (auto widget = weak.lock())
                            {
                                if (auto context = widget->getContext().lock())
                                {
                                    auto textSystem = context->getSystemT<TextSystem>();
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
                _textUpdate();
            }

            void MenuWidget::_textUpdate()
            {
                _textUpdateRequest = false;
                const auto& style = _getStyle();
                auto textSystem = _getTextSystem();
                _hasShortcuts = false;
                for (const auto & i : _items)
                {
                    const auto fontInfo = i.second->font.empty() ?
                        style->getFontInfo(AV::Font::faceDefault, MetricsRole::FontMedium) :
                        style->getFontInfo(i.second->font, AV::Font::faceDefault, MetricsRole::FontMedium);
                    _fontMetricsFutures[i.second] = _fontSystem->getMetrics(fontInfo);
                    _textSizeFutures[i.second] = _fontSystem->measure(i.second->text, fontInfo);
                    _shortcutSizeFutures[i.second] = _fontSystem->measure(i.second->shortcutLabel, fontInfo);
                    _hasShortcuts |= i.second->shortcutLabel.size() > 0;
                }
            }

            class MenuPopupWidget : public Widget
            {
                DJV_NON_COPYABLE(MenuPopupWidget);

            protected:
                void _init(const std::shared_ptr<Context>&);
                MenuPopupWidget();

            public:
                static std::shared_ptr<MenuPopupWidget> create(const std::shared_ptr<Context>&);

                void setActions(const std::map<size_t, std::shared_ptr<Action> > &);

                MetricsRole getMinimumSizeRole() const;
                void setMinimumSizeRole(MetricsRole);

                void setCloseCallback(const std::function<void(void)> &);

            protected:
                void _preLayoutEvent(Event::PreLayout &) override;
                void _layoutEvent(Event::Layout &) override;
                void _buttonPressEvent(Event::ButtonPress &) override;
                void _buttonReleaseEvent(Event::ButtonRelease &) override;

            private:
                std::shared_ptr<ScrollWidget> _scrollWidget;
                std::shared_ptr<MenuWidget> _menuWidget;
            };

            void MenuPopupWidget::_init(const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);
                setClassName("djv::UI::MenuPopupWidget");
                setPointerEnabled(true);

                _menuWidget = MenuWidget::create(context);

                _scrollWidget = ScrollWidget::create(ScrollType::Vertical, context);
                _scrollWidget->addChild(_menuWidget);
                addChild(_scrollWidget);
            }

            MenuPopupWidget::MenuPopupWidget()
            {}

            std::shared_ptr<MenuPopupWidget> MenuPopupWidget::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr< MenuPopupWidget>(new MenuPopupWidget);
                out->_init(context);
                return out;
            }

            void MenuPopupWidget::setActions(const std::map<size_t, std::shared_ptr<Action> > & actions)
            {
                _menuWidget->setActions(actions);
            }

            MetricsRole MenuPopupWidget::getMinimumSizeRole() const
            {
                return  _scrollWidget->getMinimumSizeRole();
            }

            void MenuPopupWidget::setMinimumSizeRole(MetricsRole value)
            {
                _scrollWidget->setMinimumSizeRole(value);
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
                const BBox2f& g = getGeometry();
                _scrollWidget->setGeometry(g);
            }

            class MenuLayout : public Widget
            {
                DJV_NON_COPYABLE(MenuLayout);

            protected:
                void _init(const std::shared_ptr<Context>&);
                MenuLayout();

            public:
                static std::shared_ptr<MenuLayout> create(const std::shared_ptr<Context>&);

                void setPos(const std::shared_ptr<Widget> &, const glm::vec2 &);
                void setButton(const std::shared_ptr<Widget> &, const std::weak_ptr<Button::Menu> &);

                void removeChild(const std::shared_ptr<IObject> &) override;

            protected:
                void _layoutEvent(Event::Layout &) override;
                void _paintEvent(Event::Paint &) override;

            private:
                std::map<std::shared_ptr<Widget>, glm::vec2> _widgetToPos;
                std::map<std::shared_ptr<Widget>, std::weak_ptr<Button::Menu> > _widgetToButton;
            };

            void MenuLayout::_init(const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);
                setClassName("djv::UI::MenuLayout");
            }

            MenuLayout::MenuLayout()
            {}

            std::shared_ptr<MenuLayout> MenuLayout::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<MenuLayout>(new MenuLayout);
                out->_init(context);
                return out;
            }

            void MenuLayout::setPos(const std::shared_ptr<Widget> & widget, const glm::vec2 & pos)
            {
                _widgetToPos[widget] = pos;
            }

            void MenuLayout::setButton(const std::shared_ptr<Widget> & widget, const std::weak_ptr<Button::Menu> & button)
            {
                _widgetToButton[widget] = button;
            }

            void MenuLayout::removeChild(const std::shared_ptr<IObject> & value)
            {
                Widget::removeChild(value);
                if (auto widget = std::dynamic_pointer_cast<Widget>(value))
                {
                    const auto i = _widgetToPos.find(widget);
                    if (i != _widgetToPos.end())
                    {
                        _widgetToPos.erase(i);
                    }
                    const auto j = _widgetToButton.find(widget);
                    if (j != _widgetToButton.end())
                    {
                        _widgetToButton.erase(j);
                    }
                }
            }

            void MenuLayout::_layoutEvent(Event::Layout &)
            {
                const auto& style = _getStyle();
                const BBox2f & g = getMargin().bbox(getGeometry(), style);
                for (const auto & i : _widgetToPos)
                {
                    const auto & pos = i.second;
                    const auto & minimumSize = i.first->getMinimumSize();
                    i.first->setGeometry(Layout::getPopupGeometry(g, pos, minimumSize));
                }
                for (const auto & i : _widgetToButton)
                {
                    if (auto button = i.second.lock())
                    {
                        const auto & buttonBBox = button->getGeometry();
                        const auto & minimumSize = i.first->getMinimumSize();
                        const BBox2f popupGeometry = Layout::getPopupGeometry(g, buttonBBox, minimumSize);
                        i.first->setGeometry(popupGeometry);
                    }
                }
            }

            void MenuLayout::_paintEvent(Event::Paint & event)
            {
                Widget::_paintEvent(event);
                const auto& style = _getStyle();
                const float sh = style->getMetric(MetricsRole::Shadow);
                auto render = _getRender();
                render->setFillColor(style->getColor(ColorRole::Shadow));
                for (const auto & i : getChildWidgets())
                {
                    BBox2f g = i->getGeometry();
                    g.min.x -= sh;
                    g.max.x += sh;
                    g.max.y += sh;
                    if (g.isValid())
                    {
                        render->drawShadow(g, sh);
                    }
                }
            }

        } // namespace

        struct Menu::Private
        {
            std::shared_ptr<ValueSubject<std::string> > icon;
            std::shared_ptr<ValueSubject<std::string> > text;
            std::map<size_t, std::shared_ptr<Action> > actions;
            size_t count = 0;
            MetricsRole minimumSizeRole = MetricsRole::ScrollArea;
            ColorRole backgroundRole = ColorRole::Background;
            std::shared_ptr<MenuPopupWidget> popupWidget;
            std::shared_ptr<MenuLayout> layout;
            std::shared_ptr<Layout::Overlay> overlay;
            std::function<void(void)> closeCallback;

            void createOverlay();
        };

        void Menu::_init(const std::shared_ptr<Context>& context)
        {
            IObject::_init(context);

            DJV_PRIVATE_PTR();
            p.icon = ValueSubject<std::string>::create();
            p.text = ValueSubject<std::string>::create();
        }

        Menu::Menu() :
            _p(new Private)
        {}

        Menu::~Menu()
        {}

        std::shared_ptr<Menu> Menu::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<Menu>(new Menu);
            out->_init(context);
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

        void Menu::addAction(const std::shared_ptr<Action>& action)
        {
            DJV_PRIVATE_PTR();
            p.actions[p.count++] = action;
        }

        void Menu::removeAction(const std::shared_ptr<Action>& action)
        {
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
        }

        void Menu::clearActions()
        {
            DJV_PRIVATE_PTR();
            p.actions.clear();
        }

        void Menu::addSeparator()
        {
            DJV_PRIVATE_PTR();
            p.actions[p.count++] = nullptr;
        }

        MetricsRole Menu::getMinimumSizeRole() const
        {
            return _p->minimumSizeRole;
        }

        void Menu::setMinimumSizeRole(MetricsRole value)
        {
            _p->minimumSizeRole = value;
        }
        
        ColorRole Menu::getBackgroundRole() const
        {
            return _p->backgroundRole;
        }

        void Menu::setBackgroundRole(ColorRole value)
        {
            if (value == _p->backgroundRole)
                return;
            _p->backgroundRole = value;
            if (_p->popupWidget)
            {
                _p->popupWidget->setBackgroundRole(_p->backgroundRole);
            }
        }

        void Menu::popup(const std::shared_ptr<Window>& window, const glm::vec2 & pos)
        {
            DJV_PRIVATE_PTR();
            if (p.count > 0)
            {
                if (p.overlay)
                {
                    if (auto parent = p.overlay->getParent().lock())
                    {
                        parent->removeChild(p.overlay);
                    }
                    p.overlay.reset();
                }
                _createWidgets();
                p.layout->setPos(p.popupWidget, pos);
                window->addChild(p.overlay);
                p.overlay->show();
            }
        }

        void Menu::popup(const std::shared_ptr<Window>& window, const std::weak_ptr<Button::Menu> & button)
        {
            DJV_PRIVATE_PTR();
            if (p.count > 0)
            {
                if (p.overlay)
                {
                    if (auto parent = p.overlay->getParent().lock())
                    {
                        parent->removeChild(p.overlay);
                    }
                    p.overlay.reset();
                }
                _createWidgets();
                p.layout->setButton(p.popupWidget, button);
                p.overlay->setAnchor(button);
                window->addChild(p.overlay);
                p.overlay->show();
            }
        }

        void Menu::popup(const std::shared_ptr<Window>& window, const std::weak_ptr<Button::Menu> & button, const std::weak_ptr<Widget> & anchor)
        {
            DJV_PRIVATE_PTR();
            if (p.count > 0)
            {
                if (p.overlay)
                {
                    if (auto parent = p.overlay->getParent().lock())
                    {
                        parent->removeChild(p.overlay);
                    }
                    p.overlay.reset();
                }
                _createWidgets();
                p.layout->setButton(p.popupWidget, button);
                p.overlay->setAnchor(anchor);
                window->addChild(p.overlay);
                p.overlay->show();
            }
        }

        bool Menu::isOpen() const
        {
            return _p->overlay.get();
        }

        void Menu::close()
        {
            DJV_PRIVATE_PTR();
            if (p.overlay)
            {
                if (auto parent = p.overlay->getParent().lock())
                {
                    parent->removeChild(p.overlay);
                }
                p.overlay.reset();
                if (p.closeCallback)
                {
                    p.closeCallback();
                }
            }
        }

        void Menu::setCloseCallback(const std::function<void(void)> & callback)
        {
            _p->closeCallback = callback;
        }

        void Menu::_createWidgets()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                p.popupWidget = MenuPopupWidget::create(context);
                p.popupWidget->setActions(p.actions);
                p.popupWidget->setMinimumSizeRole(p.minimumSizeRole);
                p.popupWidget->setBackgroundRole(p.backgroundRole);

                p.layout = MenuLayout::create(context);
                p.layout->setMargin(Layout::Margin(MetricsRole::None, MetricsRole::None, MetricsRole::Margin, MetricsRole::Margin));
                p.layout->addChild(p.popupWidget);

                p.overlay = Layout::Overlay::create(context);
                p.overlay->setFadeIn(false);
                p.overlay->setBackgroundRole(ColorRole::None);
                p.overlay->addChild(p.layout);

                auto weak = std::weak_ptr<Menu>(std::dynamic_pointer_cast<Menu>(shared_from_this()));
                p.popupWidget->setCloseCallback(
                    [weak]
                    {
                        if (auto menu = weak.lock())
                        {
                            menu->close();
                        }
                    });

                p.overlay->setCloseCallback(
                    [weak]
                    {
                        if (auto menu = weak.lock())
                        {
                            menu->close();
                        }
                    });
            }
        }

    } // namespace UI
} // namespace djv
