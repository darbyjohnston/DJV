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

//! \file djvShuttleButton.cpp

#include <djvShuttleButton.h>

#include <djvIconLibrary.h>

#include <djvMath.h>
#include <djvVector.h>

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>

//------------------------------------------------------------------------------
// djvShuttleButton::P
//------------------------------------------------------------------------------

struct djvShuttleButton::P
{
    P() :
        value     (0.0),
        mousePress(false)
    {}
    
    double         value;
    QVector<QIcon> icons;
    bool           mousePress;
    djvVector2i    mouseStartPos;
};

//------------------------------------------------------------------------------
// djvShuttleButton
//------------------------------------------------------------------------------

djvShuttleButton::djvShuttleButton(QWidget * parent) :
    djvAbstractToolButton(parent),
    _p(new P)
{}

djvShuttleButton::djvShuttleButton(const QVector<QIcon> & icons, QWidget * parent) :
    djvAbstractToolButton(parent),
    _p(new P)
{
    setIcons(icons);
    
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

djvShuttleButton::~djvShuttleButton()
{
    delete _p;
}
    
const QVector<QIcon> & djvShuttleButton::icons() const
{
    return _p->icons;
}

void djvShuttleButton::setIcons(const QVector<QIcon> & icons)
{
    _p->icons = icons;
    
    updateGeometry();
    update();
}

const QVector<QIcon> djvShuttleButton::iconsDefault()
{
    static const QVector<QIcon> data = QVector<QIcon>() <<
        djvIconLibrary::global()->icon("djvShuttle0Icon.png") <<
        djvIconLibrary::global()->icon("djvShuttle1Icon.png") <<
        djvIconLibrary::global()->icon("djvShuttle2Icon.png") <<
        djvIconLibrary::global()->icon("djvShuttle3Icon.png") <<
        djvIconLibrary::global()->icon("djvShuttle4Icon.png") <<
        djvIconLibrary::global()->icon("djvShuttle5Icon.png") <<
        djvIconLibrary::global()->icon("djvShuttle6Icon.png") <<
        djvIconLibrary::global()->icon("djvShuttle7Icon.png");

    return data;
}

QSize djvShuttleButton::sizeHint() const
{
    QSize sizeHint(25, 25);

    const int margin = 2;

    QStyleOptionToolButton opt;
    opt.iconSize = _p->icons.count() > 0 ?
        _p->icons[0].actualSize(sizeHint) :
        sizeHint;
    opt.iconSize += QSize(margin * 2, margin * 2);

    sizeHint = opt.iconSize.expandedTo(QApplication::globalStrut());

    return sizeHint;
}

void djvShuttleButton::mousePressEvent(QMouseEvent * event)
{
    _p->value = 0.0;
    
    _p->mousePress = true;
    
    _p->mouseStartPos = djvVector2i(event->pos().x(), event->pos().y());
    
    setCursor(Qt::SizeHorCursor);
    
    update();

    Q_EMIT mousePressed(_p->mousePress);
}

void djvShuttleButton::mouseReleaseEvent(QMouseEvent * event)
{
    _p->value = 0.0;
    
    _p->mousePress = false;
    
    setCursor(QCursor());

    update();

    Q_EMIT mousePressed(_p->mousePress);
}

void djvShuttleButton::mouseMoveEvent(QMouseEvent * event)
{
    const int tmp = event->pos().x() - _p->mouseStartPos.x;

    if (tmp != _p->value)
    {
        _p->value = static_cast<double>(tmp);

        update();

        Q_EMIT valueChanged(djvMath::round(_p->value / 5.0));
    }
}

void djvShuttleButton::paintEvent(QPaintEvent * event)
{
    djvAbstractToolButton::paintEvent(event);

    QPainter painter(this);
    
    // Draw the icon.

    const int i = djvMath::mod(djvMath::round(_p->value / 5.0), 8);
    
    QIcon::Mode  mode  = QIcon::Normal;
    QIcon::State state = QIcon::Off;

    if (!isEnabled())
        mode = QIcon::Disabled;

    const QPixmap & pixmap = _p->icons[i].pixmap(width(), height(), mode, state);

    painter.drawPixmap(
        width () / 2 - pixmap.width () / 2,
        height() / 2 - pixmap.height() / 2,
        pixmap);
}
