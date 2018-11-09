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

#include <djvUI/JPEGWidget.h>

#include <djvUI/UIContext.h>
#include <djvUI/IntEditSlider.h>
#include <djvUI/PrefsGroupBox.h>

#include <djvAV/ImageIO.h>

#include <djvCore/SignalBlocker.h>

#include <QApplication>
#include <QFormLayout>
#include <QVBoxLayout>

namespace djv
{
    namespace UI
    {
        JPEGWidget::JPEGWidget(AV::ImageIO * plugin, const QPointer<UIContext> & context) :
            ImageIOWidget(plugin, context)
        {
            // Create the widgets.
            _qualityWidget = new IntEditSlider(context);
            _qualityWidget->setRange(0, 100);
            _qualityWidget->setDefaultValue(AV::JPEG::Options().quality);

            // Layout the widgets.
            _layout = new QVBoxLayout(this);

            PrefsGroupBox * prefsGroupBox = new PrefsGroupBox(
                qApp->translate("djv::UI::JPEGWidget", "Quality"),
                qApp->translate("djv::UI::JPEGWidget", "Set the quality used when saving JPEG images."),
                context);
            QFormLayout * formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(
                qApp->translate("djv::UI::JPEGWidget", "Quality:"),
                _qualityWidget);
            _layout->addWidget(prefsGroupBox);

            _layout->addStretch();

            // Initialize.
            QStringList tmp;
            tmp = plugin->option(
                plugin->options()[AV::JPEG::QUALITY_OPTION]);
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

        void JPEGWidget::resetPreferences()
        {
            _options = AV::JPEG::Options();
            pluginUpdate();
            widgetUpdate();
        }

        void JPEGWidget::pluginCallback(const QString & option)
        {
            try
            {
                QStringList tmp;
                tmp = plugin()->option(option);
                if (0 == option.compare(plugin()->options()[
                    AV::JPEG::QUALITY_OPTION], Qt::CaseInsensitive))
                    tmp >> _options.quality;
            }
            catch (const QString &)
            {
            }
            widgetUpdate();
        }

        void JPEGWidget::qualityCallback(int in)
        {
            _options.quality = in;
            pluginUpdate();
        }

        void JPEGWidget::pluginUpdate()
        {
            QStringList tmp;
            tmp << _options.quality;
            plugin()->setOption(AV::JPEG::optionsLabels()[AV::JPEG::QUALITY_OPTION], tmp);
        }

        void JPEGWidget::widgetUpdate()
        {
            Core::SignalBlocker signalBlocker(_qualityWidget);
            _qualityWidget->setValue(_options.quality);
        }

        JPEGWidgetPlugin::JPEGWidgetPlugin(const QPointer<Core::CoreContext> & context) :
            ImageIOWidgetPlugin(context)
        {}

        ImageIOWidget * JPEGWidgetPlugin::createWidget(AV::ImageIO * plugin) const
        {
            return new JPEGWidget(plugin, uiContext());
        }

        QString JPEGWidgetPlugin::pluginName() const
        {
            return AV::JPEG::staticName;
        }

    } // namespace UI
} // namespace djv
