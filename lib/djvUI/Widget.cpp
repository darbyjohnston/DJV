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

#include <djvUI/Widget.h>

#include <djvUI/Action.h>
#include <djvUI/Border.h>
#include <djvUI/IconSystem.h>
#include <djvUI/Shortcut.h>
#include <djvUI/TextBlock.h>
#include <djvUI/Tooltip.h>
#include <djvUI/UISystem.h>
#include <djvUI/Window.h>

#include <djvAV/FontSystem.h>
#include <djvAV/Render2D.h>

#include <djvCore/Math.h>

#include <algorithm>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace
        {
            //! \todo [2.0 S] Should this be configurable?
            static const float tooltipTimeout = .5f;
            static const float tooltipHideDelta = 1.f;

            static size_t currentWidgetCount = 0;

        } // namespace

        std::weak_ptr<AV::Font::System> Widget::_fontSystem;
        std::weak_ptr<AV::Render::Render2D> Widget::_render;
        std::weak_ptr<UISystem> Widget::_uiSystem;
        std::weak_ptr<IconSystem> Widget::_iconSystem;
        bool Widget::_resizeRequest = true;
        bool Widget::_redrawRequest = true;

        void Widget::_init(Core::Context * context)
        {
            IObject::_init(context);
            
            setClassName("djv::UI::Widget");

            /*context->log("djv::UI::Widget", "Widget::Widget");
            context->log("djv::UI::Widget", String::Format("widget count = %%1").arg(currentWidgetCount));*/
            ++currentWidgetCount;

            if (!_fontSystem.lock())
            {
                _fontSystem = context->getSystemT<AV::Font::System>();
            }
            if (!_render.lock())
            {
                _render = context->getSystemT<AV::Render::Render2D>();
            }
            if (!_iconSystem.lock())
            {
                _iconSystem = context->getSystemT<IconSystem>();
            }
            if (!_uiSystem.lock())
            {
                _uiSystem = context->getSystemT<UISystem>();
            }
        }

        Widget::~Widget()
        {
            --currentWidgetCount;
            /*if (auto context = getContext().lock())
            {
              context->log("djv::UI::Widget", "Widget::~Widget");
              context->log("djv::UI::Widget", String::Format("widget count = %%1").arg(currentWidgetCount));
            }*/
        }
        
        size_t Widget::getCurrentWidgetCount()
        {
            return currentWidgetCount;
        }

        std::shared_ptr<Widget> Widget::create(Context * context)
        {
            //! \bug It would be prefereable to use std::make_shared() here, but how can we do that
            //! with protected contructors?
            auto out = std::shared_ptr<Widget>(new Widget);
            out->_init(context);
            return out;
        }

        std::weak_ptr<Window> Widget::getWindow()
        {
            auto widget = std::dynamic_pointer_cast<Widget>(shared_from_this());
            while (widget)
            {
                if (auto parent = std::dynamic_pointer_cast<Widget>(widget->getParent().lock()))
                {
                    widget = parent;
                }
                else
                {
                    break;
                }
            }
            return std::dynamic_pointer_cast<Window>(std::const_pointer_cast<Widget>(widget));
        }

        void Widget::setVisible(bool value)
        {
            if (value == _visible)
                return;
            _visible = value;
			_visibleInit = value;
            _resize();
        }

        void Widget::setOpacity(float value)
        {
            if (fuzzyCompare(value, _opacity))
                return;
            _opacity = value;
            _redraw();
        }

        void Widget::show()
        {
            setVisible(true);
        }

        void Widget::hide()
        {
            setVisible(false);
        }

        void Widget::setGeometry(const BBox2f& value)
        {
            if (fuzzyCompare(value, _geometry))
                return;
            _geometry = value;
            _resize();
        }

        void Widget::move(const glm::vec2& value)
        {
            const glm::vec2 size = _geometry.getSize();
            BBox2f geometry;
            geometry.min = value;
            geometry.max = value + size;
            setGeometry(geometry);
        }

        void Widget::resize(const glm::vec2& value)
        {
            setGeometry(BBox2f(_geometry.min, _geometry.min + value));
        }

        void Widget::setMargin(const Layout::Margin& value)
        {
            if (value == _margin)
                return;
            _margin = value;
            _resize();
        }

        void Widget::setHAlign(HAlign value)
        {
            if (value == _hAlign)
                return;
            _hAlign = value;
            _resize();
        }

        void Widget::setVAlign(VAlign value)
        {
            if (value == _vAlign)
                return;
            _vAlign = value;
            _resize();
        }

        BBox2f Widget::getAlign(const BBox2f& value, const glm::vec2& minimumSize, HAlign hAlign, VAlign vAlign)
        {
            float x = 0.f;
            float y = 0.f;
            float w = 0.f;
            float h = 0.f;
            switch (hAlign)
            {
            case HAlign::Center:
                w = minimumSize.x;
                x = value.min.x + value.w() / 2.f - minimumSize.x / 2.f;
                break;
            case HAlign::Left:
                x = value.min.x;
                w = minimumSize.x;
                break;
            case HAlign::Right:
                w = minimumSize.x;
                x = value.min.x + value.w() - minimumSize.x;
                break;
            case HAlign::Fill:
                x = value.min.x;
                w = value.w();
                break;
            default: break;
            }
            switch (vAlign)
            {
            case VAlign::Center:
                h = minimumSize.y;
                y = value.min.y + value.h() / 2.f - minimumSize.y / 2.f;
                break;
            case VAlign::Top:
                y = value.min.y;
                h = minimumSize.y;
                break;
            case VAlign::Bottom:
                h = minimumSize.y;
                y = value.min.y + value.h() - minimumSize.y;
                break;
            case VAlign::Fill:
                y = value.min.y;
                h = value.h();
                break;
            default: break;
            }
            return BBox2f(floorf(x), floorf(y), ceilf(w), ceilf(h));
        }

        void Widget::setBackgroundRole(ColorRole value)
        {
            if (value == _backgroundRole)
                return;
            _backgroundRole = value;
            _redraw();
        }

        void Widget::setStyle(const std::shared_ptr<Style::Style> & value)
        {
            _customStyle = value;
            _redraw();
        }

        void Widget::setPointerEnabled(bool value)
        {
            _pointerEnabled = value;
        }

        void Widget::addAction(const std::shared_ptr<Action>& action)
        {
            _actions.push_back(action);
        }

        void Widget::removeAction(const std::shared_ptr<Action>& action)
        {
            const auto i = std::find(_actions.begin(), _actions.end(), action);
            if (i != _actions.end())
            {
                _actions.erase(i);
            }
        }

        void Widget::clearActions()
        {
            _actions.clear();
        }

        void Widget::setTooltip(const std::string & value)
        {
            _tooltipText = value;
        }

        bool Widget::event(Event::IEvent& event)
        {
            bool out = IObject::event(event);
            if (!out)
            {
                switch (event.getEventType())
                {
                case Event::Type::ParentChanged:
                {
                    auto& parentChangedEvent = static_cast<Event::ParentChanged&>(event);
                    if (!parentChangedEvent.getPrevParent() && parentChangedEvent.getNewParent())
                    {
                        _styleInit = false;
                    }
					_clipped = parentChangedEvent.getNewParent() ? true : false;
					_clipRect = BBox2f(0.f, 0.f, 0.f, 0.f);
					_redraw();
					break;
				}
				case Event::Type::ChildAdded:
				case Event::Type::ChildRemoved:
                case Event::Type::ChildOrder:
                case Event::Type::Locale:
                    _resize();
                    break;
                case Event::Type::Update:
                {
                    auto& updateEvent = static_cast<Event::Update&>(event);

                    auto style = _customStyle;
                    if (!style)
                    {
                        if (auto parent = std::dynamic_pointer_cast<Widget>(getParent().lock()))
                        {
                            style = parent->_style.lock();
                        }
                    }
                    if (!style)
                    {
                        if (auto uiSystem = _uiSystem.lock())
                        {
                            style = uiSystem->getStyle();
                        }
                    }
                    if (style != _style.lock())
                    {
                        _styleInit = false;
                        _style = style;
                    }

                    _updateTime = updateEvent.getTime();
                    _elapsedTime += updateEvent.getDeltaTime();

                    if (auto window = getWindow().lock())
                    {
                        for (auto & i : _pointerToTooltips)
                        {
                            const auto j = _pointerHover.find(i.first);
                            if ((_updateTime - i.second.timer) > tooltipTimeout && !i.second.tooltip && j != _pointerHover.end())
                            {
                                for (
                                    auto widget = std::dynamic_pointer_cast<Widget>(shared_from_this());
                                    widget;
                                    widget = std::dynamic_pointer_cast<Widget>(widget->getParent().lock()))
                                {
                                    if (auto tooltipWidget = widget->_createTooltip(j->second))
                                    {
                                        i.second.tooltip = Tooltip::create(window, j->second, tooltipWidget, getContext());
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    break;
                }
                case Event::Type::Style:
                    _styleEvent(static_cast<Event::Style&>(event));
                    _resize();
                    break;
                case Event::Type::PreLayout:
					_visibleInit = false;
                    _preLayoutEvent(static_cast<Event::PreLayout&>(event));
                    break;
                case Event::Type::Layout:
                    _layoutEvent(static_cast<Event::Layout&>(event));
                    break;
                case Event::Type::Clip:
                {
                    auto& clipEvent = static_cast<Event::Clip&>(event);
                    if (auto parent = std::dynamic_pointer_cast<Widget>(getParent().lock()))
                    {
                        _parentsVisible = parent->_visible && parent->_parentsVisible;
                        _clipped = !clipEvent.getClipRect().isValid() || !_visible || !parent->_visible || !parent->_parentsVisible;
                        _clipRect = clipEvent.getClipRect();
                    }
                    else
                    {
                        _parentsVisible = true;
                        _clipped = false;
                        _clipRect = BBox2f(0.f, 0.f, 0.f, 0.f);
                    }
                    _clipEvent(clipEvent);
                    break;
                }
                case Event::Type::Paint:
                {
                    if (auto parent = std::dynamic_pointer_cast<Widget>(getParent().lock()))
                    {
                        _parentsOpacity = parent->_opacity * parent->_parentsOpacity;
                    }
                    else
                    {
                        _parentsOpacity = 1.f;
                    }
					if (!_visibleInit)
					{
						_paintEvent(static_cast<Event::Paint&>(event));
					}
                    break;
                }
                case Event::Type::PaintOverlay:
                {
					if (!_visibleInit)
					{
						_paintOverlayEvent(static_cast<Event::PaintOverlay&>(event));
					}
                    break;
                }
                case Event::Type::PointerEnter:
                {
                    auto& pointerEvent = static_cast<Event::PointerEnter&>(event);
                    const auto & info = pointerEvent.getPointerInfo();
                    const auto id = info.id;
                    _pointerHover[id] = info.projectedPos;
                    _pointerToTooltips[id] = TooltipData();
                    _pointerToTooltips[id].timer = _updateTime;
                    _pointerEnterEvent(static_cast<Event::PointerEnter&>(event));
                    break;
                }
                case Event::Type::PointerLeave:
                {
                    auto& pointerEvent = static_cast<Event::PointerLeave&>(event);
                    const auto id = pointerEvent.getPointerInfo().id;
                    const auto i = _pointerHover.find(id);
                    if (i != _pointerHover.end())
                    {
                        _pointerHover.erase(i);
                    }
                    const auto j = _pointerToTooltips.find(id);
                    if (j != _pointerToTooltips.end())
                    {
                        _pointerToTooltips.erase(j);
                    }
                    _pointerLeaveEvent(static_cast<Event::PointerLeave&>(event));
                    break;
                }
                case Event::Type::PointerMove:
                {
                    auto& pointerEvent = static_cast<Event::PointerMove&>(event);
                    const auto & info = pointerEvent.getPointerInfo();
                    const auto id = info.id;
                    const auto i = _pointerToTooltips.find(id);
                    if (i != _pointerToTooltips.end())
                    {
                        const auto delta = info.projectedPos - _pointerHover[id];
                        const float l = glm::length(delta);
                        if (l > tooltipHideDelta)
                        {
                            i->second.tooltip = nullptr;
                            i->second.timer = _updateTime;
                        }
                    }
                    _pointerHover[id] = info.projectedPos;
                    _pointerMoveEvent(static_cast<Event::PointerMove&>(event));
                    break;
                }
                case Event::Type::ButtonPress:
                    _buttonPressEvent(static_cast<Event::ButtonPress&>(event));
                    break;
                case Event::Type::ButtonRelease:
                    _buttonReleaseEvent(static_cast<Event::ButtonRelease&>(event));
                    break;
                case Event::Type::Scroll:
                    _scrollEvent(static_cast<Event::Scroll&>(event));
                    break;
                case Event::Type::Drop:
                    _dropEvent(static_cast<Event::Drop&>(event));
                    break;
                case Event::Type::KeyboardFocus:
                    _keyboardFocusEvent(static_cast<Event::KeyboardFocus&>(event));
                    break;
                case Event::Type::KeyboardFocusLost:
                    _keyboardFocusLostEvent(static_cast<Event::KeyboardFocusLost&>(event));
                    break;
                case Event::Type::KeyPress:
                    _keyPressEvent(static_cast<Event::KeyPress&>(event));
                    break;
                case Event::Type::KeyRelease:
                    _keyReleaseEvent(static_cast<Event::KeyRelease&>(event));
                    break;
                case Event::Type::Text:
                    _textEvent(static_cast<Event::Text&>(event));
                    break;
                default: break;
                }
                out = event.isAccepted();
            }
            return out;
        }

        void Widget::_paintEvent(Event::Paint& event)
        {
            if (_backgroundRole != ColorRole::None)
            {
                if (auto render = _render.lock())
                {
                    if (auto style = _style.lock())
                    {
                        render->setFillColor(_getColorWithOpacity(style->getColor(_backgroundRole)));
                        render->drawRect(getGeometry());
                    }
                }
            }
        }

        void Widget::_pointerEnterEvent(Event::PointerEnter& event)
        {
            if (_pointerEnabled && !event.isRejected())
            {
                event.accept();
            }
        }

        void Widget::_pointerLeaveEvent(Event::PointerLeave& event)
        {
            if (_pointerEnabled)
            {
                event.accept();
            }
        }

        void Widget::_pointerMoveEvent(Event::PointerMove& event)
        {
            if (_pointerEnabled)
            {
                event.accept();
            }
        }

        void Widget::_keyPressEvent(Event::KeyPress& event)
        {
            if (isEnabled(true))
            {
                // Find the shortcuts.
                std::vector<std::shared_ptr<Shortcut> > shortcuts;
                for (const auto& i : _actions)
                {
                    if (i->observeEnabled()->get())
                    {
                        for (auto j : i->observeShortcuts()->get())
                        {
                            shortcuts.push_back(j);
                        }
                    }
                }

                // Sort the actions so that we test those with keyboard modifiers first.
                std::sort(shortcuts.begin(), shortcuts.end(),
                    [](const std::shared_ptr<Shortcut> & a, const std::shared_ptr<Shortcut> & b) -> bool
                {
                    return a->observeShortcutModifiers()->get() > b->observeShortcutModifiers()->get();
                });

                for (const auto& i : shortcuts)
                {
                    const int key = i->observeShortcutKey()->get();
                    const int modifiers = i->observeShortcutModifiers()->get();
                    if ((key == event.getKey() && event.getKeyModifiers() & modifiers) ||
                        (key == event.getKey() && modifiers == 0 && event.getKeyModifiers() == 0))
                    {
                        event.accept();
                        i->doCallback();
                        break;
                    }
                }
            }
        }

        AV::Image::Color Widget::_getColorWithOpacity(const AV::Image::Color & value) const
        {
            AV::Image::Color out(AV::Image::Type::RGBA_F32);
            if (value.getType() != AV::Image::Type::None)
            {
                out = value.convert(AV::Image::Type::RGBA_F32);
                out.setF32(out.getF32(3) * getOpacity(true), 3);
            }
            return out;
        }

        void Widget::_redraw()
        {
            _redrawRequest = true;
        }

        void Widget::_resize()
        {
            _resizeRequest = true;
        }

        void Widget::_setMinimumSize(const glm::vec2& value)
        {
            if (value == _minimumSize)
                return;
            _minimumSize = value;
            _resize();
        }

        std::string Widget::_getTooltipText() const
        {
            std::string out;
            for (const auto & action : _actions)
            {
                const auto & actionTooltip = action->observeTooltip()->get();
                if (!actionTooltip.empty())
                {
                    out = actionTooltip;
                    break;
                }
            }
            if (!_tooltipText.empty())
            {
                out = _tooltipText;
            }
            return out;
        }

        std::shared_ptr<Widget> Widget::_createTooltipDefault(const std::string & text)
        {
            auto context = getContext();
            auto textBlock = TextBlock::create(text, context);
            textBlock->setTextColorRole(ColorRole::TooltipForeground);
            textBlock->setBackgroundRole(ColorRole::TooltipBackground);
            textBlock->setMargin(MetricsRole::Margin);
            auto border = Border::create(context);
            border->addWidget(textBlock);
            return border;
        }

        std::shared_ptr<Widget> Widget::_createTooltip(const glm::vec2 &)
        {
            std::shared_ptr<Widget> out;
            if (!_tooltipText.empty())
            {
                out = _createTooltipDefault(_getTooltipText());
            }
            return out;
        }

    } // namespace UI
} // namespace djv
