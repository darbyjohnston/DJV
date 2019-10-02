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

#include <djvCore/NumericValueModels.h>
#include <djvCore/ValueObserver.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct BasicFloatSlider::Private
        {
            std::shared_ptr<ValueObserver<float> > valueObserver;
        };

        void BasicFloatSlider::_init(Orientation orientation, const std::shared_ptr<Context>& context)
        {
            NumericSlider::_init(orientation, context);
            DJV_PRIVATE_PTR();
            setClassName("djv::UI::BasicFloatSlider");
            setModel(FloatValueModel::create());
        }

        BasicFloatSlider::BasicFloatSlider() :
            _p(new Private)
        {}

        BasicFloatSlider::~BasicFloatSlider()
        {}

        std::shared_ptr<BasicFloatSlider> BasicFloatSlider::create(Orientation orientation, const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<BasicFloatSlider>(new BasicFloatSlider);
            out->_init(orientation, context);
            return out;
        }

        void BasicFloatSlider::setModel(const std::shared_ptr<INumericValueModel<float> > & model)
        {
            INumericSlider<float>::setModel(model);
            DJV_PRIVATE_PTR();
            if (model)
            {
                auto weak = std::weak_ptr<BasicFloatSlider>(std::dynamic_pointer_cast<BasicFloatSlider>(shared_from_this()));
                p.valueObserver = ValueObserver<float>::create(
                    model->observeValue(),
                    [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_value = value;
                        widget->_redraw();
                    }
                });
            }
            else
            {
                p.valueObserver.reset();
            }
        }
        
        void BasicFloatSlider::_pointerMove(float pos)
        {
            INumericSlider<float>::_pointerMove(pos, getDelay());
        }
        
        void BasicFloatSlider::_buttonPress(float pos)
        {
            INumericSlider<float>::_buttonPress(pos, getDelay());
        }
        
        void BasicFloatSlider::_buttonRelease()
        {
            INumericSlider<float>::_buttonRelease(getDelay());
        }
        
        bool BasicFloatSlider::_keyPress(int key)
        {
            return INumericSlider<float>::_keyPress(fromGLFWKey(key));
        }
        
        void BasicFloatSlider::_valueUpdate()
        {
            INumericSlider<float>::_valueUpdate();
        }

        void BasicFloatSlider::_paintEvent(Event::Paint & event)
        {
            NumericSlider::_paintEvent(event);
            DJV_PRIVATE_PTR();
            if (auto model = getModel())
            {
                const auto & range = model->observeRange()->get();
                float v = (_value - range.min) / (range.max - range.min);
                _paint(v, _valueToPos(_value));
            }
        }

        float BasicFloatSlider::_valueToPos(float value) const
        {
            DJV_PRIVATE_PTR();
            float out = 0.F;
            if (auto model = getModel())
            {
                const auto& style = _getStyle();
                const BBox2f g = getMargin().bbox(getGeometry(), style);
                const float m = style->getMetric(MetricsRole::MarginSmall);
                const float b = style->getMetric(MetricsRole::Border);
                const float handleWidth = _getHandleWidth();
                const auto & range = model->observeRange()->get();
                float v = (value - range.min) / static_cast<float>(range.max - range.min);
                const BBox2f g2 = g.margin(-(m + b));
                switch (getOrientation())
                {
                case Orientation::Horizontal:
                    out = g2.x() + ceilf(handleWidth / 2.F + (g2.w() - handleWidth) * v);
                    break;
                case Orientation::Vertical:
                    v = 1.F - v;
                    out = g2.y() + ceilf(handleWidth / 2.F + (g2.h() - handleWidth) * v);
                    break;
                default: break;
                }
            }
            return out;
        }

        float BasicFloatSlider::_posToValue(float value) const
        {
            DJV_PRIVATE_PTR();
            float out = 0.F;
            if (auto model = getModel())
            {
                const auto& style = _getStyle();
                const BBox2f g = getMargin().bbox(getGeometry(), style);
                const float m = style->getMetric(MetricsRole::MarginSmall);
                const float b = style->getMetric(MetricsRole::Border);
                const float handleWidth = _getHandleWidth();
                const auto & range = model->observeRange()->get();
                const BBox2f g2 = g.margin(-(m + b));
                float v = 0.F;
                switch (getOrientation())
                {
                case Orientation::Horizontal:
                    v = Math::clamp((value - g2.x() - handleWidth / 2.F) / (g2.w() - handleWidth), 0.F, 1.F);
                    break;
                case Orientation::Vertical:
                    v = 1.F - Math::clamp((value - g2.y() - handleWidth / 2.F) / (g2.h() - handleWidth), 0.F, 1.F);
                    break;
                default: break;
                }
                out = v * (range.max - range.min) + range.min;
            }
            return out;
        }

        struct FloatSlider::Private
        {
            std::shared_ptr<FloatEdit> edit;
            std::shared_ptr<BasicFloatSlider> slider;
            std::shared_ptr<HorizontalLayout> layout;
            std::function<void(float)> callback;
            std::shared_ptr<ValueObserver<float> > valueObserver;
        };

        void FloatSlider::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();

            setClassName("djv::UI::FloatSlider");

            p.edit = FloatEdit::create(context);
            p.slider = BasicFloatSlider::create(Orientation::Horizontal, context);
            p.edit->setModel(p.slider->getModel());

            p.layout = HorizontalLayout::create(context);
            p.layout->setSpacing(Layout::Spacing(MetricsRole::SpacingSmall));
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

        std::shared_ptr<FloatSlider> FloatSlider::create(const std::shared_ptr<Context>& context)
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

        const std::shared_ptr<INumericValueModel<float>> & FloatSlider::getModel() const
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
