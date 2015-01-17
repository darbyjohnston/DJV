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

//! \file djvOpenExrWidget.cpp

#include <djvOpenExrWidget.h>

#include <djvFloatEditSlider.h>
#include <djvFloatObject.h>
#include <djvIntEdit.h>
#include <djvPrefsGroupBox.h>
#include <djvStyle.h>

#include <djvSignalBlocker.h>

#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvOpenExrWidget
//------------------------------------------------------------------------------

djvOpenExrWidget::djvOpenExrWidget(djvOpenExrPlugin * plugin) :
    djvAbstractPrefsWidget(djvOpenExrPlugin::staticName),
    _plugin                     (plugin),
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
    //DJV_DEBUG("djvOpenExrWidget::djvOpenExrWidget");

    // Create the thread widgets.

    _threadsEnableWidget = new QCheckBox("Enable");

    _threadCountWidget = new djvIntEdit;
    _threadCountWidget->setRange(0, 1024);
    _threadCountWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // Create the widgets.
    
    _inputColorProfileWidget = new QComboBox;
    _inputColorProfileWidget->addItems(djvOpenExrPlugin::colorProfileLabels());
    _inputColorProfileWidget->setSizePolicy(
        QSizePolicy::Fixed, QSizePolicy::Fixed);

    _inputGammaWidget = new djvFloatEditSlider;
    _inputGammaWidget->setRange(0.1, 4.0);

    _inputExposureWidget = new djvFloatEditSlider;
    _inputExposureWidget->setRange(-10.0, 10.0);

    _inputExposureDefogWidget = new djvFloatEditSlider;
    _inputExposureDefogWidget->setRange(0.0, 0.01);

    _inputExposureKneeLowWidget = new djvFloatEditSlider;
    _inputExposureKneeLowWidget->setRange(-3.0, 3.0);

    _inputExposureKneeHighWidget = new djvFloatEditSlider;
    _inputExposureKneeHighWidget->setRange(3.5, 7.5);
    
    _channelsWidget = new QComboBox;
    _channelsWidget->addItems(djvOpenExrPlugin::channelsLabels());
    _channelsWidget->setSizePolicy(
        QSizePolicy::Fixed, QSizePolicy::Fixed);

    _compressionWidget = new QComboBox;
    _compressionWidget->addItems(djvOpenExrPlugin::compressionLabels());
    _compressionWidget->setSizePolicy(
        QSizePolicy::Fixed, QSizePolicy::Fixed);

#if OPENEXR_VERSION_HEX >= 0x02020000
    _dwaCompressionLevelWidget = new djvFloatEditSlider;
    _dwaCompressionLevelWidget->editObject()->setClamp(false);
    _dwaCompressionLevelWidget->sliderObject()->setRange(0.0, 200.0);
#endif // OPENEXR_VERSION_HEX

    // Layout the widgets.

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setSpacing(djvStyle::global()->sizeMetric().largeSpacing);

    djvPrefsGroupBox * prefsGroupBox = new djvPrefsGroupBox("Multithreading");
    QFormLayout * formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_threadsEnableWidget);
    formLayout->addRow("Thread count:", _threadCountWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(
        "Color Profile",
        "Set the color profile used when loading OpenEXR images.");
    _inputColorProfileLayout = prefsGroupBox->createLayout();
    _inputColorProfileLayout->addRow("Color profile:", _inputColorProfileWidget);
    _inputColorProfileLayout->addRow("Gamma:", _inputGammaWidget);
    _inputColorProfileLayout->addRow("Exposure:", _inputExposureWidget);
    _inputColorProfileLayout->addRow("Defog:", _inputExposureDefogWidget);
    _inputColorProfileLayout->addRow("Knee low:", _inputExposureKneeLowWidget);
    _inputColorProfileLayout->addRow("Knee high:", _inputExposureKneeHighWidget);
    formLayout->addRow(prefsGroupBox);
    
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(
        "Channels",
        "Set how channels are grouped when loading OpenEXR images.");
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_channelsWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(
        "Compression",
        "Set the file compression used when saving OpenEXR images.");
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_compressionWidget);
#if OPENEXR_VERSION_HEX >= 0x02020000
    formLayout->addRow("DWA compression level:", _dwaCompressionLevelWidget);
#endif // OPENEXR_VERSION_HEX
    layout->addWidget(prefsGroupBox);

    layout->addStretch();

    // Initialize.

    _inputExposureWidget->setInc(0.1, 1.0);
    _inputExposureDefogWidget->setInc(0.0001, 0.001);
    _inputExposureKneeLowWidget->setInc(0.1, 1.0);
    _inputExposureKneeHighWidget->setInc(0.1, 1.0);

    _inputGammaWidget->setDefaultValue(
        djvOpenExrPlugin::Options().inputGamma);
    _inputExposureWidget->setDefaultValue(
        djvOpenExrPlugin::Options().inputExposure.value);
    _inputExposureDefogWidget->setDefaultValue(
        djvOpenExrPlugin::Options().inputExposure.defog);
    _inputExposureKneeLowWidget->setDefaultValue(
        djvOpenExrPlugin::Options().inputExposure.kneeLow);
    _inputExposureKneeHighWidget->setDefaultValue(
        djvOpenExrPlugin::Options().inputExposure.kneeHigh);

    QStringList tmp;
    tmp = _plugin->option(
        _plugin->options()[djvOpenExrPlugin::THREADS_ENABLE_OPTION]);
    tmp >> _options.threadsEnable;
    tmp = _plugin->option(
        _plugin->options()[djvOpenExrPlugin::THREAD_COUNT_OPTION]);
    tmp >> _options.threadCount;
    tmp = _plugin->option(
        _plugin->options()[djvOpenExrPlugin::INPUT_COLOR_PROFILE_OPTION]);
    tmp >> _options.inputColorProfile;
    tmp = _plugin->option(
        _plugin->options()[djvOpenExrPlugin::INPUT_GAMMA_OPTION]);
    tmp >> _options.inputGamma;
    tmp = _plugin->option(
        _plugin->options()[djvOpenExrPlugin::INPUT_EXPOSURE_OPTION]);
    tmp >> _options.inputExposure;
    tmp = _plugin->option(
        _plugin->options()[djvOpenExrPlugin::CHANNELS_OPTION]);
    tmp >> _options.channels;
    tmp = _plugin->option(
        _plugin->options()[djvOpenExrPlugin::COMPRESSION_OPTION]);
    tmp >> _options.compression;
#if OPENEXR_VERSION_HEX >= 0x02020000
    tmp = _plugin->option(
        _plugin->options()[djvOpenExrPlugin::DWA_COMPRESSION_LEVEL_OPTION]);
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
        SIGNAL(valueChanged(double)),
        SLOT(inputGammaCallback(double)));

    connect(
        _inputExposureWidget,
        SIGNAL(valueChanged(double)),
        SLOT(inputExposureCallback(double)));

    connect(
        _inputExposureDefogWidget,
        SIGNAL(valueChanged(double)),
        SLOT(inputExposureDefogCallback(double)));

    connect(
        _inputExposureKneeLowWidget,
        SIGNAL(valueChanged(double)),
        SLOT(inputExposureKneeLowCallback(double)));

    connect(
        _inputExposureKneeHighWidget,
        SIGNAL(valueChanged(double)),
        SLOT(inputExposureKneeHighCallback(double)));

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
        SIGNAL(valueChanged(double)),
        SLOT(dwaCompressionLevelCallback(double)));
#endif // OPENEXR_VERSION_HEX
}

djvOpenExrWidget::~djvOpenExrWidget()
{}

void djvOpenExrWidget::resetPreferences()
{
    _options = djvOpenExrPlugin::Options();
    
    pluginUpdate();
}

void djvOpenExrWidget::pluginCallback(const QString & option)
{
    try
    {
        QStringList tmp;
        tmp = _plugin->option(option);

        if (0 == option.compare(_plugin->options()[
            djvOpenExrPlugin::THREADS_ENABLE_OPTION], Qt::CaseInsensitive))
                tmp >> _options.threadsEnable;
        else if (0 == option.compare(_plugin->options()[
            djvOpenExrPlugin::THREAD_COUNT_OPTION], Qt::CaseInsensitive))
                tmp >> _options.threadCount;
        else if (0 == option.compare(_plugin->options()[
            djvOpenExrPlugin::INPUT_COLOR_PROFILE_OPTION], Qt::CaseInsensitive))
                tmp >> _options.inputColorProfile;
        else if (0 == option.compare(_plugin->options()[
            djvOpenExrPlugin::INPUT_GAMMA_OPTION], Qt::CaseInsensitive))
                tmp >> _options.inputGamma;
        else if (0 == option.compare(_plugin->options()[
            djvOpenExrPlugin::INPUT_EXPOSURE_OPTION], Qt::CaseInsensitive))
                tmp >> _options.inputExposure;
        else if (0 == option.compare(_plugin->options()[
            djvOpenExrPlugin::CHANNELS_OPTION], Qt::CaseInsensitive))
                tmp >> _options.channels;
        else if (0 == option.compare(_plugin->options()[
            djvOpenExrPlugin::COMPRESSION_OPTION], Qt::CaseInsensitive))
                tmp >> _options.compression;
#if OPENEXR_VERSION_HEX >= 0x02020000
        else if (0 == option.compare(_plugin->options()[
            djvOpenExrPlugin::DWA_COMPRESSION_LEVEL_OPTION], Qt::CaseInsensitive))
                tmp >> _options.dwaCompressionLevel;
#endif // OPENEXR_VERSION_HEX
    }
    catch (const QString &)
    {
    }

    widgetUpdate();
}

void djvOpenExrWidget::threadsEnableCallback(bool in)
{
    _options.threadsEnable = in;

    pluginUpdate();
}

void djvOpenExrWidget::threadCountCallback(int in)
{
    _options.threadCount = in;

    pluginUpdate();
}

void djvOpenExrWidget::inputColorProfileCallback(int in)
{
    //DJV_DEBUG("djvOpenExrWidget::inputColorProfileCallback()");
    //DJV_DEBUG_PRINT("in = " << in);

    _options.inputColorProfile =
        static_cast<djvOpenExrPlugin::COLOR_PROFILE>(in);

    pluginUpdate();
}

void djvOpenExrWidget::inputGammaCallback(double in)
{
    _options.inputGamma = in;

    pluginUpdate();
}

void djvOpenExrWidget::inputExposureCallback(double in)
{
    _options.inputExposure.value = in;

    pluginUpdate();
}

void djvOpenExrWidget::inputExposureDefogCallback(double in)
{
    _options.inputExposure.defog = in;
    
    pluginUpdate();
}

void djvOpenExrWidget::inputExposureKneeLowCallback(double in)
{
    _options.inputExposure.kneeLow = in;
    
    pluginUpdate();
}

void djvOpenExrWidget::inputExposureKneeHighCallback(double in)
{
    _options.inputExposure.kneeHigh = in;
    
    pluginUpdate();
}

void djvOpenExrWidget::channelsCallback(int in)
{
    _options.channels = static_cast<djvOpenExrPlugin::CHANNELS>(in);

    pluginUpdate();
}

void djvOpenExrWidget::compressionCallback(int in)
{
    _options.compression = static_cast<djvOpenExrPlugin::COMPRESSION>(in);

    pluginUpdate();
}

void djvOpenExrWidget::dwaCompressionLevelCallback(double in)
{
#if OPENEXR_VERSION_HEX >= 0x02020000
    _options.dwaCompressionLevel = in;

    pluginUpdate();
#endif // OPENEXR_VERSION_HEX
}

void djvOpenExrWidget::pluginUpdate()
{
    QStringList tmp;

    tmp << _options.threadsEnable;
    _plugin->setOption(_plugin->options()[
        djvOpenExrPlugin::THREADS_ENABLE_OPTION], tmp);
    tmp << _options.threadCount;
    _plugin->setOption(_plugin->options()[
        djvOpenExrPlugin::THREAD_COUNT_OPTION], tmp);
    tmp << _options.inputColorProfile;
    _plugin->setOption(_plugin->options()[
        djvOpenExrPlugin::INPUT_COLOR_PROFILE_OPTION], tmp);
    tmp << _options.inputGamma;
    _plugin->setOption(_plugin->options()[
        djvOpenExrPlugin::INPUT_GAMMA_OPTION], tmp);
    tmp << _options.inputExposure;
    _plugin->setOption(_plugin->options()[
        djvOpenExrPlugin::INPUT_EXPOSURE_OPTION], tmp);
    tmp << _options.channels;
    _plugin->setOption(_plugin->options()[
        djvOpenExrPlugin::CHANNELS_OPTION], tmp);
    tmp << _options.compression;
    _plugin->setOption(_plugin->options()[
        djvOpenExrPlugin::COMPRESSION_OPTION], tmp);
#if OPENEXR_VERSION_HEX >= 0x02020000
    tmp << _options.dwaCompressionLevel;
    _plugin->setOption(_plugin->options()[
        djvOpenExrPlugin::DWA_COMPRESSION_LEVEL_OPTION], tmp);
#endif // OPENEXR_VERSION_HEX
}

void djvOpenExrWidget::widgetUpdate()
{
    //DJV_DEBUG("djvOpenExrWidget::widgetUpdate()");
    
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
        djvOpenExrPlugin::COLOR_PROFILE_GAMMA == _options.inputColorProfile);
    _inputColorProfileLayout->labelForField(_inputGammaWidget)->setVisible(
        djvOpenExrPlugin::COLOR_PROFILE_GAMMA == _options.inputColorProfile);
    _inputExposureWidget->setVisible(
        djvOpenExrPlugin::COLOR_PROFILE_EXPOSURE == _options.inputColorProfile);
    _inputColorProfileLayout->labelForField(_inputExposureWidget)->setVisible(
        djvOpenExrPlugin::COLOR_PROFILE_EXPOSURE == _options.inputColorProfile);
    _inputExposureDefogWidget->setVisible(
        djvOpenExrPlugin::COLOR_PROFILE_EXPOSURE == _options.inputColorProfile);
    _inputColorProfileLayout->labelForField(_inputExposureDefogWidget)->setVisible(
        djvOpenExrPlugin::COLOR_PROFILE_EXPOSURE == _options.inputColorProfile);
    _inputExposureKneeLowWidget->setVisible(
        djvOpenExrPlugin::COLOR_PROFILE_EXPOSURE == _options.inputColorProfile);
    _inputColorProfileLayout->labelForField(_inputExposureKneeLowWidget)->setVisible(
        djvOpenExrPlugin::COLOR_PROFILE_EXPOSURE == _options.inputColorProfile);
    _inputExposureKneeHighWidget->setVisible(
        djvOpenExrPlugin::COLOR_PROFILE_EXPOSURE == _options.inputColorProfile);
    _inputColorProfileLayout->labelForField(_inputExposureKneeHighWidget)->setVisible(
        djvOpenExrPlugin::COLOR_PROFILE_EXPOSURE == _options.inputColorProfile);
    
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

