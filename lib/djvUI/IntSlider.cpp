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

#include <djvUI/IntSlider.h>

#include <djvUI/IntEdit.h>
#include <djvUI/RowLayout.h>

#include <djvAV/Render2D.h>

#include <djvCore/NumericValueModels.h>
#include <djvCore/Timer.h>
#include <djvCore/ValueObserver.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct BasicIntSlider::Private
        {
            int value = 0;
            std::shared_ptr<IntValueModel> model;
            Orientation orientation = Orientation::First;
            float handleWidth = 0.f;
            std::chrono::milliseconds delay = std::chrono::milliseconds(0);
            std::shared_ptr<Time::Timer> delayTimer;
            Event::PointerID pressedID = Event::InvalidID;
            glm::vec2 pressedPos = glm::vec2(0.f, 0.f);
            glm::vec2 prevPos = glm::vec2(0.f, 0.f);
            std::function<void(int)> callback;
            std::shared_ptr<ValueObserver<int> > valueObserver;
        };

        void BasicIntSlider::_init(Orientation orientation, Context * context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();

            setClassName("djv::UI::BasicIntSlider");
            setModel(IntValueModel::create());
            setPointerEnabled(true);

            p.orientation = orientation;

            p.delayTimer = Time::Timer::create(context);
        }

        BasicIntSlider::BasicIntSlider() :
            _p(new Private)
        {}

        BasicIntSlider::~BasicIntSlider()
        {}

        std::shared_ptr<BasicIntSlider> BasicIntSlider::create(Orientation orientation, Context * context)
        {
            auto out = std::shared_ptr<BasicIntSlider>(new BasicIntSlider);
            out->_init(orientation, context);
            return out;
        }

        IntRange BasicIntSlider::getRange() const
        {
            return _p->model->observeRange()->get();
        }

        void BasicIntSlider::setRange(const IntRange& value)
        {
            _p->model->setRange(value);
        }

        int BasicIntSlider::getValue() const
        {
            return _p->model->observeValue()->get();
        }

        void BasicIntSlider::setValue(int value)
        {
            _p->model->setValue(value);
        }

        void BasicIntSlider::setValueCallback(const std::function<void(int)>& callback)
        {
            _p->callback = callback;
        }

        std::chrono::milliseconds BasicIntSlider::getDelay() const
        {
            return _p->delay;
        }

        void BasicIntSlider::setDelay(std::chrono::milliseconds value)
        {
            _p->delay = value;
        }

        const std::shared_ptr<IntValueModel> & BasicIntSlider::getModel() const
        {
            return _p->model;
        }

        void BasicIntSlider::setModel(const std::shared_ptr<IntValueModel> & model)
        {
            DJV_PRIVATE_PTR();
            if (p.model)
            {
                p.valueObserver.reset();
            }
            p.model = model;
            if (p.model)
            {
                auto weak = std::weak_ptr<BasicIntSlider>(std::dynamic_pointer_cast<BasicIntSlider>(shared_from_this()));
                p.valueObserver = ValueObserver<int>::create(
                    p.model->observeValue(),
                    [weak](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->value = value;
                        widget->_redraw();
                    }
                });
            }
        }

        void BasicIntSlider::_preLayoutEvent(Event::PreLayout & event)
        {
            DJV_PRIVATE_PTR();
            auto style = _getStyle();
            const float tc = style->getMetric(MetricsRole::Slider);
            p.handleWidth = style->getMetric(MetricsRole::Handle);
            glm::vec2 size;
            switch (p.orientation)
            {
            case Orientation::Horizontal: size = glm::vec2(tc, p.handleWidth); break;
            case Orientation::Vertical:   size = glm::vec2(p.handleWidth, tc); break;
            default: break;
            }
            _setMinimumSize(size + getMargin().getSize(style));
        }

        void BasicIntSlider::_paintEvent(Event::Paint & event)
        {
            Widget::_paintEvent(event);
            DJV_PRIVATE_PTR();
            auto style = _getStyle();
            const BBox2f & g = getMargin().bbox(getGeometry(), style);
            const glm::vec2 c = g.getCenter();
            const float m = style->getMetric(MetricsRole::MarginSmall);
            const float b = style->getMetric(MetricsRole::Border);
            auto render = _getRender();
            if (p.value > 0.f && p.model)
            {
                const auto & range = p.model->observeRange()->get();
                float v = (p.value - range.min) / static_cast<float>(range.max - range.min);
                render->setFillColor(_getColorWithOpacity(style->getColor(ColorRole::Checked)));
                switch (p.orientation)
                {
                case Orientation::Horizontal:
                    render->drawRect(BBox2f(
                        floorf(g.min.x + p.handleWidth / 2.f - m / 2.f),
                        floorf(c.y - m / 2.f),
                        ceilf((g.w() - p.handleWidth + m) * v),
                        m));
                    break;
                case Orientation::Vertical:
                {
                    const float h = (g.h() - p.handleWidth + m) * v;
                    render->drawRect(BBox2f(
                        floorf(c.x - m / 2.f),
                        floorf(g.max.y - p.handleWidth / 2.f - h + m / 2.f),
                        m,
                        ceilf(h)));
                    break;
                }
                default: break;
                }
                render->setFillColor(_getColorWithOpacity(style->getColor(ColorRole::Trough)));
                switch (p.orientation)
                {
                case Orientation::Horizontal:
                    render->drawRect(BBox2f(
                        floorf(g.min.x + p.handleWidth / 2.f - m / 2.f + (g.w() - p.handleWidth + m) * v),
                        floorf(c.y - m / 2.f),
                        ceilf((g.w() - p.handleWidth + m) * (1.f - v)),
                        m));
                    break;
                case Orientation::Vertical:
                {
                    const float h = (g.h() - p.handleWidth + m) * (1.f - v);
                    render->drawRect(BBox2f(
                        floorf(c.x - m / 2.f),
                        floorf(g.min.y + p.handleWidth / 2.f - m / 2.f),
                        m,
                        ceilf(h)));
                    break;
                }
                default: break;
                }
            }
            else
            {
                render->setFillColor(_getColorWithOpacity(style->getColor(ColorRole::Trough)));
                switch (p.orientation)
                {
                case Orientation::Horizontal:
                    render->drawRect(BBox2f(
                        floorf(g.min.x + p.handleWidth / 2.f - m / 2.f),
                        floorf(c.y - m / 2.f),
                        ceilf(g.w() - p.handleWidth + m),
                        m));
                    break;
                case Orientation::Vertical:
                    render->drawRect(BBox2f(
                        floorf(c.x - m / 2.f),
                        floorf(g.min.y + p.handleWidth / 2.f - m / 2.f),
                        m,
                        ceilf(g.h() - p.handleWidth + m)));
                    break;
                default: break;
                }
            }
            if (p.model)
            {
                render->setFillColor(_getColorWithOpacity(style->getColor(ColorRole::Border)));
                glm::vec2 pos;
                switch (p.orientation)
                {
                case Orientation::Horizontal: pos = glm::vec2(_valueToPos(p.value), c.y); break;
                case Orientation::Vertical:   pos = glm::vec2(c.x, _valueToPos(p.value)); break;
                default: break;
                }
                const float r = ceilf(p.handleWidth / 2.f - 1.f);
                render->drawCircle(pos, r);
                render->setFillColor(_getColorWithOpacity(style->getColor(_getColorRole())));
                render->drawCircle(pos, r - b);
                if (p.pressedID != Event::InvalidID)
                {
                    render->setFillColor(_getColorWithOpacity(style->getColor(ColorRole::Pressed)));
                    render->drawCircle(pos, r);
                }
                else if (_getPointerHover().size())
                {
                    render->setFillColor(_getColorWithOpacity(style->getColor(ColorRole::Hovered)));
                    render->drawCircle(pos, r);
                }
            }
        }

        void BasicIntSlider::_pointerEnterEvent(Event::PointerEnter & event)
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

        void BasicIntSlider::_pointerLeaveEvent(Event::PointerLeave & event)
        {
            event.accept();
            if (isEnabled(true))
            {
                _redraw();
            }
        }

        void BasicIntSlider::_pointerMoveEvent(Event::PointerMove & event)
        {
            DJV_PRIVATE_PTR();
            event.accept();
            const auto & pointerInfo = event.getPointerInfo();
            if (pointerInfo.id == p.pressedID)
            {
                float v = 0.f;
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
                p.value = _posToValue(v);
                if (p.model && std::chrono::milliseconds(0) == p.delay)
                {
                    p.model->setValue(p.value);
                    if (p.callback)
                    {
                        p.callback(p.model->observeValue()->get());
                    }
                }
                if (p.delay > std::chrono::milliseconds(0) && glm::length(pointerInfo.projectedPos - p.prevPos) > 0.f)
                {
                    _resetTimer();
                }
                p.prevPos = pointerInfo.projectedPos;
                _redraw();
            }
        }

        void BasicIntSlider::_buttonPressEvent(Event::ButtonPress & event)
        {
            DJV_PRIVATE_PTR();
            if (p.pressedID)
                return;
            event.accept();
            const auto & pointerInfo = event.getPointerInfo();
            p.pressedID = pointerInfo.id;
            p.pressedPos = pointerInfo.projectedPos;
            p.prevPos = pointerInfo.projectedPos;
            float v = 0.f;
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
            p.value = _posToValue(v);
            if (p.model && std::chrono::milliseconds(0) == p.delay)
            {
                p.model->setValue(p.value);
                if (p.callback)
                {
                    p.callback(p.model->observeValue()->get());
                }
            }
            if (p.delay > std::chrono::milliseconds(0))
            {
                _resetTimer();
            }
            _redraw();
        }

        void BasicIntSlider::_buttonReleaseEvent(Event::ButtonRelease & event)
        {
            DJV_PRIVATE_PTR();
            const auto & pointerInfo = event.getPointerInfo();
            if (pointerInfo.id == p.pressedID)
            {
                event.accept();
                p.pressedID = Event::InvalidID;
                if (p.model && p.delay > std::chrono::milliseconds(0))
                {
                    p.model->setValue(p.value);
                    if (p.callback)
                    {
                        p.callback(p.model->observeValue()->get());
                    }
                }
                p.delayTimer->stop();
                _redraw();
            }
        }

        ColorRole BasicIntSlider::_getColorRole() const
        {
            return !isEnabled(true) ? ColorRole::Disabled : ColorRole::Button;
        }

        float BasicIntSlider::_valueToPos(int value) const
        {
            DJV_PRIVATE_PTR();
            float out = 0.f;
            if (p.model)
            {
                auto style = _getStyle();
                const BBox2f g = getMargin().bbox(getGeometry(), style);
                const auto & range = p.model->observeRange()->get();
                float v = (value - range.min) / static_cast<float>(range.max - range.min);
                switch (p.orientation)
                {
                case Orientation::Horizontal:
                    out = g.x() + ceilf(p.handleWidth / 2.f + (g.w() - p.handleWidth) * v);
                    break;
                case Orientation::Vertical:
                    v = 1.f - v;
                    out = g.y() + ceilf(p.handleWidth / 2.f + (g.h() - p.handleWidth) * v);
                    break;
                default: break;
                }
            }
            return out;
        }

        int BasicIntSlider::_posToValue(float value) const
        {
            DJV_PRIVATE_PTR();
            int out = 0;
            if (p.model)
            {
                auto style = _getStyle();
                const BBox2f g = getMargin().bbox(getGeometry(), style);
                const auto & range = p.model->observeRange()->get();
                const float step = g.w() / static_cast<float>(range.max - range.min) / 2.f;
                float v = 0.f;
                switch (p.orientation)
                {
                case Orientation::Horizontal:
                    v = Math::clamp((value + step - g.x() - p.handleWidth / 2.f) / (g.w() - p.handleWidth), 0.f, 1.f);
                    break;
                case Orientation::Vertical:
                    v = 1.f - Math::clamp((value + step - g.y() - p.handleWidth / 2.f) / (g.h() - p.handleWidth), 0.f, 1.f);
                    break;
                default: break;
                }
                out = static_cast<int>(v * (range.max - range.min) + range.min);
            }
            return out;
        }

        void BasicIntSlider::_resetTimer()
        {
            DJV_PRIVATE_PTR();
            auto weak = std::weak_ptr<BasicIntSlider>(std::dynamic_pointer_cast<BasicIntSlider>(shared_from_this()));
            p.delayTimer->start(
                p.delay,
                [weak](float)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->model->setValue(widget->_p->value);
                    if (widget->_p->callback)
                    {
                        widget->_p->callback(widget->_p->model->observeValue()->get());
                    }
                }
            });
        }

        struct IntSlider::Private
        {
            std::shared_ptr<IntEdit> edit;
            std::shared_ptr<BasicIntSlider> slider;
            std::shared_ptr<HorizontalLayout> layout;
            std::function<void(int)> callback;
            std::shared_ptr<ValueObserver<int> > valueObserver;
        };

        void IntSlider::_init(Context * context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();

            setClassName("djv::UI::IntSlider");

            p.edit = IntEdit::create(context);
            p.slider = BasicIntSlider::create(Orientation::Horizontal, context);
            p.edit->setModel(p.slider->getModel());

            p.layout = HorizontalLayout::create(context);
            p.layout->setSpacing(MetricsRole::SpacingSmall);
            p.layout->addChild(p.edit);
            p.layout->addChild(p.slider);
            p.layout->setStretch(p.slider, RowStretch::Expand);
            addChild(p.layout);

            auto weak = std::weak_ptr<IntSlider>(std::dynamic_pointer_cast<IntSlider>(shared_from_this()));
            p.valueObserver = ValueObserver<int>::create(
                p.slider->getModel()->observeValue(),
                [weak](int value)
            {
                if (auto widget = weak.lock())
                {
                    if (widget->_p->callback)
                    {
                        widget->_p->callback(value);
                    }
                }
            });
        }

        IntSlider::IntSlider() :
            _p(new Private)
        {}

        IntSlider::~IntSlider()
        {}

        std::shared_ptr<IntSlider> IntSlider::create(Context * context)
        {
            auto out = std::shared_ptr<IntSlider>(new IntSlider);
            out->_init(context);
            return out;
        }

        IntRange IntSlider::getRange() const
        {
            return _p->slider->getRange();
        }

        void IntSlider::setRange(const IntRange& value)
        {
            _p->slider->setRange(value);
        }

        int IntSlider::getValue() const
        {
            return _p->slider->getValue();
        }

        void IntSlider::setValue(int value)
        {
            _p->slider->setValue(value);
        }

        void IntSlider::setValueCallback(const std::function<void(int)>& callback)
        {
            _p->callback = callback;
        }

        std::chrono::milliseconds IntSlider::getDelay() const
        {
            return _p->slider->getDelay();
        }

        void IntSlider::setDelay(std::chrono::milliseconds value)
        {
            _p->slider->setDelay(value);
        }

        const std::shared_ptr<IntValueModel> & IntSlider::getModel() const
        {
            return _p->slider->getModel();
        }

        void IntSlider::_preLayoutEvent(Event::PreLayout & event)
        {
            auto style = _getStyle();
            _setMinimumSize(_p->layout->getMinimumSize() + getMargin().getSize(style));
        }

        void IntSlider::_layoutEvent(Event::Layout & event)
        {
            auto style = _getStyle();
            _p->layout->setGeometry(getMargin().bbox(getGeometry(), style));
        }

    } // namespace UI
} // namespace djv
