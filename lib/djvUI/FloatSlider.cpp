// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/FloatSlider.h>

#include <djvUI/DrawUtil.h>
#include <djvUI/FloatEdit.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ToolButton.h>

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
            return INumericSlider<float>::_doKeyPress(fromGLFWKey(key));
        }

        void BasicFloatSlider::_scroll(float value)
        {
            if (auto model = getModel())
            {
                model->setValue(model->observeValue()->get() + model->observeSmallIncrement()->get() * value);
            }
        }
        
        void BasicFloatSlider::_valueUpdate()
        {
            INumericSlider<float>::_valueUpdate();
        }

        void BasicFloatSlider::_paintEvent(Event::Paint & event)
        {
            NumericSlider::_paintEvent(event);
            if (auto model = getModel())
            {
                const auto & range = model->observeRange()->get();
                float v = (_value - range.getMin()) / (range.getMax() - range.getMin());
                _paint(v, _valueToPos(_value));
            }
        }

        float BasicFloatSlider::_valueToPos(float value) const
        {
            float out = 0.F;
            if (auto model = getModel())
            {
                const auto& style = _getStyle();
                const BBox2f g = getMargin().bbox(getGeometry(), style);
                const float m = style->getMetric(MetricsRole::MarginSmall);
                const float b = style->getMetric(MetricsRole::Border);
                const float handleWidth = _getHandleWidth();
                const auto & range = model->observeRange()->get();
                float v = (value - range.getMin()) / static_cast<float>(range.getMax() - range.getMin());
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
                out = v * (range.getMax() - range.getMin()) + range.getMin();
            }
            return out;
        }

        struct FloatSlider::Private
        {
            std::shared_ptr<Core::INumericValueModel<float> > model;
            float defaultValue = 0.F;
            std::shared_ptr<FloatEdit> edit;
            std::shared_ptr<BasicFloatSlider> slider;
            std::shared_ptr<ToolButton> resetButton;
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
            p.model = p.slider->getModel();
            
            p.resetButton = ToolButton::create(context);
            p.resetButton->setIcon("djvIconClear");
            p.resetButton->hide();

            p.layout = HorizontalLayout::create(context);
            p.layout->setSpacing(MetricsRole::None);
            p.layout->addChild(p.edit);
            p.layout->addChild(p.slider);
            p.layout->setStretch(p.slider, RowStretch::Expand);
            p.layout->addChild(p.resetButton);
            addChild(p.layout);

            _modelUpdate();
            _widgetUpdate();
            
            auto weak = std::weak_ptr<FloatSlider>(std::dynamic_pointer_cast<FloatSlider>(shared_from_this()));
            p.resetButton->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->resetValue();
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

        void FloatSlider::setModel(const std::shared_ptr<Core::INumericValueModel<float> >& model)
        {
            DJV_PRIVATE_PTR();
            p.model = model;
            _modelUpdate();
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
        
        float FloatSlider::getDefault() const
        {
            return _p->defaultValue;
        }
        
        void FloatSlider::setDefault(float value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.defaultValue)
                return;
            p.defaultValue = value;
            _widgetUpdate();
        }
        
        void FloatSlider::setDefaultVisible(bool value)
        {
            _p->resetButton->setVisible(value);
        }
        
        void FloatSlider::resetValue()
        {
            setValue(_p->defaultValue);
        }

        float FloatSlider::getSmallIncrement() const
        {
            return _p->model->observeSmallIncrement()->get();
        }

        float FloatSlider::getLargeIncrement() const
        {
            return _p->model->observeLargeIncrement()->get();
        }

        void FloatSlider::setSmallIncrement(float value)
        {
            _p->model->setSmallIncrement(value);
        }

        void FloatSlider::setLargeIncrement(float value)
        {
            _p->model->setLargeIncrement(value);
        }

        const Time::Duration& FloatSlider::getDelay() const
        {
            return _p->slider->getDelay();
        }

        void FloatSlider::setDelay(const Time::Duration& value)
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

        void FloatSlider::_initEvent(Event::Init& event)
        {
            Widget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.resetButton->setTooltip(_getText(DJV_TEXT("reset_the_value")));
            }
        }

        void FloatSlider::_modelUpdate()
        {
            DJV_PRIVATE_PTR();
            p.slider->setModel(p.model);
            p.edit->setModel(p.model);
            auto weak = std::weak_ptr<FloatSlider>(std::dynamic_pointer_cast<FloatSlider>(shared_from_this()));
            p.valueObserver = ValueObserver<float>::create(
                p.model->observeValue(),
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_widgetUpdate();
                        if (widget->_p->callback)
                        {
                            widget->_p->callback(value);
                        }
                    }
                });
        }

        void FloatSlider::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.resetButton->setEnabled(p.slider->getValue() != p.defaultValue);
        }

    } // namespace UI
} // namespace djv
