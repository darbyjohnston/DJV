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

//! \file djvPpmWidget.cpp

#include <djvPpmWidget.h>

#include <djvPrefsGroupBox.h>
#include <djvSignalBlocker.h>
#include <djvStyle.h>

#include <QComboBox>
#include <QFormLayout>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvPpmWidget
//------------------------------------------------------------------------------

djvPpmWidget::djvPpmWidget(djvPpmPlugin * plugin) :
    djvAbstractPrefsWidget(djvPpmPlugin::staticName),
    _plugin    (plugin),
    _typeWidget(0),
    _dataWidget(0)
{
    // Create the widgets.
    
    _typeWidget = new QComboBox;
    _typeWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _typeWidget->addItems(djvPpmPlugin::typeLabels());
    
    _dataWidget = new QComboBox;
    _dataWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _dataWidget->addItems(djvPpmPlugin::dataLabels());

    // Layout the widgets.

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setSpacing(djvStyle::global()->sizeMetric().largeSpacing);

    djvPrefsGroupBox * prefsGroupBox = new djvPrefsGroupBox(
        "File Type",
        "Set the file type used when saving PPM images.");
    QFormLayout * formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_typeWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(
        "Data Type",
        "Set the data type used when saving PPM images.");
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_dataWidget);
    layout->addWidget(prefsGroupBox);

    layout->addStretch();

    // Initialize.

    QStringList tmp;
    tmp = _plugin->option(
        _plugin->options()[djvPpmPlugin::TYPE_OPTION]);
    tmp >> _options.type;
    tmp = _plugin->option(
        _plugin->options()[djvPpmPlugin::DATA_OPTION]);
    tmp >> _options.data;

    widgetUpdate();

    // Setup the callbacks.
    
    connect(
        plugin,
        SIGNAL(optionChanged(const QString &)),
        SLOT(pluginCallback(const QString &)));

    connect(
        _typeWidget,
        SIGNAL(activated(int)),
        SLOT(typeCallback(int)));

    connect(
        _dataWidget,
        SIGNAL(activated(int)),
        SLOT(dataCallback(int)));
}

djvPpmWidget::~djvPpmWidget()
{}

void djvPpmWidget::resetPreferences()
{
    _options = djvPpmPlugin::Options();
    
    pluginUpdate();
    widgetUpdate();
}

void djvPpmWidget::pluginCallback(const QString & option)
{
    try
    {
        QStringList tmp;
        tmp = _plugin->option(option);

        if (0 == option.compare(_plugin->options()[
            djvPpmPlugin::TYPE_OPTION], Qt::CaseInsensitive))
                tmp >> _options.type;
        else if (0 == option.compare(_plugin->options()[
            djvPpmPlugin::DATA_OPTION], Qt::CaseInsensitive))
                tmp >> _options.data;
    }
    catch (const QString &)
    {}

    widgetUpdate();
}

void djvPpmWidget::typeCallback(int in)
{
    _options.type = static_cast<djvPpmPlugin::TYPE>(in);

    pluginUpdate();
}

void djvPpmWidget::dataCallback(int in)
{
    _options.data = static_cast<djvPpmPlugin::DATA>(in);

    pluginUpdate();
}

void djvPpmWidget::pluginUpdate()
{
    QStringList tmp;
    tmp << _options.type;
    _plugin->setOption(_plugin->options()[djvPpmPlugin::TYPE_OPTION], tmp);
    tmp << _options.data;
    _plugin->setOption(_plugin->options()[djvPpmPlugin::DATA_OPTION], tmp);
}

void djvPpmWidget::widgetUpdate()
{
    djvSignalBlocker signalBlocker(QObjectList() <<
        _typeWidget <<
        _dataWidget);

    _typeWidget->setCurrentIndex(_options.type);

    _dataWidget->setCurrentIndex(_options.data);
}
