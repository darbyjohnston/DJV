//------------------------------------------------------------------------------
// Copyright (c) 2008-2009 Mikael Sundell
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

#include <djvUI/IFFWidget.h>

#include <djvUI/UIContext.h>
#include <djvUI/PrefsGroupBox.h>
#include <djvUI/Style.h>

#include <djvCore/SignalBlocker.h>

#include <QCoreApplication>
#include <QComboBox>
#include <QFormLayout>
#include <QVBoxLayout>

namespace djv
{
    namespace UI
    {
        IFFWidget::IFFWidget(Graphics::ImageIO * plugin, UIContext * context) :
            ImageIOWidget(plugin, context),
            _compressionWidget(0)
        {
            //DJV_DEBUG("IFFWidget::IFFWidget");

            // Create the widgets.
            _compressionWidget = new QComboBox;
            _compressionWidget->addItems(Graphics::IFF::compressionLabels());
            _compressionWidget->setSizePolicy(
                QSizePolicy::Fixed, QSizePolicy::Fixed);

            // Layout the widgets.
            QVBoxLayout * layout = new QVBoxLayout(this);
            layout->setSpacing(context->style()->sizeMetric().largeSpacing);

            PrefsGroupBox * prefsGroupBox = new PrefsGroupBox(
                qApp->translate("djv::UI::IFFWidget", "Compression"),
                qApp->translate("djv::UI::IFFWidget", "Set the file compression used when saving IFF images."),
                context);
            QFormLayout * formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(
                qApp->translate("djv::UI::IFFWidget", "Compression:"),
                _compressionWidget);
            layout->addWidget(prefsGroupBox);

            layout->addStretch();

            // Initialize.
            QStringList tmp;
            tmp = plugin->option(plugin->options()[Graphics::IFF::COMPRESSION_OPTION]);
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

        IFFWidget::~IFFWidget()
        {}

        void IFFWidget::resetPreferences()
        {
            _options = Graphics::IFF::Options();
            pluginUpdate();
            widgetUpdate();
        }

        void IFFWidget::pluginCallback(const QString & option)
        {
            try
            {
                QStringList tmp;
                tmp = plugin()->option(option);
                if (0 == option.compare(plugin()->options()[
                    Graphics::IFF::COMPRESSION_OPTION], Qt::CaseInsensitive))
                    tmp >> _options.compression;
            }
            catch (const QString &)
            {
            }
            widgetUpdate();
        }

        void IFFWidget::compressionCallback(int in)
        {
            _options.compression = static_cast<Graphics::IFF::COMPRESSION>(in);
            QStringList tmp;
            tmp << _options.compression;
            plugin()->setOption(plugin()->options()[Graphics::IFF::COMPRESSION_OPTION], tmp);
        }

        void IFFWidget::pluginUpdate()
        {
            QStringList tmp;
            tmp << _options.compression;
            plugin()->setOption(plugin()->options()[Graphics::IFF::COMPRESSION_OPTION], tmp);
        }

        void IFFWidget::widgetUpdate()
        {
            djvSignalBlocker signalBlocker(_compressionWidget);
            _compressionWidget->setCurrentIndex(_options.compression);
        }

        IFFWidgetPlugin::IFFWidgetPlugin(djvCoreContext * context) :
            ImageIOWidgetPlugin(context)
        {}

        ImageIOWidget * IFFWidgetPlugin::createWidget(Graphics::ImageIO * plugin) const
        {
            return new IFFWidget(plugin, uiContext());
        }

        QString IFFWidgetPlugin::pluginName() const
        {
            return Graphics::IFF::staticName;
        }

    } // namespace UI
} // namespace djv
