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

#include <djvUI/Widget.h>

#include <djvUI/Action.h>
#include <djvUI/Shortcut.h>
#include <djvUI/System.h>
#include <djvUI/Window.h>

#include <djvAV/FontSystem.h>
#include <djvAV/IconSystem.h>
#include <djvAV/Render2DSystem.h>

#include <djvCore/Math.h>

#include <algorithm>

//#pragma optimize("", off)

using namespace djv::Core;

namespace
{
    static size_t currentWidgetCount = 0;

} // namespace

namespace djv
{
    namespace UI
    {
        void Widget::_init(Core::Context * context)
        {
            IObject::_init(context);
            
            setClassName("djv::UI::Widget");

            /*context->log("djv::UI::Widget", "Widget::Widget");
            context->log("djv::UI::Widget", String::Format("widget count = %%1").arg(currentWidgetCount));*/
            ++currentWidgetCount;

            _iconSystem = context->getSystemT<AV::Image::IconSystem>();
            _fontSystem = context->getSystemT<AV::Font::System>();
            _renderSystem = context->getSystemT<AV::Render::Render2DSystem>();
            _style = context->getSystemT<Style>();
        }

        Widget::Widget()
        {}

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
            _visible = value;
        }

        void Widget::setOpacity(float value)
        {
            _opacity = value;
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
            _geometry = value;
        }

        void Widget::move(const glm::vec2& value)
        {
            const glm::vec2 size = _geometry.getSize();
            _geometry.min = value;
            _geometry.max = value + size;
        }

        void Widget::resize(const glm::vec2& value)
        {
            setGeometry(BBox2f(_geometry.min, _geometry.min + value));
        }

        void Widget::setMargin(const Margin& value)
        {
            _margin = value;
        }

        void Widget::setHAlign(HAlign value)
        {
            _hAlign = value;
        }

        void Widget::setVAlign(VAlign value)
        {
            _vAlign = value;
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
            return BBox2f(x, y, w, h);
        }

        void Widget::setBackgroundRole(ColorRole value)
        {
            _backgroundRole = value;
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

        bool Widget::event(Event::IEvent& event)
        {
            bool out = IObject::event(event);
            if (!out)
            {
                switch (event.getEventType())
                {
                case Event::Type::PreLayout:
                    preLayoutEvent(static_cast<Event::PreLayout&>(event));
                    break;
                case Event::Type::Layout:
                    layoutEvent(static_cast<Event::Layout&>(event));
                    break;
                case Event::Type::Clip:
                {
                    auto& clipEvent = static_cast<Event::Clip&>(event);
                    if (auto parent = std::dynamic_pointer_cast<Widget>(getParent().lock()))
                    {
                        _parentsVisible = parent->_visible && parent->_parentsVisible;
                        _clipped = !clipEvent.getClipRect().isValid() || !_visible || !parent->_visible || !parent->_parentsVisible;
                        _parentsOpacity = parent->_opacity * parent->_parentsOpacity;
                    }
                    else
                    {
                        _parentsVisible = true;
                        _clipped = false;
                        _parentsOpacity = 1.f;
                    }
                    this->clipEvent(clipEvent);
                    break;
                }
                case Event::Type::Paint:
                    paintEvent(static_cast<Event::Paint&>(event));
                    break;
                case Event::Type::Scroll:
                    scrollEvent(static_cast<Event::Scroll&>(event));
                    break;
                case Event::Type::Drop:
                    dropEvent(static_cast<Event::Drop&>(event));
                    break;
                case Event::Type::KeyboardFocus:
                    keyboardFocusEvent(static_cast<Event::KeyboardFocus&>(event));
                    break;
                case Event::Type::KeyboardFocusLost:
                    keyboardFocusLostEvent(static_cast<Event::KeyboardFocusLost&>(event));
                    break;
                case Event::Type::KeyPress:
                    keyPressEvent(static_cast<Event::KeyPress&>(event));
                    break;
                case Event::Type::KeyRelease:
                    keyReleaseEvent(static_cast<Event::KeyRelease&>(event));
                    break;
                case Event::Type::Text:
                    textEvent(static_cast<Event::Text&>(event));
                    break;
                default: break;
                }
                out = event.isAccepted();
            }
            return out;
        }

        void Widget::paintEvent(Event::Paint& event)
        {
            if (_backgroundRole != ColorRole::None)
            {
                if (auto render = _renderSystem.lock())
                {
                    if (auto style = _style.lock())
                    {
                        render->setFillColor(_getColorWithOpacity(style->getColor(_backgroundRole)));
                        render->drawRectangle(getGeometry());
                    }
                }
            }
        }

        void Widget::pointerEnterEvent(Event::PointerEnter& event)
        {
            if (_pointerEnabled && !event.isRejected())
            {
                event.accept();
            }
        }

        void Widget::pointerLeaveEvent(Event::PointerLeave& event)
        {
            if (_pointerEnabled)
            {
                event.accept();
            }
        }

        void Widget::pointerMoveEvent(Event::PointerMove& event)
        {
            if (_pointerEnabled)
            {
                event.accept();
            }
        }

        void Widget::keyPressEvent(Event::KeyPress& event)
        {
            if (isEnabled())
            {
                // Find the actions that are enabled and have shortcuts.
                std::vector<std::shared_ptr<Action> > actions;
                for (const auto& i : _actions)
                {
                    if (i->isEnabled()->get())
                    {
                        if (i->getShortcut())
                        {
                            actions.push_back(i);
                        }
                    }
                }

                // Sort the actions so that we test those with keyboard modifiers first.
                std::sort(actions.begin(), actions.end(),
                    [](const std::shared_ptr<Action> & a, const std::shared_ptr<Action> & b) -> bool
                {
                    return a->getShortcut()->getShortcutModifiers()->get() > b->getShortcut()->getShortcutModifiers()->get();
                });

                for (const auto& i : actions)
                {
                    const int key = i->getShortcut()->getShortcutKey()->get();
                    const int modifiers = i->getShortcut()->getShortcutModifiers()->get();
                    if ((key == event.getKey() && event.getKeyModifiers() & modifiers) ||
                        (key == event.getKey() && modifiers == 0 && event.getKeyModifiers() == 0))
                    {
                        event.accept();
                        i->getShortcut()->doCallback();
                        break;
                    }
                }
            }
        }

        void Widget::setParent(const std::shared_ptr<IObject>& value, int insert)
        {
            IObject::setParent(value, insert);
            if (!value)
            {
                _clipped = true;
            }
        }

        AV::Image::Color Widget::_getColorWithOpacity(const AV::Image::Color & value) const
        {
            auto out = value.convert(AV::Image::Type::RGBA_F32);
            out.setF32(out.getF32(3) * getOpacity(true), 3);
            return out;
        }

        void Widget::_setMinimumSize(const glm::vec2& value)
        {
            _minimumSize = value;
        }

    } // namespace UI
} // namespace djv
