// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/NumericSlider.h>

#include <djvUI/DrawUtil.h>
#include <djvUI/Style.h>

#include <djvAV/Render2D.h>

#include <djvCore/Context.h>
#include <djvCore/Timer.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct NumericSlider::Private
        {
            Orientation orientation = Orientation::First;
            float handleWidth = 0.F;
            Time::Duration delay = Time::Duration::zero();
            std::shared_ptr<Time::Timer> delayTimer;
            Event::PointerID pressedID = Event::invalidID;
            glm::vec2 pressedPos = glm::vec2(0.F, 0.F);
            glm::vec2 prevPos = glm::vec2(0.F, 0.F);
        };

        void NumericSlider::_init(Orientation orientation, const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();
            setPointerEnabled(true);
            switch (orientation)
            {
                case Orientation::Horizontal: setVAlign(VAlign::Center); break;
                case Orientation::Vertical:   setHAlign(HAlign::Center); break;
                default: break;
            }
            p.orientation = orientation;
            p.delayTimer = Time::Timer::create(context);
        }

        NumericSlider::NumericSlider() :
            _p(new Private)
        {}

        NumericSlider::~NumericSlider()
        {}

        Orientation NumericSlider::getOrientation() const
        {
            return _p->orientation;
        }

        const Time::Duration& NumericSlider::getDelay() const
        {
            return _p->delay;
        }

        void NumericSlider::setDelay(const Time::Duration& value)
        {
            _p->delay = value;
        }

        bool NumericSlider::acceptFocus(TextFocusDirection)
        {
            bool out = false;
            if (isEnabled(true) && isVisible(true) && !isClipped())
            {
                takeTextFocus();
                out = true;
            }
            return out;
        }
        
        float NumericSlider::_getHandleWidth() const
        {
            return _p->handleWidth;
        }
        
        void NumericSlider::_paint(float v, float pos)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const BBox2f& g = getMargin().bbox(getGeometry(), style);
            const float m = style->getMetric(MetricsRole::MarginSmall);
            const float b = style->getMetric(MetricsRole::Border);
            const auto& render = _getRender();
            
            if (hasTextFocus())
            {
                render->setFillColor(style->getColor(ColorRole::TextFocus));
                drawBorder(render, g, b * 2.F);
            }

            const BBox2f g2 = g.margin(-b * 2.F);
            if (_getPointerHover().size())
            {
                render->setFillColor(style->getColor(ColorRole::Hovered));
                render->drawRect(g2);
            }

            const BBox2f g3 = g2.margin(-m);
            float troughHeight = 0.F;
            switch (p.orientation)
            {
            case Orientation::Horizontal:
            {
                troughHeight = g3.h() / 3.F;
                const BBox2f g4 = BBox2f(
                    g3.min.x,
                    floorf(g3.min.y + g3.h() / 2.F - troughHeight / 2.F),
                    g3.w(),
                    troughHeight);
                render->setFillColor(style->getColor(ColorRole::Border));
                drawBorder(render, g4, b);
                render->setFillColor(style->getColor(ColorRole::Trough));
                const BBox2f g5 = g4.margin(-b);
                render->drawRect(g5);
                render->setFillColor(style->getColor(ColorRole::Checked));
                render->drawRect(BBox2f(
                    g5.min.x,
                    g5.min.y,
                    ceilf((g5.w() - p.handleWidth / 2.F) * v),
                    g5.h()));
                break;
            }
            case Orientation::Vertical:
            {
                troughHeight = g3.w() / 3.F;
                const BBox2f g4 = BBox2f(
                    floorf(g3.min.x + g3.w() / 2.F - troughHeight / 2.F),
                    g3.min.y,
                    troughHeight,
                    g3.h());
                render->setFillColor(style->getColor(ColorRole::Border));
                drawBorder(render, g4, b);
                render->setFillColor(style->getColor(ColorRole::Trough));
                const BBox2f g5 = g4.margin(-b);
                render->drawRect(g5);
                render->setFillColor(style->getColor(ColorRole::Checked));
                render->drawRect(BBox2f(
                    g5.min.x,
                    g5.min.y,
                    g5.w(),
                    ceilf((g5.h() - p.handleWidth / 2.F) * v)));
                break;
            }
            default: break;
            }

            BBox2f handleBBox;
            switch (p.orientation)
            {
            case Orientation::Horizontal:
                handleBBox = BBox2f(
                    floorf(pos - p.handleWidth / 2.F),
                    g3.min.y,
                    p.handleWidth,
                    g3.h());
                break;
            case Orientation::Vertical:
                handleBBox = BBox2f(
                    g3.min.x,
                    floorf(pos - p.handleWidth / 2.F),
                    g3.w(),
                    p.handleWidth);
                break;
            default: break;
            }
            render->setFillColor(style->getColor(ColorRole::BorderButton));
            render->drawRect(handleBBox);
            render->setFillColor(style->getColor(ColorRole::Button));
            render->drawRect(handleBBox.margin(-b));
            if (p.pressedID != Event::invalidID)
            {
                render->setFillColor(style->getColor(ColorRole::Pressed));
                render->drawRect(handleBBox);
            }
            else if (_getPointerHover().size())
            {
                render->setFillColor(style->getColor(ColorRole::Hovered));
                render->drawRect(handleBBox);
            }
        }

        void NumericSlider::_preLayoutEvent(Event::PreLayout& event)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const float s = style->getMetric(MetricsRole::Slider);
            const float m = style->getMetric(MetricsRole::MarginSmall);
            const float b = style->getMetric(MetricsRole::Border);
            const float is = style->getMetric(MetricsRole::IconSmall);
            glm::vec2 size(0.F, 0.F);
            switch (p.orientation)
            {
            case Orientation::Horizontal: size = glm::vec2(s, is); break;
            case Orientation::Vertical:   size = glm::vec2(is, s); break;
            default: break;
            }
            _setMinimumSize(size + m * 2.F + b * 4.F + getMargin().getSize(style));
        }

        void NumericSlider::_pointerEnterEvent(Event::PointerEnter& event)
        {
            if (!event.isRejected())
            {
                event.accept();
                if (isEnabled(true))
                {
                    _redraw();
                }
            }
        }

        void NumericSlider::_pointerLeaveEvent(Event::PointerLeave& event)
        {
            event.accept();
            if (isEnabled(true))
            {
                _redraw();
            }
        }

        void NumericSlider::_pointerMoveEvent(Event::PointerMove& event)
        {
            DJV_PRIVATE_PTR();
            event.accept();
            const auto& pointerInfo = event.getPointerInfo();
            if (pointerInfo.id == p.pressedID)
            {
                float v = 0.F;
                switch (p.orientation)
                {
                case Orientation::Horizontal:
                    v = pointerInfo.projectedPos.x;
                    break;
                case Orientation::Vertical:
                    v = pointerInfo.projectedPos.y;
                    break;
                default: break;
                }
                _pointerMove(v);
                if (p.delay > Time::Duration::zero() && glm::length(pointerInfo.projectedPos - p.prevPos) > 0.F)
                {
                    _resetTimer();
                }
                p.prevPos = pointerInfo.projectedPos;
                _redraw();
            }
        }

        void NumericSlider::_buttonPressEvent(Event::ButtonPress& event)
        {
            DJV_PRIVATE_PTR();
            if (p.pressedID)
                return;
            event.accept();
            takeTextFocus();
            const auto& pointerInfo = event.getPointerInfo();
            p.pressedID = pointerInfo.id;
            p.pressedPos = pointerInfo.projectedPos;
            p.prevPos = pointerInfo.projectedPos;
            float v = 0.F;
            switch (p.orientation)
            {
            case Orientation::Horizontal:
                v = pointerInfo.projectedPos.x;
                break;
            case Orientation::Vertical:
                v = pointerInfo.projectedPos.y;
                break;
            default: break;
            }
            _buttonPress(v);
            if (p.delay > Time::Duration::zero())
            {
                _resetTimer();
            }
            _redraw();
        }

        void NumericSlider::_buttonReleaseEvent(Event::ButtonRelease& event)
        {
            DJV_PRIVATE_PTR();
            const auto& pointerInfo = event.getPointerInfo();
            if (pointerInfo.id == p.pressedID)
            {
                event.accept();
                p.pressedID = Event::invalidID;
                _buttonRelease();
                p.delayTimer->stop();
                _redraw();
            }
        }

        void NumericSlider::_keyPressEvent(Event::KeyPress& event)
        {
            Widget::_keyPressEvent(event);
            if (!event.isAccepted() && hasTextFocus())
            {
                if (_keyPress(event.getKey()))
                {
                    event.accept();
                }
                else
                {
                    switch (event.getKey())
                    {
                    case GLFW_KEY_ESCAPE:
                        event.accept();
                        releaseTextFocus();
                        break;
                    default: break;
                    }
                }
            }
        }

        void NumericSlider::_textFocusEvent(Event::TextFocus&)
        {
            _redraw();
        }

        void NumericSlider::_textFocusLostEvent(Event::TextFocusLost&)
        {
            _redraw();
        }

        void NumericSlider::_scrollEvent(Event::Scroll& event)
        {
            if (isEnabled(true))
            {
                event.accept();
                takeTextFocus();
                _scroll(event.getScrollDelta().y);
            }
        }

        void NumericSlider::_initEvent(Event::Init& event)
        {
            Widget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().resize)
            {
                if (auto context = getContext().lock())
                {
                    const auto& style = _getStyle();
                    p.handleWidth = style->getMetric(MetricsRole::Handle);
                }
            }
        }

        void NumericSlider::_resetTimer()
        {
            DJV_PRIVATE_PTR();
            auto weak = std::weak_ptr<NumericSlider>(std::dynamic_pointer_cast<NumericSlider>(shared_from_this()));
            p.delayTimer->start(
                p.delay,
                [weak](const std::chrono::steady_clock::time_point&, const Time::Duration&)
            {
                if (auto widget = weak.lock())
                {
                    widget->_valueUpdate();
                }
            });
        }

    } // namespace UI
} // namespace djv
