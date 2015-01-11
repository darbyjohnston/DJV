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

//! \file djvLutWidget.cpp

#include <djvLutWidget.h>

#include <djvPrefsGroupBox.h>
#include <djvStyle.h>

#include <djvSignalBlocker.h>

#include <QComboBox>
#include <QFormLayout>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvLutWidget
//------------------------------------------------------------------------------

djvLutWidget::djvLutWidget(djvLutPlugin * plugin) :
    djvAbstractPrefsWidget(djvLutPlugin::staticName),
    _plugin    (plugin),
    _typeWidget(0)
{
    // Create the widgets.

    _typeWidget = new QComboBox;
    _typeWidget->addItems(djvLutPlugin::typeLabels());
    _typeWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // Layout the widgets.

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setSpacing(djvStyle::global()->sizeMetric().largeSpacing);

    djvPrefsGroupBox * prefsGroupBox = new djvPrefsGroupBox(
        "Pixel Type",
        "Set the pixel type used when loading LUTs.");
    QFormLayout * formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_typeWidget);
    layout->addWidget(prefsGroupBox);

    layout->addStretch();

    // Initialize.

    QStringList tmp;
    tmp = _plugin->option(
        _plugin->options()[djvLutPlugin::TYPE_OPTION]);
    tmp >> _options.type;

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
}

djvLutWidget::~djvLutWidget()
{}

void djvLutWidget::resetPreferences()
{
    _options = djvLutPlugin::Options();
    
    pluginUpdate();
    widgetUpdate();
}

void djvLutWidget::pluginCallback(const QString & option)
{
    try
    {
        QStringList tmp;
        tmp = _plugin->option(option);

        if (0 == option.compare(_plugin->options()[
            djvLutPlugin::TYPE_OPTION], Qt::CaseInsensitive))
                tmp >> _options.type;
    }
    catch (const QString &)
    {}

    widgetUpdate();
}

void djvLutWidget::typeCallback(int in)
{
    _options.type = static_cast<djvLutPlugin::TYPE>(in);

    pluginUpdate();
}

void djvLutWidget::pluginUpdate()
{
    QStringList tmp;
    tmp << _options.type;
    _plugin->setOption(_plugin->options()[djvLutPlugin::TYPE_OPTION], tmp);
}

void djvLutWidget::widgetUpdate()
{
    djvSignalBlocker signalBlocker(QObjectList() <<
        _typeWidget);

    _typeWidget->setCurrentIndex(_options.type);
}
