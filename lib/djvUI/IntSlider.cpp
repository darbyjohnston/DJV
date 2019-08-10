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

#include <djvUI/DrawUtil.h>
#include <djvUI/IntEdit.h>
#include <djvUI/RowLayout.h>

#include <djvCore/NumericValueModels.h>
#include <djvCore/ValueObserver.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct BasicIntSlider::Private
        {
            int value = 0;
            std::shared_ptr<IntValueModel> model;
            std::function<void(int)> callback;
            std::shared_ptr<ValueObserver<int> > valueObserver;
        };

        void BasicIntSlider::_init(Orientation orientation, Context * context)
        {
            INumericSlider::_init(orientation, context);
            DJV_PRIVATE_PTR();
            setClassName("djv::UI::BasicIntSlider");
            setModel(IntValueModel::create());
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
        
        void BasicIntSlider::_pointerMove(float pos)
        {
            DJV_PRIVATE_PTR();
            p.value = _posToValue(pos);
            if (p.model && std::chrono::milliseconds(0) == getDelay())
            {
                p.model->setValue(p.value);
                if (p.callback)
                {
                    p.callback(p.model->observeValue()->get());
                }
            }
        }
        
        void BasicIntSlider::_buttonPress(float pos)
        {
            DJV_PRIVATE_PTR();
            p.value = _posToValue(pos);
            if (p.model && std::chrono::milliseconds(0) == getDelay())
            {
                p.model->setValue(p.value);
                if (p.callback)
                {
                    p.callback(p.model->observeValue()->get());
                }
            }
        }
        
        void BasicIntSlider::_buttonRelease()
        {
            DJV_PRIVATE_PTR();
            if (p.model && getDelay() > std::chrono::milliseconds(0))
            {
                p.model->setValue(p.value);
                if (p.callback)
                {
                    p.callback(p.model->observeValue()->get());
                }
            }
        }
        
        bool BasicIntSlider::_keyPress(int key)
        {
            DJV_PRIVATE_PTR();
            bool out = false;
            switch (key)
            {
            case GLFW_KEY_HOME:
                if (p.model)
                {
                    out = true;
                    p.model->setMin();
                }
                break;
            case GLFW_KEY_END:
                if (p.model)
                {
                    out = true;
                    p.model->setMax();
                }
                break;
            case GLFW_KEY_UP:
            case GLFW_KEY_RIGHT:
                if (p.model)
                {
                    out = true;
                    p.model->incrementSmall();
                }
                break;
            case GLFW_KEY_DOWN:
            case GLFW_KEY_LEFT:
                if (p.model)
                {
                    out = true;
                    p.model->decrementSmall();
                }
                break;
            case GLFW_KEY_PAGE_UP:
                if (p.model)
                {
                    out = true;
                    p.model->incrementLarge();
                }
                break;
            case GLFW_KEY_PAGE_DOWN:
                if (p.model)
                {
                    out = true;
                    p.model->decrementLarge();
                }
                break;
            }
            return out;
        }
        
        void BasicIntSlider::_valueUpdate()
        {
            DJV_PRIVATE_PTR();
            p.model->setValue(p.value);
            if (p.callback)
            {
                p.callback(p.model->observeValue()->get());
            }
        }

        void BasicIntSlider::_paintEvent(Event::Paint & event)
        {
            INumericSlider::_paintEvent(event);
            DJV_PRIVATE_PTR();
            if (p.model)
            {
                const auto& range = p.model->observeRange()->get();
                const float v = (p.value - range.min) / static_cast<float>(range.max - range.min);
                _paint(v, _valueToPos(p.value));
            }
        }

        float BasicIntSlider::_valueToPos(int value) const
        {
            DJV_PRIVATE_PTR();
            float out = 0.f;
            if (p.model)
            {
                const auto& style = _getStyle();
                const BBox2f g = getMargin().bbox(getGeometry(), style);
                const float handleWidth = _getHandleWidth();
                const auto & range = p.model->observeRange()->get();
                float v = (value - range.min) / static_cast<float>(range.max - range.min);
                switch (getOrientation())
                {
                case Orientation::Horizontal:
                    out = g.x() + ceilf(handleWidth / 2.f + (g.w() - handleWidth) * v);
                    break;
                case Orientation::Vertical:
                    v = 1.f - v;
                    out = g.y() + ceilf(handleWidth / 2.f + (g.h() - handleWidth) * v);
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
                const auto& style = _getStyle();
                const BBox2f g = getMargin().bbox(getGeometry(), style);
                const float handleWidth = _getHandleWidth();
                const auto & range = p.model->observeRange()->get();
                const float step = g.w() / static_cast<float>(range.max - range.min) / 2.f;
                float v = 0.f;
                switch (getOrientation())
                {
                case Orientation::Horizontal:
                    v = Math::clamp((value + step - g.x() - handleWidth / 2.f) / (g.w() - handleWidth), 0.f, 1.f);
                    break;
                case Orientation::Vertical:
                    v = 1.f - Math::clamp((value + step - g.y() - handleWidth / 2.f) / (g.h() - handleWidth), 0.f, 1.f);
                    break;
                default: break;
                }
                out = static_cast<int>(v * (range.max - range.min) + range.min);
            }
            return out;
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
            const auto& style = _getStyle();
            _setMinimumSize(_p->layout->getMinimumSize() + getMargin().getSize(style));
        }

        void IntSlider::_layoutEvent(Event::Layout & event)
        {
            const auto& style = _getStyle();
            _p->layout->setGeometry(getMargin().bbox(getGeometry(), style));
        }

    } // namespace UI
} // namespace djv
