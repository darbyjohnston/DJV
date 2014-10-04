//------------------------------------------------------------------------------
// Copyright (c) 2004-2014 Darby Johnston
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
    
    djvVector2f    value;
    djvVector2f    tmp;
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
    _p->widget->setRange(-1.0, 1.0);
    
    _p->widget2 = new djvFloatEdit;
    _p->widget2->setRange(-1.0, 1.0);

    QHBoxLayout * layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(_p->widget);
    layout->addWidget(_p->widget2);

    widgetUpdate();

    connect(
        _p->widget,
        SIGNAL(valueChanged(double)),
        SLOT(widgetCallback(double)));
    
    connect(
        _p->widget2,
        SIGNAL(valueChanged(double)),
        SLOT(widget2Callback(double)));
}

djvVector2fEditWidget::~djvVector2fEditWidget()
{
    delete _p;
}

const djvVector2f & djvVector2fEditWidget::value() const
{
    return _p->value;
}

void djvVector2fEditWidget::setValue(const djvVector2f & value)
{
    if (value == _p->value)
        return;

    _p->value = value;

    widgetUpdate();

    Q_EMIT valueChanged(_p->value);
}

void djvVector2fEditWidget::widgetCallback(double in)
{
    setValue(djvVector2f(in, _p->value.y));
}

void djvVector2fEditWidget::widget2Callback(double in)
{
    setValue(djvVector2f(_p->value.x, in));
}

void djvVector2fEditWidget::widgetUpdate()
{
    djvSignalBlocker signalBlocker(QObjectList() <<
        _p->widget <<
        _p->widget2);

    _p->widget->setValue(_p->value.x);
    
    _p->widget2->setValue(_p->value.y);
}
