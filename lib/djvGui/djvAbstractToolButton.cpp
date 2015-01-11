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

//! \file djvAbstractToolButton.cpp

#include <djvAbstractToolButton.h>

#include <djvAssert.h>

#include <QApplication>
#include <QDebug>
#include <QEvent>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>

//------------------------------------------------------------------------------
// djvAbstractToolButton
//------------------------------------------------------------------------------

djvAbstractToolButton::djvAbstractToolButton(QWidget * parent) :
    QAbstractButton(parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

djvAbstractToolButton::~djvAbstractToolButton()
{}

void djvAbstractToolButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    
    // Draw the button.

    if (underMouse() || isChecked())
    {
        QStyleOptionButton styleOption;

        styleOption.rect = this->rect();

        if (underMouse())
            styleOption.state = QStyle::State_Raised;
        if (isDown() || isChecked())
            styleOption.state |= QStyle::State_On;

        style()->drawControl(QStyle::CE_PushButton, &styleOption, &painter);
    }
}

bool djvAbstractToolButton::event(QEvent * event)
{
    switch (event->type())
    {
        case QEvent::Enter:
        case QEvent::Leave:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick:
        
            update();
            
            break;
        
        default: break;
    }
        
    return QAbstractButton::event(event);
}
