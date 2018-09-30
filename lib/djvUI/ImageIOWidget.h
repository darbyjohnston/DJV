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

#pragma once

#include <djvUI/AbstractPrefsWidget.h>

#include <djvCore/Plugin.h>
#include <djvCore/System.h>

#include <QWidget>

namespace djv
{
    namespace Graphics
    {
        class ImageIO;

    } // namespace Graphics

    namespace UI
    {
        class UIContext;

        //! This class provides the base functionality for image I/O widgets.
        class ImageIOWidget : public AbstractPrefsWidget
        {
            Q_OBJECT

        public:
            explicit ImageIOWidget(
                Graphics::ImageIO * plugin,
                const QPointer<UIContext> & context,
                QWidget * parent = nullptr);
            ~ImageIOWidget() override;

            //! Get the plugin.
            Graphics::ImageIO * plugin() const;

            //! Get the context.
            const QPointer<UIContext> & context() const;

        private:
            struct Private;
            std::unique_ptr<Private> _p;
        };

        //! This class provides an image I/O widget plugin.
        class ImageIOWidgetPlugin : public Core::Plugin
        {
        public:
            ImageIOWidgetPlugin(const QPointer<Core::CoreContext> &);

            //! Create a widget.    
            virtual ImageIOWidget * createWidget(Graphics::ImageIO * plugin) const = 0;

            //! Get the context.
            QPointer<UIContext> uiContext() const;

            virtual Core::Plugin * copyPlugin() const;
        };

        //! This class provides a factory for image I/O widget plugins.
        class ImageIOWidgetFactory : public Core::PluginFactory
        {
            Q_OBJECT

        public:
            explicit ImageIOWidgetFactory(
                const QPointer<UIContext> &,
                const QStringList & searchPath = Core::System::searchPath(),
                QObject * parent = nullptr);
            ~ImageIOWidgetFactory() override;
            
            //! Create a widget.    
            ImageIOWidget * createWidget(Graphics::ImageIO *) const;

        private:
            DJV_PRIVATE_COPY(ImageIOWidgetFactory);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace UI
} // namespace djv
