// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/Menu.h>

#include <djvUI/Action.h>
#include <djvUI/DrawUtil.h>
#include <djvUI/EventSystem.h>
#include <djvUI/ITooltipWidget.h>
#include <djvUI/IconSystem.h>
#include <djvUI/LayoutUtil.h>
#include <djvUI/MenuButton.h>
#include <djvUI/Overlay.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/Shortcut.h>
#include <djvUI/ShortcutData.h>
#include <djvUI/Window.h>

#include <djvRender2D/FontSystem.h>
#include <djvRender2D/Render.h>

#include <djvImage/Image.h>

#include <djvSystem/Context.h>
#include <djvSystem/TextSystem.h>

#include <djvCore/StringFunc.h>

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
                void _init(const std::shared_ptr<System::Context>&);
                MenuWidget();

            public:
                static std::shared_ptr<MenuWidget> create(const std::shared_ptr<System::Context>&);

                void setActions(const std::map<size_t, std::shared_ptr<Action> >&);

                void setCloseCallback(const std::function<void(void)>&);

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;
                void _paintEvent(System::Event::Paint&) override;
                void _pointerEnterEvent(System::Event::PointerEnter&) override;
                void _pointerLeaveEvent(System::Event::PointerLeave&) override;
                void _pointerMoveEvent(System::Event::PointerMove&) override;
                void _buttonPressEvent(System::Event::ButtonPress&) override;
                void _buttonReleaseEvent(System::Event::ButtonRelease&) override;

                std::shared_ptr<ITooltipWidget> _createTooltip(const glm::vec2& pos) override;

                void _initEvent(System::Event::Init&) override;
                void _updateEvent(System::Event::Update&) override;

            private:
                struct Item
                {
                    ButtonType buttonType = ButtonType::First;
                    bool checked = false;
                    std::shared_ptr<Image::Image> icon;
                    std::string text;
                    std::string font;
                    Render2D::Font::FontInfo fontInfo;
                    Render2D::Font::Metrics fontMetrics;
                    glm::vec2 textSize = glm::vec2(0.F, 0.F);
                    std::vector<std::shared_ptr<Render2D::Font::Glyph> > textGlyphs;
                    std::string shortcutLabel;
                    glm::vec2 shortcutSize = glm::vec2(0.F, 0.F);
                    std::vector<std::shared_ptr<Render2D::Font::Glyph> > shortcutGlyphs;
                    bool enabled = true;
                    glm::vec2 size = glm::vec2(0.F, 0.F);
                    Math::BBox2f geom = Math::BBox2f(0.F, 0.F, 0.F, 0.F);
                };

                std::shared_ptr<Item> _getItem(const glm::vec2&) const;
                void _itemsUpdate();
                void _textUpdate();

                std::shared_ptr<Render2D::Font::FontSystem> _fontSystem;
                std::map<size_t, std::shared_ptr<Action> > _actions;
                bool _hasIcons = false;
                bool _hasShortcuts = false;
                std::map<size_t, std::shared_ptr<Item> > _items;
                std::map<std::shared_ptr<Action>, std::shared_ptr<Item> > _actionToItem;
                std::map<std::shared_ptr<Item>, std::shared_ptr<Action> > _itemToAction;
                std::map<std::shared_ptr<Item>, bool> _hasIcon;
                std::map<std::shared_ptr<Item>, std::future<std::shared_ptr<Image::Image> > > _iconFutures;
                std::map<std::shared_ptr<Item>, std::future<Render2D::Font::Metrics> > _fontMetricsFutures;
                std::map<std::shared_ptr<Item>, std::future<glm::vec2> > _textSizeFutures;
                std::map<std::shared_ptr<Item>, std::future<std::vector<std::shared_ptr<Render2D::Font::Glyph> > > > _textGlyphsFutures;
                std::map<std::shared_ptr<Item>, std::future<glm::vec2> > _shortcutSizeFutures;
                std::map<std::shared_ptr<Item>, std::future<std::vector<std::shared_ptr<Render2D::Font::Glyph> > > > _shortcutGlyphsFutures;
                std::map<System::Event::PointerID, std::shared_ptr<Item> > _hoveredItems;
                std::pair<System::Event::PointerID, std::shared_ptr<Item> > _pressed;
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

            void MenuWidget::_init(const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);
                setClassName("djv::UI::MenuWidget");
                _fontSystem = context->getSystemT<Render2D::Font::FontSystem>();
            }

            MenuWidget::MenuWidget()
            {}

            std::shared_ptr<MenuWidget> MenuWidget::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<MenuWidget>(new MenuWidget);
                out->_init(context);
                return out;
            }

            void MenuWidget::setActions(const std::map<size_t, std::shared_ptr<Action> >& actions)
            {
                _actions = actions;
                _itemsUpdate();
            }

            void MenuWidget::setCloseCallback(const std::function<void(void)>& value)
            {
                _closeCallback = value;
            }

            void MenuWidget::_preLayoutEvent(System::Event::PreLayout&)
            {
                const auto& style = _getStyle();
                const float m = style->getMetric(MetricsRole::MarginInside);
                const float s = style->getMetric(MetricsRole::Spacing);
                const float b = style->getMetric(MetricsRole::Border);
                const float is = style->getMetric(MetricsRole::Icon);

                glm::vec2 textSize(0.F, 0.F);
                glm::vec2 shortcutSize(0.F, 0.F);
                for (const auto& i : _items)
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
                const glm::vec2 checkBoxSize = getCheckBoxSize(style);
                itemSize.x += checkBoxSize.x + m * 2.F;
                itemSize.y = std::max(itemSize.y, checkBoxSize.y + m * 2.F);
                if (_hasIcons)
                {
                    itemSize.x += is;
                    itemSize.y = std::max(itemSize.y, is);
                }
                itemSize.x += textSize.x + m * 2.F;
                itemSize.y = std::max(itemSize.y, textSize.y + m * 2.F);
                if (_hasShortcuts)
                {
                    itemSize.x += s + shortcutSize.x + m * 2.F;
                    itemSize.y = std::max(itemSize.y, shortcutSize.y + m * 2.F);
                }

                std::map<size_t, glm::vec2> sizes;
                for (auto& i : _items)
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
                for (const auto& i : sizes)
                {
                    size.x = std::max(size.x, i.second.x);
                    size.y += i.second.y;
                }
                _setMinimumSize(size + b * 2.F);
            }

            void MenuWidget::_layoutEvent(System::Event::Layout&)
            {
                const auto& style = _getStyle();
                const float b = style->getMetric(MetricsRole::Border);
                const Math::BBox2f g = getGeometry().margin(-b);
                float y = g.min.y;
                for (auto& i : _items)
                {
                    i.second->geom.min.x = g.min.x;
                    i.second->geom.min.y = y;
                    i.second->geom.max.x = g.max.x;
                    i.second->geom.max.y = y + i.second->size.y;
                    y += i.second->size.y;
                }
            }

            void MenuWidget::_paintEvent(System::Event::Paint& event)
            {
                Widget::_paintEvent(event);

                const auto& style = _getStyle();
                const float s = style->getMetric(MetricsRole::Spacing);
                const float m = style->getMetric(MetricsRole::MarginInside);
                const float b = style->getMetric(MetricsRole::Border);
                const float is = style->getMetric(MetricsRole::Icon);
                const Math::BBox2f& g = getGeometry();

                const auto& render = _getRender();
                render->setFillColor(style->getColor(ColorRole::Border));
                drawBorder(render, g, b);
                const Math::BBox2f g2 = g.margin(-b);

                for (const auto& i : _items)
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
                            for (const auto& hovered : _hoveredItems)
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

                for (const auto& i : _items)
                {
                    float x = i.second->geom.min.x + m;
                    float y = 0.F;

                    render->setAlphaMult(i.second->enabled ? 1.F : style->getPalette().getDisabledMult());

                    const glm::vec2 checkBoxSize = getCheckBoxSize(style);
                    switch (i.second->buttonType)
                    {
                    case ButtonType::Toggle:
                    case ButtonType::Radio:
                    case ButtonType::Exclusive:
                    {
                        const Math::BBox2f checkBoxGeometry(
                            x + m,
                            floorf(i.second->geom.min.y + ceilf(i.second->size.y / 2.F - checkBoxSize.y / 2.F)),
                            checkBoxSize.x,
                            checkBoxSize.y);
                        drawCheckBox(render, style, checkBoxGeometry, i.second->checked);
                        break;
                    }
                    default: break;
                    }
                    x += checkBoxSize.x + m * 2.F;

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
                        render->drawText(i.second->textGlyphs, glm::vec2(x + m, y));
                        x += i.second->textSize.x + m * 2.F;

                        if (!i.second->shortcutLabel.empty())
                        {
                            x = g2.max.x - i.second->shortcutSize.x - m;
                            render->drawText(i.second->shortcutGlyphs, glm::vec2(x, y));
                        }
                    }
                    else
                    {
                        render->setFillColor(style->getColor(ColorRole::Border));
                        render->drawRect(i.second->geom);
                    }

                    render->setAlphaMult(1.F);
                }
            }

            void MenuWidget::_pointerEnterEvent(System::Event::PointerEnter& event)
            {
                event.accept();
                const auto& pointerInfo = event.getPointerInfo();
                if (auto item = _getItem(pointerInfo.projectedPos))
                {
                    _hoveredItems[pointerInfo.id] = item;
                }
                _redraw();
            }

            void MenuWidget::_pointerLeaveEvent(System::Event::PointerLeave& event)
            {
                event.accept();
                const auto& pointerInfo = event.getPointerInfo();
                const auto i = _hoveredItems.find(pointerInfo.id);
                if (i != _hoveredItems.end())
                {
                    _hoveredItems.erase(i);
                }
                _redraw();
            }

            void MenuWidget::_pointerMoveEvent(System::Event::PointerMove& event)
            {
                event.accept();
                const auto& pointerInfo = event.getPointerInfo();
                const auto id = pointerInfo.id;
                const auto& pos = pointerInfo.projectedPos;
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

            void MenuWidget::_buttonPressEvent(System::Event::ButtonPress& event)
            {
                if (_pressed.second)
                    return;
                const auto& pointerInfo = event.getPointerInfo();
                const auto id = pointerInfo.id;
                const auto& pos = pointerInfo.projectedPos;
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

            void MenuWidget::_buttonReleaseEvent(System::Event::ButtonRelease& event)
            {
                const auto& pointerInfo = event.getPointerInfo();
                const auto id = pointerInfo.id;
                const auto& pos = pointerInfo.projectedPos;
                if (_pressed.second)
                {
                    const auto i = _itemToAction.find(_pressed.second);
                    if (i != _itemToAction.end())
                    {
                        i->second->doClick();
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

            std::shared_ptr<ITooltipWidget> MenuWidget::_createTooltip(const glm::vec2& pos)
            {
                std::shared_ptr<ITooltipWidget> out;
                std::string text;
                for (const auto& i : _items)
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
                if (!text.empty())
                {
                    out = _createTooltipDefault();
                    out->setTooltip(text);
                }
                return out;
            }

            void MenuWidget::_initEvent(System::Event::Init& event)
            {
                if (event.getData().resize || event.getData().font)
                {
                    _itemsUpdate();
                }
            }

            void MenuWidget::_updateEvent(System::Event::Update&)
            {
                if (_textUpdateRequest)
                {
                    _textUpdate();
                }
                for (auto& i : _iconFutures)
                {
                    if (i.second.valid() &&
                        i.second.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                    {
                        try
                        {
                            i.first->icon = i.second.get();
                            _resize();
                        }
                        catch (const std::exception& e)
                        {
                            _log(e.what(), System::LogLevel::Error);
                        }
                    }
                }
                for (auto& i : _fontMetricsFutures)
                {
                    if (i.second.valid() &&
                        i.second.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                    {
                        try
                        {
                            i.first->fontMetrics = i.second.get();
                            _resize();
                        }
                        catch (const std::exception& e)
                        {
                            _log(e.what(), System::LogLevel::Error);
                        }
                    }
                }
                for (auto& i : _textSizeFutures)
                {
                    if (i.second.valid() &&
                        i.second.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                    {
                        try
                        {
                            i.first->textSize = i.second.get();
                            _resize();
                        }
                        catch (const std::exception& e)
                        {
                            _log(e.what(), System::LogLevel::Error);
                        }
                    }
                }
                for (auto& i : _textGlyphsFutures)
                {
                    if (i.second.valid() &&
                        i.second.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                    {
                        try
                        {
                            i.first->textGlyphs = i.second.get();
                            _resize();
                        }
                        catch (const std::exception& e)
                        {
                            _log(e.what(), System::LogLevel::Error);
                        }
                    }
                }
                for (auto& i : _shortcutSizeFutures)
                {
                    if (i.second.valid() &&
                        i.second.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                    {
                        try
                        {
                            i.first->shortcutSize = i.second.get();
                            _resize();
                        }
                        catch (const std::exception& e)
                        {
                            _log(e.what(), System::LogLevel::Error);
                        }
                    }
                }
                for (auto& i : _shortcutGlyphsFutures)
                {
                    if (i.second.valid() &&
                        i.second.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                    {
                        try
                        {
                            i.first->shortcutGlyphs = i.second.get();
                            _resize();
                        }
                        catch (const std::exception& e)
                        {
                            _log(e.what(), System::LogLevel::Error);
                        }
                    }
                }
            }

            std::shared_ptr<MenuWidget::Item> MenuWidget::_getItem(const glm::vec2& pos) const
            {
                std::shared_ptr<MenuWidget::Item> out;
                for (const auto& i : _items)
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
                _textGlyphsFutures.clear();
                _shortcutSizeFutures.clear();
                _shortcutGlyphsFutures.clear();
                _buttonTypeObservers.clear();
                _checkedObservers.clear();
                _iconObservers.clear();
                _textObservers.clear();
                _fontObservers.clear();
                _shortcutsObservers.clear();
                _enabledObservers.clear();
                auto weak = std::weak_ptr<MenuWidget>(std::dynamic_pointer_cast<MenuWidget>(shared_from_this()));
                auto contextWeak = getContext();
                for (const auto& i : _actions)
                {
                    auto item = std::shared_ptr<Item>(new Item);
                    if (i.second)
                    {
                        _buttonTypeObservers[item] = ValueObserver<ButtonType>::create(
                            i.second->observeButtonType(),
                            [item, weak](ButtonType value)
                            {
                                if (auto widget = weak.lock())
                                {
                                    item->buttonType = value;
                                    widget->_redraw();
                                }
                            });
                        _checkedObservers[item] = ValueObserver<bool>::create(
                            i.second->observeChecked(),
                            [item, weak](bool value)
                            {
                                if (auto widget = weak.lock())
                                {
                                    item->checked = value;
                                    widget->_redraw();
                                }
                            });
                        _iconObservers[item] = ValueObserver<std::string>::create(
                            i.second->observeIcon(),
                            [item, weak](const std::string& value)
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
                                            widget->_iconFutures[item] = iconSystem->getIcon(value, style->getMetric(MetricsRole::Icon));
                                            widget->_resize();
                                        }
                                    }
                                }
                            });
                        _textObservers[item] = ValueObserver<std::string>::create(
                            i.second->observeText(),
                            [item, weak](const std::string& value)
                            {
                                if (auto widget = weak.lock())
                                {
                                    item->text = value;
                                    widget->_textUpdateRequest = true;
                                }
                            });
                        _fontObservers[item] = ValueObserver<std::string>::create(
                            i.second->observeFont(),
                            [item, weak](const std::string& value)
                        {
                            if (auto widget = weak.lock())
                            {
                                item->font = value;
                                widget->_textUpdateRequest = true;
                            }
                        });
                        _shortcutsObservers[item] = ListObserver<std::shared_ptr<Shortcut> >::create(
                            i.second->observeShortcuts(),
                            [item, weak, contextWeak](const std::vector<std::shared_ptr<Shortcut> >& value)
                        {
                            if (auto context = contextWeak.lock())
                            {
                                if (auto widget = weak.lock())
                                {
                                    auto textSystem = context->getSystemT<System::TextSystem>();
                                    std::vector<std::string> labels;
                                    for (const auto& i : value)
                                    {
                                        const auto& shortcut = i->observeShortcut()->get();
                                        if (shortcut.isValid())
                                        {
                                            labels.push_back(ShortcutData::getText(shortcut.key, shortcut.modifiers, textSystem));
                                        }
                                    }
                                    item->shortcutLabel = String::join(labels, ", ");
                                    widget->_textUpdateRequest = true;
                                }
                            }
                        });
                        _enabledObservers[item] = ValueObserver<bool>::create(
                            i.second->observeEnabled(),
                            [item, weak](bool value)
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
                for (auto& i : _items)
                {
                    i.second->fontInfo = i.second->font.empty() ?
                        style->getFontInfo(Render2D::Font::faceDefault, MetricsRole::FontMedium) :
                        style->getFontInfo(i.second->font, Render2D::Font::faceDefault, MetricsRole::FontMedium);
                    _fontMetricsFutures[i.second] = _fontSystem->getMetrics(i.second->fontInfo);
                    _textSizeFutures[i.second] = _fontSystem->measure(i.second->text, i.second->fontInfo);
                    _textGlyphsFutures[i.second] = _fontSystem->getGlyphs(i.second->text, i.second->fontInfo);
                    _shortcutSizeFutures[i.second] = _fontSystem->measure(i.second->shortcutLabel, i.second->fontInfo);
                    _shortcutGlyphsFutures[i.second] = _fontSystem->getGlyphs(i.second->shortcutLabel, i.second->fontInfo);
                    _hasShortcuts |= i.second->shortcutLabel.size() > 0;
                }
            }

            class MenuPopupWidget : public Widget
            {
                DJV_NON_COPYABLE(MenuPopupWidget);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                MenuPopupWidget();

            public:
                static std::shared_ptr<MenuPopupWidget> create(const std::shared_ptr<System::Context>&);

                void setActions(const std::map<size_t, std::shared_ptr<Action> >&);

                MetricsRole getMinimumSizeRole() const;
                void setMinimumSizeRole(MetricsRole);

                void setCloseCallback(const std::function<void(void)>&);

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;
                void _buttonPressEvent(System::Event::ButtonPress&) override;
                void _buttonReleaseEvent(System::Event::ButtonRelease&) override;

            private:
                std::shared_ptr<ScrollWidget> _scrollWidget;
                std::shared_ptr<MenuWidget> _menuWidget;
            };

            void MenuPopupWidget::_init(const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);
                setClassName("djv::UI::MenuPopupWidget");
                setPointerEnabled(true);

                _menuWidget = MenuWidget::create(context);

                _scrollWidget = ScrollWidget::create(ScrollType::Vertical, context);
                _scrollWidget->setBorder(false);
                _scrollWidget->addChild(_menuWidget);
                addChild(_scrollWidget);
            }

            MenuPopupWidget::MenuPopupWidget()
            {}

            std::shared_ptr<MenuPopupWidget> MenuPopupWidget::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr< MenuPopupWidget>(new MenuPopupWidget);
                out->_init(context);
                return out;
            }

            void MenuPopupWidget::setActions(const std::map<size_t, std::shared_ptr<Action> >& actions)
            {
                _menuWidget->setActions(actions);
            }

            MetricsRole MenuPopupWidget::getMinimumSizeRole() const
            {
                return _scrollWidget->getMinimumSizeRole();
            }

            void MenuPopupWidget::setMinimumSizeRole(MetricsRole value)
            {
                _scrollWidget->setMinimumSizeRole(value);
            }

            void MenuPopupWidget::setCloseCallback(const std::function<void(void)>& callback)
            {
                _menuWidget->setCloseCallback(callback);
            }

            void MenuPopupWidget::_buttonPressEvent(System::Event::ButtonPress& event)
            {
                event.accept();
            }

            void MenuPopupWidget::_buttonReleaseEvent(System::Event::ButtonRelease& event)
            {
                event.accept();
            }

            void MenuPopupWidget::_preLayoutEvent(System::Event::PreLayout&)
            {
                _setMinimumSize(_scrollWidget->getMinimumSize());
            }

            void MenuPopupWidget::_layoutEvent(System::Event::Layout&)
            {
                const Math::BBox2f& g = getGeometry();
                _scrollWidget->setGeometry(g);
            }

            class MenuLayout : public Widget
            {
                DJV_NON_COPYABLE(MenuLayout);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                MenuLayout();

            public:
                static std::shared_ptr<MenuLayout> create(const std::shared_ptr<System::Context>&);

                void setPos(const std::shared_ptr<MenuPopupWidget>&, const glm::vec2&);
                void setButton(const std::shared_ptr<MenuPopupWidget>&, const std::weak_ptr<Button::Menu>&);


            protected:
                void _layoutEvent(System::Event::Layout&) override;
                void _paintEvent(System::Event::Paint&) override;

                void _childRemovedEvent(System::Event::ChildRemoved&) override;

            private:
                std::map<std::shared_ptr<MenuPopupWidget>, glm::vec2> _widgetToPos;
                std::map<std::shared_ptr<MenuPopupWidget>, std::weak_ptr<Button::Menu> > _widgetToButton;
                std::map<std::shared_ptr<MenuPopupWidget>, Popup> _widgetToPopup;
            };

            void MenuLayout::_init(const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);
                setClassName("djv::UI::MenuLayout");
            }

            MenuLayout::MenuLayout()
            {}

            std::shared_ptr<MenuLayout> MenuLayout::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<MenuLayout>(new MenuLayout);
                out->_init(context);
                return out;
            }

            void MenuLayout::setPos(const std::shared_ptr<MenuPopupWidget>& widget, const glm::vec2& pos)
            {
                _widgetToPos[widget] = pos;
            }

            void MenuLayout::setButton(const std::shared_ptr<MenuPopupWidget>& widget, const std::weak_ptr<Button::Menu>& button)
            {
                _widgetToButton[widget] = button;
            }

            void MenuLayout::_layoutEvent(System::Event::Layout&)
            {
                const auto& style = _getStyle();
                const Math::BBox2f& g = getMargin().bbox(getGeometry(), style);
                for (const auto& i : _widgetToPos)
                {
                    const auto& pos = i.second;
                    const auto& minimumSize = i.first->getMinimumSize();
                    Popup popup = Popup::BelowRight;
                    auto j = _widgetToPopup.find(i.first);
                    if (j != _widgetToPopup.end())
                    {
                        popup = j->second;
                    }
                    else
                    {
                        popup = Layout::getPopup(popup, g, pos, minimumSize);
                        _widgetToPopup[i.first] = popup;
                    }
                    const Math::BBox2f popupGeometry = Layout::getPopupGeometry(popup, pos, minimumSize);
                    i.first->setGeometry(popupGeometry.intersect(g));
                }
                for (const auto& i : _widgetToButton)
                {
                    if (auto button = i.second.lock())
                    {
                        const auto& buttonBBox = button->getGeometry();
                        const auto& minimumSize = i.first->getMinimumSize();
                        Popup popup = Popup::BelowRight;
                        auto j = _widgetToPopup.find(i.first);
                        if (j != _widgetToPopup.end())
                        {
                            popup = j->second;
                        }
                        else
                        {
                            popup = Layout::getPopup(popup, g, buttonBBox, minimumSize);
                        }
                        const Math::BBox2f popupGeometry = Layout::getPopupGeometry(popup, buttonBBox, minimumSize);
                        i.first->setGeometry(popupGeometry.intersect(g));
                    }
                }
            }

            void MenuLayout::_paintEvent(System::Event::Paint& event)
            {
                Widget::_paintEvent(event);
                const auto& style = _getStyle();
                const float sh = style->getMetric(MetricsRole::Shadow);
                const auto& render = _getRender();
                render->setFillColor(style->getColor(ColorRole::Shadow));
                for (const auto& i : getChildWidgets())
                {
                    Math::BBox2f g = i->getGeometry();
                    g.min.x -= sh;
                    g.max.x += sh;
                    g.max.y += sh;
                    if (g.isValid())
                    {
                        render->drawShadow(g, sh);
                    }
                }
            }

            void MenuLayout::_childRemovedEvent(System::Event::ChildRemoved& event)
            {
                if (auto widget = std::dynamic_pointer_cast<MenuPopupWidget>(event.getChild()))
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
                    const auto k = _widgetToPopup.find(widget);
                    if (k != _widgetToPopup.end())
                    {
                        _widgetToPopup.erase(k);
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
            std::shared_ptr<Window> window;
            std::function<void(void)> closeCallback;

            void createOverlay();
        };

        void Menu::_init(const std::shared_ptr<System::Context>& context)
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
        {
            DJV_PRIVATE_PTR();
            if (p.window)
            {
                p.window->close();
            }
        }

        std::shared_ptr<Menu> Menu::create(const std::shared_ptr<System::Context>& context)
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

        void Menu::setIcon(const std::string& value)
        {
            _p->icon->setIfChanged(value);
        }

        void Menu::setText(const std::string& value)
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
            DJV_PRIVATE_PTR();
            if (value == p.backgroundRole)
                return;
            p.backgroundRole = value;
            if (p.popupWidget)
            {
                p.popupWidget->setBackgroundRole(p.backgroundRole);
            }
        }

        std::shared_ptr<Widget> Menu::popup(const glm::vec2& pos)
        {
            DJV_PRIVATE_PTR();
            std::shared_ptr<Widget> out;
            _createWidgets();
            if (p.popupWidget)
            {
                p.layout->setPos(p.popupWidget, pos);
            }
            p.overlay->show();
            p.window->show();
            out = p.overlay;
            return out;
        }

        std::shared_ptr<Widget> Menu::popup(const std::weak_ptr<Button::Menu>& button)
        {
            DJV_PRIVATE_PTR();
            std::shared_ptr<Widget> out;
            _createWidgets();
            if (p.popupWidget)
            {
                p.layout->setButton(p.popupWidget, button);
            }
            p.overlay->show();
            p.window->show();
            out = p.overlay;
            return out;
        }

        std::shared_ptr<Widget> Menu::popup(const std::weak_ptr<Button::Menu>& button, const std::weak_ptr<Widget>& anchor)
        {
            DJV_PRIVATE_PTR();
            std::shared_ptr<Widget> out;
            _createWidgets();
            if (p.popupWidget)
            {
                p.layout->setButton(p.popupWidget, button);
            }
            p.overlay->show();
            p.window->show();
            out = p.overlay;
            return out;
        }

        bool Menu::isOpen() const
        {
            return _p->overlay.get();
        }

        void Menu::close()
        {
            DJV_PRIVATE_PTR();
            if (p.window)
            {
                p.popupWidget.reset();
                p.layout.reset();
                p.overlay.reset();
                p.window->close();
                p.window.reset();
                if (p.closeCallback)
                {
                    p.closeCallback();
                }
            }
        }

        void Menu::setCloseCallback(const std::function<void(void)>& callback)
        {
            _p->closeCallback = callback;
        }

        void Menu::_createWidgets()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                if (p.window)
                {
                    p.window->close();
                    p.window.reset();
                }

                if (p.count)
                {
                    p.popupWidget = MenuPopupWidget::create(context);
                    p.popupWidget->setActions(p.actions);
                    p.popupWidget->setMinimumSizeRole(p.minimumSizeRole);
                    p.popupWidget->setBackgroundRole(p.backgroundRole);
                }
                
                p.layout = MenuLayout::create(context);
                p.layout->setMargin(Layout::Margin(MetricsRole::None, MetricsRole::None, MetricsRole::Margin, MetricsRole::Margin));
                if (p.popupWidget)
                {
                    p.layout->addChild(p.popupWidget);
                }

                p.overlay = Layout::Overlay::create(context);
                p.overlay->setFadeIn(false);
                p.overlay->setBackgroundRole(ColorRole::None);
                p.overlay->addChild(p.layout);

                p.window = Window::create(context);
                p.window->setBackgroundRole(ColorRole::None);
                p.window->addChild(p.overlay);

                auto weak = std::weak_ptr<Menu>(std::dynamic_pointer_cast<Menu>(shared_from_this()));
                if (p.popupWidget)
                {
                    p.popupWidget->setCloseCallback(
                        [weak]
                        {
                            if (auto menu = weak.lock())
                            {
                                menu->close();
                            }
                        });
                }
                
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
