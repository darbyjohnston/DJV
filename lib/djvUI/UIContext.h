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

#include <djvUI/Core.h>

#include <djvGraphics/GraphicsContext.h>

#include <djvCore/Util.h>

#include <memory>

namespace djv
{
    namespace UI
    {
        class AboutDialog;
        class DebugLogDialog;
        class FileBrowser;
        class FileBrowserCache;
        class FileBrowserPrefs;
        class FileBrowserThumbnailSystem;
        class HelpPrefs;
        class IconLibrary;
        class ImageIOPrefs;
        class ImageIOWidgetFactory;
        class ImagePrefs;
        class InfoDialog;
        class MessagesDialog;
        class PrefsDialog;
        class SequencePrefs;
        class StylePrefs;
        class TimePrefs;

        //! \class djvUIContext
        //!
        //! This class provides global functionality for the library.
        class UIContext : public Graphics::GraphicsContext
        {
            Q_OBJECT

        public:
            explicit UIContext(QObject * parent = nullptr);
            ~UIContext() override;

            //! Get whether the user-interface has started.
            bool isValid() const;

            //! Set whether the user-interface has started.
            virtual void setValid(bool);

            //! Open the documentation.
            void help();

            //! Get the file browser.
            FileBrowser * fileBrowser(const QString & title = QString()) const;

            //! Get the image I/O widget plugin factory.
            ImageIOWidgetFactory * imageIOWidgetFactory() const;

            //! Get the preferences dialog.
            PrefsDialog * prefsDialog() const;

            //! Get the information dialog.
            InfoDialog * infoDialog() const;

            //! Get the about dialog.
            AboutDialog * aboutDialog() const;

            //! Get the messages dialog.
            MessagesDialog * messagesDialog() const;

            //! Get the debugging dialog.
            DebugLogDialog * debugLogDialog() const;

            //! Get the file browser preferences.
            FileBrowserPrefs * fileBrowserPrefs() const;

            //! Get the help preferences.
            HelpPrefs * helpPrefs() const;

            //! Get the image preferences.
            ImagePrefs * imagePrefs() const;

            //! Get the image I/O preferences.
            ImageIOPrefs * imageIOPrefs() const;

            //! Get the sequence preferences.
            SequencePrefs * sequencePrefs() const;

            //! Get the time preferences.
            TimePrefs * timePrefs() const;

            //! Get the style preferences.
            StylePrefs * stylePrefs() const;

            //! Get the file browser cache.
            FileBrowserCache * fileBrowserCache() const;

            //! Get the file browser thumbnail system.
            const QPointer<FileBrowserThumbnailSystem> & fileBrowserThumbnailSystem() const;

            //! Get the icon library.
            IconLibrary * iconLibrary() const;

            QString info() const override;
            void print(const QString &, bool newLine = true, int indent = 0) override;

        protected:
            bool commandLineParse(QStringList &) override;
            QString commandLineHelp() const override;

        private Q_SLOTS:
            void styleUpdate();
            
        private:
            DJV_PRIVATE_COPY(UIContext);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace UI
} // namespace djv
