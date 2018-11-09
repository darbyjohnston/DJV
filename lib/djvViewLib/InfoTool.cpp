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

#include <djvViewLib/InfoTool.h>

#include <djvViewLib/FileGroup.h>
#include <djvViewLib/Session.h>

#include <djvUI/Prefs.h>

#include <djvCore/SignalBlocker.h>
#include <djvCore/Time.h>

#include <djvAV/ImageIO.h>

#include <QApplication>
#include <QDir>
#include <QFormLayout>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QVBoxLayout>

namespace djv
{
    namespace ViewLib
    {
        struct InfoTool::Private
        {
            AV::ImageIOInfo info;

            QPointer<QLineEdit>      fileNameWidget;
            QPointer<QLineEdit>      layerNameWidget;
            QPointer<QLineEdit>      sizeWidget;
            QPointer<QLineEdit>      pixelWidget;
            QPointer<QLineEdit>      timeWidget;
            QPointer<QPlainTextEdit> tagsWidget;
        };

        InfoTool::InfoTool(
            const QPointer<Session> & session,
            const QPointer<ViewContext> & context,
            QWidget * parent) :
            AbstractTool(session, context, parent),
            _p(new Private)
        {
            // Create the widgets.
            _p->fileNameWidget = new QLineEdit;
            _p->fileNameWidget->setReadOnly(true);

            _p->layerNameWidget = new QLineEdit;
            _p->layerNameWidget->setReadOnly(true);

            _p->sizeWidget = new QLineEdit;
            _p->sizeWidget->setReadOnly(true);

            _p->pixelWidget = new QLineEdit;
            _p->pixelWidget->setReadOnly(true);

            _p->timeWidget = new QLineEdit;
            _p->timeWidget->setReadOnly(true);

            _p->tagsWidget = new QPlainTextEdit;
            _p->tagsWidget->setReadOnly(true);

            // Layout the widgets.
            auto layout = new QVBoxLayout(this);

            auto formLayout = new QFormLayout;
            formLayout->addRow(
                qApp->translate("djv::ViewLib::InfoTool", "File name:"),
                _p->fileNameWidget);
            formLayout->addRow(
                qApp->translate("djv::ViewLib::InfoTool", "Layer:"),
                _p->layerNameWidget);
            formLayout->addRow(
                qApp->translate("djv::ViewLib::InfoTool", "Size:"),
                _p->sizeWidget);
            formLayout->addRow(
                qApp->translate("djv::ViewLib::InfoTool", "Pixel:"),
                _p->pixelWidget);
            formLayout->addRow(
                qApp->translate("djv::ViewLib::InfoTool", "Time:"),
                _p->timeWidget);
            formLayout->addRow(
                qApp->translate("djv::ViewLib::InfoTool", "Tags:"),
                _p->tagsWidget);
            layout->addLayout(formLayout);

            auto hLayout = new QHBoxLayout;
            hLayout->setMargin(0);
            hLayout->addStretch();
            layout->addLayout(hLayout);

            // Initialize.
            setWindowTitle(qApp->translate("djv::ViewLib::InfoTool", "Information"));

            // Setup the callbacks.
            connect(
                session->fileGroup(),
                &FileGroup::imageIOInfoChanged,
                [this](const AV::ImageIOInfo & value)
            {
                _p->info = value;
                widgetUpdate();
            });
        }

        InfoTool::~InfoTool()
        {}

        void InfoTool::showEvent(QShowEvent *)
        {
            widgetUpdate();
        }

        void InfoTool::widgetUpdate()
        {
            Core::SignalBlocker signalBlocker(QObjectList() <<
                _p->fileNameWidget <<
                _p->layerNameWidget <<
                _p->sizeWidget <<
                _p->pixelWidget <<
                _p->timeWidget <<
                _p->tagsWidget);
            _p->fileNameWidget->setText(
                QDir::toNativeSeparators(_p->info.fileName));
            _p->layerNameWidget->setText(_p->info.layerName);
            _p->sizeWidget->setText(
                qApp->translate("djv::ViewLib::InfoTool", "%1x%2:%3").
                arg(_p->info.size.x).
                arg(_p->info.size.y).
                arg(Core::VectorUtil::aspect(_p->info.size)));
            QStringList pixelLabel;
            pixelLabel << _p->info.pixel;
            _p->pixelWidget->setText(pixelLabel.join(", "));
            _p->timeWidget->setText(
                qApp->translate("djv::ViewLib::InfoTool", "%1@%2").
                arg(Core::Time::frameToString(_p->info.sequence.frames.count(), _p->info.sequence.speed)).
                arg(Core::Speed::speedToFloat(_p->info.sequence.speed), 0, 'f', 2));
            QString tmp;
            const QStringList keys = _p->info.tags.keys();
            for (int i = 0; i < keys.count(); ++i)
            {
                tmp += qApp->translate("djv::ViewLib::InfoTool", "%1 = %2\n").
                    arg(keys[i]).arg(_p->info.tags[keys[i]]);
            }
            if (!tmp.isEmpty())
            {
                _p->tagsWidget->setPlainText(tmp);
            }
            else
            {
                _p->tagsWidget->clear();
            }
        }

    } // namespace ViewLib
} // namespace djv
