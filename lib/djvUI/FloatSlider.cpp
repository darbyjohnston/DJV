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

#include <djvUI/FloatSlider.h>

#include <djvUI/DrawUtil.h>
#include <djvUI/FloatEdit.h>
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
        struct BasicFloatSlider::Private
        {
            float value = 0.f;
            std::shared_ptr<FloatValueModel> model;
            Orientation orientation = Orientation::First;
            AV::Font::Metrics fontMetrics;
            std::future<AV::Font::Metrics> fontMetricsFuture;
            float handleWidth = 0.f;
            std::chrono::milliseconds delay = std::chrono::milliseconds(0);
            std::shared_ptr<Time::Timer> delayTimer;
            Event::PointerID pressedID = Event::InvalidID;
            glm::vec2 pressedPos = glm::vec2(0.f, 0.f);
            glm::vec2 prevPos = glm::vec2(0.f, 0.f);
            std::function<void(float)> callback;
            std::shared_ptr<ValueObserver<float> > valueObserver;
        };

        void BasicFloatSlider::_init(Orientation orientation, Context * context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();

            setClassName("djv::UI::BasicFloatSlider");
            setModel(FloatValueModel::create());
            setPointerEnabled(true);

            p.orientation = orientation;

            p.delayTimer = Time::Timer::create(context);
        }

        BasicFloatSlider::BasicFloatSlider() :
            _p(new Private)
        {}

        BasicFloatSlider::~BasicFloatSlider()
        {}

        std::shared_ptr<BasicFloatSlider> BasicFloatSlider::create(Orientation orientation, Context * context)
        {
            auto out = std::shared_ptr<BasicFloatSlider>(new BasicFloatSlider);
            out->_init(orientation, context);
            return out;
        }

        FloatRange BasicFloatSlider::getRange() const
        {
            return _p->model->observeRange()->get();
        }

        void BasicFloatSlider::setRange(const FloatRange& value)
        {
            _p->model->setRange(value);
        }

        float BasicFloatSlider::getValue() const
        {
            return _p->model->observeValue()->get();
        }

        void BasicFloatSlider::setValue(float value)
        {
            _p->model->setValue(value);
        }

        void BasicFloatSlider::setValueCallback(const std::function<void(float)>& callback)
        {
            _p->callback = callback;
        }

        const std::shared_ptr<FloatValueModel> & BasicFloatSlider::getModel() const
        {
            return _p->model;
        }

        std::chrono::milliseconds BasicFloatSlider::getDelay() const
        {
            return _p->delay;
        }

        void BasicFloatSlider::setDelay(std::chrono::milliseconds value)
        {
            _p->delay = value;
        }

        void BasicFloatSlider::setModel(const std::shared_ptr<FloatValueModel> & model)
        {
            DJV_PRIVATE_PTR();
            if (p.model)
            {
                p.valueObserver.reset();
            }
            p.model = model;
            if (p.model)
            {
                auto weak = std::weak_ptr<BasicFloatSlider>(std::dynamic_pointer_cast<BasicFloatSlider>(shared_from_this()));
                p.valueObserver = ValueObserver<float>::create(
                    p.model->observeValue(),
                    [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->value = value;
                        widget->_redraw();
                    }
                });
            }
        }

        void BasicFloatSlider::_styleEvent(Event::Style& event)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            p.handleWidth = style->getMetric(MetricsRole::Handle);
            auto fontSystem = _getFontSystem();
            p.fontMetricsFuture = fontSystem->getMetrics(style->getFontInfo(AV::Font::faceDefault, MetricsRole::FontMedium));
        }

        void BasicFloatSlider::_preLayoutEvent(Event::PreLayout & event)
        {
            DJV_PRIVATE_PTR();
            if (p.fontMetricsFuture.valid())
            {
                try
                {
                    p.fontMetrics = p.fontMetricsFuture.get();
                }
                catch (const std::exception& e)
                {
                    _log(e.what(), LogLevel::Error);
                }
            }
            const auto& style = _getStyle();
            const float s = style->getMetric(MetricsRole::Slider);
            glm::vec2 size(0.f, 0.f);
            switch (p.orientation)
            {
            case Orientation::Horizontal: size = glm::vec2(s, p.fontMetrics.lineHeight); break;
            case Orientation::Vertical:   size = glm::vec2(p.fontMetrics.lineHeight, s); break;
            default: break;
            }
            _setMinimumSize(size + getMargin().getSize(style));
        }

        void BasicFloatSlider::_paintEvent(Event::Paint & event)
        {
            Widget::_paintEvent(event);
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const BBox2f& g = getMargin().bbox(getGeometry(), style);
            const glm::vec2 c = g.getCenter();
            const float m = style->getMetric(MetricsRole::MarginSmall);
            const float b = style->getMetric(MetricsRole::Border);
            auto render = _getRender();
            render->setFillColor(style->getColor(ColorRole::Border));
            drawBorder(render, g, b);
            const BBox2f g2 = g.margin(-b);
            if (p.model)
            {
                const auto & range = p.model->observeRange()->get();
                float v = (p.value - range.min) / (range.max - range.min);
                render->setFillColor(style->getColor(ColorRole::Checked));
                switch (p.orientation)
                {
                case Orientation::Horizontal:
                    render->drawRect(BBox2f(
                        g2.min.x,
                        g2.min.y,
                        ceilf((g2.w() - p.handleWidth / 2.f) * v),
                        g2.h()));
                    break;
                case Orientation::Vertical:
                {
                    render->drawRect(BBox2f(
                        g2.min.x,
                        g2.min.y,
                        g2.w(),
                        ceilf((g2.h() - p.handleWidth / 2.f) * v)));
                    break;
                }
                default: break;
                }
            }
            if (p.model)
            {
                BBox2f handleBBox;
                switch (p.orientation)
                {
                case Orientation::Horizontal:
                    handleBBox = BBox2f(
                        floorf(_valueToPos(p.value) - p.handleWidth / 2.f),
                        g.min.y,
                        p.handleWidth,
                        g.h());
                    break;
                case Orientation::Vertical:
                    handleBBox = BBox2f(
                        g.min.x,
                        floorf(_valueToPos(p.value) - p.handleWidth / 2.f),
                        g.w(),
                        p.handleWidth);
                    break;
                default: break;
                }
                render->setFillColor(style->getColor(ColorRole::Border));
                render->drawRect(handleBBox);
                render->setFillColor(style->getColor(ColorRole::Button));
                render->drawRect(handleBBox.margin(-b));
                if (p.pressedID != Event::InvalidID)
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
        }

        void BasicFloatSlider::_pointerEnterEvent(Event::PointerEnter & event)
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

        void BasicFloatSlider::_pointerLeaveEvent(Event::PointerLeave & event)
        {
            event.accept();
            if (isEnabled(true))
            {
                _redraw();
            }
        }

        void BasicFloatSlider::_pointerMoveEvent(Event::PointerMove & event)
        {
            DJV_PRIVATE_PTR();
            event.accept();
            const auto & pointerInfo = event.getPointerInfo();
            if (pointerInfo.id == p.pressedID)
            {
                float v = 0.f;
                switch (p.orientation)
                {
                case Orientation::Horizontal: v = pointerInfo.projectedPos.x; break;
                case Orientation::Vertical:   v = pointerInfo.projectedPos.y; break;
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

        void BasicFloatSlider::_buttonPressEvent(Event::ButtonPress & event)
        {
            DJV_PRIVATE_PTR();
            if (p.pressedID)
                return;
            event.accept();
            const auto & pointerInfo = event.getPointerInfo();
            p.pressedID = pointerInfo.id;
            p.pressedPos = pointerInfo.projectedPos;
            float v = 0.f;
            switch (p.orientation)
            {
            case Orientation::Horizontal:  v = pointerInfo.projectedPos.x; break;
            case Orientation::Vertical:    v = pointerInfo.projectedPos.y; break;
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

        void BasicFloatSlider::_buttonReleaseEvent(Event::ButtonRelease & event)
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

        float BasicFloatSlider::_valueToPos(float value) const
        {
            DJV_PRIVATE_PTR();
            float out = 0.f;
            if (p.model)
            {
                const auto& style = _getStyle();
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

        float BasicFloatSlider::_posToValue(float value) const
        {
            DJV_PRIVATE_PTR();
            float out = 0.f;
            if (p.model)
            {
                const auto& style = _getStyle();
                const BBox2f g = getMargin().bbox(getGeometry(), style);
                const auto & range = p.model->observeRange()->get();
                float v = 0.f;
                switch (p.orientation)
                {
                case Orientation::Horizontal:
                    v = Math::clamp((value - g.x() - p.handleWidth / 2.f) / (g.w() - p.handleWidth), 0.f, 1.f);
                    break;
                case Orientation::Vertical:
                    v = 1.f - Math::clamp((value - g.y() - p.handleWidth / 2.f) / (g.h() - p.handleWidth), 0.f, 1.f);
                    break;
                default: break;
                }
                out = v * (range.max - range.min) + range.min;
            }
            return out;
        }

        void BasicFloatSlider::_resetTimer()
        {
            DJV_PRIVATE_PTR();
            auto weak = std::weak_ptr<BasicFloatSlider>(std::dynamic_pointer_cast<BasicFloatSlider>(shared_from_this()));
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

        struct FloatSlider::Private
        {
            std::shared_ptr<FloatEdit> edit;
            std::shared_ptr<BasicFloatSlider> slider;
            std::shared_ptr<HorizontalLayout> layout;
            std::function<void(float)> callback;
            std::shared_ptr<ValueObserver<float> > valueObserver;
        };

        void FloatSlider::_init(Context * context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();

            setClassName("djv::UI::FloatSlider");

            p.edit = FloatEdit::create(context);
            p.slider = BasicFloatSlider::create(Orientation::Horizontal, context);
            p.edit->setModel(p.slider->getModel());

            p.layout = HorizontalLayout::create(context);
            p.layout->setSpacing(MetricsRole::SpacingSmall);
            p.layout->addChild(p.edit);
            p.layout->addChild(p.slider);
            p.layout->setStretch(p.slider, RowStretch::Expand);
            addChild(p.layout);

            auto weak = std::weak_ptr<FloatSlider>(std::dynamic_pointer_cast<FloatSlider>(shared_from_this()));
            p.valueObserver = ValueObserver<float>::create(
                p.slider->getModel()->observeValue(),
                [weak](float value)
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

        FloatSlider::FloatSlider() :
            _p(new Private)
        {}

        FloatSlider::~FloatSlider()
        {}

        std::shared_ptr<FloatSlider> FloatSlider::create(Context * context)
        {
            auto out = std::shared_ptr<FloatSlider>(new FloatSlider);
            out->_init(context);
            return out;
        }

        FloatRange FloatSlider::getRange() const
        {
            return _p->slider->getRange();
        }

        void FloatSlider::setRange(const FloatRange & value)
        {
            _p->slider->setRange(value);
        }

        float FloatSlider::getValue() const
        {
            return _p->slider->getValue();
        }

        void FloatSlider::setValue(float value)
        {
            _p->slider->setValue(value);
        }

        void FloatSlider::setValueCallback(const std::function<void(float)> & callback)
        {
            _p->callback = callback;
        }

        std::chrono::milliseconds FloatSlider::getDelay() const
        {
            return _p->slider->getDelay();
        }

        void FloatSlider::setDelay(std::chrono::milliseconds value)
        {
            _p->slider->setDelay(value);
        }

        const std::shared_ptr<FloatValueModel> & FloatSlider::getModel() const
        {
            return _p->slider->getModel();
        }

        void FloatSlider::_preLayoutEvent(Event::PreLayout & event)
        {
            const auto& style = _getStyle();
            _setMinimumSize(_p->layout->getMinimumSize() + getMargin().getSize(style));
        }

        void FloatSlider::_layoutEvent(Event::Layout & event)
        {
            const auto& style = _getStyle();
            _p->layout->setGeometry(getMargin().bbox(getGeometry(), style));
        }

    } // namespace UI
} // namespace djv
