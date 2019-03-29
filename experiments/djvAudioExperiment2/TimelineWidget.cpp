//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <TimelineWidget.h>

#include <Context.h>
#include <PlaybackSystem.h>
#include <TimelineSlider.h>
#include <Util.h>

#include <QHBoxLayout>
#include <QLabel>

using namespace djv;

namespace djv
{
    namespace AudioExperiment2
    {
        TimelineWidget::TimelineWidget(const QPointer<Context> & context, QWidget * parent) :
            QWidget(parent)
        {
            _stopButton = new UI::ToolButton(context.data());
            _stopButton->setDefaultAction(context->actions()["Playback/Stop"]);

            _forwardButton = new UI::ToolButton(context.data());
            _forwardButton->setDefaultAction(context->actions()["Playback/Forward"]);

            _slider = new TimelineSlider(context);

            _timeLabel = new QLabel;

            auto layout = new QHBoxLayout(this);
            layout->addWidget(_stopButton);
            layout->addWidget(_forwardButton);
            layout->addWidget(_slider, 1);
            layout->addWidget(_timeLabel);

            widgetUpdate();

            connect(
                context->playbackSystem(),
                SIGNAL(durationChanged(int64_t)),
                SLOT(setDuration(int64_t)));
            connect(
                context->playbackSystem(),
                SIGNAL(currentTimeChanged(int64_t)),
                SLOT(setCurrentTime(int64_t)));
            connect(
                context->playbackSystem(),
                SIGNAL(playbackChanged(Util::PLAYBACK)),
                SLOT(setPlayback(Util::PLAYBACK)));

            context->playbackSystem()->connect(
                this,
                SIGNAL(playbackChanged(Util::PLAYBACK)),
                SLOT(setPlayback(Util::PLAYBACK)));
        }

        TimelineWidget::~TimelineWidget()
        {}

        void TimelineWidget::setDuration(int64_t value)
        {
            if (value == _duration)
                return;
            _duration = value;
            widgetUpdate();
            Q_EMIT durationChanged(_duration);
        }

        void TimelineWidget::setCurrentTime(int64_t value)
        {
            if (value == _currentTime)
                return;
            _currentTime = value;
            widgetUpdate();
            Q_EMIT currentTimeChanged(_currentTime);
        }

        void TimelineWidget::setPlayback(Util::PLAYBACK value)
        {
            if (value == _playback)
                return;
            _playback = value;
            widgetUpdate();
            Q_EMIT playbackChanged(_playback);
        }
        
        void TimelineWidget::widgetUpdate()
        {
            _stopButton->setChecked(Util::PLAYBACK_STOP == _playback);
            _forwardButton->setChecked(Util::PLAYBACK_FORWARD == _playback);
            _timeLabel->setText(QString("%1/%2").
                arg(Util::timeToSeconds(_currentTime), 0, 'f', 2).
                arg(Util::timeToSeconds(_duration), 0, 'f', 2));
        }

    } // namespace AudioExperiment2
} // namespace djv
