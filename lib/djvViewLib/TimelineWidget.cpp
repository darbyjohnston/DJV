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
            DJV_PRIVATE_PTR();
            p.context = context;

            p.stopButton = new QToolButton;
            p.stopButton->setCheckable(true);
            p.stopButton->setText("Stop");

            p.forwardButton = new QToolButton;
            p.forwardButton->setCheckable(true);
            p.forwardButton->setText("Forward");

            p.buttonGroup = new QButtonGroup(this);
            p.buttonGroup->setExclusive(true);
            p.buttonGroup->addButton(p.stopButton);
            p.buttonGroup->addButton(p.forwardButton);

            p.slider = new TimelineSlider(context);

            p.timeLabel = new QLabel;

            auto layout = new QHBoxLayout(this);
            layout->addWidget(p.stopButton);
            layout->addWidget(p.forwardButton);
            layout->addWidget(p.slider, 1);
            layout->addWidget(p.timeLabel);

            _widgetUpdate();

            connect(
                p.slider,
                SIGNAL(currentTimeChanged(AV::Timestamp)),
                this,
                SIGNAL(currentTimeChanged(AV::Timestamp)));
            connect(
                p.buttonGroup,
                SIGNAL(buttonToggled(QAbstractButton *, bool)),
                SLOT(_buttonCallback(QAbstractButton *, bool)));
        }

        TimelineWidget::~TimelineWidget()
        {}

        void TimelineWidget::setDuration(AV::Duration value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.duration)
                return;
            p.duration = value;
            _widgetUpdate();
        }

        void TimelineWidget::setCurrentTime(AV::Timestamp value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.currentTime)
                return;
            p.currentTime = value;
            _widgetUpdate();
            Q_EMIT currentTimeChanged(p.currentTime);
        }

        void TimelineWidget::setPlayback(Enum::Playback value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.playback)
                return;
            p.playback = value;
            _widgetUpdate();
            Q_EMIT playbackChanged(p.playback);
        }

        void TimelineWidget::_buttonCallback(QAbstractButton * button, bool value)
        {
            DJV_PRIVATE_PTR();
            if (button == p.stopButton)
            {
                setPlayback(Enum::Playback::Stop);
            }
            else if (button == p.forwardButton)
            {
                setPlayback(Enum::Playback::Forward);
            }
        }

        void TimelineWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            const QSignalBlocker buttonGroupBlocker(p.buttonGroup);
            const QSignalBlocker sliderBlocker(p.slider);
            switch (p.playback)
            {
            case Enum::Playback::Stop: p.stopButton->setChecked(true); break;
            case Enum::Playback::Forward: p.forwardButton->setChecked(true); break;
            default: break;
            }
            p.slider->setDuration(p.duration);
            p.slider->setCurrentTime(p.currentTime);
            p.timeLabel->setText(QString("%1/%2").
                arg(AV::timestampToSeconds(p.currentTime), 0, 'f', 2).
                arg(AV::timestampToSeconds(p.duration), 0, 'f', 2));
        }

    } // namespace ViewLib
} // namespace djv

