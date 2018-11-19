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

#include <TimelineSlider.h>

#include <Context.h>
#include <PlaybackSystem.h>

#include <QMouseEvent>
#include <QPainter>

using namespace djv;

namespace djv
{
    namespace AudioExperiment2
    {
        TimelineSlider::TimelineSlider(const QPointer<Context> & context, QWidget * parent) :
            QWidget(parent),
            _context(context)
        {
            setBackgroundRole(QPalette::Base);
            setAutoFillBackground(true);
            connect(
                context->playbackSystem().data(),
                &PlaybackSystem::durationChanged,
                [this](int64_t value)
            {
                _duration = value;
                update();
            });
            connect(
                context->playbackSystem().data(),
                &PlaybackSystem::currentTimeChanged,
                [this](int64_t value)
            {
                _currentTime = value;
                update();
            });
        }

        TimelineSlider::~TimelineSlider()
        {}

        void TimelineSlider::paintEvent(QPaintEvent * event)
        {
            QWidget::paintEvent(event);
            QPainter painter(this);
            const int w = width();
            const int h = height();
            const int x = _currentTime / static_cast<double>(_duration) * (w - 1);
            painter.setPen(palette().color(QPalette::Foreground));
            painter.drawLine(x, 0, x, h - 1);
        }

        void TimelineSlider::mousePressEvent(QMouseEvent * event)
        {
            _mousePressed = true;
            _context->playbackSystem()->setCurrentTime(posToTime(event->pos().x()));
        }

        void TimelineSlider::mouseReleaseEvent(QMouseEvent * event)
        {
            _mousePressed = false;
        }

        void TimelineSlider::mouseMoveEvent(QMouseEvent * event)
        {
            if (_mousePressed)
            {
                _context->playbackSystem()->setCurrentTime(posToTime(event->pos().x()));
            }
        }

        int64_t TimelineSlider::posToTime(int value) const
        {
            const double v = value / static_cast<double>(width() - 1);
            return Core::Math::clamp(static_cast<int64_t>(v * _duration), static_cast<int64_t>(0), _duration - 1);
        }

    } // namespace AudioExperiment2
} // namespace djv
