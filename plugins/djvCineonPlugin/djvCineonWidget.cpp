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

//! \file djvCineonWidget.cpp

#include <djvCineonWidget.h>

#include <djvFloatEditSlider.h>
#include <djvIntEditSlider.h>
#include <djvPrefsGroupBox.h>
#include <djvStyle.h>

#include <djvSignalBlocker.h>

#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvCineonWidget
//------------------------------------------------------------------------------

djvCineonWidget::djvCineonWidget(djvCineonPlugin * plugin) :
    djvAbstractPrefsWidget(djvCineon::staticName),
    _plugin                  (plugin),
    _colorProfileLayout      (0),
    _inputColorProfileWidget (0),
    _inputBlackPointWidget   (0),
    _inputWhitePointWidget   (0),
    _inputGammaWidget        (0),
    _inputSoftClipWidget     (0),
    _outputColorProfileWidget(0),
    _outputBlackPointWidget  (0),
    _outputWhitePointWidget  (0),
    _outputGammaWidget       (0)
{
    //DJV_DEBUG("djvCineonWidget::djvCineonWidget");

    // Create the widgets.

    _inputColorProfileWidget = new QComboBox;
    _inputColorProfileWidget->addItems(djvCineon::colorProfileLabels());
    _inputColorProfileWidget->setSizePolicy(
        QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    _inputBlackPointWidget = new djvIntEditSlider;
    _inputBlackPointWidget->setRange(0, 1023);

    _inputWhitePointWidget = new djvIntEditSlider;
    _inputWhitePointWidget->setRange(0, 1023);

    _inputGammaWidget = new djvFloatEditSlider;
    _inputGammaWidget->setRange(0.01, 4.0);

    _inputSoftClipWidget = new djvIntEditSlider;
    _inputSoftClipWidget->setRange(0, 50);
    
    _outputColorProfileWidget = new QComboBox;
    _outputColorProfileWidget->addItems(djvCineon::colorProfileLabels());
    _outputColorProfileWidget->setSizePolicy(
        QSizePolicy::Fixed, QSizePolicy::Fixed);

    _outputBlackPointWidget = new djvIntEditSlider;
    _outputBlackPointWidget->setRange(0, 1023);

    _outputWhitePointWidget = new djvIntEditSlider;
    _outputWhitePointWidget->setRange(0, 1023);

    _outputGammaWidget = new djvFloatEditSlider;
    _outputGammaWidget->setRange(0.01, 4.0);

    // Layout the widgets.

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setSpacing(djvStyle::global()->sizeMetric().largeSpacing);

    djvPrefsGroupBox * prefsGroupBox = new djvPrefsGroupBox(
        "Color Profile",
        "Set the color profile used when loading and saving Cineon images.");
    _colorProfileLayout = prefsGroupBox->createLayout();
    _colorProfileLayout->addRow("Input profile:", _inputColorProfileWidget);
    _colorProfileLayout->addRow("Black:", _inputBlackPointWidget);
    _colorProfileLayout->addRow("White:", _inputWhitePointWidget);
    _colorProfileLayout->addRow("Gamma:", _inputGammaWidget);
    _colorProfileLayout->addRow("Soft clip:", _inputSoftClipWidget);
    _colorProfileLayout->addRow("Output profile:", _outputColorProfileWidget);
    _colorProfileLayout->addRow("Black:", _outputBlackPointWidget);
    _colorProfileLayout->addRow("White:", _outputWhitePointWidget);
    _colorProfileLayout->addRow("Gamma:", _outputGammaWidget);
    layout->addWidget(prefsGroupBox);

    layout->addStretch();

    // Initialize.

    _inputBlackPointWidget->setDefaultValue(
        djvCineonPlugin::Options().inputFilmPrint.black);
    _inputWhitePointWidget->setDefaultValue(
        djvCineonPlugin::Options().inputFilmPrint.white);
    _inputGammaWidget->setDefaultValue(
        djvCineonPlugin::Options().inputFilmPrint.gamma);
    _inputSoftClipWidget->setDefaultValue(
        djvCineonPlugin::Options().inputFilmPrint.softClip);

    _outputBlackPointWidget->setDefaultValue(
        djvCineonPlugin::Options().outputFilmPrint.black);
    _outputWhitePointWidget->setDefaultValue(
        djvCineonPlugin::Options().outputFilmPrint.white);
    _outputGammaWidget->setDefaultValue(
        djvCineonPlugin::Options().outputFilmPrint.gamma);

    QStringList tmp;
    tmp = _plugin->option(
        _plugin->options()[djvCineonPlugin::INPUT_COLOR_PROFILE_OPTION]);
    tmp >> _options.inputColorProfile;
    tmp = _plugin->option(
        _plugin->options()[djvCineonPlugin::INPUT_FILM_PRINT_OPTION]);
    tmp >> _options.inputFilmPrint;
    tmp = _plugin->option(
        _plugin->options()[djvCineonPlugin::OUTPUT_COLOR_PROFILE_OPTION]);
    tmp >> _options.outputColorProfile;
    tmp = _plugin->option(
        _plugin->options()[djvCineonPlugin::OUTPUT_FILM_PRINT_OPTION]);
    tmp >> _options.outputFilmPrint;

    widgetUpdate();

    // Setup the callbacks.
    
    connect(
        plugin,
        SIGNAL(optionChanged(const QString &)),
        SLOT(pluginCallback(const QString &)));
    
    connect(
        _inputColorProfileWidget,
        SIGNAL(activated(int)),
        SLOT(inputColorProfileCallback(int)));

    connect(
        _inputBlackPointWidget,
        SIGNAL(valueChanged(int)),
        SLOT(inputBlackPointCallback(int)));

    connect(
        _inputWhitePointWidget,
        SIGNAL(valueChanged(int)),
        SLOT(inputWhitePointCallback(int)));

    connect(
        _inputGammaWidget,
        SIGNAL(valueChanged(double)),
        SLOT(inputGammaCallback(double)));

    connect(
        _inputSoftClipWidget,
        SIGNAL(valueChanged(int)),
        SLOT(inputSoftClipCallback(int)));

    connect(
        _outputColorProfileWidget,
        SIGNAL(activated(int)),
        SLOT(outputColorProfileCallback(int)));

    connect(
        _outputBlackPointWidget,
        SIGNAL(valueChanged(int)),
        SLOT(outputBlackPointCallback(int)));

    connect(
        _outputWhitePointWidget,
        SIGNAL(valueChanged(int)),
        SLOT(outputWhitePointCallback(int)));

    connect(
        _outputGammaWidget,
        SIGNAL(valueChanged(double)),
        SLOT(outputGammaCallback(double)));
}

djvCineonWidget::~djvCineonWidget()
{
    //DJV_DEBUG("djvCineonWidget::~djvCineonWidget");
}

void djvCineonWidget::resetPreferences()
{
    _options = djvCineonPlugin::Options();
    
    pluginUpdate();
    widgetUpdate();
}

void djvCineonWidget::pluginCallback(const QString & option)
{
    try
    {
        QStringList tmp;
        tmp = _plugin->option(option);

        if (0 == option.compare(_plugin->options()[
            djvCineonPlugin::INPUT_COLOR_PROFILE_OPTION], Qt::CaseInsensitive))
                tmp >> _options.inputColorProfile;
        else if (0 == option.compare(_plugin->options()[
            djvCineonPlugin::INPUT_FILM_PRINT_OPTION], Qt::CaseInsensitive))
                tmp >> _options.inputFilmPrint;
        else if (0 == option.compare(_plugin->options()[
            djvCineonPlugin::OUTPUT_COLOR_PROFILE_OPTION], Qt::CaseInsensitive))
                tmp >> _options.outputColorProfile;
        else if (0 == option.compare(_plugin->options()[
            djvCineonPlugin::OUTPUT_FILM_PRINT_OPTION], Qt::CaseInsensitive))
                tmp >> _options.outputFilmPrint;
    }
    catch (const QString &)
    {}

    widgetUpdate();
}

void djvCineonWidget::inputColorProfileCallback(int in)
{
    _options.inputColorProfile = static_cast<djvCineon::COLOR_PROFILE>(in);

    pluginUpdate();
}

void djvCineonWidget::inputBlackPointCallback(int in)
{
    _options.inputFilmPrint.black = in;

    pluginUpdate();
}

void djvCineonWidget::inputWhitePointCallback(int in)
{
    _options.inputFilmPrint.white = in;

    pluginUpdate();
}

void djvCineonWidget::inputGammaCallback(double in)
{
    _options.inputFilmPrint.gamma = in;

    pluginUpdate();
}

void djvCineonWidget::inputSoftClipCallback(int in)
{
    _options.inputFilmPrint.softClip = in;

    pluginUpdate();
}

void djvCineonWidget::outputColorProfileCallback(int in)
{
    _options.outputColorProfile = static_cast<djvCineon::COLOR_PROFILE>(in);

    pluginUpdate();
}

void djvCineonWidget::outputBlackPointCallback(int in)
{
    _options.outputFilmPrint.black = in;

    pluginUpdate();
}

void djvCineonWidget::outputWhitePointCallback(int in)
{
    _options.outputFilmPrint.white = in;

    pluginUpdate();
}

void djvCineonWidget::outputGammaCallback(double in)
{
    _options.outputFilmPrint.gamma = in;

    pluginUpdate();
}

void djvCineonWidget::pluginUpdate()
{
    //DJV_DEBUG("djvCineonWidget::pluginUpdate");

    QStringList tmp;
    tmp << _options.inputColorProfile;
    _plugin->setOption(
        _plugin->options()[djvCineonPlugin::INPUT_COLOR_PROFILE_OPTION], tmp);
    tmp << _options.inputFilmPrint;
    _plugin->setOption(
        _plugin->options()[djvCineonPlugin::INPUT_FILM_PRINT_OPTION], tmp);
    tmp << _options.outputColorProfile;
    _plugin->setOption(
        _plugin->options()[djvCineonPlugin::OUTPUT_COLOR_PROFILE_OPTION], tmp);
    tmp << _options.outputFilmPrint;
    _plugin->setOption(
        _plugin->options()[djvCineonPlugin::OUTPUT_FILM_PRINT_OPTION], tmp);
}

void djvCineonWidget::widgetUpdate()
{
    //DJV_DEBUG("djvCineonWidget::widgetUpdate");

    djvSignalBlocker signalBlocker(QObjectList() <<
        _inputColorProfileWidget <<
        _inputBlackPointWidget <<
        _inputWhitePointWidget <<
        _inputGammaWidget <<
        _inputSoftClipWidget <<
        _outputColorProfileWidget <<
        _outputBlackPointWidget <<
        _outputWhitePointWidget <<
        _outputGammaWidget);
    
    _inputBlackPointWidget->setVisible(
        djvCineon::COLOR_PROFILE_AUTO == _options.inputColorProfile ||
        djvCineon::COLOR_PROFILE_FILM_PRINT == _options.inputColorProfile);
    _colorProfileLayout->labelForField(_inputBlackPointWidget)->setVisible(
        djvCineon::COLOR_PROFILE_AUTO == _options.inputColorProfile ||
        djvCineon::COLOR_PROFILE_FILM_PRINT == _options.inputColorProfile);
    _inputWhitePointWidget->setVisible(
        djvCineon::COLOR_PROFILE_AUTO == _options.inputColorProfile ||
        djvCineon::COLOR_PROFILE_FILM_PRINT == _options.inputColorProfile);
    _colorProfileLayout->labelForField(_inputWhitePointWidget)->setVisible(
        djvCineon::COLOR_PROFILE_AUTO == _options.inputColorProfile ||
        djvCineon::COLOR_PROFILE_FILM_PRINT == _options.inputColorProfile);
    _inputGammaWidget->setVisible(
        djvCineon::COLOR_PROFILE_AUTO == _options.inputColorProfile ||
        djvCineon::COLOR_PROFILE_FILM_PRINT == _options.inputColorProfile);
    _colorProfileLayout->labelForField(_inputGammaWidget)->setVisible(
        djvCineon::COLOR_PROFILE_AUTO == _options.inputColorProfile ||
        djvCineon::COLOR_PROFILE_FILM_PRINT == _options.inputColorProfile);
    _inputSoftClipWidget->setVisible(
        djvCineon::COLOR_PROFILE_AUTO == _options.inputColorProfile ||
        djvCineon::COLOR_PROFILE_FILM_PRINT == _options.inputColorProfile);
    _colorProfileLayout->labelForField(_inputSoftClipWidget)->setVisible(
        djvCineon::COLOR_PROFILE_AUTO == _options.inputColorProfile ||
        djvCineon::COLOR_PROFILE_FILM_PRINT == _options.inputColorProfile);

    _outputBlackPointWidget->setVisible(
        djvCineon::COLOR_PROFILE_AUTO == _options.outputColorProfile ||
        djvCineon::COLOR_PROFILE_FILM_PRINT == _options.outputColorProfile);
    _colorProfileLayout->labelForField(_outputBlackPointWidget)->setVisible(
        djvCineon::COLOR_PROFILE_AUTO == _options.outputColorProfile ||
        djvCineon::COLOR_PROFILE_FILM_PRINT == _options.outputColorProfile);
    _outputWhitePointWidget->setVisible(
        djvCineon::COLOR_PROFILE_AUTO == _options.outputColorProfile ||
        djvCineon::COLOR_PROFILE_FILM_PRINT == _options.outputColorProfile);
    _colorProfileLayout->labelForField(_outputWhitePointWidget)->setVisible(
        djvCineon::COLOR_PROFILE_AUTO == _options.outputColorProfile ||
        djvCineon::COLOR_PROFILE_FILM_PRINT == _options.outputColorProfile);
    _outputGammaWidget->setVisible(
        djvCineon::COLOR_PROFILE_AUTO == _options.outputColorProfile ||
        djvCineon::COLOR_PROFILE_FILM_PRINT == _options.outputColorProfile);
    _colorProfileLayout->labelForField(_outputGammaWidget)->setVisible(
        djvCineon::COLOR_PROFILE_AUTO == _options.outputColorProfile ||
        djvCineon::COLOR_PROFILE_FILM_PRINT == _options.outputColorProfile);

    _inputColorProfileWidget->setCurrentIndex(_options.inputColorProfile);
    _inputBlackPointWidget->setValue(_options.inputFilmPrint.black);
    _inputWhitePointWidget->setValue(_options.inputFilmPrint.white);
    _inputGammaWidget->setValue(_options.inputFilmPrint.gamma);
    _inputSoftClipWidget->setValue(_options.inputFilmPrint.softClip);

    _outputColorProfileWidget->setCurrentIndex(_options.outputColorProfile);
    _outputBlackPointWidget->setValue(_options.outputFilmPrint.black);
    _outputWhitePointWidget->setValue(_options.outputFilmPrint.white);
    _outputGammaWidget->setValue(_options.outputFilmPrint.gamma);
}
