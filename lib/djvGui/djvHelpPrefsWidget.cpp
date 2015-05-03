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

//! \file djvHelpPrefsWidget.cpp

#include <djvHelpPrefsWidget.h>

#include <djvApplication.h>
#include <djvHelpPrefs.h>
#include <djvPrefsGroupBox.h>
#include <djvStyle.h>

#include <djvDebug.h>
#include <djvSignalBlocker.h>

#include <QCheckBox>
#include <QFormLayout>
#include <QLabel>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvHelpPrefsWidgetPrivate
//------------------------------------------------------------------------------

struct djvHelpPrefsWidgetPrivate
{
    djvHelpPrefsWidgetPrivate() :
        toolTipsWidget(0)
    {}

    QCheckBox *  toolTipsWidget;
};

//------------------------------------------------------------------------------
// djvHelpPrefsWidget
//------------------------------------------------------------------------------

djvHelpPrefsWidget::djvHelpPrefsWidget(QWidget * parent) :
    djvAbstractPrefsWidget(
        qApp->translate("djvHelpPrefsWidget", "Help"), parent),
    _p(new djvHelpPrefsWidgetPrivate)
{
    // Create the widgets.

    _p->toolTipsWidget = new QCheckBox(
        qApp->translate("djvHelpPrefsWidget", "Enable tool tips"));

    // Layout the widgets.

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setSpacing(djvStyle::global()->sizeMetric().largeSpacing);

    djvPrefsGroupBox * prefsGroupBox = new djvPrefsGroupBox(
        qApp->translate("djvHelpPrefsWidget", "Tool Tips"));
    QFormLayout * formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_p->toolTipsWidget);
    layout->addWidget(prefsGroupBox);

    layout->addStretch();

    // Initialize.

    widgetUpdate();

    // Setup the callbacks.

    connect(
        _p->toolTipsWidget,
        SIGNAL(toggled(bool)),
        SLOT(toolTipsCallback(bool)));
}

djvHelpPrefsWidget::~djvHelpPrefsWidget()
{
    delete _p;
}

void djvHelpPrefsWidget::resetPreferences()
{
    djvHelpPrefs::global()->setToolTips(djvHelpPrefs::toolTipsDefault());

    widgetUpdate();
}

void djvHelpPrefsWidget::toolTipsCallback(bool toolTips)
{
    djvHelpPrefs::global()->setToolTips(toolTips);
}

void djvHelpPrefsWidget::widgetUpdate()
{
    djvSignalBlocker signalBlocker(QObjectList() <<
        _p->toolTipsWidget);

    _p->toolTipsWidget->setChecked(djvHelpPrefs::global()->hasToolTips());
}

