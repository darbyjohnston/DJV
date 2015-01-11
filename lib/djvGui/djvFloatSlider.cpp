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

//! \file djvFloatSlider.cpp

#include <djvFloatSlider.h>

#include <djvFloatObject.h>

#include <djvSignalBlocker.h>

#include <djvDebug.h>

#include <QHBoxLayout>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QSlider>
#include <QStyle>

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

//------------------------------------------------------------------------------
// djvFloatSlider::P
//------------------------------------------------------------------------------

struct djvFloatSlider::P
{
    P() :
        object(0),
        slider(0)
    {}
    
    djvFloatObject * object;
    Slider *         slider;
};

//------------------------------------------------------------------------------
// djvFloatSlider
//------------------------------------------------------------------------------

namespace
{

const int steps = 10000;

} // namespace

djvFloatSlider::djvFloatSlider(QWidget * parent) :
    QWidget(parent),
    _p(new P)
{
    _p->object = new djvFloatObject(this);

    _p->slider = new Slider;
    _p->slider->setRange(0, steps);
    
    QHBoxLayout * layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(_p->slider);

    _p->object->setRange(0.0, 1.0);
    
    widgetUpdate();

    connect(
        _p->object,
        SIGNAL(valueChanged(double)),
        SLOT(valueCallback()));

    connect(
        _p->object,
        SIGNAL(minChanged(double)),
        SLOT(rangeCallback()));

    connect(
        _p->object,
        SIGNAL(maxChanged(double)),
        SLOT(rangeCallback()));

    connect(
        _p->object,
        SIGNAL(rangeChanged(double, double)),
        SLOT(rangeCallback()));
    
    connect(
        _p->slider,
        SIGNAL(valueChanged(int)),
        SLOT(sliderCallback(int)));
}

djvFloatSlider::~djvFloatSlider()
{
    delete _p;
}

double djvFloatSlider::value() const
{
    return _p->object->value();
}

double djvFloatSlider::min() const
{
    return _p->object->min();
}

double djvFloatSlider::max() const
{
    return _p->object->max();
}

djvFloatObject * djvFloatSlider::object() const
{
    return _p->object;
}

void djvFloatSlider::setValue(double value)
{
    _p->object->setValue(value);
}

void djvFloatSlider::setMin(double min)
{
    _p->object->setMin(min);
}

void djvFloatSlider::setMax(double max)
{
    _p->object->setMax(max);
}

void djvFloatSlider::setRange(double min, double max)
{
    _p->object->setRange(min, max);
}

void djvFloatSlider::keyPressEvent(QKeyEvent * event)
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

void djvFloatSlider::valueCallback()
{
    widgetUpdate();

    Q_EMIT valueChanged(_p->object->value());
}

void djvFloatSlider::rangeCallback()
{
    widgetUpdate();

    Q_EMIT rangeChanged(_p->object->min(), _p->object->max());
}

void djvFloatSlider::sliderCallback(int value)
{
    setValue(
        _p->object->min() +
        ((_p->object->max() - _p->object->min()) * value / static_cast<double>(steps)));
}

void djvFloatSlider::widgetUpdate()
{
    //DJV_DEBUG("djvFloatSlider::widgetUpdate");
    //DJV_DEBUG_PRINT("value = " << value());
    //DJV_DEBUG_PRINT("min = " << min());
    //DJV_DEBUG_PRINT("max = " << max());
    
    djvSignalBlocker signalBlocker(_p->slider);
    
    _p->slider->setValue(static_cast<int>(
        ((_p->object->value() - _p->object->min()) /
         (_p->object->max() - _p->object->min())) * steps));
}
