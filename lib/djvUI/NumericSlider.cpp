// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/NumericSlider.h>

#include <djvUI/DrawUtil.h>
#include <djvUI/Style.h>

#include <djvRender2D/Render.h>

#include <djvSystem/Context.h>

#include <djvSystem/Timer.h>

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
            std::shared_ptr<System::Timer> delayTimer;
            System::Event::PointerID pressedID = System::Event::invalidID;
            glm::vec2 pressedPos = glm::vec2(0.F, 0.F);
            glm::vec2 prevPos = glm::vec2(0.F, 0.F);
        };

        void NumericSlider::_init(Orientation orientation, const std::shared_ptr<System::Context>& context)
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
            p.delayTimer = System::Timer::create(context);
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
            const Math::BBox2f& g = getMargin().bbox(getGeometry(), style);
            const float m = style->getMetric(MetricsRole::MarginSmall);
            const float b = style->getMetric(MetricsRole::Border);
            const float btf = style->getMetric(MetricsRole::BorderTextFocus);
            const auto& render = _getRender();
            
            if (hasTextFocus())
            {
                render->setFillColor(style->getColor(ColorRole::TextFocus));
                drawBorder(render, g, btf);
            }

            const Math::BBox2f g2 = g.margin(-btf);
            if (_getPointerHover().size())
            {
                render->setFillColor(style->getColor(ColorRole::Hovered));
                render->drawRect(g2);
            }

            const Math::BBox2f g3 = g2.margin(-m);
            float troughHeight = 0.F;
            switch (p.orientation)
            {
            case Orientation::Horizontal:
            {
                troughHeight = g3.h() / 3.F;
                const Math::BBox2f g4 = Math::BBox2f(
                    g3.min.x,
                    floorf(g3.min.y + g3.h() / 2.F - troughHeight / 2.F),
                    g3.w(),
                    troughHeight);
                render->setFillColor(style->getColor(ColorRole::Border));
                drawBorder(render, g4, b);
                render->setFillColor(style->getColor(ColorRole::Trough));
                const Math::BBox2f g5 = g4.margin(-b);
                render->drawRect(g5);
                render->setFillColor(style->getColor(ColorRole::Checked));
                render->drawRect(Math::BBox2f(
                    g5.min.x,
                    g5.min.y,
                    ceilf((g5.w() - p.handleWidth / 2.F) * v),
                    g5.h()));
                break;
            }
            case Orientation::Vertical:
            {
                troughHeight = g3.w() / 3.F;
                const Math::BBox2f g4 = Math::BBox2f(
                    floorf(g3.min.x + g3.w() / 2.F - troughHeight / 2.F),
                    g3.min.y,
                    troughHeight,
                    g3.h());
                render->setFillColor(style->getColor(ColorRole::Border));
                drawBorder(render, g4, b);
                render->setFillColor(style->getColor(ColorRole::Trough));
                const Math::BBox2f g5 = g4.margin(-b);
                render->drawRect(g5);
                render->setFillColor(style->getColor(ColorRole::Checked));
                render->drawRect(Math::BBox2f(
                    g5.min.x,
                    g5.min.y,
                    g5.w(),
                    ceilf((g5.h() - p.handleWidth / 2.F) * v)));
                break;
            }
            default: break;
            }

            Math::BBox2f handleBBox;
            switch (p.orientation)
            {
            case Orientation::Horizontal:
                handleBBox = Math::BBox2f(
                    floorf(pos - p.handleWidth / 2.F),
                    g3.min.y,
                    p.handleWidth,
                    g3.h());
                break;
            case Orientation::Vertical:
                handleBBox = Math::BBox2f(
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
            if (p.pressedID != System::Event::invalidID)
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

        void NumericSlider::_preLayoutEvent(System::Event::PreLayout& event)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const float s = style->getMetric(MetricsRole::Slider);
            const float m = style->getMetric(MetricsRole::MarginSmall);
            const float b = style->getMetric(MetricsRole::Border);
            const float btf = style->getMetric(MetricsRole::BorderTextFocus);
            const float is = style->getMetric(MetricsRole::IconSmall);
            glm::vec2 size(0.F, 0.F);
            switch (p.orientation)
            {
            case Orientation::Horizontal: size = glm::vec2(s, is); break;
            case Orientation::Vertical:   size = glm::vec2(is, s); break;
            default: break;
            }
            _setMinimumSize(size + m * 2.F + btf * 2.F + getMargin().getSize(style));
        }

        void NumericSlider::_pointerEnterEvent(System::Event::PointerEnter& event)
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

        void NumericSlider::_pointerLeaveEvent(System::Event::PointerLeave& event)
        {
            event.accept();
            if (isEnabled(true))
            {
                _redraw();
            }
        }

        void NumericSlider::_pointerMoveEvent(System::Event::PointerMove& event)
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

        void NumericSlider::_buttonPressEvent(System::Event::ButtonPress& event)
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

        void NumericSlider::_buttonReleaseEvent(System::Event::ButtonRelease& event)
        {
            DJV_PRIVATE_PTR();
            const auto& pointerInfo = event.getPointerInfo();
            if (pointerInfo.id == p.pressedID)
            {
                event.accept();
                p.pressedID = System::Event::invalidID;
                _buttonRelease();
                p.delayTimer->stop();
                _redraw();
            }
        }

        void NumericSlider::_keyPressEvent(System::Event::KeyPress& event)
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

        void NumericSlider::_textFocusEvent(System::Event::TextFocus&)
        {
            _redraw();
        }

        void NumericSlider::_textFocusLostEvent(System::Event::TextFocusLost&)
        {
            _redraw();
        }

        void NumericSlider::_scrollEvent(System::Event::Scroll& event)
        {
            if (isEnabled(true))
            {
                event.accept();
                takeTextFocus();
                _scroll(event.getScrollDelta().y);
            }
        }

        void NumericSlider::_initEvent(System::Event::Init& event)
        {
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
