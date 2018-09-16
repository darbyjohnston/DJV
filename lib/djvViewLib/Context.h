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

#pragma once

#include <djvViewLib/Util.h>

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
        class FilePrefs;
        class FileSave;
        class ImagePrefs;
        class InputPrefs;
        class PlaybackPrefs;
        class ShortcutPrefs;
        class ViewPrefs;
        class WindowPrefs;

        //! \class Context
        //!
        //! This class provides global functionality for the library.
        class Context : public UI::UIContext
        {
            Q_OBJECT

        public:
            explicit Context(QObject * parent = nullptr);

            virtual ~Context();

            //! Get the list of inputs.
            const QStringList & input() const;

            //! Get whether to combine the inputs.
            bool hasCombine() const;

            //! Get the file sequencing.
            Core::Sequence::COMPRESS sequence() const;

            //! Get whether to automatically detext sequences.
            bool hasAutoSequence() const;

            //! Get the file layer.
            const QScopedPointer<int> & fileLayer() const;

            //! Get the file proxy.
            const QScopedPointer<Graphics::PixelDataInfo::PROXY> & fileProxy() const;

            //! Get whether the file cache is enabled.
            const QScopedPointer<bool> & hasFileCache() const;

            //! Get whether the window is full screen.
            const QScopedPointer<bool> & isWindowFullScreen() const;

            //! Get the playback.
            const QScopedPointer<Util::PLAYBACK> & playback() const;

            //! Get the playback frame.
            const QScopedPointer<int> & playbackFrame() const;

            //! Get the playback speed.
            const QScopedPointer<Core::Speed> & playbackSpeed() const;

            //! Get the file preferences.
            FilePrefs * filePrefs() const;

            //! Get the image preferences.
            ImagePrefs * imagePrefs() const;

            //! Get the input preferences.
            InputPrefs * inputPrefs() const;

            //! Get the playback preferences.
            PlaybackPrefs * playbackPrefs() const;

            //! Get the shortcut preferences.
            ShortcutPrefs * shortcutPrefs() const;

            //! Get the view preferences.
            ViewPrefs * viewPrefs() const;

            //! Get the window preferences.
            WindowPrefs * windowPrefs() const;

            //! Get the file cache.
            FileCache * fileCache() const;

            //! Get the file save.
            FileSave * fileSave() const;

            virtual void setValid(bool);

        protected:
            virtual bool commandLineParse(QStringList &) throw (QString);

            virtual QString commandLineHelp() const;

        private:
            DJV_PRIVATE_COPY(Context);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace ViewLib
} // namespace djv
