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

//! \file djvSpinner.cpp

#include <djvSpinner.h>

#include <djvGuiContext.h>
#include <djvIconLibrary.h>

#include <QPainter>

//------------------------------------------------------------------------------
// djvSpinner
//------------------------------------------------------------------------------

djvSpinner::djvSpinner(
    djvGuiContext * context,
    QWidget *       parent) :
    QWidget(parent),
    _current(0),
    _timer  (0)
{
    const QString name("djvSpinner%1Icon.png");
    
    _pixmaps += context->iconLibrary()->pixmap(name.arg( 0));
    _pixmaps += context->iconLibrary()->pixmap(name.arg( 1));
    _pixmaps += context->iconLibrary()->pixmap(name.arg( 2));
    _pixmaps += context->iconLibrary()->pixmap(name.arg( 3));
    _pixmaps += context->iconLibrary()->pixmap(name.arg( 4));
    _pixmaps += context->iconLibrary()->pixmap(name.arg( 5));
    _pixmaps += context->iconLibrary()->pixmap(name.arg( 6));
    _pixmaps += context->iconLibrary()->pixmap(name.arg( 7));
    _pixmaps += context->iconLibrary()->pixmap(name.arg( 8));
    _pixmaps += context->iconLibrary()->pixmap(name.arg( 9));
    _pixmaps += context->iconLibrary()->pixmap(name.arg(10));
    _pixmaps += context->iconLibrary()->pixmap(name.arg(11));
}

djvSpinner::~djvSpinner()
{
    if (_timer != 0)
    {
        killTimer(_timer);
        
        _timer = 0;
    }
}

bool djvSpinner::isSpinning() const
{
    return _timer != 0;
}

QSize djvSpinner::sizeHint() const
{
    return _pixmaps[0].size();
}

void djvSpinner::start()
{
    if (! _timer)
    {
        _timer = startTimer(75);

        update();
    }
}

void djvSpinner::stop()
{
    if (_timer != 0)
    {
        killTimer(_timer);
        
        _timer = 0;

        update();
    }
}

void djvSpinner::timerEvent(QTimerEvent *)
{
    ++_current;
    
    if (_current >= _pixmaps.count())
    {
        _current = 0;
    }
    
    update();
}

void djvSpinner::paintEvent(QPaintEvent *)
{
    if (! _timer)
        return;

    QPainter painter(this);
    
    painter.drawPixmap(
        width () / 2 - _pixmaps[_current].width () / 2,
        height() / 2 - _pixmaps[_current].height() / 2,
        _pixmaps[_current]);
}

