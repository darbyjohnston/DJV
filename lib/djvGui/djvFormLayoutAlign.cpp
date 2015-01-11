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

//! \file djvFormLayoutAlign.cpp

#include <djvFormLayoutAlign.h>

#include <djvStyle.h>

#include <djvMath.h>

#include <QFormLayout>
#include <QWidget>

//------------------------------------------------------------------------------
// djvFormLayoutAlign::P
//------------------------------------------------------------------------------

struct djvFormLayoutAlign::P
{
    QList<QFormLayout *> layouts;
};

//------------------------------------------------------------------------------
// djvFormLayoutAlign
//------------------------------------------------------------------------------

djvFormLayoutAlign::djvFormLayoutAlign(QObject * parent) :
    QObject(parent),
    _p(new P)
{
    connect(
        djvStyle::global(),
        SIGNAL(sizeMetricsChanged()),
        SLOT(align()));
}

djvFormLayoutAlign::~djvFormLayoutAlign()
{
    delete _p;
}

void djvFormLayoutAlign::addLayout(QFormLayout * layout)
{
    if (_p->layouts.contains(layout))
        return;

    _p->layouts += layout;

    align();
}

void djvFormLayoutAlign::clear()
{
    _p->layouts.clear();

    align();
}

void djvFormLayoutAlign::align()
{
    int width = 0;

    Q_FOREACH(QFormLayout * layout, _p->layouts)
    {
        for (int i = 0; i < layout->rowCount(); ++i)
        {
            if (QWidget * widget = layout->itemAt(i)->widget())
            {
                if (QWidget * label = layout->labelForField(widget))
                {
                    label->setFixedSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));
                }
            }
        }
    }

    Q_FOREACH(QFormLayout * layout, _p->layouts)
    {
        for (int i = 0; i < layout->rowCount(); ++i)
        {
            if (QWidget * widget = layout->itemAt(i)->widget())
            {
                if (QWidget * label = layout->labelForField(widget))
                {
                    width = djvMath::max(width, label->sizeHint().width());
                }
            }
        }
    }

    Q_FOREACH(QFormLayout * layout, _p->layouts)
    {
        for (int i = 0; i < layout->rowCount(); ++i)
        {
            if (QWidget * widget = layout->itemAt(i)->widget())
            {
                if (QWidget * label = layout->labelForField(widget))
                {
                    label->setFixedWidth(width);
                }
            }
        }
    }
}
