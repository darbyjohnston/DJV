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

//! \file djvFFmpegWidget.cpp

#include <djvFFmpegWidget.h>

#include <djvPrefsGroupBox.h>
#include <djvSignalBlocker.h>
#include <djvStyle.h>

#include <QApplication>
#include <QComboBox>
#include <QFormLayout>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvFFmpegWidget
//------------------------------------------------------------------------------

djvFFmpegWidget::djvFFmpegWidget(djvFFmpegPlugin * plugin) :
    djvAbstractPrefsWidget(djvFFmpegPlugin::staticName),
    _plugin       (plugin),
    _codecWidget  (0),
    _qualityWidget(0)
{
    // Create the widgets.

    _codecWidget = new QComboBox;
    _codecWidget->addItems(djvFFmpegPlugin::codecLabels());
    _codecWidget->setSizePolicy(
        QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    _qualityWidget = new QComboBox;
    _qualityWidget->addItems(djvFFmpegPlugin::qualityLabels());
    _qualityWidget->setSizePolicy(
        QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    // Layout the widgets.

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setSpacing(djvStyle::global()->sizeMetric().largeSpacing);

    djvPrefsGroupBox * prefsGroupBox = new djvPrefsGroupBox(
        qApp->translate("djvFFmpegWidget", "Codec"),
        qApp->translate("djvFFmpegWidget", "Set the codec used when saving movies."));
    QFormLayout * formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(
        qApp->translate("djvFFmpegWidget", "Codec:"),
        _codecWidget);
    formLayout->addRow(
        qApp->translate("djvFFmpegWidget", "Quality:"),
        _qualityWidget);
    layout->addWidget(prefsGroupBox);

    layout->addStretch();

    // Initialize.

    widgetUpdate();

    // Setup the callbacks.
    
    connect(
        plugin,
        SIGNAL(optionChanged(const QString &)),
        SLOT(pluginCallback(const QString &)));

    connect(
        _codecWidget,
        SIGNAL(activated(int)),
        SLOT(codecCallback(int)));

    connect(
        _qualityWidget,
        SIGNAL(activated(int)),
        SLOT(qualityCallback(int)));
}

djvFFmpegWidget::~djvFFmpegWidget()
{}

void djvFFmpegWidget::resetPreferences()
{
    _options = djvFFmpegPlugin::Options();
    
    pluginUpdate();
    widgetUpdate();
}

void djvFFmpegWidget::pluginCallback(const QString & option)
{
    try
    {
        QStringList tmp;
        tmp = _plugin->option(option);

        if (0 == option.compare(_plugin->options()[
            djvFFmpegPlugin::OPTIONS_CODEC], Qt::CaseInsensitive))
                tmp >> _options.codec;
        else if (0 == option.compare(_plugin->options()[
            djvFFmpegPlugin::OPTIONS_QUALITY], Qt::CaseInsensitive))
                tmp >> _options.quality;
    }
    catch (const QString &)
    {}

    widgetUpdate();
}

void djvFFmpegWidget::codecCallback(int in)
{
    _options.codec = static_cast<djvFFmpegPlugin::CODEC>(in);

    pluginUpdate();
}

void djvFFmpegWidget::qualityCallback(int in)
{
    _options.quality = static_cast<djvFFmpegPlugin::QUALITY>(in);

    pluginUpdate();
}

void djvFFmpegWidget::pluginUpdate()
{
    QStringList tmp;
    tmp << _options.codec;
    _plugin->setOption(
        _plugin->options()[djvFFmpegPlugin::OPTIONS_CODEC], tmp);
    tmp << _options.quality;
    _plugin->setOption(
        _plugin->options()[djvFFmpegPlugin::OPTIONS_QUALITY], tmp);
}

void djvFFmpegWidget::widgetUpdate()
{
    djvSignalBlocker signalBlocker(QObjectList() <<
        _codecWidget <<
        _qualityWidget);
    
    try
    {
        QStringList tmp;
        tmp = _plugin->option(
            _plugin->options()[djvFFmpegPlugin::OPTIONS_CODEC]);
        tmp >> _options.codec;
        tmp = _plugin->option(
            _plugin->options()[djvFFmpegPlugin::OPTIONS_QUALITY]);
        tmp >> _options.quality;
    }
    catch (QString)
    {}

    _codecWidget->setCurrentIndex(_options.codec);
    _qualityWidget->setCurrentIndex(_options.quality);
}
