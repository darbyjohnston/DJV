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

#include <djvUI/FloatSlider.h>

#include <djvUI/FloatObject.h>

#include <djvCore/SignalBlocker.h>

#include <djvCore/Debug.h>

#include <QHBoxLayout>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QSlider>
#include <QStyle>

namespace djv
{
    namespace UI
    {
        namespace
        {
            //! \todo Should we implement our own slider for better control of the position
            //! and value?
            class Slider : public QSlider
            {
            public:
                Slider() :
                    QSlider(Qt::Horizontal)
                {}

            protected:
                virtual void mousePressEvent(QMouseEvent * event)
                {
                    const int t = style()->pixelMetric(QStyle::PM_SliderThickness);

                    setValue(QStyle::sliderValueFromPosition(
                        minimum(),
                        maximum(),
                        event->pos().x() - t / 2,
                        width() - t));
                }

                virtual void mouseMoveEvent(QMouseEvent * event)
                {
                    const int t = style()->pixelMetric(QStyle::PM_SliderThickness);

                    setValue(QStyle::sliderValueFromPosition(
                        minimum(),
                        maximum(),
                        event->pos().x() - t / 2,
                        width() - t));
                }
            };

        } // namespace

        struct FloatSlider::Private
        {
            FloatObject * object = nullptr;
            Slider * slider = nullptr;
        };

        namespace
        {
            const int steps = 10000;

        } // namespace

        FloatSlider::FloatSlider(QWidget * parent) :
            QWidget(parent),
            _p(new Private)
        {
            _p->object = new FloatObject(this);

            _p->slider = new Slider;
            _p->slider->setRange(0, steps);

            QHBoxLayout * layout = new QHBoxLayout(this);
            layout->setMargin(0);
            layout->addWidget(_p->slider);

            _p->object->setRange(0.f, 1.f);

            widgetUpdate();

            connect(
                _p->object,
                SIGNAL(valueChanged(float)),
                SLOT(valueCallback()));
            connect(
                _p->object,
                SIGNAL(minChanged(float)),
                SLOT(rangeCallback()));
            connect(
                _p->object,
                SIGNAL(maxChanged(float)),
                SLOT(rangeCallback()));
            connect(
                _p->object,
                SIGNAL(rangeChanged(float, float)),
                SLOT(rangeCallback()));
            connect(
                _p->slider,
                SIGNAL(valueChanged(int)),
                SLOT(sliderCallback(int)));
        }

        FloatSlider::~FloatSlider()
        {}

        float FloatSlider::value() const
        {
            return _p->object->value();
        }

        float FloatSlider::min() const
        {
            return _p->object->min();
        }

        float FloatSlider::max() const
        {
            return _p->object->max();
        }

        FloatObject * FloatSlider::object() const
        {
            return _p->object;
        }

        void FloatSlider::setValue(float value)
        {
            _p->object->setValue(value);
        }

        void FloatSlider::setMin(float min)
        {
            _p->object->setMin(min);
        }

        void FloatSlider::setMax(float max)
        {
            _p->object->setMax(max);
        }

        void FloatSlider::setRange(float min, float max)
        {
            _p->object->setRange(min, max);
        }

        void FloatSlider::keyPressEvent(QKeyEvent * event)
        {
            switch (event->key())
            {
            case Qt::Key_Home:     _p->object->setToMin();       break;
            case Qt::Key_End:      _p->object->setToMax();       break;
            case Qt::Key_Left:
            case Qt::Key_Down:     _p->object->smallDecAction(); break;
            case Qt::Key_Right:
            case Qt::Key_Up:       _p->object->smallIncAction(); break;
            case Qt::Key_PageDown: _p->object->largeDecAction(); break;
            case Qt::Key_PageUp:   _p->object->largeIncAction(); break;
            default: break;
            }
        }

        void FloatSlider::valueCallback()
        {
            widgetUpdate();
            Q_EMIT valueChanged(_p->object->value());
        }

        void FloatSlider::rangeCallback()
        {
            widgetUpdate();
            Q_EMIT rangeChanged(_p->object->min(), _p->object->max());
        }

        void FloatSlider::sliderCallback(int value)
        {
            setValue(
                _p->object->min() +
                ((_p->object->max() - _p->object->min()) * value / static_cast<float>(steps)));
        }

        void FloatSlider::widgetUpdate()
        {
            //DJV_DEBUG("FloatSlider::widgetUpdate");
            //DJV_DEBUG_PRINT("value = " << value());
            //DJV_DEBUG_PRINT("min = " << min());
            //DJV_DEBUG_PRINT("max = " << max());
            Core::SignalBlocker signalBlocker(_p->slider);
            _p->slider->setValue(static_cast<int>(
                ((_p->object->value() - _p->object->min()) /
                (_p->object->max() - _p->object->min())) * steps));
        }

    } // namespace UI
} // namespace djv
