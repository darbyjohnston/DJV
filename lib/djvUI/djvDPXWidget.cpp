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

#include <djvDPXWidget.h>

#include <djvFloatEditSlider.h>
#include <djvUIContext.h>
#include <djvIntEditSlider.h>
#include <djvPrefsGroupBox.h>
#include <djvStyle.h>

#include <djvImageIO.h>

#include <djvSignalBlocker.h>

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QVBoxLayout>

extern "C"
{

DJV_PLUGIN_EXPORT djvPlugin * djvImageIOWidgetEntry(djvCoreContext * context)
{
    return new djvDPXWidgetPlugin(context);
}

} // extern "C"

//------------------------------------------------------------------------------
// djvDPXWidget
//------------------------------------------------------------------------------

djvDPXWidget::djvDPXWidget(djvImageIO * plugin, djvUIContext * context) :
    djvImageIOWidget(plugin, context),
    _inputColorProfileLayout (0),
    _inputColorProfileWidget (0),
    _inputBlackPointWidget   (0),
    _inputWhitePointWidget   (0),
    _inputGammaWidget        (0),
    _inputSoftClipWidget     (0),
    _outputColorProfileLayout(0),
    _outputColorProfileWidget(0),
    _outputBlackPointWidget  (0),
    _outputWhitePointWidget  (0),
    _outputGammaWidget       (0),
    _versionWidget           (0),
    _typeWidget              (0),
    _endianWidget            (0)
{
    //DJV_DEBUG("djvDPXWidget::djvDPXWidget");

    // Create the widgets.
    _inputColorProfileWidget = new QComboBox;
    _inputColorProfileWidget->addItems(djvCineon::colorProfileLabels());
    _inputColorProfileWidget->setSizePolicy(
        QSizePolicy::Fixed, QSizePolicy::Fixed);

    _inputBlackPointWidget = new djvIntEditSlider(context);
    _inputBlackPointWidget->setRange(0, 1023);

    _inputWhitePointWidget = new djvIntEditSlider(context);
    _inputWhitePointWidget->setRange(0, 1023);

    _inputGammaWidget = new djvFloatEditSlider(context);
    _inputGammaWidget->setRange(.01f, 4.f);

    _inputSoftClipWidget = new djvIntEditSlider(context);
    _inputSoftClipWidget->setRange(0, 50);

    _outputColorProfileWidget = new QComboBox;
    _outputColorProfileWidget->addItems(djvCineon::colorProfileLabels());
    _outputColorProfileWidget->setSizePolicy(
        QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    _outputBlackPointWidget = new djvIntEditSlider(context);
    _outputBlackPointWidget->setRange(0, 1023);

    _outputWhitePointWidget = new djvIntEditSlider(context);
    _outputWhitePointWidget->setRange(0, 1023);

    _outputGammaWidget = new djvFloatEditSlider(context);
    _outputGammaWidget->setRange(.01f, 4.f);

    _versionWidget = new QComboBox;
    _versionWidget->addItems(djvDPX::versionLabels());
    _versionWidget->setSizePolicy(
        QSizePolicy::Fixed, QSizePolicy::Fixed);

    _typeWidget = new QComboBox;
    _typeWidget->addItems(djvDPX::typeLabels());
    _typeWidget->setSizePolicy(
        QSizePolicy::Fixed, QSizePolicy::Fixed);

    _endianWidget = new QComboBox;
    _endianWidget->addItems(djvDPX::endianLabels());
    _endianWidget->setSizePolicy(
        QSizePolicy::Fixed, QSizePolicy::Fixed);

    // Layout the widgets.
    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setSpacing(context->style()->sizeMetric().largeSpacing);

    djvPrefsGroupBox * prefsGroupBox = new djvPrefsGroupBox(
        qApp->translate("djvDPXWidget", "Input Color Profile"),
        qApp->translate("djvDPXWidget", "Set the color profile used when loading DPX images."),
        context);
    _inputColorProfileLayout = prefsGroupBox->createLayout();
    _inputColorProfileLayout->addRow(
        qApp->translate("djvDPXWidget", "Profile:"),
        _inputColorProfileWidget);
    _inputColorProfileLayout->addRow(
        qApp->translate("djvDPXWidget", "Black:"),
        _inputBlackPointWidget);
    _inputColorProfileLayout->addRow(
        qApp->translate("djvDPXWidget", "White:"),
        _inputWhitePointWidget);
    _inputColorProfileLayout->addRow(
        qApp->translate("djvDPXWidget", "Gamma:"),
        _inputGammaWidget);
    _inputColorProfileLayout->addRow(
        qApp->translate("djvDPXWidget", "Soft clip:"),
        _inputSoftClipWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(
        qApp->translate("djvDPXWidget", "Output Color Profile"),
        qApp->translate("djvDPXWidget", "Set the color profile used when saving DPX images."),
        context);
    _outputColorProfileLayout = prefsGroupBox->createLayout();
    _outputColorProfileLayout->addRow(
        qApp->translate("djvDPXWidget", "Profile:"),
        _outputColorProfileWidget);
    _outputColorProfileLayout->addRow(
        qApp->translate("djvDPXWidget", "Black:"),
        _outputBlackPointWidget);
    _outputColorProfileLayout->addRow(
        qApp->translate("djvDPXWidget", "White:"),
        _outputWhitePointWidget);
    _outputColorProfileLayout->addRow(
        qApp->translate("djvDPXWidget", "Gamma:"),
        _outputGammaWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(
        qApp->translate("djvDPXWidget", "Version"),
        qApp->translate("djvDPXWidget", "Set the file version used when saving DPX images."),
        context);
    QFormLayout * formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(
        qApp->translate("djvDPXWidget", "Version:"),
        _versionWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(
        qApp->translate("djvDPXWidget", "Pixel Type"),
        qApp->translate("djvDPXWidget", "Set the pixel type used when saving DPX images."),
        context);
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(
        qApp->translate("djvDPXWidget", "Pixel type:"),
        _typeWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(
        qApp->translate("djvDPXWidget", "Endian"),
        qApp->translate("djvDPXWidget",
        "Set the endian used when saving DPX images. Setting the endian to "
        "\"Auto\" will use the endian of the current hardware."),
        context);
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(
        qApp->translate("djvDPXWidget", "Endian:"),
        _endianWidget);
    layout->addWidget(prefsGroupBox);

    layout->addStretch();

    // Initialize.
    _inputBlackPointWidget->setDefaultValue(
        djvDPX::Options().inputFilmPrint.black);
    _inputWhitePointWidget->setDefaultValue(
        djvDPX::Options().inputFilmPrint.white);
    _inputGammaWidget->setDefaultValue(
        djvDPX::Options().inputFilmPrint.gamma);
    _inputSoftClipWidget->setDefaultValue(
        djvDPX::Options().inputFilmPrint.softClip);

    _outputBlackPointWidget->setDefaultValue(
        djvDPX::Options().outputFilmPrint.black);
    _outputWhitePointWidget->setDefaultValue(
        djvDPX::Options().outputFilmPrint.white);
    _outputGammaWidget->setDefaultValue(
        djvDPX::Options().outputFilmPrint.gamma);

    QStringList tmp;
    tmp = plugin->option(
        plugin->options()[djvDPX::INPUT_COLOR_PROFILE_OPTION]);
    tmp >> _options.inputColorProfile;
    tmp = plugin->option(
        plugin->options()[djvDPX::INPUT_FILM_PRINT_OPTION]);
    tmp >> _options.inputFilmPrint;
    tmp = plugin->option(
        plugin->options()[djvDPX::OUTPUT_COLOR_PROFILE_OPTION]);
    tmp >> _options.outputColorProfile;
    tmp = plugin->option(
        plugin->options()[djvDPX::OUTPUT_FILM_PRINT_OPTION]);
    tmp >> _options.outputFilmPrint;
    tmp = plugin->option(
        plugin->options()[djvDPX::VERSION_OPTION]);
    tmp >> _options.version;
    tmp = plugin->option(
        plugin->options()[djvDPX::TYPE_OPTION]);
    tmp >> _options.type;
    tmp = plugin->option(
        plugin->options()[djvDPX::ENDIAN_OPTION]);
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
        SIGNAL(valueChanged(float)),
        SLOT(inputGammaCallback(float)));
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
        SIGNAL(valueChanged(float)),
        SLOT(outputGammaCallback(float)));
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

djvDPXWidget::~djvDPXWidget()
{}

void djvDPXWidget::resetPreferences()
{
    //DJV_DEBUG("djvDPXWidget::resetPreferences");
    _options = djvDPX::Options();
    //DJV_DEBUG_PRINT("input color profile = " << _options.inputColorProfile);
    //DJV_DEBUG_PRINT("output color profile = " << _options.outputColorProfile);

    pluginUpdate();
    widgetUpdate();
}

void djvDPXWidget::pluginCallback(const QString & option)
{
    //DJV_DEBUG("djvDPXWidget::pluginCallback");
    //DJV_DEBUG_PRINT("option = " << option);
    try
    {
        QStringList tmp;
        tmp = plugin()->option(option);
        if (0 == option.compare(plugin()->options()[
            djvDPX::INPUT_COLOR_PROFILE_OPTION], Qt::CaseInsensitive))
                tmp >> _options.inputColorProfile;
        else if (0 == option.compare(plugin()->options()[
            djvDPX::INPUT_FILM_PRINT_OPTION], Qt::CaseInsensitive))
                tmp >> _options.inputFilmPrint;
        else if (0 == option.compare(plugin()->options()[
            djvDPX::OUTPUT_COLOR_PROFILE_OPTION], Qt::CaseInsensitive))
                tmp >> _options.outputColorProfile;
        else if (0 == option.compare(plugin()->options()[
            djvDPX::OUTPUT_FILM_PRINT_OPTION], Qt::CaseInsensitive))
                tmp >> _options.outputFilmPrint;
        else if (0 == option.compare(plugin()->options()[
            djvDPX::VERSION_OPTION], Qt::CaseInsensitive))
                tmp >> _options.version;
        else if (0 == option.compare(plugin()->options()[
            djvDPX::TYPE_OPTION], Qt::CaseInsensitive))
                tmp >> _options.type;
        else if (0 == option.compare(plugin()->options()[
            djvDPX::ENDIAN_OPTION], Qt::CaseInsensitive))
                tmp >> _options.endian;
    }
    catch (const QString &)
    {}
    widgetUpdate();
}

void djvDPXWidget::inputColorProfileCallback(int in)
{
    _options.inputColorProfile = static_cast<djvCineon::COLOR_PROFILE>(in);
    pluginUpdate();
}

void djvDPXWidget::inputBlackPointCallback(int in)
{
    _options.inputFilmPrint.black = in;
    pluginUpdate();
}

void djvDPXWidget::inputWhitePointCallback(int in)
{
    _options.inputFilmPrint.white = in;
    pluginUpdate();
}

void djvDPXWidget::inputGammaCallback(float in)
{
    _options.inputFilmPrint.gamma = in;
    pluginUpdate();
}

void djvDPXWidget::inputSoftClipCallback(int in)
{
    _options.inputFilmPrint.softClip = in;
    pluginUpdate();
}

void djvDPXWidget::outputColorProfileCallback(int in)
{
    _options.outputColorProfile = static_cast<djvCineon::COLOR_PROFILE>(in);
    pluginUpdate();
}

void djvDPXWidget::outputBlackPointCallback(int in)
{
    _options.outputFilmPrint.black = in;
    pluginUpdate();
}

void djvDPXWidget::outputWhitePointCallback(int in)
{
    _options.outputFilmPrint.white = in;
    pluginUpdate();
}

void djvDPXWidget::outputGammaCallback(float in)
{
    _options.outputFilmPrint.gamma = in;
    pluginUpdate();
}

void djvDPXWidget::versionCallback(int in)
{
    _options.version = static_cast<djvDPX::VERSION>(in);
    pluginUpdate();
}

void djvDPXWidget::typeCallback(int in)
{
    _options.type = static_cast<djvDPX::TYPE>(in);
    pluginUpdate();
}

void djvDPXWidget::endianCallback(int in)
{
    _options.endian = static_cast<djvDPX::ENDIAN>(in);
    pluginUpdate();
}

void djvDPXWidget::pluginUpdate()
{
    QStringList tmp;
    tmp << _options.inputColorProfile;
    plugin()->setOption(plugin()->options()[djvDPX::INPUT_COLOR_PROFILE_OPTION], tmp);
    tmp << _options.inputFilmPrint;
    plugin()->setOption(plugin()->options()[djvDPX::INPUT_FILM_PRINT_OPTION], tmp);
    tmp << _options.outputColorProfile;
    plugin()->setOption(plugin()->options()[djvDPX::OUTPUT_COLOR_PROFILE_OPTION], tmp);
    tmp << _options.outputFilmPrint;
    plugin()->setOption(plugin()->options()[djvDPX::OUTPUT_FILM_PRINT_OPTION], tmp);
    tmp << _options.version;
    plugin()->setOption(plugin()->options()[djvDPX::VERSION_OPTION], tmp);
    tmp << _options.type;
    plugin()->setOption(plugin()->options()[djvDPX::TYPE_OPTION], tmp);
    tmp << _options.endian;
    plugin()->setOption(plugin()->options()[djvDPX::ENDIAN_OPTION], tmp);
}

void djvDPXWidget::widgetUpdate()
{
    //DJV_DEBUG("djvDPXWidget::widgetUpdate");
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
    _inputColorProfileLayout->labelForField(_inputBlackPointWidget)->setVisible(
        djvCineon::COLOR_PROFILE_AUTO == _options.inputColorProfile ||
        djvCineon::COLOR_PROFILE_FILM_PRINT == _options.inputColorProfile);
    _inputWhitePointWidget->setVisible(
        djvCineon::COLOR_PROFILE_AUTO == _options.inputColorProfile ||
        djvCineon::COLOR_PROFILE_FILM_PRINT == _options.inputColorProfile);
    _inputColorProfileLayout->labelForField(_inputWhitePointWidget)->setVisible(
        djvCineon::COLOR_PROFILE_AUTO == _options.inputColorProfile ||
        djvCineon::COLOR_PROFILE_FILM_PRINT == _options.inputColorProfile);
    _inputGammaWidget->setVisible(
        djvCineon::COLOR_PROFILE_AUTO == _options.inputColorProfile ||
        djvCineon::COLOR_PROFILE_FILM_PRINT == _options.inputColorProfile);
    _inputColorProfileLayout->labelForField(_inputGammaWidget)->setVisible(
        djvCineon::COLOR_PROFILE_AUTO == _options.inputColorProfile ||
        djvCineon::COLOR_PROFILE_FILM_PRINT == _options.inputColorProfile);
    _inputSoftClipWidget->setVisible(
        djvCineon::COLOR_PROFILE_AUTO == _options.inputColorProfile ||
        djvCineon::COLOR_PROFILE_FILM_PRINT == _options.inputColorProfile);
    _inputColorProfileLayout->labelForField(_inputSoftClipWidget)->setVisible(
        djvCineon::COLOR_PROFILE_AUTO == _options.inputColorProfile ||
        djvCineon::COLOR_PROFILE_FILM_PRINT == _options.inputColorProfile);
    _outputBlackPointWidget->setVisible(
        djvCineon::COLOR_PROFILE_AUTO == _options.outputColorProfile ||
        djvCineon::COLOR_PROFILE_FILM_PRINT == _options.outputColorProfile);
    _outputColorProfileLayout->labelForField(_outputBlackPointWidget)->setVisible(
        djvCineon::COLOR_PROFILE_AUTO == _options.outputColorProfile ||
        djvCineon::COLOR_PROFILE_FILM_PRINT == _options.outputColorProfile);
    _outputWhitePointWidget->setVisible(
        djvCineon::COLOR_PROFILE_AUTO == _options.outputColorProfile ||
        djvCineon::COLOR_PROFILE_FILM_PRINT == _options.outputColorProfile);
    _outputColorProfileLayout->labelForField(_outputWhitePointWidget)->setVisible(
        djvCineon::COLOR_PROFILE_AUTO == _options.outputColorProfile ||
        djvCineon::COLOR_PROFILE_FILM_PRINT == _options.outputColorProfile);
    _outputGammaWidget->setVisible(
        djvCineon::COLOR_PROFILE_AUTO == _options.outputColorProfile ||
        djvCineon::COLOR_PROFILE_FILM_PRINT == _options.outputColorProfile);
    _outputColorProfileLayout->labelForField(_outputGammaWidget)->setVisible(
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

//------------------------------------------------------------------------------
// djvDPXWidgetPlugin
//------------------------------------------------------------------------------

djvDPXWidgetPlugin::djvDPXWidgetPlugin(djvCoreContext * context) :
    djvImageIOWidgetPlugin(context)
{}

djvImageIOWidget * djvDPXWidgetPlugin::createWidget(djvImageIO * plugin) const
{
    return new djvDPXWidget(plugin, uiContext());
}

QString djvDPXWidgetPlugin::pluginName() const
{
    return djvDPX::staticName;
}

