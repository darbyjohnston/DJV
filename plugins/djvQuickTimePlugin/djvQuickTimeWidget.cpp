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

//! \file djvQuickTimeWidget.cpp

#if defined(DJV_OSX)

//! \todo Force include order on OS X.

#include <sys/types.h>
#include <sys/stat.h>

#endif

#include <djvQuickTimeWidget.h>

#include <djvIntEdit.h>
#include <djvPrefsGroupBox.h>
#include <djvStyle.h>

#include <djvSignalBlocker.h>

#include <QComboBox>
#include <QFormLayout>
#include <QVBoxLayout>

using namespace djv;

//------------------------------------------------------------------------------
// djvQuickTimeWidget
//------------------------------------------------------------------------------

djvQuickTimeWidget::djvQuickTimeWidget(djvQuickTimePlugin * plugin) :
    djvAbstractPrefsWidget(djvQuickTimePlugin::staticName),
    _plugin       (plugin),
    _codecWidget  (0),
    _qualityWidget(0)
{
    // Create the widgets.

    _codecWidget = new QComboBox;
    _codecWidget->addItems(djvQuickTimePlugin::codecLabels());
    _codecWidget->setSizePolicy(
        QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    _qualityWidget = new QComboBox;
    _qualityWidget->addItems(djvQuickTimePlugin::qualityLabels());
    _qualityWidget->setSizePolicy(
        QSizePolicy::Fixed, QSizePolicy::Fixed);

    // Layout.

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setSpacing(djvStyle::global()->sizeMetric().largeSpacing);

    djvPrefsGroupBox * prefsGroupBox = new djvPrefsGroupBox(
        "Codec",
        "Set the codec used when saving QuickTime movies.");
    QFormLayout * formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_codecWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(
        "Quality",
        "Set the quality used when saving QuickTime movies.");
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_qualityWidget);
    layout->addWidget(prefsGroupBox);

    layout->addStretch();

    // Initialize.

    /*QStringList tmp;
    tmp = _plugin->option(
        _plugin->options()[djvQuickTimePlugin::CODEC_OPTION]);
    tmp >> _options.codec;
    tmp = _plugin->option(
        _plugin->options()[djvQuickTimePlugin::QUALITY_OPTION]);
    tmp >> _options.quality;*/

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

djvQuickTimeWidget::~djvQuickTimeWidget()
{}

void djvQuickTimeWidget::resetPreferences()
{
    _options = djvQuickTimePlugin::Options();
    
    pluginUpdate();
    widgetUpdate();
}

void djvQuickTimeWidget::pluginCallback(const QString & option)
{
    try
    {
        QStringList tmp;
        tmp = _plugin->option(option);

        if (0 == option.compare(_plugin->options()[
            djvQuickTimePlugin::CODEC_OPTION], Qt::CaseInsensitive))
                tmp >> _options.codec;
        else if (0 == option.compare(_plugin->options()[
            djvQuickTimePlugin::QUALITY_OPTION], Qt::CaseInsensitive))
                tmp >> _options.quality;
    }
    catch (const QString &)
    {}

    widgetUpdate();
}

void djvQuickTimeWidget::codecCallback(int in)
{
    _options.codec = static_cast<djvQuickTimePlugin::CODEC>(in);

    pluginUpdate();
}

void djvQuickTimeWidget::qualityCallback(int in)
{
    _options.quality = static_cast<djvQuickTimePlugin::QUALITY>(in);

    pluginUpdate();
}

void djvQuickTimeWidget::pluginUpdate()
{
    QStringList tmp;
    tmp << _options.codec;
    _plugin->setOption(
        _plugin->options()[djvQuickTimePlugin::CODEC_OPTION], tmp);
    tmp << _options.quality;
    _plugin->setOption(
        _plugin->options()[djvQuickTimePlugin::QUALITY_OPTION], tmp);
}

void djvQuickTimeWidget::widgetUpdate()
{
    djvSignalBlocker signalBlocker(QObjectList() <<
        _codecWidget <<
        _qualityWidget);
    
    _codecWidget->setCurrentIndex(_options.codec);
    
    _qualityWidget->setCurrentIndex(_options.quality);
}
