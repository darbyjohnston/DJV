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

//! \file djvJpegWidget.cpp

#include <djvJpegWidget.h>

#include <djvIntEditSlider.h>
#include <djvPrefsGroupBox.h>
#include <djvStyle.h>

#include <djvSignalBlocker.h>

#include <QFormLayout>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvJpegWidget
//------------------------------------------------------------------------------

djvJpegWidget::djvJpegWidget(djvJpegPlugin * plugin) :
    djvAbstractPrefsWidget(djvJpegPlugin::staticName),
    _plugin       (plugin),
    _qualityWidget(0)
{
    // Create the widgets.

    _qualityWidget = new djvIntEditSlider;
    _qualityWidget->setRange(0, 100);
    _qualityWidget->setDefaultValue(djvJpegPlugin::Options().quality);

    // Layout the widgets.

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setSpacing(djvStyle::global()->sizeMetric().largeSpacing);

    djvPrefsGroupBox * prefsGroupBox = new djvPrefsGroupBox(
        "Quality",
        "Set the quality used when saving JPEG images.");
    QFormLayout * formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_qualityWidget);
    layout->addWidget(prefsGroupBox);

    layout->addStretch();

    // Initialize.

    QStringList tmp;
    tmp = _plugin->option(
        _plugin->options()[djvJpegPlugin::QUALITY_OPTION]);
    tmp >> _options.quality;

    widgetUpdate();

    // Setup the callbacks.
    
    connect(
        plugin,
        SIGNAL(optionChanged(const QString &)),
        SLOT(pluginCallback(const QString &)));

    connect(
        _qualityWidget,
        SIGNAL(valueChanged(int)),
        SLOT(qualityCallback(int)));
}

djvJpegWidget::~djvJpegWidget()
{}

void djvJpegWidget::resetPreferences()
{
    _options = djvJpegPlugin::Options();
    
    pluginUpdate();
    widgetUpdate();
}

void djvJpegWidget::pluginCallback(const QString & option)
{
    try
    {
        QStringList tmp;
        tmp = _plugin->option(option);

        if (0 == option.compare(_plugin->options()[
            djvJpegPlugin::QUALITY_OPTION], Qt::CaseInsensitive))
                tmp >> _options.quality;
    }
    catch (const QString &)
    {
    }

    widgetUpdate();
}

void djvJpegWidget::qualityCallback(int in)
{
    _options.quality = in;

    pluginUpdate();
}

void djvJpegWidget::pluginUpdate()
{
    QStringList tmp;
    tmp << _options.quality;
    _plugin->setOption(
        djvJpegPlugin::optionsLabels()[djvJpegPlugin::QUALITY_OPTION], tmp);
}

void djvJpegWidget::widgetUpdate()
{
    djvSignalBlocker signalBlocker(_qualityWidget);

    _qualityWidget->setValue(_options.quality);
}
