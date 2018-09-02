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

#include <djvTiffWidget.h>

#include <djvGuiContext.h>
#include <djvPrefsGroupBox.h>
#include <djvStyle.h>

#include <djvImageIo.h>
#include <djvSignalBlocker.h>

#include <QApplication>
#include <QComboBox>
#include <QFormLayout>
#include <QVBoxLayout>

extern "C"
{

DJV_PLUGIN_EXPORT djvPlugin * djvImageIoWidgetEntry(djvCoreContext * context)
{
    return new djvTiffWidgetPlugin(context);
}

} // extern "C"

//------------------------------------------------------------------------------
// djvTiffWidget
//------------------------------------------------------------------------------

djvTiffWidget::djvTiffWidget(djvImageIo * plugin, djvGuiContext * context) :
    djvImageIoWidget(plugin, context),
    _compressionWidget(0)
{
    //DJV_DEBUG("djvTiffWidget::djvTiffWidget");

    // Create the widgets.
    _compressionWidget = new QComboBox;
    _compressionWidget->addItems(djvTiff::compressionLabels());
    _compressionWidget->setSizePolicy(
        QSizePolicy::Fixed, QSizePolicy::Fixed);

    // Layout the widgets.
    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setSpacing(context->style()->sizeMetric().largeSpacing);

    djvPrefsGroupBox * prefsGroupBox = new djvPrefsGroupBox(
        qApp->translate("djvTiffWidget", "Compression"),
        qApp->translate("djvTiffWidget", "Set the file compression used when saving TIFF images."),
        context);
    QFormLayout * formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(
        qApp->translate("djvTiffWidget", "Compression:"),
        _compressionWidget);
    layout->addWidget(prefsGroupBox);

    layout->addStretch();

    // Initialize.
    QStringList tmp;
    tmp = plugin->option(
        plugin->options()[djvTiff::COMPRESSION_OPTION]);
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

djvTiffWidget::~djvTiffWidget()
{}

void djvTiffWidget::resetPreferences()
{
    _options = djvTiff::Options();
    pluginUpdate();
    widgetUpdate();
}

void djvTiffWidget::pluginCallback(const QString & option)
{
    try
    {
        QStringList tmp;
        tmp = plugin()->option(option);
        if (0 == option.compare(plugin()->options()[
            djvTiff::COMPRESSION_OPTION], Qt::CaseInsensitive))
                tmp >> _options.compression;
    }
    catch (const QString &)
    {}
    widgetUpdate();
}

void djvTiffWidget::compressionCallback(int in)
{
    _options.compression = static_cast<djvTiff::COMPRESSION>(in);
    pluginUpdate();
}

void djvTiffWidget::pluginUpdate()
{
    QStringList tmp;
    tmp << _options.compression;
    plugin()->setOption(
        plugin()->options()[djvTiff::COMPRESSION_OPTION], tmp);
}

void djvTiffWidget::widgetUpdate()
{
    djvSignalBlocker signalBlocker(_compressionWidget);
    _compressionWidget->setCurrentIndex(_options.compression);
}

//------------------------------------------------------------------------------
// djvTiffWidgetPlugin
//------------------------------------------------------------------------------

djvTiffWidgetPlugin::djvTiffWidgetPlugin(djvCoreContext * context) :
    djvImageIoWidgetPlugin(context)
{}

djvImageIoWidget * djvTiffWidgetPlugin::createWidget(djvImageIo * plugin) const
{
    return new djvTiffWidget(plugin, guiContext());
}

QString djvTiffWidgetPlugin::pluginName() const
{
    return djvTiff::staticName;
}

