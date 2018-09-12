//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
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

#include <djvUI/IntEditSlider.h>

#include <djvUI/UIContext.h>
#include <djvUI/IconLibrary.h>
#include <djvUI/IntEdit.h>
#include <djvUI/IntObject.h>
#include <djvUI/IntSlider.h>
#include <djvUI/ToolButton.h>

#include <djvCore/Debug.h>
#include <djvCore/SignalBlocker.h>

#include <QApplication>
#include <QHBoxLayout>

namespace djv
{
    namespace UI
    {
        struct IntEditSlider::Private
        {
            bool resetToDefault = false;
            IntEdit * edit = nullptr;
            IntSlider * slider = nullptr;
            ToolButton * defaultButton = nullptr;
        };

        IntEditSlider::IntEditSlider(UIContext * context, QWidget * parent) :
            QWidget(parent),
            _p(new Private)
        {
            _p->edit = new IntEdit;

            _p->slider = new IntSlider;

            _p->defaultButton = new ToolButton;
            _p->defaultButton->setIconSize(QSize(16, 16));
            _p->defaultButton->setIcon(
                context->iconLibrary()->icon("djvResetIcon.png"));
            _p->defaultButton->setToolTip(
                qApp->translate("djv::UI::IntEditSlider", "Reset the value"));

            QHBoxLayout * layout = new QHBoxLayout(this);
            layout->setSpacing(5);
            layout->setMargin(0);
            layout->addWidget(_p->edit);
            layout->addWidget(_p->slider);
            layout->addWidget(_p->defaultButton);

            _p->slider->setRange(0, 100);

            _p->defaultButton->hide();

            widgetUpdate();

            connect(
                _p->edit,
                SIGNAL(valueChanged(int)),
                SLOT(valueCallback()));
            connect(
                _p->edit,
                SIGNAL(minChanged(int)),
                SIGNAL(minChanged(int)));
            connect(
                _p->edit,
                SIGNAL(maxChanged(int)),
                SIGNAL(maxChanged(int)));
            connect(
                _p->edit,
                SIGNAL(rangeChanged(int, int)),
                SIGNAL(rangeChanged(int, int)));
            connect(
                _p->edit->object(),
                SIGNAL(defaultValidChanged(bool)),
                SLOT(widgetUpdate()));
            connect(
                _p->edit->object(),
                SIGNAL(defaultValueChanged(int)),
                SIGNAL(defaultValueChanged(int)));
            connect(
                _p->slider,
                SIGNAL(valueChanged(int)),
                SLOT(sliderCallback(int)));
            connect(
                _p->defaultButton,
                SIGNAL(clicked()),
                SLOT(defaultCallback()));
        }

        IntEditSlider::~IntEditSlider()
        {}

        int IntEditSlider::value() const
        {
            return _p->edit->value();
        }

        int IntEditSlider::defaultValue() const
        {
            return _p->edit->object()->defaultValue();
        }

        bool IntEditSlider::hasResetToDefault() const
        {
            return _p->resetToDefault;
        }

        int IntEditSlider::min() const
        {
            return _p->edit->min();
        }

        int IntEditSlider::max() const
        {
            return _p->edit->max();
        }

        IntObject * IntEditSlider::editObject() const
        {
            return _p->edit->object();
        }

        IntObject * IntEditSlider::sliderObject() const
        {
            return _p->slider->object();
        }

        void IntEditSlider::setValue(int value)
        {
            _p->edit->setValue(value);
        }

        void IntEditSlider::setDefaultValue(int value)
        {
            _p->edit->object()->setDefaultValue(value);
        }

        void IntEditSlider::setResetToDefault(bool value)
        {
            if (value == _p->resetToDefault)
                return;
            _p->resetToDefault = value;
            widgetUpdate();
        }

        void IntEditSlider::setMin(int value)
        {
            _p->edit->setMin(value);
            _p->slider->setMin(value);
        }

        void IntEditSlider::setMax(int value)
        {
            _p->edit->setMax(value);
            _p->slider->setMax(value);
        }

        void IntEditSlider::setRange(int min, int max)
        {
            _p->edit->setRange(min, max);
            _p->slider->setRange(min, max);
        }

        void IntEditSlider::valueCallback()
        {
            widgetUpdate();
            Q_EMIT valueChanged(_p->edit->value());
        }

        void IntEditSlider::sliderCallback(int value)
        {
            _p->edit->setValue(value);
        }

        void IntEditSlider::defaultCallback()
        {
            _p->edit->setValue(_p->edit->object()->defaultValue());
        }

        void IntEditSlider::widgetUpdate()
        {
            //DJV_DEBUG("IntEditSlider::widgetUpdate");
            //DJV_DEBUG_PRINT("value = " << value());
            //DJV_DEBUG_PRINT("defaultValue = " << defaultValue());
            djvSignalBlocker signalBlocker(QObjectList() <<
                _p->edit <<
                _p->slider <<
                _p->defaultButton);
            _p->slider->setValue(_p->edit->value());
            _p->defaultButton->setVisible(_p->resetToDefault);
            _p->defaultButton->setDisabled(_p->edit->object()->isDefaultValid());
        }

    } // namespace UI
} // namespace djv
