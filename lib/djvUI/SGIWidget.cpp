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

#include <djvUI/SGIWidget.h>

#include <djvUI/UIContext.h>
#include <djvUI/PrefsGroupBox.h>
#include <djvUI/Style.h>

#include <djvCore/SignalBlocker.h>

#include <QApplication>
#include <QComboBox>
#include <QFormLayout>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvSGIWidget
//------------------------------------------------------------------------------

djvSGIWidget::djvSGIWidget(djvImageIO * plugin, djvUIContext * context) :
    djvImageIOWidget(plugin, context),
    _compressionWidget(0)
{
    // Create the output widgets.    
    _compressionWidget = new QComboBox;
    _compressionWidget->addItems(djvSGI::compressionLabels());
    _compressionWidget->setSizePolicy(
        QSizePolicy::Fixed, QSizePolicy::Fixed);

    // Layout the widgets.
    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setSpacing(context->style()->sizeMetric().largeSpacing);

    djvPrefsGroupBox * prefsGroupBox = new djvPrefsGroupBox(
        qApp->translate("djvSGIWidget", "Compression"),
        qApp->translate("djvSGIWidget", "Set the file compression used when saving SGI images."),
        context);
    QFormLayout * formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(
        qApp->translate("djvSGIWidget", "Compression:"),
        _compressionWidget);
    layout->addWidget(prefsGroupBox);

    layout->addStretch();

    // Initialize.
    QStringList tmp;
    tmp = plugin->option(
        plugin->options()[djvSGI::COMPRESSION_OPTION]);
    tmp >> _options.compression;

    widgetUpdate();

    // Setup the callbacks.
    connect(
        plugin,
        SIGNAL(optionChanged(const QString &)),
        SLOT(pluginCallback(const QString &)));
    connect(
        _compressionWidget,
        SIGNAL(activated(int)),
        SLOT(compressionCallback(int)));
}

djvSGIWidget::~djvSGIWidget()
{}

void djvSGIWidget::resetPreferences()
{
    _options = djvSGI::Options();
    pluginUpdate();
    widgetUpdate();
}

void djvSGIWidget::pluginCallback(const QString & option)
{
    try
    {
        QStringList tmp;
        tmp = plugin()->option(option);
        if (0 == option.compare(plugin()->options()[
            djvSGI::COMPRESSION_OPTION], Qt::CaseInsensitive))
                tmp >> _options.compression;
    }
    catch (const QString &)
    {}
    widgetUpdate();
}

void djvSGIWidget::compressionCallback(int in)
{
    _options.compression = static_cast<djvSGI::COMPRESSION>(in);
    pluginUpdate();
}

void djvSGIWidget::pluginUpdate()
{
    QStringList tmp;
    tmp << _options.compression;
    plugin()->setOption(
        plugin()->options()[djvSGI::COMPRESSION_OPTION], tmp);
}

void djvSGIWidget::widgetUpdate()
{
    djvSignalBlocker signalBlocker(_compressionWidget);
    _compressionWidget->setCurrentIndex(_options.compression);
}

//------------------------------------------------------------------------------
// djvSGIWidgetPlugin
//------------------------------------------------------------------------------

djvSGIWidgetPlugin::djvSGIWidgetPlugin(djvCoreContext * context) :
    djvImageIOWidgetPlugin(context)
{}

djvImageIOWidget * djvSGIWidgetPlugin::createWidget(djvImageIO * plugin) const
{
    return new djvSGIWidget(plugin, uiContext());
}

QString djvSGIWidgetPlugin::pluginName() const
{
    return djvSGI::staticName;
}
