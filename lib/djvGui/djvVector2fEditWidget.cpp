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

//! \file djvVector2fEditWidget.cpp

#include <djvVector2fEditWidget.h>

#include <djvFloatEdit.h>
#include <djvFloatObject.h>
#include <djvSignalBlocker.h>

#include <QHBoxLayout>

//------------------------------------------------------------------------------
// djvVector2fEditWidget::P
//------------------------------------------------------------------------------

struct djvVector2fEditWidget::P
{
    P() :
        widget (0),
        widget2(0)
    {}
    
    djvFloatEdit * widget;
    djvFloatEdit * widget2;
};

//------------------------------------------------------------------------------
// djvVector2fEditWidget
//------------------------------------------------------------------------------

djvVector2fEditWidget::djvVector2fEditWidget(QWidget * parent) :
    QWidget(parent),
    _p(new P)
{
    _p->widget = new djvFloatEdit;
    _p->widget->setRange(djvFloatObject::floatMin, djvFloatObject::floatMax);
    
    _p->widget2 = new djvFloatEdit;
    _p->widget2->setRange(djvFloatObject::floatMin, djvFloatObject::floatMax);

    QHBoxLayout * layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(_p->widget);
    layout->addWidget(_p->widget2);

    connect(
        _p->widget,
        SIGNAL(valueChanged(double)),
        SLOT(valueCallback()));
    connect(
        _p->widget,
        SIGNAL(rangeChanged(double, double)),
        SLOT(rangeCallback()));
    
    connect(
        _p->widget2,
        SIGNAL(valueChanged(double)),
        SLOT(valueCallback()));
    connect(
        _p->widget2,
        SIGNAL(rangeChanged(double, double)),
        SLOT(rangeCallback()));
}

djvVector2fEditWidget::~djvVector2fEditWidget()
{
    delete _p;
}

djvVector2f djvVector2fEditWidget::value() const
{
    return djvVector2f(_p->widget->value(), _p->widget2->value());
}

djvVector2f djvVector2fEditWidget::min() const
{
    return djvVector2f(_p->widget->min(), _p->widget2->min());
}

djvVector2f djvVector2fEditWidget::max() const
{
    return djvVector2f(_p->widget->max(), _p->widget2->max());
}

void djvVector2fEditWidget::setValue(const djvVector2f & value)
{
    _p->widget->setValue(value.x);
    _p->widget2->setValue(value.y);
}

void djvVector2fEditWidget::setMin(const djvVector2f & min)
{
    _p->widget->setMin(min.x);
    _p->widget2->setMin(min.y);
}

void djvVector2fEditWidget::setMax(const djvVector2f & max)
{
    _p->widget->setMax(max.x);
    _p->widget2->setMax(max.y);
}

void djvVector2fEditWidget::setRange(
    const djvVector2f & min,
    const djvVector2f & max)
{
    _p->widget->setRange(min.x, max.x);
    _p->widget2->setRange(min.y, max.y);
}

void djvVector2fEditWidget::valueCallback()
{
    Q_EMIT valueChanged(value());
}

void djvVector2fEditWidget::rangeCallback()
{
    Q_EMIT rangeChanged(min(), max());
}
