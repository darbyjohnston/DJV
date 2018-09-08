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

#include <djvSpinner.h>

#include <djvUIContext.h>
#include <djvIconLibrary.h>

#include <djvMath.h>

#include <QPainter>
#include <QSvgRenderer>
#include <QTimerEvent>

//------------------------------------------------------------------------------
// djvSpinner::Private
//------------------------------------------------------------------------------

struct djvSpinner::Private
{
    Private(djvUIContext * context) :
        context(context)
    {}

    djvUIContext *  context    = nullptr;
    int              tick       = 0;
    int              timer      = 0;
    int              startTimer = 0;
    QSvgRenderer *   svg        = nullptr;
};

//------------------------------------------------------------------------------
// djvSpinner
//------------------------------------------------------------------------------

djvSpinner::djvSpinner(
    djvUIContext * context,
    QWidget *       parent) :
    QWidget(parent),
    _p(new Private(context))
{
    _p->svg = new QSvgRenderer(QString(":djvSpinner.svg"), this);
    
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

djvSpinner::~djvSpinner()
{
    if (_p->timer != 0)
    {
        killTimer(_p->timer);        
        _p->timer = 0;
    }
    if (_p->startTimer != 0)
    {
        killTimer(_p->startTimer);   
        _p->startTimer = 0;
    }
}

bool djvSpinner::isSpinning() const
{
    return _p->timer != 0;
}

void djvSpinner::start()
{
    stop();
    _p->tick = 0;
    _p->timer = startTimer(10);
    update();
}

void djvSpinner::startDelayed(int msec)
{
    stop();
    _p->startTimer = startTimer(msec);
}

void djvSpinner::stop()
{
    if (_p->timer != 0)
    {
        killTimer(_p->timer);
        _p->timer = 0;
        update();
    }
    if (_p->startTimer != 0)
    {
        killTimer(_p->startTimer);
        _p->startTimer = 0;
    }
}

void djvSpinner::timerEvent(QTimerEvent * event)
{
    const int id = event->timerId();
    if (id == _p->timer)
    {
        ++_p->tick;
        update();
    }
    else if (id == _p->startTimer)
    {
        killTimer(_p->startTimer);
        _p->startTimer = 0;
        start();
    }
}

void djvSpinner::paintEvent(QPaintEvent *)
{
    if (! _p->timer)
        return;
    QPainter painter(this);
    const int w = width ();
    const int h = height();
    painter.translate(w / 2, h / 2);
    painter.rotate(-_p->tick);
    painter.translate(-w / 2, -h / 2);
    const int s = djvMath::min<int>(w, h) / 2;
    _p->svg->render(&painter, QRectF(w / 2 - s / 2, h / 2 - s / 2, s, s));
}

