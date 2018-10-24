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

#include <djvViewLib/Enum.h>

#include <djvUI/UIContext.h>

#include <djvGraphics/PixelData.h>

#include <djvCore/Sequence.h>

#include <QScopedPointer>

#include <memory>

namespace djv
{
    namespace ViewLib
    {
        class FileCache;
        class FileExport;
        class FilePrefs;
        class ImagePrefs;
        class InputPrefs;
        class PlaybackPrefs;
        class ShortcutPrefs;
        class ViewPrefs;
        class WindowPrefs;

        //! This class provides global functionality for the library.
        class ViewContext : public UI::UIContext
        {
            Q_OBJECT

        public:
            explicit ViewContext(int & argc, char ** argv, QObject * parent = nullptr);
            ~ViewContext() override;

            //! This struct povides the command-line options.
            struct CommandLineOptions
            {
                QStringList                                    input;
                bool                                           combine = false;
                QScopedPointer<int>                            fileLayer;
                QScopedPointer<Graphics::PixelDataInfo::PROXY> fileProxy;
                QScopedPointer<bool>                           fileCacheEnable;
                QScopedPointer<bool>                           windowFullScreen;
                QScopedPointer<Enum::PLAYBACK>                 playback;
                QScopedPointer<int>                            playbackFrame;
                QScopedPointer<Core::Speed>                    playbackSpeed;
            };

            //! Get the command-line options
            const CommandLineOptions & commandLineOptions() const;

            //! Get the file preferences.
            const QPointer<FilePrefs> & filePrefs() const;

            //! Get the image preferences.
            const QPointer<ImagePrefs> & imagePrefs() const;

            //! Get the input preferences.
            const QPointer<InputPrefs> & inputPrefs() const;

            //! Get the playback preferences.
            const QPointer<PlaybackPrefs> & playbackPrefs() const;

            //! Get the shortcut preferences.
            const QPointer<ShortcutPrefs> & shortcutPrefs() const;

            //! Get the view preferences.
            const QPointer<ViewPrefs> & viewPrefs() const;

            //! Get the window preferences.
            const QPointer<WindowPrefs> & windowPrefs() const;

            //! Get the file cache.
            const QPointer<FileCache> & fileCache() const;

            //! Get the file exporter.
            const QPointer<FileExport> & fileExport() const;

            void setValid(bool) override;
            
            QString doc() const override;

        protected:
            bool commandLineParse(QStringList &) override;
            QString commandLineHelp() const override;

        private:
            DJV_PRIVATE_COPY(ViewContext);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace ViewLib
} // namespace djv
