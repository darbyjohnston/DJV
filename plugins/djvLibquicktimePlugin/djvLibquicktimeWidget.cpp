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

//! \file djvLibquicktimeWidget.cpp

#include <djvLibquicktimeWidget.h>

#include <djvIntEdit.h>
#include <djvPrefsGroupBox.h>
#include <djvStyle.h>

#include <djvListUtil.h>
#include <djvSignalBlocker.h>
#include <djvStringUtil.h>

#include <QComboBox>
#include <QFormLayout>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvLibquicktimeWidget
//------------------------------------------------------------------------------

djvLibquicktimeWidget::djvLibquicktimeWidget(djvLibquicktimePlugin * plugin) :
    djvAbstractPrefsWidget(djvLibquicktimePlugin::staticName),
    _plugin     (plugin),
    _codecWidget(0)
{
    // Create the save widgets.

    _codecWidget = new QComboBox;
    _codecWidget->addItems(djvLibquicktimePlugin::codecLabels());
    _codecWidget->setSizePolicy(
        QSizePolicy::Fixed, QSizePolicy::Fixed);

    // Layout.

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setSpacing(djvStyle::global()->sizeMetric().largeSpacing);

    djvPrefsGroupBox * prefsGroupBox = new djvPrefsGroupBox(
        "Codec",
        "Set the codec used when saving movies.");
    QFormLayout * formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_codecWidget);
    layout->addWidget(prefsGroupBox);

    layout->addStretch();

    // Initialize.

    widgetUpdate();

    // Setup the callbacks.
    
    connect(
        plugin,
        SIGNAL(optionChanged(const QString &)),
        SLOT(widgetUpdate()));

    connect(
        _codecWidget,
        SIGNAL(activated(int)),
        SLOT(codecCallback(int)));
}

djvLibquicktimeWidget::~djvLibquicktimeWidget()
{}

void djvLibquicktimeWidget::resetPreferences()
{
    _options = djvLibquicktimePlugin::Options();
    
    pluginUpdate();
    widgetUpdate();
}

void djvLibquicktimeWidget::pluginCallback(const QString & option)
{
    try
    {
        QStringList tmp;
        tmp = _plugin->option(option);

        if (0 == option.compare(_plugin->options()[
            djvLibquicktimePlugin::CODEC], Qt::CaseInsensitive))
                tmp >> _options.codec;
    }
    catch (const QString &)
    {}

    widgetUpdate();
}

void djvLibquicktimeWidget::codecCallback(int in)
{
    _options.codec = djvLibquicktimePlugin::codecLabels()[in];

    pluginUpdate();
}

void djvLibquicktimeWidget::pluginUpdate()
{
    QStringList tmp;
    tmp << _options.codec;
    _plugin->setOption(
        _plugin->options()[djvLibquicktimePlugin::CODEC], tmp);
}

void djvLibquicktimeWidget::widgetUpdate()
{
    djvSignalBlocker signalBlocker(QObjectList() <<
        _codecWidget);
    
    try
    {
        QStringList tmp;
        tmp = _plugin->option(
            _plugin->options()[djvLibquicktimePlugin::CODEC]);
        tmp >> _options.codec;
    }
    catch (QString)
    {}

    _codecWidget->setCurrentIndex(
        djvLibquicktimePlugin::codecLabels().indexOf(_options.codec));
}

