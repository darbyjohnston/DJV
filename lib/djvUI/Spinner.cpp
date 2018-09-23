//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
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

#include <djvUI/Spinner.h>

#include <djvUI/UIContext.h>
#include <djvUI/IconLibrary.h>

#include <djvCore/Math.h>

#include <QPainter>
#include <QPointer>
#include <QSvgRenderer>
#include <QTimerEvent>

namespace djv
{
    namespace UI
    {
        struct Spinner::Private
        {
            Private(UIContext * context) :
                context(context)
            {}

            UIContext * context = nullptr;
            int tick = 0;
            int timer = 0;
            int startTimer = 0;
            QPointer<QSvgRenderer> svg;
        };

        Spinner::Spinner(
            UIContext * context,
            QWidget * parent) :
            QWidget(parent),
            _p(new Private(context))
        {
            _p->svg = new QSvgRenderer(QString(":djv/UI/Spinner.svg"), this);

            setAttribute(Qt::WA_TransparentForMouseEvents);
        }

        Spinner::~Spinner()
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

        bool Spinner::isSpinning() const
        {
            return _p->timer != 0;
        }

        void Spinner::start()
        {
            stop();
            _p->tick = 0;
            _p->timer = startTimer(10);
            update();
        }

        void Spinner::startDelayed(int msec)
        {
            stop();
            _p->startTimer = startTimer(msec);
        }

        void Spinner::stop()
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

        void Spinner::timerEvent(QTimerEvent * event)
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

        void Spinner::paintEvent(QPaintEvent *)
        {
            if (!_p->timer)
                return;
            QPainter painter(this);
            const int w = width();
            const int h = height();
            painter.translate(w / 2, h / 2);
            painter.rotate(-_p->tick);
            painter.translate(-w / 2, -h / 2);
            const int s = Core::Math::min<int>(w, h) / 2;
            _p->svg->render(&painter, QRectF(w / 2 - s / 2, h / 2 - s / 2, s, s));
        }

    } // namespace UI
} // namespace djv
