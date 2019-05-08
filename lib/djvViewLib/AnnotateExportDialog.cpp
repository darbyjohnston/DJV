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

#include <djvViewLib/AnnotateExportDialog.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPainter>
#include <QPointer>
#include <QPushButton>
#include <QStyle>
#include <QTextEdit>
#include <QVBoxLayout>

namespace djv
{
    namespace ViewLib
    {
        struct Spinner::Private
        {
            float spin = 0.f;
            int timerId = 0;
        };

        Spinner::Spinner(QWidget* parent) :
            QWidget(parent),
            _p(new Private)
        {
        }

        QSize Spinner::sizeHint() const
        {
            const int iconSize = style()->pixelMetric(QStyle::PM_ToolBarIconSize);
            const int margin = style()->pixelMetric(QStyle::PM_ButtonMargin);
            QSize sizeHint(iconSize, iconSize);
            sizeHint += QSize(margin * 2, margin * 2);
            return sizeHint.expandedTo(QApplication::globalStrut());
        }

        void Spinner::start()
        {
            _p->spin = 0.f;
            _p->timerId = startTimer(20);
        }

        void Spinner::stop()
        {
            _p->spin = 0.f;
            if (_p->timerId != 0)
            {
                killTimer(_p->timerId);
                _p->timerId = 0;
            }
            update();
        }

        void Spinner::timerEvent(QTimerEvent*)
        {
            _p->spin += .1f;
            update();
        }

        void Spinner::paintEvent(QPaintEvent* event)
        {
            if (_p->timerId)
            {
                QPainter painter(this);
                const int margin = style()->pixelMetric(QStyle::PM_ButtonMargin);
                const int w = height() / 10.f;
                const int x = margin + w / 2 + static_cast<int>((width() - margin * 2 - w) * (cos(_p->spin) + 1.f) / 2.f);
                const int y = height() / 2.f;
                painter.setBrush(palette().color(QPalette::ColorRole::Foreground));
                painter.drawEllipse(QPoint(x, y), w, w);
            }
        }

        struct AnnotateExportDialog::Private
        {
            QPointer<QProcess> process;
            QProcess::ProcessState state = QProcess::ProcessState::NotRunning;
            QPointer<Spinner> spinner;
            QPointer<QLabel> stateLabel;
            QPointer<QTextEdit> outputTextEdit;
            QPointer<QDialogButtonBox> buttonBox;
        };

        AnnotateExportDialog::AnnotateExportDialog(QProcess* process, QWidget * parent) :
            QDialog(parent),
            _p(new Private)
        {
            _p->process = process;

            _p->spinner = new Spinner;

            _p->stateLabel = new QLabel;

            _p->outputTextEdit = new QTextEdit;
            _p->outputTextEdit->setReadOnly(true);

            _p->buttonBox = new QDialogButtonBox(
                QDialogButtonBox::Ok |
                QDialogButtonBox::Cancel);

            auto layout = new QVBoxLayout(this);
            auto vLayout = new QVBoxLayout;
            auto hLayout = new QHBoxLayout;
            hLayout->addWidget(_p->stateLabel);
            hLayout->addWidget(_p->spinner);
            hLayout->addStretch();
            vLayout->addLayout(hLayout);
            vLayout->addWidget(new QLabel(qApp->translate("djv::UI::AnnotateExportDialog", "Output:")));
            vLayout->addWidget(_p->outputTextEdit);
            layout->addLayout(vLayout);
            layout->addWidget(_p->buttonBox);

            setWindowTitle(qApp->translate("djv::UI::AnnotateExportDialog", "Annotate Export Script"));
            widgetUpdate();

            connect(_p->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
            connect(_p->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

            connect(
                process,
                SIGNAL(stateChanged(QProcess::ProcessState)),
                SLOT(stateCallback(QProcess::ProcessState)));
            connect(
                process,
                SIGNAL(readyReadStandardOutput()),
                SLOT(standardOutputCallback()));
            connect(
                process,
                SIGNAL(readyReadStandardError()),
                SLOT(standardErrorCallback()));
            connect(
                process,
                SIGNAL(errorOccurred(QProcess::ProcessError)),
                SLOT(scriptErrorCallback(QProcess::ProcessError)));
        }

        AnnotateExportDialog::~AnnotateExportDialog()
        {}

        void AnnotateExportDialog::stateCallback(QProcess::ProcessState value)
        {
            _p->state = value;
            if (QProcess::ProcessState::Starting == value)
            {
                _p->spinner->start();
                _p->outputTextEdit->clear();
            }
            else if (QProcess::ProcessState::NotRunning == value)
            {
                _p->spinner->stop();
            }
            widgetUpdate();
        }

        void AnnotateExportDialog::standardOutputCallback()
        {
            _p->outputTextEdit->append(QString(_p->process->readAllStandardOutput()));
        }

        void AnnotateExportDialog::standardErrorCallback()
        {
            _p->outputTextEdit->append(QString(_p->process->readAllStandardError()));
        }

        void AnnotateExportDialog::scriptErrorCallback(QProcess::ProcessError value)
        {
            static const std::map< QProcess::ProcessError, QString> errorStrings =
            {
                { QProcess::FailedToStart, qApp->translate("djv::ViewLib::AnnotateExport", "Failed to start") },
                { QProcess::Crashed, qApp->translate("djv::ViewLib::AnnotateExport", "Crashed") },
                { QProcess::Timedout, qApp->translate("djv::ViewLib::AnnotateExport", "Timed out") },
                { QProcess::WriteError, qApp->translate("djv::ViewLib::AnnotateExport", "Write error") },
                { QProcess::ReadError, qApp->translate("djv::ViewLib::AnnotateExport", "Read error") }
            };
            QString text = qApp->translate("djv::ViewLib::AnnotateExport", "Unknown");
            const auto i = errorStrings.find(value);
            if (i != errorStrings.end())
            {
                text = i->second;
            }
            _p->outputTextEdit->append(qApp->translate("djv::ViewLib::AnnotateExportDialog", "Error running script: %1").arg(text));
        }

        void AnnotateExportDialog::widgetUpdate()
        {
            static const std::map<QProcess::ProcessState, QString> stateLabels =
            {
                { QProcess::NotRunning, qApp->translate("djv::ViewLib::AnnotateExportDialog", "The script has finished.") },
                { QProcess::Starting, qApp->translate("djv::ViewLib::AnnotateExportDialog", "The script is starting...") },
                { QProcess::Running, qApp->translate("djv::ViewLib::AnnotateExportDialog", "The script is running...") }
            };
            QString text = qApp->translate("djv::ViewLib::AnnotateExportDialog", "Unknown");
            const auto i = stateLabels.find(_p->state);
            if (i != stateLabels.end())
            {
                text = i->second;
            }
            _p->stateLabel->setText(text);
            _p->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(QProcess::ProcessState::NotRunning == _p->state);
            _p->buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(QProcess::ProcessState::NotRunning != _p->state);
        }

    } // namespace ViewLib
} // namespace djv
