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

//! \file djvVector2iEditWidget.cpp

#include <djvVector2iEditWidget.h>

#include <djvIntEdit.h>
#include <djvSignalBlocker.h>

#include <QHBoxLayout>

//------------------------------------------------------------------------------
// djvVector2iEditWidget::P
//------------------------------------------------------------------------------

struct djvVector2iEditWidget::P
{
    P() :
        widget (0),
        widget2(0)
    {}
    
    djvVector2i  value;
    djvVector2i  tmp;
    djvIntEdit * widget;
    djvIntEdit * widget2;
};

//------------------------------------------------------------------------------
// djvVector2iEditWidget
//------------------------------------------------------------------------------

djvVector2iEditWidget::djvVector2iEditWidget(QWidget * parent) :
    QWidget(parent),
    _p(new P)
{
    _p->widget = new djvIntEdit;
    _p->widget->setRange(-100, 100);
    
    _p->widget2 = new djvIntEdit;
    _p->widget2->setRange(-100, 100);

    QHBoxLayout * layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(_p->widget);
    layout->addWidget(_p->widget2);

    widgetUpdate();

    connect(_p->widget, SIGNAL(valueChanged(int)), SLOT(widgetCallback(int)));
    connect(_p->widget2, SIGNAL(valueChanged(int)), SLOT(widget2Callback(int)));
}

djvVector2iEditWidget::~djvVector2iEditWidget()
{
    delete _p;
}

const djvVector2i & djvVector2iEditWidget::value() const
{
    return _p->value;
}

void djvVector2iEditWidget::setValue(const djvVector2i & value)
{
    if (value == _p->value)
        return;

    _p->value = value;

    widgetUpdate();

    Q_EMIT valueChanged(_p->value);
}

void djvVector2iEditWidget::widgetCallback(int in)
{
    setValue(djvVector2i(in, _p->value.y));
}

void djvVector2iEditWidget::widget2Callback(int in)
{
    setValue(djvVector2i(_p->value.x, in));
}

void djvVector2iEditWidget::widgetUpdate()
{
    djvSignalBlocker signalBlocker(QObjectList() <<
        _p->widget <<
        _p->widget2);

    _p->widget->setValue(_p->value.x);
    
    _p->widget2->setValue(_p->value.y);
}
