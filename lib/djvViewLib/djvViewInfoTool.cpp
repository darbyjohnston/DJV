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

//! \file djvViewInfoTool.cpp

#include <djvViewInfoTool.h>

#include <djvViewImageView.h>
#include <djvViewMainWindow.h>

#include <djvPrefs.h>

#include <djvImageIo.h>
#include <djvSignalBlocker.h>
#include <djvTime.h>

#include <QDir>
#include <QFormLayout>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvViewInfoTool::P
//------------------------------------------------------------------------------

struct djvViewInfoTool::P
{
    P() :
        fileNameWidget (0),
        layerNameWidget(0),
        sizeWidget     (0),
        pixelWidget    (0),
        timeWidget     (0),
        tagsWidget     (0)
    {}
    
    QLineEdit *      fileNameWidget;
    QLineEdit *      layerNameWidget;
    QLineEdit *      sizeWidget;
    QLineEdit *      pixelWidget;
    QLineEdit *      timeWidget;
    QPlainTextEdit * tagsWidget;
};

//------------------------------------------------------------------------------
// djvViewInfoTool
//------------------------------------------------------------------------------

djvViewInfoTool::djvViewInfoTool(
    djvViewMainWindow * mainWindow,
    QWidget *           parent) :
    djvViewAbstractTool(mainWindow, parent),
    _p(new P)
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

    QVBoxLayout * layout = new QVBoxLayout(this);
    
    QFormLayout * formLayout = new QFormLayout;
    formLayout->addRow(tr("File name:"), _p->fileNameWidget);
    formLayout->addRow(tr("Layer:"), _p->layerNameWidget);
    formLayout->addRow(tr("Size:"), _p->sizeWidget);
    formLayout->addRow(tr("Pixel:"), _p->pixelWidget);
    formLayout->addRow(tr("Time:"), _p->timeWidget);
    formLayout->addRow(tr("Tags:"), _p->tagsWidget);
    layout->addLayout(formLayout);

    QHBoxLayout * hLayout = new QHBoxLayout;
    hLayout->setMargin(0);
    hLayout->addStretch();
    layout->addLayout(hLayout);

    // Initialize.
    
    setWindowTitle(tr("Information"));

    // Setup the callbacks.

    connect(
        mainWindow,
        SIGNAL(imageChanged()),
        SLOT(widgetUpdate()));
}

djvViewInfoTool::~djvViewInfoTool()
{
    delete _p;
}

void djvViewInfoTool::showEvent(QShowEvent *)
{
    widgetUpdate();
}

void djvViewInfoTool::widgetUpdate()
{
    djvSignalBlocker signalBlocker(QObjectList() <<
        _p->fileNameWidget <<
        _p->layerNameWidget <<
        _p->sizeWidget <<
        _p->pixelWidget <<
        _p->timeWidget <<
        _p->tagsWidget);

    const djvImageIoInfo & imageIoInfo = mainWindow()->imageIoInfo();

    if (isVisible())
    {
        _p->fileNameWidget->setText(
            QDir::toNativeSeparators(imageIoInfo.fileName));

        _p->layerNameWidget->setText(imageIoInfo.layerName);

        _p->sizeWidget->setText(QString(tr("%1x%2:%3")).
            arg(imageIoInfo.size.x).
            arg(imageIoInfo.size.y).
            arg(djvVectorUtil::aspect(imageIoInfo.size)));

        _p->pixelWidget->setText(
            djvStringUtil::label(imageIoInfo.pixel).join(", "));

        _p->timeWidget->setText(QString(tr("%1@%2")).
            arg(djvTime::frameToString(
                imageIoInfo.sequence.frames.count(),
                imageIoInfo.sequence.speed)).
            arg(djvSpeed::speedToFloat(imageIoInfo.sequence.speed), 0, 'f', 2));

        QString tmp;

        const QStringList keys = imageIoInfo.tags.keys();

        for (int i = 0; i < keys.count(); ++i)
        {
            tmp += QString(tr("%1 = %2\n")).
                arg(keys[i]).arg(imageIoInfo.tags[keys[i]]);
        }

        if (! tmp.isEmpty())
        {
            _p->tagsWidget->setPlainText(tmp);
        }
        else
        {
            _p->tagsWidget->clear();
        }
    }
}
