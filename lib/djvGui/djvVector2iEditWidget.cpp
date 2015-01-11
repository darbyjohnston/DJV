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

//! \file djvVector2iEditWidget.cpp

#include <djvVector2iEditWidget.h>

#include <djvIntEdit.h>
#include <djvIntObject.h>
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
    _p->widget->setRange(djvIntObject::intMin, djvIntObject::intMax);
    
    _p->widget2 = new djvIntEdit;
    _p->widget2->setRange(djvIntObject::intMin, djvIntObject::intMax);

    QHBoxLayout * layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(_p->widget);
    layout->addWidget(_p->widget2);

    connect(_p->widget,
        SIGNAL(valueChanged(int)),
        SLOT(valueCallback()));
    connect(_p->widget,
        SIGNAL(rangeChanged(int, int)),
        SLOT(rangeCallback()));
    
    connect(_p->widget2,
        SIGNAL(valueChanged(int)),
        SLOT(valueCallback()));
    connect(_p->widget2,
        SIGNAL(rangeChanged(int, int)),
        SLOT(rangeCallback()));
}

djvVector2iEditWidget::~djvVector2iEditWidget()
{
    delete _p;
}

djvVector2i djvVector2iEditWidget::value() const
{
    return djvVector2i(_p->widget->value(), _p->widget2->value());
}

djvVector2i djvVector2iEditWidget::min() const
{
    return djvVector2i(_p->widget->min(), _p->widget2->min());
}

djvVector2i djvVector2iEditWidget::max() const
{
    return djvVector2i(_p->widget->max(), _p->widget2->max());
}

void djvVector2iEditWidget::setValue(const djvVector2i & value)
{
    _p->widget->setValue(value.x);
    _p->widget2->setValue(value.y);
}

void djvVector2iEditWidget::setMin(const djvVector2i & min)
{
    _p->widget->setMin(min.x);
    _p->widget2->setMin(min.y);
}

void djvVector2iEditWidget::setMax(const djvVector2i & max)
{
    _p->widget->setMax(max.x);
    _p->widget2->setMax(max.y);
}

void djvVector2iEditWidget::setRange(
    const djvVector2i & min,
    const djvVector2i & max)
{
    _p->widget->setRange(min.x, max.x);
    _p->widget2->setRange(min.y, max.y);
}

void djvVector2iEditWidget::valueCallback()
{
    Q_EMIT valueChanged(value());
}

void djvVector2iEditWidget::rangeCallback()
{
    Q_EMIT rangeChanged(min(), max());
}
