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
//------------------------------------------------------------------------------s

#include <djvUI/ProgressDialog.h>

#include <djvCore/Time.h>
#include <djvCore/Timer.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QVBoxLayout>

namespace djv
{
    namespace UI
    {
        namespace
        {
            class ProgressWidget : public QWidget
            {
            public:

                ProgressWidget()
                {
                    setAttribute(Qt::WA_OpaquePaintEvent);
                }

                void setValue(int value)
                {
                    if (value == _value)
                        return;
                    _value = value;
                    update();
                }

                void setMaxValue(int maxValue)
                {
                    if (maxValue == _maxValue)
                        return;
                    _maxValue = maxValue;
                    update();
                }

                virtual QSize sizeHint() const { return QSize(200, 20); }

            protected:
                virtual void paintEvent(QPaintEvent *)
                {
                    QPainter painter(this);
                    const QPalette & palette = this->palette();
                    painter.fillRect(
                        0, 0, width(), height(),
                        palette.color(QPalette::Base));
                    float v = 0.f;
                    if (_maxValue != 0)
                    {
                        v = _value / static_cast<float>(_maxValue);
                    }
                    int w = static_cast<int>(v * width());
                    painter.fillRect(
                        0, 0, w, height(),
                        palette.color(QPalette::Highlight));
                    painter.setPen(palette.color(QPalette::Text));
                    painter.drawText(
                        QRect(0, 0, width(), height()),
                        Qt::AlignCenter,
                        QString("%1%").arg(static_cast<int>(v * 100.f)));
                }

            private:
                int _value = 0;
                int _maxValue = 0;
            };

        } // namespace

        struct ProgressDialog::Private
        {
            int              totalTicks = 0;
            int              currentTick = 0;
            Core::Timer      time;
            float            timeAccum = 0.f;
            Core::Timer      elapsed;
            int              timerId = 0;
            ProgressWidget * widget = nullptr;
            QString          label = nullptr;
            QLabel *         labelWidget = nullptr;
            QLabel *         estimateLabel = nullptr;
            QLabel *         elapsedLabel = nullptr;
        };

        ProgressDialog::ProgressDialog(const QString & label, QWidget * parent) :
            QDialog(parent),
            _p(new Private)
        {
            // Create the widgets.
            _p->widget = new ProgressWidget;

            _p->labelWidget = new QLabel;

            _p->estimateLabel = new QLabel;

            _p->elapsedLabel = new QLabel;

            QDialogButtonBox * buttonBox = new QDialogButtonBox(
                QDialogButtonBox::Cancel);

            // Layout the widgets.
            QVBoxLayout * layout = new QVBoxLayout(this);
            QVBoxLayout * vLayout = new QVBoxLayout;
            vLayout->setMargin(20);
            vLayout->addWidget(_p->labelWidget);
            vLayout->addWidget(_p->widget);
            vLayout->addWidget(_p->estimateLabel);
            vLayout->addWidget(_p->elapsedLabel);
            layout->addLayout(vLayout);
            layout->addWidget(buttonBox);

            // Initialize.
            setWindowTitle(qApp->translate("djv::UI::ProgressDialog", "Progress Dialog"));

            _p->labelWidget->setText(label);

            // Setup the callbacks.
            connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
            connect(buttonBox, SIGNAL(rejected()), this, SLOT(rejectedCallback()));
            connect(this, SIGNAL(rejected()), SIGNAL(finishedSignal()));
        }

        ProgressDialog::~ProgressDialog()
        {
            stopTimer();
        }

        void ProgressDialog::rejectedCallback()
        {
            stopTimer();
            Q_EMIT finishedSignal();
        }

        const QString & ProgressDialog::label() const
        {
            return _p->label;
        }

        void ProgressDialog::setLabel(const QString & label)
        {
            if (label == _p->label)
                return;
            _p->label = label;
            _p->labelWidget->setText(label);
        }

        void ProgressDialog::start(int totalTicks)
        {
            _p->totalTicks = totalTicks;
            _p->currentTick = 0;
            _p->time.start();
            _p->timeAccum = 0.f;
            _p->elapsed.start();
            _p->widget->setMaxValue(static_cast<float>(_p->totalTicks));
            _p->widget->setValue(static_cast<float>(_p->currentTick));
            _p->timerId = startTimer(0);
        }

        void ProgressDialog::hideEvent(QHideEvent *)
        {
            stopTimer();
        }

        void ProgressDialog::timerEvent(QTimerEvent *)
        {
            // If the progress is finished then hide the dialog otherwise emit the
            // progress signal.
            if (_p->currentTick >= _p->totalTicks)
            {
                hide();
                stopTimer();
                Q_EMIT finishedSignal();
            }
            else
            {
                Q_EMIT progressSignal(_p->currentTick);
            }

            // Update the progress widget.
            _p->widget->setValue(static_cast<float>(_p->currentTick));

            // Update the labels.
            _p->time.check();
            _p->elapsed.check();

            _p->timeAccum += _p->time.seconds();
            _p->time.start();

            const float estimate =
                _p->timeAccum /
                static_cast<float>(_p->currentTick + 1) *
                (_p->totalTicks - (_p->currentTick + 1));

            const QString estimateLabel = QString(
                qApp->translate("djv::UI::ProgressDialog", "Estimated: %1 (%2 Frames/Second)")).
                arg(Core::Time::labelTime(estimate)).
                arg(_p->currentTick / _p->elapsed.seconds(), 0, 'f', 2);

            _p->estimateLabel->setText(estimateLabel);

            const QString elapsedLabel = QString(
                qApp->translate("Core::::UI::ProgressDialog", "Elapsed: %1")).
                arg(Core::Time::labelTime(_p->elapsed.seconds()));

            _p->elapsedLabel->setText(elapsedLabel);

            ++_p->currentTick;
        }

        void ProgressDialog::stopTimer()
        {
            if (_p->timerId != 0)
            {
                killTimer(_p->timerId);
                _p->timerId = 0;
            }
        }

    } // namespace UI
} // namespace djv
