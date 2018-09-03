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

#include <djvSequencePrefsWidget.h>

#include <djvGuiContext.h>
#include <djvIntEdit.h>
#include <djvIntObject.h>
#include <djvSequencePrefs.h>
#include <djvPrefsGroupBox.h>
#include <djvStyle.h>

#include <djvDebug.h>
#include <djvSequence.h>
#include <djvSignalBlocker.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvSequencePrefsWidget::Private
//------------------------------------------------------------------------------

struct djvSequencePrefsWidget::Private
{
    Private() :
        maxFramesWidget(0)
    {}

    djvIntEdit * maxFramesWidget;
};

//------------------------------------------------------------------------------
// djvSequencePrefsWidget
//------------------------------------------------------------------------------

djvSequencePrefsWidget::djvSequencePrefsWidget(djvGuiContext * context, QWidget * parent) :
    djvAbstractPrefsWidget(
        qApp->translate("djvSequencePrefsWidget", "Sequences"), context, parent),
    _p(new Private)
{
    // Create the widgets.
    _p->maxFramesWidget = new djvIntEdit;
    _p->maxFramesWidget->setMax(djvIntObject::intMax);
    _p->maxFramesWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // Layout the widgets.
    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setSpacing(context->style()->sizeMetric().largeSpacing);

    djvPrefsGroupBox * prefsGroupBox = new djvPrefsGroupBox(
        qApp->translate("djvSequencePrefsWidget", "Max Frames"),
        qApp->translate("djvSequencePrefsWidget",
        "Set the maximum number of frames allowed in a sequence. This is used "
        "as a safety feature to prevent file sequences from becoming too large."),
        context);
    QFormLayout * formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(
        qApp->translate("djvSequencePrefsWidget", "Frames:"),
        _p->maxFramesWidget);
    layout->addWidget(prefsGroupBox);

    layout->addStretch();

    // Initialize.
    widgetUpdate();

    // Setup the callbacks.
    connect(
        _p->maxFramesWidget,
        SIGNAL(valueChanged(int)),
        SLOT(maxFramesCallback(int)));
}

djvSequencePrefsWidget::~djvSequencePrefsWidget()
{}

void djvSequencePrefsWidget::resetPreferences()
{
    context()->sequencePrefs()->setMaxFrames(djvSequence::maxFramesDefault());
    widgetUpdate();
}

void djvSequencePrefsWidget::maxFramesCallback(int size)
{
    context()->sequencePrefs()->setMaxFrames(size);
}

void djvSequencePrefsWidget::widgetUpdate()
{
    djvSignalBlocker signalBlocker(QObjectList() <<
        _p->maxFramesWidget);
    _p->maxFramesWidget->setValue(djvSequence::maxFrames());
}
