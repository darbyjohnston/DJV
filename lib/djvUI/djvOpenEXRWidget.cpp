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

#include <djvOpenEXRWidget.h>

#include <djvUIContext.h>
#include <djvFloatEditSlider.h>
#include <djvFloatObject.h>
#include <djvIntEdit.h>
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
    return new djvOpenEXRWidgetPlugin(context);
}

} // extern "C"

//------------------------------------------------------------------------------
// djvOpenEXRWidget
//------------------------------------------------------------------------------

djvOpenEXRWidget::djvOpenEXRWidget(djvImageIO * plugin, djvUIContext * context) :
    djvImageIOWidget(plugin, context),
    _threadsEnableWidget        (0),
    _threadCountWidget          (0),
    _inputColorProfileWidget    (0),
    _inputColorProfileLayout    (0),
    _inputGammaWidget           (0),
    _inputExposureWidget        (0),
    _inputExposureDefogWidget   (0),
    _inputExposureKneeLowWidget (0),
    _inputExposureKneeHighWidget(0),
    _channelsWidget             (0),
    _compressionWidget          (0)
#if OPENEXR_VERSION_HEX >= 0x02020000
    ,
    _dwaCompressionLevelWidget  (0)
#endif // OPENEXR_VERSION_HEX
{
    //DJV_DEBUG("djvOpenEXRWidget::djvOpenEXRWidget");

    // Create the thread widgets.
    _threadsEnableWidget = new QCheckBox(
        qApp->translate("djvOpenEXRWidget", "Enable multi-threading"));

    _threadCountWidget = new djvIntEdit;
    _threadCountWidget->setRange(0, 1024);
    _threadCountWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // Create the widgets.
    _inputColorProfileWidget = new QComboBox;
    _inputColorProfileWidget->addItems(djvOpenEXR::colorProfileLabels());
    _inputColorProfileWidget->setSizePolicy(
        QSizePolicy::Fixed, QSizePolicy::Fixed);

    _inputGammaWidget = new djvFloatEditSlider(context);
    _inputGammaWidget->setRange(.1f, 4.f);

    _inputExposureWidget = new djvFloatEditSlider(context);
    _inputExposureWidget->setRange(-10.f, 10.f);

    _inputExposureDefogWidget = new djvFloatEditSlider(context);
    _inputExposureDefogWidget->setRange(0.f, .01f);

    _inputExposureKneeLowWidget = new djvFloatEditSlider(context);
    _inputExposureKneeLowWidget->setRange(-3.f, 3.f);

    _inputExposureKneeHighWidget = new djvFloatEditSlider(context);
    _inputExposureKneeHighWidget->setRange(3.5f, 7.5f);
    
    _channelsWidget = new QComboBox;
    _channelsWidget->addItems(djvOpenEXR::channelsLabels());
    _channelsWidget->setSizePolicy(
        QSizePolicy::Fixed, QSizePolicy::Fixed);

    _compressionWidget = new QComboBox;
    _compressionWidget->addItems(djvOpenEXR::compressionLabels());
    _compressionWidget->setSizePolicy(
        QSizePolicy::Fixed, QSizePolicy::Fixed);

#if OPENEXR_VERSION_HEX >= 0x02020000
    _dwaCompressionLevelWidget = new djvFloatEditSlider(context);
    _dwaCompressionLevelWidget->editObject()->setClamp(false);
    _dwaCompressionLevelWidget->sliderObject()->setRange(0.f, 200.f);
#endif // OPENEXR_VERSION_HEX

    // Layout the widgets.
    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setSpacing(context->style()->sizeMetric().largeSpacing);

    djvPrefsGroupBox * prefsGroupBox = new djvPrefsGroupBox(
        qApp->translate("djvOpenEXRWidget", "Multi-Threading"), context);
    QFormLayout * formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_threadsEnableWidget);
    formLayout->addRow(
        qApp->translate("djvOpenEXRWidget", "Thread count:"),
        _threadCountWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(
        qApp->translate("djvOpenEXRWidget", "Color Profile"),
        qApp->translate("djvOpenEXRWidget",
        "Set the color profile used when loading OpenEXR images."),
        context);
    _inputColorProfileLayout = prefsGroupBox->createLayout();
    _inputColorProfileLayout->addRow(
        qApp->translate("djvOpenEXRWidget", "Profile:"), _inputColorProfileWidget);
    _inputColorProfileLayout->addRow(
        qApp->translate("djvOpenEXRWidget", "Gamma:"), _inputGammaWidget);
    _inputColorProfileLayout->addRow(
        qApp->translate("djvOpenEXRWidget", "Exposure:"), _inputExposureWidget);
    _inputColorProfileLayout->addRow(
        qApp->translate("djvOpenEXRWidget", "Defog:"), _inputExposureDefogWidget);
    _inputColorProfileLayout->addRow(
        qApp->translate("djvOpenEXRWidget", "Knee low:"), _inputExposureKneeLowWidget);
    _inputColorProfileLayout->addRow(
        qApp->translate("djvOpenEXRWidget", "Knee high:"), _inputExposureKneeHighWidget);
    formLayout->addRow(prefsGroupBox);
    
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(
        qApp->translate("djvOpenEXRWidget", "Channels"),
        qApp->translate("djvOpenEXRWidget",
        "Set how channels are grouped when loading OpenEXR images."),
        context);
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(
        qApp->translate("djvOpenEXRWidget", "Channels:"),
        _channelsWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(
        qApp->translate("djvOpenEXRWidget", "Compression"),
        qApp->translate("djvOpenEXRWidget",
        "Set the file compression used when saving OpenEXR images."),
        context);
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(
        qApp->translate("djvOpenEXRWidget", "Compression:"),
        _compressionWidget);
#if OPENEXR_VERSION_HEX >= 0x02020000
    formLayout->addRow(
        qApp->translate("djvOpenEXRWidget", "DWA compression level:"),
        _dwaCompressionLevelWidget);
#endif // OPENEXR_VERSION_HEX
    layout->addWidget(prefsGroupBox);

    layout->addStretch();

    // Initialize.
    _inputExposureWidget->setInc(.1f, 1.f);
    _inputExposureDefogWidget->setInc(.0001f, .001f);
    _inputExposureKneeLowWidget->setInc(.1f, 1.f);
    _inputExposureKneeHighWidget->setInc(.1f, 1.f);

    _inputGammaWidget->setDefaultValue(
        djvOpenEXR::Options().inputGamma);
    _inputExposureWidget->setDefaultValue(
        djvOpenEXR::Options().inputExposure.value);
    _inputExposureDefogWidget->setDefaultValue(
        djvOpenEXR::Options().inputExposure.defog);
    _inputExposureKneeLowWidget->setDefaultValue(
        djvOpenEXR::Options().inputExposure.kneeLow);
    _inputExposureKneeHighWidget->setDefaultValue(
        djvOpenEXR::Options().inputExposure.kneeHigh);

    QStringList tmp;
    tmp = plugin->option(
        plugin->options()[djvOpenEXR::THREADS_ENABLE_OPTION]);
    tmp >> _options.threadsEnable;
    tmp = plugin->option(
        plugin->options()[djvOpenEXR::THREAD_COUNT_OPTION]);
    tmp >> _options.threadCount;
    tmp = plugin->option(
        plugin->options()[djvOpenEXR::INPUT_COLOR_PROFILE_OPTION]);
    tmp >> _options.inputColorProfile;
    tmp = plugin->option(
        plugin->options()[djvOpenEXR::INPUT_GAMMA_OPTION]);
    tmp >> _options.inputGamma;
    tmp = plugin->option(
        plugin->options()[djvOpenEXR::INPUT_EXPOSURE_OPTION]);
    tmp >> _options.inputExposure;
    tmp = plugin->option(
        plugin->options()[djvOpenEXR::CHANNELS_OPTION]);
    tmp >> _options.channels;
    tmp = plugin->option(
        plugin->options()[djvOpenEXR::COMPRESSION_OPTION]);
    tmp >> _options.compression;
#if OPENEXR_VERSION_HEX >= 0x02020000
    tmp = plugin->option(
        plugin->options()[djvOpenEXR::DWA_COMPRESSION_LEVEL_OPTION]);
    tmp >> _options.dwaCompressionLevel;
#endif // OPENEXR_VERSION_HEX

    widgetUpdate();

    // Setup the callbacks.
    connect(
        plugin,
        SIGNAL(optionChanged(const QString &)),
        SLOT(pluginCallback(const QString &)));
    connect(
        _threadsEnableWidget,
        SIGNAL(toggled(bool)),
        SLOT(threadsEnableCallback(bool)));
    connect(
        _threadCountWidget,
        SIGNAL(valueChanged(int)),
        SLOT(threadCountCallback(int)));
    connect(
        _inputColorProfileWidget,
        SIGNAL(activated(int)),
        SLOT(inputColorProfileCallback(int)));
    connect(
        _inputGammaWidget,
        SIGNAL(valueChanged(float)),
        SLOT(inputGammaCallback(float)));
    connect(
        _inputExposureWidget,
        SIGNAL(valueChanged(float)),
        SLOT(inputExposureCallback(float)));
    connect(
        _inputExposureDefogWidget,
        SIGNAL(valueChanged(float)),
        SLOT(inputExposureDefogCallback(float)));
    connect(
        _inputExposureKneeLowWidget,
        SIGNAL(valueChanged(float)),
        SLOT(inputExposureKneeLowCallback(float)));
    connect(
        _inputExposureKneeHighWidget,
        SIGNAL(valueChanged(float)),
        SLOT(inputExposureKneeHighCallback(float)));
    connect(
        _channelsWidget,
        SIGNAL(activated(int)),
        SLOT(channelsCallback(int)));
    connect(
        _compressionWidget,
        SIGNAL(activated(int)),
        SLOT(compressionCallback(int)));
#if OPENEXR_VERSION_HEX >= 0x02020000
    connect(
        _dwaCompressionLevelWidget,
        SIGNAL(valueChanged(float)),
        SLOT(dwaCompressionLevelCallback(float)));
#endif // OPENEXR_VERSION_HEX
}

djvOpenEXRWidget::~djvOpenEXRWidget()
{}

void djvOpenEXRWidget::resetPreferences()
{
    _options = djvOpenEXR::Options();
    pluginUpdate();
}

void djvOpenEXRWidget::pluginCallback(const QString & option)
{
    try
    {
        QStringList tmp;
        tmp = plugin()->option(option);
        if (0 == option.compare(plugin()->options()[
            djvOpenEXR::THREADS_ENABLE_OPTION], Qt::CaseInsensitive))
                tmp >> _options.threadsEnable;
        else if (0 == option.compare(plugin()->options()[
            djvOpenEXR::THREAD_COUNT_OPTION], Qt::CaseInsensitive))
                tmp >> _options.threadCount;
        else if (0 == option.compare(plugin()->options()[
            djvOpenEXR::INPUT_COLOR_PROFILE_OPTION], Qt::CaseInsensitive))
                tmp >> _options.inputColorProfile;
        else if (0 == option.compare(plugin()->options()[
            djvOpenEXR::INPUT_GAMMA_OPTION], Qt::CaseInsensitive))
                tmp >> _options.inputGamma;
        else if (0 == option.compare(plugin()->options()[
            djvOpenEXR::INPUT_EXPOSURE_OPTION], Qt::CaseInsensitive))
                tmp >> _options.inputExposure;
        else if (0 == option.compare(plugin()->options()[
            djvOpenEXR::CHANNELS_OPTION], Qt::CaseInsensitive))
                tmp >> _options.channels;
        else if (0 == option.compare(plugin()->options()[
            djvOpenEXR::COMPRESSION_OPTION], Qt::CaseInsensitive))
                tmp >> _options.compression;
#if OPENEXR_VERSION_HEX >= 0x02020000
        else if (0 == option.compare(plugin()->options()[
            djvOpenEXR::DWA_COMPRESSION_LEVEL_OPTION], Qt::CaseInsensitive))
                tmp >> _options.dwaCompressionLevel;
#endif // OPENEXR_VERSION_HEX
    }
    catch (const QString &)
    {
    }
    widgetUpdate();
}

void djvOpenEXRWidget::threadsEnableCallback(bool in)
{
    _options.threadsEnable = in;
    pluginUpdate();
}

void djvOpenEXRWidget::threadCountCallback(int in)
{
    _options.threadCount = in;
    pluginUpdate();
}

void djvOpenEXRWidget::inputColorProfileCallback(int in)
{
    //DJV_DEBUG("djvOpenEXRWidget::inputColorProfileCallback()");
    //DJV_DEBUG_PRINT("in = " << in);

    _options.inputColorProfile =
        static_cast<djvOpenEXR::COLOR_PROFILE>(in);

    pluginUpdate();
}

void djvOpenEXRWidget::inputGammaCallback(float in)
{
    _options.inputGamma = in;
    pluginUpdate();
}

void djvOpenEXRWidget::inputExposureCallback(float in)
{
    _options.inputExposure.value = in;
    pluginUpdate();
}

void djvOpenEXRWidget::inputExposureDefogCallback(float in)
{
    _options.inputExposure.defog = in;
    pluginUpdate();
}

void djvOpenEXRWidget::inputExposureKneeLowCallback(float in)
{
    _options.inputExposure.kneeLow = in;
    pluginUpdate();
}

void djvOpenEXRWidget::inputExposureKneeHighCallback(float in)
{
    _options.inputExposure.kneeHigh = in;
    pluginUpdate();
}

void djvOpenEXRWidget::channelsCallback(int in)
{
    _options.channels = static_cast<djvOpenEXR::CHANNELS>(in);
    pluginUpdate();
}

void djvOpenEXRWidget::compressionCallback(int in)
{
    _options.compression = static_cast<djvOpenEXR::COMPRESSION>(in);
    pluginUpdate();
}

void djvOpenEXRWidget::dwaCompressionLevelCallback(float in)
{
#if OPENEXR_VERSION_HEX >= 0x02020000
    _options.dwaCompressionLevel = in;
    pluginUpdate();
#endif // OPENEXR_VERSION_HEX
}

void djvOpenEXRWidget::pluginUpdate()
{
    QStringList tmp;
    tmp << _options.threadsEnable;
    plugin()->setOption(plugin()->options()[
        djvOpenEXR::THREADS_ENABLE_OPTION], tmp);
    tmp << _options.threadCount;
    plugin()->setOption(plugin()->options()[
        djvOpenEXR::THREAD_COUNT_OPTION], tmp);
    tmp << _options.inputColorProfile;
    plugin()->setOption(plugin()->options()[
        djvOpenEXR::INPUT_COLOR_PROFILE_OPTION], tmp);
    tmp << _options.inputGamma;
    plugin()->setOption(plugin()->options()[
        djvOpenEXR::INPUT_GAMMA_OPTION], tmp);
    tmp << _options.inputExposure;
    plugin()->setOption(plugin()->options()[
        djvOpenEXR::INPUT_EXPOSURE_OPTION], tmp);
    tmp << _options.channels;
    plugin()->setOption(plugin()->options()[
        djvOpenEXR::CHANNELS_OPTION], tmp);
    tmp << _options.compression;
    plugin()->setOption(plugin()->options()[
        djvOpenEXR::COMPRESSION_OPTION], tmp);
#if OPENEXR_VERSION_HEX >= 0x02020000
    tmp << _options.dwaCompressionLevel;
    plugin()->setOption(plugin()->options()[
        djvOpenEXR::DWA_COMPRESSION_LEVEL_OPTION], tmp);
#endif // OPENEXR_VERSION_HEX
}

void djvOpenEXRWidget::widgetUpdate()
{
    //DJV_DEBUG("djvOpenEXRWidget::widgetUpdate()");
    djvSignalBlocker signalBlocker(QObjectList() <<
        _threadsEnableWidget <<
        _threadCountWidget <<
        _inputColorProfileWidget <<
        _inputGammaWidget <<
        _inputExposureWidget <<
        _inputExposureDefogWidget <<
        _inputExposureKneeLowWidget <<
        _inputExposureKneeHighWidget <<
        _channelsWidget <<
        _compressionWidget
#if OPENEXR_VERSION_HEX >= 0x02020000
        <<
        _dwaCompressionLevelWidget
#endif // OPENEXR_VERSION_HEX
        );
    _inputGammaWidget->setVisible(
        djvOpenEXR::COLOR_PROFILE_GAMMA == _options.inputColorProfile);
    _inputColorProfileLayout->labelForField(_inputGammaWidget)->setVisible(
        djvOpenEXR::COLOR_PROFILE_GAMMA == _options.inputColorProfile);
    _inputExposureWidget->setVisible(
        djvOpenEXR::COLOR_PROFILE_EXPOSURE == _options.inputColorProfile);
    _inputColorProfileLayout->labelForField(_inputExposureWidget)->setVisible(
        djvOpenEXR::COLOR_PROFILE_EXPOSURE == _options.inputColorProfile);
    _inputExposureDefogWidget->setVisible(
        djvOpenEXR::COLOR_PROFILE_EXPOSURE == _options.inputColorProfile);
    _inputColorProfileLayout->labelForField(_inputExposureDefogWidget)->setVisible(
        djvOpenEXR::COLOR_PROFILE_EXPOSURE == _options.inputColorProfile);
    _inputExposureKneeLowWidget->setVisible(
        djvOpenEXR::COLOR_PROFILE_EXPOSURE == _options.inputColorProfile);
    _inputColorProfileLayout->labelForField(_inputExposureKneeLowWidget)->setVisible(
        djvOpenEXR::COLOR_PROFILE_EXPOSURE == _options.inputColorProfile);
    _inputExposureKneeHighWidget->setVisible(
        djvOpenEXR::COLOR_PROFILE_EXPOSURE == _options.inputColorProfile);
    _inputColorProfileLayout->labelForField(_inputExposureKneeHighWidget)->setVisible(
        djvOpenEXR::COLOR_PROFILE_EXPOSURE == _options.inputColorProfile);
    _threadsEnableWidget->setChecked(_options.threadsEnable);
    _threadCountWidget->setValue(_options.threadCount);
    _inputColorProfileWidget->setCurrentIndex(_options.inputColorProfile);
    _inputGammaWidget->setValue(_options.inputGamma);
    _inputExposureWidget->setValue(_options.inputExposure.value);
    _inputExposureDefogWidget->setValue(_options.inputExposure.defog);
    _inputExposureKneeLowWidget->setValue(_options.inputExposure.kneeLow);
    _inputExposureKneeHighWidget->setValue(_options.inputExposure.kneeHigh);
    _channelsWidget->setCurrentIndex(_options.channels);
#if OPENEXR_VERSION_HEX >= 0x02020000
    _compressionWidget->setCurrentIndex(_options.compression);
    _dwaCompressionLevelWidget->setValue(_options.dwaCompressionLevel);
#endif // OPENEXR_VERSION_HEX
}

//------------------------------------------------------------------------------
// djvOpenEXRWidgetPlugin
//------------------------------------------------------------------------------

djvOpenEXRWidgetPlugin::djvOpenEXRWidgetPlugin(djvCoreContext * context) :
    djvImageIOWidgetPlugin(context)
{}

djvImageIOWidget * djvOpenEXRWidgetPlugin::createWidget(djvImageIO * plugin) const
{
    return new djvOpenEXRWidget(plugin, uiContext());
}

QString djvOpenEXRWidgetPlugin::pluginName() const
{
    return djvOpenEXR::staticName;
}


