//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
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

#include <djvUI/PPMWidget.h>

#include <djvUI/UIContext.h>
#include <djvUI/PrefsGroupBox.h>

#include <djvAV/ImageIO.h>

#include <djvCore/SignalBlocker.h>

#include <QApplication>
#include <QComboBox>
#include <QFormLayout>
#include <QVBoxLayout>

namespace djv
{
    namespace UI
    {
        PPMWidget::PPMWidget(AV::ImageIO * plugin, const QPointer<UIContext> & context) :
            ImageIOWidget(plugin, context)
        {
            // Create the widgets.
            _typeWidget = new QComboBox;
            _typeWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            _typeWidget->addItems(AV::PPM::typeLabels());

            _dataWidget = new QComboBox;
            _dataWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            _dataWidget->addItems(AV::PPM::dataLabels());

            // Layout the widgets.
            _layout = new QVBoxLayout(this);

            PrefsGroupBox * prefsGroupBox = new PrefsGroupBox(
                qApp->translate("djv::UI::PPMWidget", "File Type"),
                qApp->translate("djv::UI::PPMWidget", "Set the file type used when saving PPM images."),
                context);
            QFormLayout * formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(
                qApp->translate("djv::UI::PPMWidget", "File type:"),
                _typeWidget);
            _layout->addWidget(prefsGroupBox);

            prefsGroupBox = new PrefsGroupBox(
                qApp->translate("djv::UI::PPMWidget", "Data Type"),
                qApp->translate("djv::UI::PPMWidget", "Set the data type used when saving PPM images."),
                context);
            formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(
                qApp->translate("djv::UI::PPMWidget", "Data type:"),
                _dataWidget);
            _layout->addWidget(prefsGroupBox);

            _layout->addStretch();

            // Initialize.
            QStringList tmp;
            tmp = plugin->option(plugin->options()[AV::PPM::TYPE_OPTION]);
            tmp >> _options.type;
            tmp = plugin->option(plugin->options()[AV::PPM::DATA_OPTION]);
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

        void PPMWidget::resetPreferences()
        {
            _options = AV::PPM::Options();
            pluginUpdate();
            widgetUpdate();
        }

        void PPMWidget::pluginCallback(const QString & option)
        {
            try
            {
                QStringList tmp;
                tmp = plugin()->option(option);
                if (0 == option.compare(plugin()->options()[
                    AV::PPM::TYPE_OPTION], Qt::CaseInsensitive))
                    tmp >> _options.type;
                else if (0 == option.compare(plugin()->options()[
                    AV::PPM::DATA_OPTION], Qt::CaseInsensitive))
                    tmp >> _options.data;
            }
            catch (const QString &)
            {
            }
            widgetUpdate();
        }

        void PPMWidget::typeCallback(int in)
        {
            _options.type = static_cast<AV::PPM::TYPE>(in);
            pluginUpdate();
        }

        void PPMWidget::dataCallback(int in)
        {
            _options.data = static_cast<AV::PPM::DATA>(in);
            pluginUpdate();
        }

        void PPMWidget::pluginUpdate()
        {
            QStringList tmp;
            tmp << _options.type;
            plugin()->setOption(plugin()->options()[AV::PPM::TYPE_OPTION], tmp);
            tmp << _options.data;
            plugin()->setOption(plugin()->options()[AV::PPM::DATA_OPTION], tmp);
        }

        void PPMWidget::widgetUpdate()
        {
            Core::SignalBlocker signalBlocker(QObjectList() <<
                _typeWidget <<
                _dataWidget);
            _typeWidget->setCurrentIndex(_options.type);
            _dataWidget->setCurrentIndex(_options.data);
        }

        PPMWidgetPlugin::PPMWidgetPlugin(const QPointer<Core::CoreContext> & context) :
            ImageIOWidgetPlugin(context)
        {}

        ImageIOWidget * PPMWidgetPlugin::createWidget(AV::ImageIO * plugin) const
        {
            return new PPMWidget(plugin, uiContext());
        }

        QString PPMWidgetPlugin::pluginName() const
        {
            return AV::PPM::staticName;
        }

    } // namespace UI
} // namespace djv
