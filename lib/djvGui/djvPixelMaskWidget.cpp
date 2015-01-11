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

//! \file djvPixelMaskWidget.cpp

#include <djvPixelMaskWidget.h>

#include <djvIconLibrary.h>
#include <djvSignalBlocker.h>
#include <djvToolButton.h>

#include <QHBoxLayout>
#include <QMenu>

//------------------------------------------------------------------------------
// djvPixelMaskWidget::P
//------------------------------------------------------------------------------

struct djvPixelMaskWidget::P
{
    P() :
        button(0)
    {}
    
    djvPixel::Mask mask;
    
    djvToolButton * button;
};

//------------------------------------------------------------------------------
// djvPixelMaskWidget
//------------------------------------------------------------------------------

djvPixelMaskWidget::djvPixelMaskWidget(QWidget * parent) :
    QWidget(parent),
    _p(new P)
{
    _p->button = new djvToolButton(
        djvIconLibrary::global()->icon("djvPixelMaskIcon.png"));
    _p->button->setCheckable(true);
    _p->button->setToolTip(tr("Set the pixel mask"));

    QHBoxLayout * layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(_p->button);

    widgetUpdate();

    connect(
        _p->button,
        SIGNAL(pressed()),
        SLOT(buttonCallback()));
}

djvPixelMaskWidget::~djvPixelMaskWidget()
{
    delete _p;
}

const djvPixel::Mask & djvPixelMaskWidget::mask() const
{
    return _p->mask;
}

void djvPixelMaskWidget::setMask(const djvPixel::Mask & mask)
{
    if (mask == _p->mask)
        return;

    _p->mask = mask;

    widgetUpdate();

    Q_EMIT maskChanged(_p->mask);
}

void djvPixelMaskWidget::buttonCallback()
{
    QMenu menu;
    
    static const QString text [] =
    {
        tr("Solo red"),
        tr("Solo green"),
        tr("Solo blue"),
        tr("Solo alpha")
    };
    
    int count = sizeof(text) / sizeof(text[0]);
    
    int data = 0;
    
    for (int i = 0; i < count; ++i)
    {
        QAction * action = menu.addAction(text[i]);
        action->setData(data++);
        
        connect(action, SIGNAL(triggered()), SLOT(soloCallback()));
    }
    
    menu.addSeparator();

    static const QString text2 [] =
    {
        tr("Red"),
        tr("Green"),
        tr("Blue"),
        tr("Alpha")
    };
    
    count = sizeof(text) / sizeof(text[0]);
    
    data = 0;
    
    for (int i = 0; i < count; ++i)
    {
        QAction * action = menu.addAction(text2[i]);
        action->setCheckable(true);
        action->setChecked(_p->mask[i]);
        action->setData(data++);
        
        connect(action, SIGNAL(toggled(bool)), SLOT(toggleCallback(bool)));
    }
    
    menu.addSeparator();
    
    QAction * action = menu.addAction(tr("Reset"));
    
    connect(action, SIGNAL(triggered()), SLOT(resetCallback()));
    
    menu.exec(mapToGlobal(
        QPoint(_p->button->x(), _p->button->y() + _p->button->height())));

    _p->button->setDown(false);
}

void djvPixelMaskWidget::soloCallback()
{
    QAction * action = qobject_cast<QAction *>(sender());

    djvPixel::Mask mask(false);
    
    mask[action->data().toInt()] = true;
    
    setMask(mask);
}

void djvPixelMaskWidget::toggleCallback(bool checked)
{
    QAction * action = qobject_cast<QAction *>(sender());

    djvPixel::Mask mask = _p->mask;
    
    mask[action->data().toInt()] = checked;
    
    setMask(mask);
}

void djvPixelMaskWidget::resetCallback()
{
    setMask(djvPixel::Mask());
}

void djvPixelMaskWidget::widgetUpdate()
{
    djvSignalBlocker signalBlocker(_p->button);
   
    bool checked = false;
    
    for (int i = 0; i < djvPixel::channelsMax; ++i)
    {
        if (! _p->mask[i])
        {
            checked = true;
            
            break;
        }
    }

    _p->button->setChecked(checked);
}
