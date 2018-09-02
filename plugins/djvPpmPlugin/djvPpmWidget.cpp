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

#include <djvPpmWidget.h>

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
    return new djvPpmWidgetPlugin(context);
}

} // extern "C"

//------------------------------------------------------------------------------
// djvPpmWidget
//------------------------------------------------------------------------------

djvPpmWidget::djvPpmWidget(djvImageIo * plugin, djvGuiContext * context) :
    djvImageIoWidget(plugin, context),
    _typeWidget(0),
    _dataWidget(0)
{
    // Create the widgets.
    _typeWidget = new QComboBox;
    _typeWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _typeWidget->addItems(djvPpm::typeLabels());
    
    _dataWidget = new QComboBox;
    _dataWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _dataWidget->addItems(djvPpm::dataLabels());

    // Layout the widgets.
    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setSpacing(context->style()->sizeMetric().largeSpacing);

    djvPrefsGroupBox * prefsGroupBox = new djvPrefsGroupBox(
        qApp->translate("djvPpmWidget", "File Type"),
        qApp->translate("djvPpmWidget", "Set the file type used when saving PPM images."),
        context);
    QFormLayout * formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(
        qApp->translate("djvPpmWidget", "File type:"),
        _typeWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(
        qApp->translate("djvPpmWidget", "Data Type"),
        qApp->translate("djvPpmWidget", "Set the data type used when saving PPM images."),
        context);
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(
        qApp->translate("djvPpmWidget", "Data type:"),
        _dataWidget);
    layout->addWidget(prefsGroupBox);

    layout->addStretch();

    // Initialize.
    QStringList tmp;
    tmp = plugin->option(plugin->options()[djvPpm::TYPE_OPTION]);
    tmp >> _options.type;
    tmp = plugin->option(plugin->options()[djvPpm::DATA_OPTION]);
    tmp >> _options.data;

    widgetUpdate();

    // Setup the callbacks.
    connect(
        plugin,
        SIGNAL(optionChanged(const QString &)),
        SLOT(pluginCallback(const QString &)));
    connect(
        _typeWidget,
        SIGNAL(activated(int)),
        SLOT(typeCallback(int)));
    connect(
        _dataWidget,
        SIGNAL(activated(int)),
        SLOT(dataCallback(int)));
}

djvPpmWidget::~djvPpmWidget()
{}

void djvPpmWidget::resetPreferences()
{
    _options = djvPpm::Options();
    pluginUpdate();
    widgetUpdate();
}

void djvPpmWidget::pluginCallback(const QString & option)
{
    try
    {
        QStringList tmp;
        tmp = plugin()->option(option);
        if (0 == option.compare(plugin()->options()[
            djvPpm::TYPE_OPTION], Qt::CaseInsensitive))
                tmp >> _options.type;
        else if (0 == option.compare(plugin()->options()[
            djvPpm::DATA_OPTION], Qt::CaseInsensitive))
                tmp >> _options.data;
    }
    catch (const QString &)
    {}
    widgetUpdate();
}

void djvPpmWidget::typeCallback(int in)
{
    _options.type = static_cast<djvPpm::TYPE>(in);
    pluginUpdate();
}

void djvPpmWidget::dataCallback(int in)
{
    _options.data = static_cast<djvPpm::DATA>(in);
    pluginUpdate();
}

void djvPpmWidget::pluginUpdate()
{
    QStringList tmp;
    tmp << _options.type;
    plugin()->setOption(plugin()->options()[djvPpm::TYPE_OPTION], tmp);
    tmp << _options.data;
    plugin()->setOption(plugin()->options()[djvPpm::DATA_OPTION], tmp);
}

void djvPpmWidget::widgetUpdate()
{
    djvSignalBlocker signalBlocker(QObjectList() <<
        _typeWidget <<
        _dataWidget);
    _typeWidget->setCurrentIndex(_options.type);
    _dataWidget->setCurrentIndex(_options.data);
}

//------------------------------------------------------------------------------
// djvPpmWidgetPlugin
//------------------------------------------------------------------------------

djvPpmWidgetPlugin::djvPpmWidgetPlugin(djvCoreContext * context) :
    djvImageIoWidgetPlugin(context)
{}

djvImageIoWidget * djvPpmWidgetPlugin::createWidget(djvImageIo * plugin) const
{
    return new djvPpmWidget(plugin, guiContext());
}

QString djvPpmWidgetPlugin::pluginName() const
{
    return djvPpm::staticName;
}
