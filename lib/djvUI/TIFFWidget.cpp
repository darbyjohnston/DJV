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

#include <djvUI/TIFFWidget.h>

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
        TIFFWidget::TIFFWidget(AV::ImageIO * plugin, const QPointer<UIContext> & context) :
            ImageIOWidget(plugin, context)
        {
            //DJV_DEBUG("TIFFWidget::TIFFWidget");

            // Create the widgets.
            _compressionWidget = new QComboBox;
            _compressionWidget->addItems(AV::TIFF::compressionLabels());
            _compressionWidget->setSizePolicy(
                QSizePolicy::Fixed, QSizePolicy::Fixed);

            // Layout the widgets.
            _layout = new QVBoxLayout(this);

            PrefsGroupBox * prefsGroupBox = new PrefsGroupBox(
                qApp->translate("djv::UI::TIFFWidget", "Compression"),
                qApp->translate("djv::UI::TIFFWidget", "Set the file compression used when saving TIFF images."),
                context);
            QFormLayout * formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(
                qApp->translate("djv::UI::TIFFWidget", "Compression:"),
                _compressionWidget);
            _layout->addWidget(prefsGroupBox);

            _layout->addStretch();

            // Initialize.
            QStringList tmp;
            tmp = plugin->option(
                plugin->options()[AV::TIFF::COMPRESSION_OPTION]);
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

        void TIFFWidget::resetPreferences()
        {
            _options = AV::TIFF::Options();
            pluginUpdate();
            widgetUpdate();
        }

        void TIFFWidget::pluginCallback(const QString & option)
        {
            try
            {
                QStringList tmp;
                tmp = plugin()->option(option);
                if (0 == option.compare(plugin()->options()[
                    AV::TIFF::COMPRESSION_OPTION], Qt::CaseInsensitive))
                    tmp >> _options.compression;
            }
            catch (const QString &)
            {
            }
            widgetUpdate();
        }

        void TIFFWidget::compressionCallback(int in)
        {
            _options.compression = static_cast<AV::TIFF::COMPRESSION>(in);
            pluginUpdate();
        }

        void TIFFWidget::pluginUpdate()
        {
            QStringList tmp;
            tmp << _options.compression;
            plugin()->setOption(
                plugin()->options()[AV::TIFF::COMPRESSION_OPTION], tmp);
        }

        void TIFFWidget::widgetUpdate()
        {
            Core::SignalBlocker signalBlocker(_compressionWidget);
            _compressionWidget->setCurrentIndex(_options.compression);
        }

        TIFFWidgetPlugin::TIFFWidgetPlugin(const QPointer<Core::CoreContext> & context) :
            ImageIOWidgetPlugin(context)
        {}

        ImageIOWidget * TIFFWidgetPlugin::createWidget(AV::ImageIO * plugin) const
        {
            return new TIFFWidget(plugin, uiContext());
        }

        QString TIFFWidgetPlugin::pluginName() const
        {
            return AV::TIFF::staticName;
        }

    } // namespace UI
} // namespace djv
