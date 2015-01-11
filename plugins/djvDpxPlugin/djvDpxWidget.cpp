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

//! \file djvDpxWidget.cpp

#include <djvDpxWidget.h>

#include <djvFloatEditSlider.h>
#include <djvIntEditSlider.h>
#include <djvPrefsGroupBox.h>
#include <djvStyle.h>

#include <djvSignalBlocker.h>

#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvDpxWidget
//------------------------------------------------------------------------------

djvDpxWidget::djvDpxWidget(djvDpxPlugin * plugin) :
    djvAbstractPrefsWidget(djvDpxPlugin::staticName),
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
    _outputGammaWidget       (0),
    _versionWidget           (0),
    _typeWidget              (0),
    _endianWidget            (0)
{
    //DJV_DEBUG("djvDpxWidget::djvDpxWidget");

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

    _versionWidget = new QComboBox;
    _versionWidget->addItems(djvDpxPlugin::versionLabels());
    _versionWidget->setSizePolicy(
        QSizePolicy::Fixed, QSizePolicy::Fixed);

    _typeWidget = new QComboBox;
    _typeWidget->addItems(djvDpxPlugin::typeLabels());
    _typeWidget->setSizePolicy(
        QSizePolicy::Fixed, QSizePolicy::Fixed);

    _endianWidget = new QComboBox;
    _endianWidget->addItems(djvDpxPlugin::endianLabels());
    _endianWidget->setSizePolicy(
        QSizePolicy::Fixed, QSizePolicy::Fixed);

    // Layout the widgets.

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setSpacing(djvStyle::global()->sizeMetric().largeSpacing);

    djvPrefsGroupBox * prefsGroupBox = new djvPrefsGroupBox(
        "Color Profile",
        "Set the color profile used when loading and saving DPX images.");
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

    prefsGroupBox = new djvPrefsGroupBox(
        "Version",
        "Set the file format version used when saving DPX images.");
    QFormLayout * formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_versionWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(
        "Pixel Type",
        "Set the pixel type used when saving DPX images.");
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_typeWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(
        "Endian",
        "Set the endian used when saving DPX images. Setting the endian to "
        "\"Auto\" will use the endian of the current hardware.");
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_endianWidget);
    layout->addWidget(prefsGroupBox);

    layout->addStretch();

    // Initialize.

    _inputBlackPointWidget->setDefaultValue(
        djvDpxPlugin::Options().inputFilmPrint.black);
    _inputWhitePointWidget->setDefaultValue(
        djvDpxPlugin::Options().inputFilmPrint.white);
    _inputGammaWidget->setDefaultValue(
        djvDpxPlugin::Options().inputFilmPrint.gamma);
    _inputSoftClipWidget->setDefaultValue(
        djvDpxPlugin::Options().inputFilmPrint.softClip);

    _outputBlackPointWidget->setDefaultValue(
        djvDpxPlugin::Options().outputFilmPrint.black);
    _outputWhitePointWidget->setDefaultValue(
        djvDpxPlugin::Options().outputFilmPrint.white);
    _outputGammaWidget->setDefaultValue(
        djvDpxPlugin::Options().outputFilmPrint.gamma);

    QStringList tmp;
    tmp = _plugin->option(
        _plugin->options()[djvDpxPlugin::INPUT_COLOR_PROFILE_OPTION]);
    tmp >> _options.inputColorProfile;
    tmp = _plugin->option(
        _plugin->options()[djvDpxPlugin::INPUT_FILM_PRINT_OPTION]);
    tmp >> _options.inputFilmPrint;
    tmp = _plugin->option(
        _plugin->options()[djvDpxPlugin::OUTPUT_COLOR_PROFILE_OPTION]);
    tmp >> _options.outputColorProfile;
    tmp = _plugin->option(
        _plugin->options()[djvDpxPlugin::OUTPUT_FILM_PRINT_OPTION]);
    tmp >> _options.outputFilmPrint;
    tmp = _plugin->option(
        _plugin->options()[djvDpxPlugin::VERSION_OPTION]);
    tmp >> _options.version;
    tmp = _plugin->option(
        _plugin->options()[djvDpxPlugin::TYPE_OPTION]);
    tmp >> _options.type;
    tmp = _plugin->option(
        _plugin->options()[djvDpxPlugin::ENDIAN_OPTION]);
    tmp >> _options.endian;

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

    connect(
        _versionWidget,
        SIGNAL(activated(int)),
        SLOT(versionCallback(int)));

    connect(
        _typeWidget,
        SIGNAL(activated(int)),
        SLOT(typeCallback(int)));

    connect(
        _endianWidget,
        SIGNAL(activated(int)),
        SLOT(endianCallback(int)));
}

djvDpxWidget::~djvDpxWidget()
{}

void djvDpxWidget::resetPreferences()
{
    //DJV_DEBUG("djvDpxWidget::resetPreferences");

    _options = djvDpxPlugin::Options();

    //DJV_DEBUG_PRINT("input color profile = " << _options.inputColorProfile);
    //DJV_DEBUG_PRINT("output color profile = " << _options.outputColorProfile);

    pluginUpdate();
    widgetUpdate();
}

void djvDpxWidget::pluginCallback(const QString & option)
{
    //DJV_DEBUG("djvDpxWidget::pluginCallback");
    //DJV_DEBUG_PRINT("option = " << option);

    try
    {
        QStringList tmp;
        tmp = _plugin->option(option);

        if (0 == option.compare(_plugin->options()[
            djvDpxPlugin::INPUT_COLOR_PROFILE_OPTION], Qt::CaseInsensitive))
                tmp >> _options.inputColorProfile;
        else if (0 == option.compare(_plugin->options()[
            djvDpxPlugin::INPUT_FILM_PRINT_OPTION], Qt::CaseInsensitive))
                tmp >> _options.inputFilmPrint;
        else if (0 == option.compare(_plugin->options()[
            djvDpxPlugin::OUTPUT_COLOR_PROFILE_OPTION], Qt::CaseInsensitive))
                tmp >> _options.outputColorProfile;
        else if (0 == option.compare(_plugin->options()[
            djvDpxPlugin::OUTPUT_FILM_PRINT_OPTION], Qt::CaseInsensitive))
                tmp >> _options.outputFilmPrint;
        else if (0 == option.compare(_plugin->options()[
            djvDpxPlugin::VERSION_OPTION], Qt::CaseInsensitive))
                tmp >> _options.version;
        else if (0 == option.compare(_plugin->options()[
            djvDpxPlugin::TYPE_OPTION], Qt::CaseInsensitive))
                tmp >> _options.type;
        else if (0 == option.compare(_plugin->options()[
            djvDpxPlugin::ENDIAN_OPTION], Qt::CaseInsensitive))
                tmp >> _options.endian;
    }
    catch (const QString &)
    {}

    widgetUpdate();
}

void djvDpxWidget::inputColorProfileCallback(int in)
{
    _options.inputColorProfile = static_cast<djvCineon::COLOR_PROFILE>(in);

    pluginUpdate();
}

void djvDpxWidget::inputBlackPointCallback(int in)
{
    _options.inputFilmPrint.black = in;

    pluginUpdate();
}

void djvDpxWidget::inputWhitePointCallback(int in)
{
    _options.inputFilmPrint.white = in;

    pluginUpdate();
}

void djvDpxWidget::inputGammaCallback(double in)
{
    _options.inputFilmPrint.gamma = in;

    pluginUpdate();
}

void djvDpxWidget::inputSoftClipCallback(int in)
{
    _options.inputFilmPrint.softClip = in;

    pluginUpdate();
}

void djvDpxWidget::outputColorProfileCallback(int in)
{
    _options.outputColorProfile = static_cast<djvCineon::COLOR_PROFILE>(in);

    pluginUpdate();
}

void djvDpxWidget::outputBlackPointCallback(int in)
{
    _options.outputFilmPrint.black = in;

    pluginUpdate();
}

void djvDpxWidget::outputWhitePointCallback(int in)
{
    _options.outputFilmPrint.white = in;

    pluginUpdate();
}

void djvDpxWidget::outputGammaCallback(double in)
{
    _options.outputFilmPrint.gamma = in;

    pluginUpdate();
}

void djvDpxWidget::versionCallback(int in)
{
    _options.version = static_cast<djvDpxPlugin::VERSION>(in);

    pluginUpdate();
}

void djvDpxWidget::typeCallback(int in)
{
    _options.type = static_cast<djvDpxPlugin::TYPE>(in);

    pluginUpdate();
}

void djvDpxWidget::endianCallback(int in)
{
    _options.endian = static_cast<djvDpxPlugin::ENDIAN>(in);

    pluginUpdate();
}

void djvDpxWidget::pluginUpdate()
{
    QStringList tmp;
    tmp << _options.inputColorProfile;
    _plugin->setOption(
        _plugin->options()[djvDpxPlugin::INPUT_COLOR_PROFILE_OPTION], tmp);
    tmp << _options.inputFilmPrint;
    _plugin->setOption(
        _plugin->options()[djvDpxPlugin::INPUT_FILM_PRINT_OPTION], tmp);
    tmp << _options.outputColorProfile;
    _plugin->setOption(
        _plugin->options()[djvDpxPlugin::OUTPUT_COLOR_PROFILE_OPTION], tmp);
    tmp << _options.outputFilmPrint;
    _plugin->setOption(
        _plugin->options()[djvDpxPlugin::OUTPUT_FILM_PRINT_OPTION], tmp);
    tmp << _options.version;
    _plugin->setOption(
        _plugin->options()[djvDpxPlugin::VERSION_OPTION], tmp);
    tmp << _options.type;
    _plugin->setOption(
        _plugin->options()[djvDpxPlugin::TYPE_OPTION], tmp);
    tmp << _options.endian;
    _plugin->setOption(
        _plugin->options()[djvDpxPlugin::ENDIAN_OPTION], tmp);
}

void djvDpxWidget::widgetUpdate()
{
    //DJV_DEBUG("djvDpxWidget::widgetUpdate");

    djvSignalBlocker signalBlocker(QObjectList() <<
        _inputColorProfileWidget <<
        _inputBlackPointWidget <<
        _inputWhitePointWidget <<
        _inputGammaWidget <<
        _inputSoftClipWidget <<
        _outputColorProfileWidget <<
        _outputBlackPointWidget <<
        _outputWhitePointWidget <<
        _outputGammaWidget <<
        _versionWidget <<
        _typeWidget <<
        _endianWidget);

    //DJV_DEBUG_PRINT("input color profile = " << _options.inputColorProfile);
    //DJV_DEBUG_PRINT("output color profile = " << _options.outputColorProfile);
    
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

    _versionWidget->setCurrentIndex(_options.version);

    _typeWidget->setCurrentIndex(_options.type);

    _endianWidget->setCurrentIndex(_options.endian);
}
