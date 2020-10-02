// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/Widget.h>

#include <djvUI/Action.h>
#include <djvUI/EventSystem.h>
#include <djvUI/ITooltipWidget.h>
#include <djvUI/Shortcut.h>
#include <djvUI/ShortcutDataFunc.h>
#include <djvUI/Style.h>
#include <djvUI/TextBlock.h>
#include <djvUI/Tooltip.h>
#include <djvUI/UISystem.h>
#include <djvUI/Window.h>

#include <djvRender2D/Render.h>

#include <djvSystem/Context.h>

#include <djvMath/Math.h>

#include <djvCore/StringFunc.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <algorithm>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace
        {
            //! \todo Should this be configurable?
            const Time::Duration tooltipTimeout = std::chrono::milliseconds(500);
            const float tooltipHideDelta = 1.F;

            std::atomic<size_t> globalWidgetCount(0);

            class DefaultTooltipWidget : public ITooltipWidget
            {
            protected:
                void _init(const std::shared_ptr<System::Context>& context)
                {
                    ITooltipWidget::_init(context);
                    _textBlock = Text::Block::create(context);
                    _textBlock->setTextColorRole(ColorRole::TooltipForeground);
                    _textBlock->setBackgroundRole(ColorRole::TooltipBackground);
                    _textBlock->setMargin(MetricsRole::Margin);
                    addChild(_textBlock);
                }

            public:
                static std::shared_ptr<DefaultTooltipWidget> create(const std::shared_ptr<System::Context>& context)
                {
                    auto out = std::shared_ptr<DefaultTooltipWidget>(new DefaultTooltipWidget);
                    out->_init(context);
                    return out;
                }

                void setTooltip(const std::string& value) override
                {
                    _textBlock->setText(value);
                }

                float getHeightForWidth(float value) const override
                {
                    return _textBlock->getHeightForWidth(value);
                }

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override
                {
                    _setMinimumSize(_textBlock->getMinimumSize());
                }

                void _layoutEvent(System::Event::Layout&) override
                {
                    _textBlock->setGeometry(getGeometry());
                }

            private:
                std::shared_ptr<Text::Block> _textBlock;
            };

        } // namespace

        void Widget::_init(const std::shared_ptr<System::Context>& context)
        {
            IObject::_init(context);
            
            setClassName("djv::UI::Widget");

            ++globalWidgetCount;

            _eventSystem = context->getSystemT<EventSystem>();
            _render = context->getSystemT<Render2D::Render>();
            auto uiSystem = context->getSystemT<UISystem>();
            _style = uiSystem->getStyle();
        }

        Widget::Widget()
        {}

        Widget::~Widget()
        {
            --globalWidgetCount;
        }
        
        std::shared_ptr<Widget> Widget::create(const std::shared_ptr<System::Context>& context)
        {
            //! \bug It would be prefereable to use std::make_shared() here, but how can we do that
            //! with protected contructors?
            auto out = std::shared_ptr<Widget>(new Widget);
            out->_init(context);
            return out;
        }

        std::shared_ptr<Window> Widget::getWindow() const
        {
            return getParentRecursiveT<Window>();
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
            if (value == _opacity)
                return;
            _opacity = value;
            _resize();
        }

        void Widget::setGeometry(const Math::BBox2f& value)
        {
            if (value == _geometry)
                return;
            _geometry = value;
            _resize();
        }

        void Widget::move(const glm::vec2& value)
        {
            const glm::vec2 size = _geometry.getSize();
            Math::BBox2f geometry;
            geometry.min = value;
            geometry.max = value + size;
            setGeometry(geometry);
        }

        void Widget::resize(const glm::vec2& value)
        {
            setGeometry(Math::BBox2f(_geometry.min, _geometry.min + value));
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

        void Widget::setBackgroundRole(ColorRole value)
        {
            if (value == _backgroundRole)
                return;
            _backgroundRole = value;
            _redraw();
        }

        void Widget::setShadowOverlay(const std::set<Side>& value)
        {
            if (value == _shadowOverlay)
                return;
            _shadowOverlay = value;
            _redraw();
        }

        void Widget::setPointerEnabled(bool value)
        {
            _pointerEnabled = value;
        }

        std::shared_ptr<Widget> Widget::getFocusWidget()
        {
            return std::dynamic_pointer_cast<Widget>(shared_from_this());
        }

        bool Widget::acceptFocus(TextFocusDirection direction)
        {
            bool out = false;
            if (isEnabled(true) && isVisible(true) && !isClipped())
            {
                const auto& childWidgets = getChildWidgets();
                const size_t size = childWidgets.size();
                switch (direction)
                {
                case TextFocusDirection::Next:
                    for (size_t i = 0; i < size; ++i)
                    {
                        if (childWidgets[i]->acceptFocus(direction))
                        {
                            out = true;
                            break;
                        }
                    }
                    break;
                case TextFocusDirection::Prev:
                    for (int i = static_cast<int>(size) - 1; i >= 0; --i)
                    {
                        if (childWidgets[i]->acceptFocus(direction))
                        {
                            out = true;
                            break;
                        }
                    }
                    break;
                default: break;
                }
            }
            return out;
        }

        bool Widget::hasTextFocus()
        {
            bool out = false;
            if (auto eventSystem = _getEventSystem().lock())
            {
                out = eventSystem->getTextFocus().lock() == getFocusWidget();
            }
            return out;
        }

        void Widget::takeTextFocus()
        {
            if (auto eventSystem = _getEventSystem().lock())
            {
                eventSystem->setTextFocus(getFocusWidget());
            }
        }

        void Widget::releaseTextFocus()
        {
            if (auto eventSystem = _getEventSystem().lock())
            {
                if (eventSystem->getTextFocus().lock() == getFocusWidget())
                {
                    eventSystem->setTextFocus(nullptr);
                }
            }
        }

        void Widget::nextTextFocus(const std::shared_ptr<Widget>& widget)
        {
            // Find the next sibling that can accept focus.
            const auto& childWidgets = getChildWidgets();
            const int size = static_cast<int>(childWidgets.size());
            bool accepted = false;
            auto i = std::find_if(
                childWidgets.begin(),
                childWidgets.end(),
                [widget](const std::shared_ptr<Widget>& value)
                {
                    return widget && widget->getFocusWidget() == value->getFocusWidget();
                });
            if (i != childWidgets.end())
            {
                for (int j = static_cast<int>(i - childWidgets.begin()) + 1; j < size; ++j)
                {
                    if (childWidgets[j]->getFocusWidget()->acceptFocus(TextFocusDirection::Next))
                    {
                        accepted = true;
                        break;
                    }
                }
            }

            if (!accepted)
            {
                if (auto parent = std::dynamic_pointer_cast<Widget>(getParent().lock()))
                {
                    // Try the parent widget.
                    parent->nextTextFocus(std::dynamic_pointer_cast<Widget>(shared_from_this()));
                }
                else
                {
                    // Try the child widgets.
                    for (int j = 0; j < size; ++j)
                    {
                        if (childWidgets[j]->getFocusWidget()->acceptFocus(TextFocusDirection::Next))
                        {
                            break;
                        }
                    }
                }
            }
        }

        void Widget::prevTextFocus(const std::shared_ptr<Widget>& widget)
        {
            // Find the previous sibling that can accept focus.
            const auto& childWidgets = getChildWidgets();
            const int size = static_cast<int>(childWidgets.size());
            bool accepted = false;
            auto i = std::find_if(
                childWidgets.begin(),
                childWidgets.end(),
                [widget](const std::shared_ptr<Widget>& value)
                {
                    return widget && widget->getFocusWidget() == value->getFocusWidget();
                });
            if (i != childWidgets.end())
            {
                for (int j = static_cast<int>(i - childWidgets.begin()) - 1; j >= 0; --j)
                {
                    if (childWidgets[j]->getFocusWidget()->acceptFocus(TextFocusDirection::Prev))
                    {
                        accepted = true;
                        break;
                    }
                }
            }
            if (!accepted)
            {
                if (auto parent = std::dynamic_pointer_cast<Widget>(getParent().lock()))
                {
                    // Try the parent widget.
                    parent->prevTextFocus(std::dynamic_pointer_cast<Widget>(shared_from_this()));
                }
                else
                {
                    // Try the child widgets.
                    for (int j = size - 1; j >= 0; --j)
                    {
                        if (childWidgets[j]->getFocusWidget()->acceptFocus(TextFocusDirection::Prev))
                        {
                            break;
                        }
                    }
                }
            }
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

        void Widget::setTooltip(const std::string& value)
        {
            _tooltipText = value;
            for (auto& i : _pointerToTooltips)
            {
                if (auto tooltip = i.second.tooltip)
                {
                    tooltip->getWidget()->setTooltip(_tooltipText);
                }
            }
            setPointerEnabled(true);
        }

        size_t Widget::getGlobalWidgetCount()
        {
            return globalWidgetCount;
        }

        void Widget::moveToFront()
        {
            IObject::moveToFront();
            if (auto parent = std::dynamic_pointer_cast<Widget>(getParent().lock()))
            {
                auto object = std::dynamic_pointer_cast<Widget>(shared_from_this());
                auto& siblings = parent->_childWidgets;
                const auto i = std::find(siblings.begin(), siblings.end(), object);
                if (i != siblings.end())
                {
                    siblings.erase(i);
                }
                siblings.push_back(object);
            }
        }

        void Widget::moveToBack()
        {
            IObject::moveToBack();
            if (auto parent = std::dynamic_pointer_cast<Widget>(getParent().lock()))
            {
                auto object = std::dynamic_pointer_cast<Widget>(shared_from_this());
                auto& siblings = parent->_childWidgets;
                const auto i = std::find(siblings.begin(), siblings.end(), object);
                if (i != siblings.end())
                {
                    siblings.erase(i);
                }
                siblings.insert(siblings.begin(), object);
            }
        }

        void Widget::setEnabled(bool value)
        {
            IObject::setEnabled(value);
            if (!value)
            {
                releaseTextFocus();
            }
        }

        bool Widget::event(System::Event::Event& event)
        {
            bool out = IObject::event(event);
            if (!out)
            {
                switch (event.getEventType())
                {
                case System::Event::Type::ParentChanged:
                {
                    auto& parentChangedEvent = static_cast<System::Event::ParentChanged&>(event);
                    const bool newParent = parentChangedEvent.getNewParent() ? true : false;
                    if (!newParent)
                    {
                        if (auto eventSystem = _getEventSystem().lock())
                        {
                            auto parent = shared_from_this();
                            auto widget = eventSystem->getTextFocus().lock();
                            while (widget)
                            {
                                if (widget == parent)
                                {
                                    eventSystem->setTextFocus(nullptr);
                                    break;
                                }
                                widget = widget->getParent().lock();
                            }
                        }
                    }
                    _clipped = newParent;
                    _clipRect = Math::BBox2f(0.F, 0.F, 0.F, 0.F);
                    _redraw();
                    break;
                }
                case System::Event::Type::ChildAdded:
                {
                    auto& childAddedEvent = static_cast<System::Event::ChildAdded&>(event);
                    if (auto widget = std::dynamic_pointer_cast<Widget>(childAddedEvent.getChild()))
                    {
                        const auto i = std::find(_childWidgets.begin(), _childWidgets.end(), widget);
                        if (i != _childWidgets.end())
                        {
                            _childWidgets.erase(i);
                        }
                        _childWidgets.push_back(widget);
                    }
                    _resize();
                    break;
                }
                case System::Event::Type::ChildRemoved:
                {
                    auto& childRemovedEvent = static_cast<System::Event::ChildRemoved&>(event);
                    if (auto widget = std::dynamic_pointer_cast<Widget>(childRemovedEvent.getChild()))
                    {
                        const auto i = std::find(_childWidgets.begin(), _childWidgets.end(), widget);
                        if (i != _childWidgets.end())
                        {
                            _childWidgets.erase(i);
                        }
                    }
                    _resize();
                    break;
                }
                case System::Event::Type::ChildOrder:
                case System::Event::Type::Init:
                    _resize();
                    break;
                case System::Event::Type::Update:
                {
                    auto& updateEvent = static_cast<System::Event::Update&>(event);
                    _updateTime = updateEvent.getTime();

                    _tooltipsToDelete.clear();
                    if (!_pointerToTooltips.empty())
                    {
                        if (auto context = getContext().lock())
                        {
                            if (auto eventSystem = _eventSystem.lock())
                            {
                                const bool tooltipsEnabled = eventSystem->areTooltipsEnabled();
                                if (tooltipsEnabled)
                                {
                                    for (auto& i : _pointerToTooltips)
                                    {
                                        const auto j = _pointerHover.find(i.first);
                                        const auto t = std::chrono::duration_cast<std::chrono::milliseconds>(_updateTime - i.second.timer);
                                        const auto& g = getGeometry();
                                        if (t > tooltipTimeout &&
                                            !i.second.tooltip &&
                                            j != _pointerHover.end() &&
                                            g.contains(j->second))
                                        {
                                            for (
                                                auto widget = std::dynamic_pointer_cast<Widget>(shared_from_this());
                                                widget;
                                                widget = std::dynamic_pointer_cast<Widget>(widget->getParent().lock()))
                                            {
                                                if (auto tooltipWidget = widget->_createTooltip(j->second))
                                                {
                                                    i.second.tooltip = Tooltip::create(getWindow(), j->second, tooltipWidget, context);
                                                    break;
                                                }
                                            }

                                        }
                                    }
                                }
                            }
                        }
                    }
                    break;
                }
                case System::Event::Type::InitLayout:
                    _visibleInit = false;
                    _initLayoutEvent(static_cast<System::Event::InitLayout&>(event));
                    break;
                case System::Event::Type::PreLayout:
                    _preLayoutEvent(static_cast<System::Event::PreLayout&>(event));
                    break;
                case System::Event::Type::Layout:
                    _layoutEvent(static_cast<System::Event::Layout&>(event));
                    break;
                case System::Event::Type::Clip:
                {
                    auto& clipEvent = static_cast<System::Event::Clip&>(event);
                    if (auto parent = std::dynamic_pointer_cast<Widget>(getParent().lock()))
                    {
                        _parentsVisible = parent->_visible && parent->_parentsVisible;
                        _clipped =
                            !clipEvent.getClipRect().isValid() ||
                            !_visible ||
                            !_parentsVisible ||
                            0.F == _opacity ||
                            0.F == _parentsOpacity;
                        _clipRect = clipEvent.getClipRect();
                    }
                    else
                    {
                        _parentsVisible = true;
                        _clipped = false;
                        _clipRect = Math::BBox2f(0.F, 0.F, 0.F, 0.F);
                    }
                    if (_clipped)
                    {
                        for (auto& i : _pointerToTooltips)
                        {
                            _tooltipsToDelete.insert(i.second.tooltip);
                            i.second.tooltip.reset();
                            i.second.timer = _updateTime;
                        }
                        releaseTextFocus();
                    }
                    _clipEvent(clipEvent);
                    break;
                }
                case System::Event::Type::Paint:
                {
                    if (auto parent = std::dynamic_pointer_cast<Widget>(getParent().lock()))
                    {
                        _parentsOpacity = parent->_opacity * parent->_parentsOpacity;
                    }
                    else
                    {
                        _parentsOpacity = 1.F;
                    }
                    if (!_visibleInit)
                    {
                        float opacity = getOpacity(true);
                        opacity *= isEnabled(true) ? 1.F : _style->getPalette().getDisabledMult();
                        _render->setAlphaMult(opacity);
                        _paintEvent(static_cast<System::Event::Paint&>(event));
                        _render->setAlphaMult(1.F);
                    }
                    break;
                }
                case System::Event::Type::PaintOverlay:
                {
                    if (!_visibleInit)
                    {
                        _paintOverlayEvent(static_cast<System::Event::PaintOverlay&>(event));
                    }
                    break;
                }
                case System::Event::Type::PointerEnter:
                {
                    auto& pointerEvent = static_cast<System::Event::PointerEnter&>(event);
                    const auto& info = pointerEvent.getPointerInfo();
                    const auto id = info.id;
                    _pointerHover[id] = info.projectedPos;
                    _pointerToTooltips[id] = TooltipData();
                    _pointerToTooltips[id].timer = _updateTime;
                    _pointerEnterEvent(static_cast<System::Event::PointerEnter&>(event));
                    break;
                }
                case System::Event::Type::PointerLeave:
                {
                    auto& pointerEvent = static_cast<System::Event::PointerLeave&>(event);
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
                    _pointerLeaveEvent(static_cast<System::Event::PointerLeave&>(event));
                    break;
                }
                case System::Event::Type::PointerMove:
                {
                    auto& pointerEvent = static_cast<System::Event::PointerMove&>(event);
                    const auto& info = pointerEvent.getPointerInfo();
                    const auto id = info.id;
                    const auto i = _pointerToTooltips.find(id);
                    if (i != _pointerToTooltips.end())
                    {
                        const auto delta = info.projectedPos - _pointerHover[id];
                        const float l = glm::length(delta);
                        if (l > tooltipHideDelta)
                        {
                            _tooltipsToDelete.insert(i->second.tooltip);
                            i->second.tooltip.reset();
                            i->second.timer = _updateTime;
                        }
                    }
                    _pointerHover[id] = info.projectedPos;
                    _pointerMoveEvent(static_cast<System::Event::PointerMove&>(event));
                    break;
                }
                case System::Event::Type::ButtonPress:
                    _buttonPressEvent(static_cast<System::Event::ButtonPress&>(event));
                    break;
                case System::Event::Type::ButtonRelease:
                    _buttonReleaseEvent(static_cast<System::Event::ButtonRelease&>(event));
                    break;
                case System::Event::Type::Scroll:
                    _scrollEvent(static_cast<System::Event::Scroll&>(event));
                    break;
                case System::Event::Type::Drop:
                    _dropEvent(static_cast<System::Event::Drop&>(event));
                    break;
                case System::Event::Type::KeyPress:
                    _keyPressEvent(static_cast<System::Event::KeyPress&>(event));
                    break;
                case System::Event::Type::KeyRelease:
                    _keyReleaseEvent(static_cast<System::Event::KeyRelease&>(event));
                    break;
                case System::Event::Type::TextFocus:
                    _textFocusEvent(static_cast<System::Event::TextFocus&>(event));
                    break;
                case System::Event::Type::TextFocusLost:
                    _textFocusLostEvent(static_cast<System::Event::TextFocusLost&>(event));
                    break;
                case System::Event::Type::TextInput:
                    _textInputEvent(static_cast<System::Event::TextInput&>(event));
                    break;
                default: break;
                }
                out = event.isAccepted();
            }
            return out;
        }

        void Widget::_paintEvent(System::Event::Paint& event)
        {
            if (_backgroundRole != ColorRole::None)
            {
                _render->setFillColor(_style->getColor(_backgroundRole));
                _render->drawRect(getGeometry());
            }
        }

        void Widget::_paintOverlayEvent(System::Event::PaintOverlay& event)
        {
            if (!_shadowOverlay.empty())
            {
                const auto& style = _getStyle();
                const float ss = style->getMetric(MetricsRole::ShadowSmall);
                const Math::BBox2f& g = getGeometry();
                _render->setFillColor(_style->getColor(ColorRole::Shadow));
                for (const auto& i : _shadowOverlay)
                {
                    switch (i)
                    {
                    case Side::Left:
                        _render->drawShadow(Math::BBox2f(g.min.x, g.min.y, ss, g.h()), Render2D::Side::Right);
                        break;
                    case Side::Right:
                        _render->drawShadow(Math::BBox2f(g.max.x - ss, g.min.y, ss, g.h()), Render2D::Side::Left);
                        break;
                    case Side::Top:
                        _render->drawShadow(Math::BBox2f(g.min.x, g.min.y, g.w(), ss), Render2D::Side::Bottom);
                        break;
                    case Side::Bottom:
                        _render->drawShadow(Math::BBox2f(g.min.x, g.max.y - ss, g.w(), ss), Render2D::Side::Top);
                        break;
                    default: break;
                    }
                }
            }
        }

        void Widget::_pointerEnterEvent(System::Event::PointerEnter& event)
        {
            if (_pointerEnabled && !event.isRejected())
            {
                event.accept();
            }
        }

        void Widget::_pointerLeaveEvent(System::Event::PointerLeave& event)
        {
            if (_pointerEnabled)
            {
                event.accept();
            }
        }

        void Widget::_pointerMoveEvent(System::Event::PointerMove& event)
        {
            if (_pointerEnabled)
            {
                event.accept();
            }
        }

        void Widget::_keyPressEvent(System::Event::KeyPress& event)
        {
            switch (event.getKey())
            {
            case GLFW_KEY_TAB:
                if (auto parent = std::dynamic_pointer_cast<Widget>(getParent().lock()))
                {
                    event.accept();
                    auto widget = std::dynamic_pointer_cast<Widget>(shared_from_this());
                    if (event.getKeyModifiers() & GLFW_MOD_SHIFT)
                    {
                        parent->prevTextFocus(widget);
                    }
                    else
                    {
                        parent->nextTextFocus(widget);
                    }
                }
                else
                {
                    if (event.getKeyModifiers() & GLFW_MOD_SHIFT)
                    {
                        prevTextFocus(nullptr);
                    }
                    else
                    {
                        nextTextFocus(nullptr);
                    }
                }
                break;
            default:
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
                    [](const std::shared_ptr<Shortcut>& a, const std::shared_ptr<Shortcut>& b)
                    {
                        return a->observeShortcut()->get().modifiers > b->observeShortcut()->get().modifiers;
                    });

                for (const auto& i : shortcuts)
                {
                    const auto& shortcut = i->observeShortcut()->get();
                    if ((shortcut.key == event.getKey() && event.getKeyModifiers() == shortcut.modifiers) ||
                        (shortcut.key == event.getKey() && shortcut.modifiers == 0 && event.getKeyModifiers() == 0))
                    {
                        event.accept();
                        i->doCallback();
                        break;
                    }
                }
                break;
            }
        }

        void Widget::_resize()
        {
            if (auto eventSystem = _eventSystem.lock())
            {
                eventSystem->resizeRequest();
            }
        }

        void Widget::_redraw()
        {
            if (auto eventSystem = _eventSystem.lock())
            {
                eventSystem->redrawRequest();
            }
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
            std::stringstream out;
            if (!_tooltipText.empty())
            {
                out << _tooltipText;
            }
            else
            {
                for (const auto& action : _actions)
                {
                    const auto& actionTooltip = action->observeTooltip()->get();
                    if (!actionTooltip.empty())
                    {
                        out << actionTooltip;
                        break;
                    }
                }
            }
            if (_actions.size())
            {
                const auto action = _actions[0];
                const auto& shortcuts = action->observeShortcuts()->get();
                const size_t shortcutsSize = shortcuts.size();
                if (shortcutsSize)
                {
                    out << "\n\n";
                    out << "(";
                    out << _getText(1 == shortcutsSize ?
                        DJV_TEXT("keyboard_shortcut") :
                        DJV_TEXT("keyboard_shortcuts"));
                    out << ": ";
                    std::vector<std::string> shortcutsText;
                    const auto& textSystem = _getTextSystem();
                    for (const auto& i : action->observeShortcuts()->get())
                    {
                        const auto& shortcut = i->observeShortcut()->get();
                        if (shortcut.isValid())
                        {
                            shortcutsText.push_back(getText(shortcut, textSystem));
                        }
                    }
                    out << String::join(shortcutsText, ", ");
                    out << ")";
                }
            }
            return out.str();
        }

        std::shared_ptr<ITooltipWidget> Widget::_createTooltipDefault()
        {
            std::shared_ptr<ITooltipWidget> out;
            if (auto context = getContext().lock())
            {
                out = DefaultTooltipWidget::create(context);
            }
            return out;
        }

        std::shared_ptr<ITooltipWidget> Widget::_createTooltip(const glm::vec2&)
        {
            std::shared_ptr<ITooltipWidget> out;
            if (!_tooltipText.empty())
            {
                out = _createTooltipDefault();
                out->setTooltip(_getTooltipText());
            }
            return out;
        }

    } // namespace UI
} // namespace djv
