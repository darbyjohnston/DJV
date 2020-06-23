// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/IntSlider.h>

#include <djvUI/DrawUtil.h>
#include <djvUI/IntEdit.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ToolButton.h>

#include <djvCore/NumericValueModels.h>
#include <djvCore/ValueObserver.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct BasicIntSlider::Private
        {
            std::shared_ptr<ValueObserver<int> > valueObserver;
        };

        void BasicIntSlider::_init(Orientation orientation, const std::shared_ptr<Context>& context)
        {
            NumericSlider::_init(orientation, context);
            setClassName("djv::UI::BasicIntSlider");
            setModel(IntValueModel::create());
        }

        BasicIntSlider::BasicIntSlider() :
            _p(new Private)
        {}

        BasicIntSlider::~BasicIntSlider()
        {}

        std::shared_ptr<BasicIntSlider> BasicIntSlider::create(Orientation orientation, const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<BasicIntSlider>(new BasicIntSlider);
            out->_init(orientation, context);
            return out;
        }

        void BasicIntSlider::setModel(const std::shared_ptr<INumericValueModel<int> >& model)
        {
            INumericSlider<int>::setModel(model);
            DJV_PRIVATE_PTR();
            if (model)
            {
                auto weak = std::weak_ptr<BasicIntSlider>(std::dynamic_pointer_cast<BasicIntSlider>(shared_from_this()));
                p.valueObserver = ValueObserver<int>::create(
                    model->observeValue(),
                    [weak](int value)
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
        
        void BasicIntSlider::_pointerMove(float pos)
        {
            INumericSlider<int>::_pointerMove(pos, getDelay());
        }
        
        void BasicIntSlider::_buttonPress(float pos)
        {
            INumericSlider<int>::_buttonPress(pos, getDelay());
        }
        
        void BasicIntSlider::_buttonRelease()
        {
            INumericSlider<int>::_buttonRelease(getDelay());
        }
        
        bool BasicIntSlider::_keyPress(int key)
        {
            return INumericSlider<int>::_doKeyPress(fromGLFWKey(key));
        }

        void BasicIntSlider::_scroll(float value)
        {
            if (auto model = getModel())
            {
                model->setValue(model->observeValue()->get() + static_cast<int>(model->observeSmallIncrement()->get() * value));
            }
        }

        void BasicIntSlider::_valueUpdate()
        {
            INumericSlider<int>::_valueUpdate();
        }

        void BasicIntSlider::_paintEvent(Event::Paint& event)
        {
            NumericSlider::_paintEvent(event);
            if (auto model = getModel())
            {
                const auto& range = model->observeRange()->get();
                const float v = (_value - range.getMin()) / static_cast<float>(range.getMax() - range.getMin());
                _paint(v, _valueToPos(_value));
            }
        }

        float BasicIntSlider::_valueToPos(int value) const
        {
            float out = 0.F;
            if (auto model = getModel())
            {
                const auto& style = _getStyle();
                const BBox2f g = getMargin().bbox(getGeometry(), style);
                const float m = style->getMetric(MetricsRole::MarginSmall);
                const float b = style->getMetric(MetricsRole::Border);
                const float handleWidth = _getHandleWidth();
                const auto& range = model->observeRange()->get();
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

        int BasicIntSlider::_posToValue(float value) const
        {
            int out = 0;
            if (auto model = getModel())
            {
                const auto& style = _getStyle();
                const BBox2f g = getMargin().bbox(getGeometry(), style);
                const float m = style->getMetric(MetricsRole::MarginSmall);
                const float b = style->getMetric(MetricsRole::Border);
                const float handleWidth = _getHandleWidth();
                const auto& range = model->observeRange()->get();
                const BBox2f g2 = g.margin(-(m + b));
                const float step = g2.w() / static_cast<float>(range.getMax() - range.getMin()) / 2.F;
                float v = 0.F;
                switch (getOrientation())
                {
                case Orientation::Horizontal:
                    v = Math::clamp((value + step - g2.x() - handleWidth / 2.F) / (g2.w() - handleWidth), 0.F, 1.F);
                    break;
                case Orientation::Vertical:
                    v = 1.F - Math::clamp((value + step - g2.y() - handleWidth / 2.F) / (g2.h() - handleWidth), 0.F, 1.F);
                    break;
                default: break;
                }
                out = static_cast<int>(v * (range.getMax() - range.getMin()) + range.getMin());
            }
            return out;
        }

        struct IntSlider::Private
        {
            std::shared_ptr<Core::INumericValueModel<int> > model;
            int defaultValue = 0;
            std::shared_ptr<IntEdit> edit;
            std::shared_ptr<BasicIntSlider> slider;
            std::shared_ptr<ToolButton> resetButton;
            std::shared_ptr<HorizontalLayout> layout;
            std::function<void(int)> callback;
            std::shared_ptr<ValueObserver<int> > valueObserver;
        };

        void IntSlider::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::UI::IntSlider");

            p.edit = IntEdit::create(context);
            p.slider = BasicIntSlider::create(Orientation::Horizontal, context);
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
            
            auto weak = std::weak_ptr<IntSlider>(std::dynamic_pointer_cast<IntSlider>(shared_from_this()));
            p.resetButton->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->resetValue();
                    }
                });
        }

        IntSlider::IntSlider() :
            _p(new Private)
        {}

        IntSlider::~IntSlider()
        {}

        std::shared_ptr<IntSlider> IntSlider::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<IntSlider>(new IntSlider);
            out->_init(context);
            return out;
        }

        void IntSlider::setModel(const std::shared_ptr<Core::INumericValueModel<int> >& model)
        {
            DJV_PRIVATE_PTR();
            p.model = model;
            _modelUpdate();
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
        
        float IntSlider::getDefault() const
        {
            return _p->defaultValue;
        }
        
        void IntSlider::setDefault(float value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.defaultValue)
                return;
            p.defaultValue = value;
            _widgetUpdate();
        }
        
        void IntSlider::setDefaultVisible(bool value)
        {
            _p->resetButton->setVisible(value);
        }
        
        void IntSlider::resetValue()
        {
            setValue(_p->defaultValue);
        }

        int IntSlider::getSmallIncrement() const
        {
            return _p->model->observeSmallIncrement()->get();
        }

        int IntSlider::getLargeIncrement() const
        {
            return _p->model->observeLargeIncrement()->get();
        }

        void IntSlider::setSmallIncrement(int value)
        {
            _p->model->setSmallIncrement(value);
        }

        void IntSlider::setLargeIncrement(int value)
        {
            _p->model->setLargeIncrement(value);
        }

        const Time::Duration& IntSlider::getDelay() const
        {
            return _p->slider->getDelay();
        }

        void IntSlider::setDelay(const Time::Duration& value)
        {
            _p->slider->setDelay(value);
        }

        const std::shared_ptr<INumericValueModel<int> >& IntSlider::getModel() const
        {
            return _p->slider->getModel();
        }

        void IntSlider::_preLayoutEvent(Event::PreLayout& event)
        {
            const auto& style = _getStyle();
            _setMinimumSize(_p->layout->getMinimumSize() + getMargin().getSize(style));
        }

        void IntSlider::_layoutEvent(Event::Layout& event)
        {
            const auto& style = _getStyle();
            _p->layout->setGeometry(getMargin().bbox(getGeometry(), style));
        }

        void IntSlider::_initEvent(Event::Init& event)
        {
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.resetButton->setTooltip(_getText(DJV_TEXT("reset_the_value")));
            }
        }

        void IntSlider::_modelUpdate()
        {
            DJV_PRIVATE_PTR();
            p.slider->setModel(p.model);
            p.edit->setModel(p.model);
            auto weak = std::weak_ptr<IntSlider>(std::dynamic_pointer_cast<IntSlider>(shared_from_this()));
            p.valueObserver = ValueObserver<int>::create(
                p.model->observeValue(),
                [weak](int value)
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
        
        void IntSlider::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.resetButton->setEnabled(p.slider->getValue() != p.defaultValue);
        }

    } // namespace UI
} // namespace djv
