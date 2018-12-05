//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvViewLib/TimelineWidget.h>

#include <djvViewLib/Context.h>
#include <djvViewLib/TimelineSlider.h>

#include <QButtonGroup>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>

namespace djv
{
    namespace ViewLib
    {
        struct TimelineWidget::Private
        {
            std::weak_ptr<Context> context;
            int64_t duration = 0;
            int64_t currentTime = 0;
            Enum::Playback playback = Enum::Playback::Stop;
            QPointer<QToolButton> stopButton;
            QPointer<QToolButton> forwardButton;
            QPointer<QButtonGroup> buttonGroup;
            QPointer<TimelineSlider> slider;
            QPointer<QLabel> timeLabel;
        };

        TimelineWidget::TimelineWidget(const std::shared_ptr<Context> & context, QWidget * parent) :
            QWidget(parent),
            _p(new Private)
        {
            _p->context = context;

            _p->stopButton = new QToolButton;
            _p->stopButton->setCheckable(true);
            _p->stopButton->setText("Stop");

            _p->forwardButton = new QToolButton;
            _p->forwardButton->setCheckable(true);
            _p->forwardButton->setText("Forward");

            _p->buttonGroup = new QButtonGroup(this);
            _p->buttonGroup->setExclusive(true);
            _p->buttonGroup->addButton(_p->stopButton);
            _p->buttonGroup->addButton(_p->forwardButton);

            _p->slider = new TimelineSlider(context);

            _p->timeLabel = new QLabel;

            auto layout = new QHBoxLayout(this);
            layout->addWidget(_p->stopButton);
            layout->addWidget(_p->forwardButton);
            layout->addWidget(_p->slider, 1);
            layout->addWidget(_p->timeLabel);

            _widgetUpdate();

            connect(
                _p->slider,
                SIGNAL(currentTimeChanged(AV::Timestamp)),
                this,
                SIGNAL(currentTimeChanged(AV::Timestamp)));
            connect(
                _p->buttonGroup,
                SIGNAL(buttonToggled(QAbstractButton *, bool)),
                SLOT(_buttonCallback(QAbstractButton *, bool)));
        }

        TimelineWidget::~TimelineWidget()
        {}

        void TimelineWidget::setDuration(AV::Duration value)
        {
            if (value == _p->duration)
                return;
            _p->duration = value;
            _widgetUpdate();
        }

        void TimelineWidget::setCurrentTime(AV::Timestamp value)
        {
            if (value == _p->currentTime)
                return;
            _p->currentTime = value;
            _widgetUpdate();
            Q_EMIT currentTimeChanged(_p->currentTime);
        }

        void TimelineWidget::setPlayback(Enum::Playback value)
        {
            if (value == _p->playback)
                return;
            _p->playback = value;
            _widgetUpdate();
            Q_EMIT playbackChanged(_p->playback);
        }

        void TimelineWidget::_buttonCallback(QAbstractButton * button, bool value)
        {
            if (button == _p->stopButton)
            {
                setPlayback(Enum::Playback::Stop);
            }
            else if (button == _p->forwardButton)
            {
                setPlayback(Enum::Playback::Forward);
            }
        }

        void TimelineWidget::_widgetUpdate()
        {
            const QSignalBlocker buttonGroupBlocker(_p->buttonGroup);
            const QSignalBlocker sliderBlocker(_p->slider);
            switch (_p->playback)
            {
            case Enum::Playback::Stop: _p->stopButton->setChecked(true); break;
            case Enum::Playback::Forward: _p->forwardButton->setChecked(true); break;
            default: break;
            }
            _p->slider->setDuration(_p->duration);
            _p->slider->setCurrentTime(_p->currentTime);
            _p->timeLabel->setText(QString("%1/%2").
                arg(AV::timestampToSeconds(_p->currentTime), 0, 'f', 2).
                arg(AV::timestampToSeconds(_p->duration), 0, 'f', 2));
        }

    } // namespace ViewLib
} // namespace djv

