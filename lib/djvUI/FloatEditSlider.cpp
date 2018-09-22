//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
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

#include <djvUI/FloatEditSlider.h>

#include <djvUI/IconLibrary.h>
#include <djvUI/FloatEdit.h>
#include <djvUI/FloatObject.h>
#include <djvUI/FloatSlider.h>
#include <djvUI/ToolButton.h>
#include <djvUI/UIContext.h>

#include <djvCore/Debug.h>
#include <djvCore/SignalBlocker.h>

#include <QApplication>
#include <QHBoxLayout>

namespace djv
{
    namespace UI
    {
        struct FloatEditSlider::Private
        {
            UIContext * context = nullptr;
            bool resetToDefault = true;
            FloatEdit * edit = nullptr;
            FloatSlider * slider = nullptr;
            ToolButton * defaultButton = nullptr;
            QHBoxLayout * layout = nullptr;
        };

        FloatEditSlider::FloatEditSlider(UIContext * context, QWidget * parent) :
            QWidget(parent),
            _p(new Private)
        {
            _p->context = context;
            
            _p->edit = new FloatEdit;
            _p->slider = new FloatSlider;
            _p->defaultButton = new ToolButton(context);
            _p->defaultButton->setToolTip(
                qApp->translate("djv::UI::FloatEditSlider", "Reset the value"));

            _p->layout = new QHBoxLayout(this);
            _p->layout->setMargin(0);
            _p->layout->addWidget(_p->edit);
            _p->layout->addWidget(_p->slider);
            _p->layout->addWidget(_p->defaultButton);

            _p->slider->setRange(0.f, 1.f);
            _p->defaultButton->hide();
            styleUpdate();
            valueUpdate();

            connect(
                _p->edit,
                SIGNAL(valueChanged(float)),
                SLOT(valueCallback()));
            connect(
                _p->edit,
                SIGNAL(minChanged(float)),
                SIGNAL(minChanged(float)));
            connect(
                _p->edit,
                SIGNAL(maxChanged(float)),
                SIGNAL(maxChanged(float)));
            connect(
                _p->edit,
                SIGNAL(rangeChanged(float, float)),
                SIGNAL(rangeChanged(float, float)));
            connect(
                _p->edit->object(),
                SIGNAL(defaultValidChanged(bool)),
                SLOT(valueUpdate()));
            connect(
                _p->edit->object(),
                SIGNAL(defaultValueChanged(float)),
                SIGNAL(defaultValueChanged(float)));
            connect(
                _p->slider,
                SIGNAL(valueChanged(float)),
                SLOT(sliderCallback(float)));
            connect(
                _p->defaultButton,
                SIGNAL(clicked()),
                SLOT(defaultCallback()));
        }
        
        FloatEditSlider::~FloatEditSlider()
        {}

        float FloatEditSlider::value() const
        {
            return _p->edit->value();
        }

        float FloatEditSlider::defaultValue() const
        {
            return _p->edit->object()->defaultValue();
        }

        bool FloatEditSlider::hasResetToDefault() const
        {
            return _p->resetToDefault;
        }

        float FloatEditSlider::min() const
        {
            return _p->edit->min();
        }

        float FloatEditSlider::max() const
        {
            return _p->edit->max();
        }

        float FloatEditSlider::smallInc() const
        {
            return _p->edit->object()->smallInc();
        }

        float FloatEditSlider::largeInc() const
        {
            return _p->edit->object()->largeInc();
        }

        FloatObject * FloatEditSlider::editObject() const
        {
            return _p->edit->object();
        }

        FloatObject * FloatEditSlider::sliderObject() const
        {
            return _p->slider->object();
        }

        void FloatEditSlider::setValue(float value)
        {
            _p->edit->setValue(value);
        }

        void FloatEditSlider::setDefaultValue(float value)
        {
            //DJV_DEBUG("FloatEditSlider::setDefaultValue");
            //DJV_DEBUG_PRINT("in = " << value);
            _p->edit->object()->setDefaultValue(value);
        }

        void FloatEditSlider::setResetToDefault(bool value)
        {
            if (value == _p->resetToDefault)
                return;
            _p->resetToDefault = value;
            valueUpdate();
        }

        void FloatEditSlider::setMin(float value)
        {
            _p->edit->setMin(value);
            _p->slider->setMin(value);
        }

        void FloatEditSlider::setMax(float value)
        {
            _p->edit->setMax(value);
            _p->slider->setMax(value);
        }

        void FloatEditSlider::setRange(float min, float max)
        {
            _p->edit->setRange(min, max);
            _p->slider->setRange(min, max);
        }

        void FloatEditSlider::setInc(float smallInc, float largeInc)
        {
            _p->edit->object()->setInc(smallInc, largeInc);
            _p->slider->object()->setInc(smallInc, largeInc);
        }

        bool FloatEditSlider::event(QEvent * event)
        {
            if (QEvent::StyleChange == event->type())
            {
                styleUpdate();
            }
            return QWidget::event(event);
        }

        void FloatEditSlider::valueCallback()
        {
            valueUpdate();
            Q_EMIT valueChanged(_p->edit->value());
        }

        void FloatEditSlider::sliderCallback(float value)
        {
            _p->edit->setValue(value);
        }

        void FloatEditSlider::defaultCallback()
        {
            _p->edit->setValue(_p->edit->object()->defaultValue());
        }

        void FloatEditSlider::styleUpdate()
        {
            _p->defaultButton->setIcon(_p->context->iconLibrary()->icon("djv/UI/ResetIcon"));
            updateGeometry();
        }

        void FloatEditSlider::valueUpdate()
        {
            //DJV_DEBUG("FloatEditSlider::valueUpdate");
            //DJV_DEBUG_PRINT("value = " << value());
            //DJV_DEBUG_PRINT("min = " << min());
            //DJV_DEBUG_PRINT("max = " << max());
            //DJV_DEBUG_PRINT("defaultValue = " << defaultValue());
            Core::SignalBlocker signalBlocker(QObjectList() <<
                _p->edit <<
                _p->slider <<
                _p->defaultButton);
            _p->slider->setValue(_p->edit->value());
            _p->defaultButton->setVisible(_p->resetToDefault);
            _p->defaultButton->setDisabled(_p->edit->object()->isDefaultValid());
        }

    } // namespace UI
} // namespace djv
