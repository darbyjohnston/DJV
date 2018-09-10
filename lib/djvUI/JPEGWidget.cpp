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

#include <djvUI/JPEGWidget.h>

#include <djvUI/UIContext.h>
#include <djvUI/IntEditSlider.h>
#include <djvUI/PrefsGroupBox.h>
#include <djvUI/Style.h>

#include <djvGraphics/ImageIO.h>

#include <djvCore/SignalBlocker.h>

#include <QApplication>
#include <QFormLayout>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvJPEGWidget
//------------------------------------------------------------------------------

djvJPEGWidget::djvJPEGWidget(djvImageIO * plugin, djvUIContext * context) :
    djvImageIOWidget(plugin, context),
    _qualityWidget(0)
{
    // Create the widgets.
    _qualityWidget = new djvIntEditSlider(context);
    _qualityWidget->setRange(0, 100);
    _qualityWidget->setDefaultValue(djvJPEG::Options().quality);

    // Layout the widgets.
    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setSpacing(context->style()->sizeMetric().largeSpacing);

    djvPrefsGroupBox * prefsGroupBox = new djvPrefsGroupBox(
        qApp->translate("djvJPEGWidget", "Quality"),
        qApp->translate("djvJPEGWidget", "Set the quality used when saving JPEG images."),
        context);
    QFormLayout * formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(
        qApp->translate("djvJPEGWidget", "Quality:"),
        _qualityWidget);
    layout->addWidget(prefsGroupBox);

    layout->addStretch();

    // Initialize.
    QStringList tmp;
    tmp = plugin->option(
        plugin->options()[djvJPEG::QUALITY_OPTION]);
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

djvJPEGWidget::~djvJPEGWidget()
{}

void djvJPEGWidget::resetPreferences()
{
    _options = djvJPEG::Options();
    pluginUpdate();
    widgetUpdate();
}

void djvJPEGWidget::pluginCallback(const QString & option)
{
    try
    {
        QStringList tmp;
        tmp = plugin()->option(option);
        if (0 == option.compare(plugin()->options()[
            djvJPEG::QUALITY_OPTION], Qt::CaseInsensitive))
                tmp >> _options.quality;
    }
    catch (const QString &)
    {
    }
    widgetUpdate();
}

void djvJPEGWidget::qualityCallback(int in)
{
    _options.quality = in;
    pluginUpdate();
}

void djvJPEGWidget::pluginUpdate()
{
    QStringList tmp;
    tmp << _options.quality;
    plugin()->setOption(
        djvJPEG::optionsLabels()[djvJPEG::QUALITY_OPTION], tmp);
}

void djvJPEGWidget::widgetUpdate()
{
    djvSignalBlocker signalBlocker(_qualityWidget);
    _qualityWidget->setValue(_options.quality);
}

//------------------------------------------------------------------------------
// djvJPEGWidgetPlugin
//------------------------------------------------------------------------------

djvJPEGWidgetPlugin::djvJPEGWidgetPlugin(djvCoreContext * context) :
    djvImageIOWidgetPlugin(context)
{}

djvImageIOWidget * djvJPEGWidgetPlugin::createWidget(djvImageIO * plugin) const
{
    return new djvJPEGWidget(plugin, uiContext());
}

QString djvJPEGWidgetPlugin::pluginName() const
{
    return djvJPEG::staticName;
}

