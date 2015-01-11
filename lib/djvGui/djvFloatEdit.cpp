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

//! \file djvFloatEdit.cpp

#include <djvFloatEdit.h>

#include <djvFloatObject.h>

#include <djvDebug.h>
#include <djvSignalBlocker.h>

#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QKeyEvent>

//------------------------------------------------------------------------------
// djvFloatEdit::P
//------------------------------------------------------------------------------

struct djvFloatEdit::P
{
    P() :
        object (0),
        spinBox(0)
    {}
    
    djvFloatObject * object;
    QDoubleSpinBox * spinBox;
};

//------------------------------------------------------------------------------
// djvFloatEdit
//------------------------------------------------------------------------------

djvFloatEdit::djvFloatEdit(QWidget * parent) :
    QWidget(parent),
    _p(new P)
{
    _p->object = new djvFloatObject(this);

    _p->spinBox = new QDoubleSpinBox;
    _p->spinBox->setKeyboardTracking(false);
    
    QHBoxLayout * layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(_p->spinBox);
    
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
        _p->object,
        SIGNAL(incChanged(double, double)),
        SLOT(widgetUpdate()));

    connect(
        _p->spinBox,
        SIGNAL(valueChanged(double)),
        SLOT(spinBoxCallback(double)));
}

djvFloatEdit::~djvFloatEdit()
{
    delete _p;
}

double djvFloatEdit::value() const
{
    return _p->object->value();
}

double djvFloatEdit::min() const
{
    return _p->object->min();
}

double djvFloatEdit::max() const
{
    return _p->object->max();
}

djvFloatObject * djvFloatEdit::object() const
{
    return _p->object;
}

void djvFloatEdit::setValue(double value)
{
    _p->object->setValue(value);
}

void djvFloatEdit::setMin(double min)
{
    _p->object->setMin(min);
}

void djvFloatEdit::setMax(double max)
{
    _p->object->setMax(max);
}

void djvFloatEdit::setRange(double min, double max)
{
    _p->object->setRange(min, max);
}

void djvFloatEdit::keyPressEvent(QKeyEvent * event)
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

void djvFloatEdit::valueCallback()
{
    widgetUpdate();

    Q_EMIT valueChanged(_p->object->value());
}

void djvFloatEdit::rangeCallback()
{
    widgetUpdate();

    Q_EMIT rangeChanged(_p->object->min(), _p->object->max());
}

void djvFloatEdit::spinBoxCallback(double value)
{
    _p->object->setValue(value);
}

void djvFloatEdit::widgetUpdate()
{
    djvSignalBlocker signalBlocker(_p->spinBox);
    
    _p->spinBox->setRange(_p->object->min(), _p->object->max());
    _p->spinBox->setSingleStep(_p->object->smallInc());
    _p->spinBox->setValue(_p->object->value());
}
