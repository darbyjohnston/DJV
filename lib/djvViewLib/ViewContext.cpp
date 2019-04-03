//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvViewLib/ViewContext.h>

#include <djvViewLib/AnnotatePrefs.h>
#include <djvViewLib/FileCache.h>
#include <djvViewLib/FileExport.h>
#include <djvViewLib/FilePrefs.h>
#include <djvViewLib/FilePrefsWidget.h>
#include <djvViewLib/ImagePrefs.h>
#include <djvViewLib/ImagePrefsWidget.h>
#include <djvViewLib/MousePrefs.h>
#include <djvViewLib/MousePrefsWidget.h>
#include <djvViewLib/PlaybackPrefs.h>
#include <djvViewLib/PlaybackPrefsWidget.h>
#include <djvViewLib/ShortcutPrefs.h>
#include <djvViewLib/ShortcutPrefsWidget.h>
#include <djvViewLib/ViewPrefs.h>
#include <djvViewLib/ViewPrefsWidget.h>
#include <djvViewLib/WindowPrefs.h>
#include <djvViewLib/WindowPrefsWidget.h>

#include <djvUI/PrefsDialog.h>

#include <djvCore/DebugLog.h>
#include <djvCore/FileInfoUtil.h>

#include <QApplication>

static void initResources()
{
    Q_INIT_RESOURCE(djvViewLib);
}

namespace djv
{
    namespace ViewLib
    {
        struct ViewContext::Private
        {
            CommandLineOptions commandLineOptions;
            
            QScopedPointer<AnnotatePrefs> annotatePrefs;
            QScopedPointer<FilePrefs>     filePrefs;
            QScopedPointer<ImagePrefs>    imagePrefs;
            QScopedPointer<MousePrefs>    mousePrefs;
            QScopedPointer<PlaybackPrefs> playbackPrefs;
            QScopedPointer<ShortcutPrefs> shortcutPrefs;
            QScopedPointer<ViewPrefs>     viewPrefs;
            QScopedPointer<WindowPrefs>   windowPrefs;

            QScopedPointer<FileCache>  fileCache;
            QScopedPointer<FileExport> fileExport;
        };

        ViewContext::ViewContext(int & argc, char ** argv, QObject * parent) :
            UI::UIContext(argc, argv, parent),
            _p(new Private)
        {
            //DJV_DEBUG("ViewContext::ViewContext");
            
            initResources();

            qRegisterMetaType<Enum::KEYBOARD_MODIFIER>("djv::ViewLib::Enum::KEYBOARD_MODIFIER");
            qRegisterMetaType<Enum::MOUSE_BUTTON>("djv::ViewLib::Enum::MOUSE_BUTTON");
            qRegisterMetaType<Enum::MOUSE_BUTTON_ACTION>("djv::ViewLib::Enum::MOUSE_BUTTON_ACTION");

            // Load translators.
            loadTranslator("djvViewLib");

            // Load preferences.
            DJV_LOG(debugLog(), "ViewContext", "Load the preferences...");
            _p->annotatePrefs.reset(new AnnotatePrefs(this));
            _p->filePrefs.reset(new FilePrefs(this));
            _p->imagePrefs.reset(new ImagePrefs(this));
            _p->mousePrefs.reset(new MousePrefs(this));
            _p->playbackPrefs.reset(new PlaybackPrefs(this));
            _p->shortcutPrefs.reset(new ShortcutPrefs(this));
            _p->viewPrefs.reset(new ViewPrefs(this));
            _p->windowPrefs.reset(new WindowPrefs(this));

            // Initialize objects.
            DJV_LOG(debugLog(), "ViewContext", "Initialize objects...");
            _p->fileCache.reset(new FileCache(this));
            _p->fileExport.reset(new FileExport(this));
        }

        ViewContext::~ViewContext()
        {
            //DJV_DEBUG("ViewContext::ViewContext");
        }

        const ViewContext::CommandLineOptions & ViewContext::commandLineOptions() const
        {
            return _p->commandLineOptions;
        }

        QPointer<AnnotatePrefs> ViewContext::annotatePrefs() const
        {
            return _p->annotatePrefs.data();
        }

        QPointer<FilePrefs> ViewContext::filePrefs() const
        {
            return _p->filePrefs.data();
        }

        QPointer<ImagePrefs> ViewContext::imagePrefs() const
        {
            return _p->imagePrefs.data();
        }

        QPointer<MousePrefs> ViewContext::mousePrefs() const
        {
            return _p->mousePrefs.data();
        }

        QPointer<PlaybackPrefs> ViewContext::playbackPrefs() const
        {
            return _p->playbackPrefs.data();
        }

        QPointer<ShortcutPrefs> ViewContext::shortcutPrefs() const
        {
            return _p->shortcutPrefs.data();
        }

        QPointer<ViewPrefs> ViewContext::viewPrefs() const
        {
            return _p->viewPrefs.data();
        }

        QPointer<WindowPrefs> ViewContext::windowPrefs() const
        {
            return _p->windowPrefs.data();
        }

        QPointer<FileCache> ViewContext::fileCache() const
        {
            return _p->fileCache.data();
        }

        QPointer<FileExport> ViewContext::fileExport() const
        {
            return _p->fileExport.data();
        }

        void ViewContext::setValid(bool valid)
        {
            UI::UIContext::setValid(true);
            if (isValid())
            {
                prefsDialog()->addWidget(new ShortcutPrefsWidget(this));
                prefsDialog()->addWidget(new MousePrefsWidget(this));
                prefsDialog()->addWidget(new PlaybackPrefsWidget(this));
                prefsDialog()->addWidget(new ImagePrefsWidget(this));
                prefsDialog()->addWidget(new ViewPrefsWidget(this));
                prefsDialog()->addWidget(new WindowPrefsWidget(this));
                prefsDialog()->addWidget(new FilePrefsWidget(this));
            }
        }
        
        bool ViewContext::commandLineParse(QStringList & in)
        {
            //DJV_DEBUG("ViewContext::commandLineParse");
            //DJV_DEBUG_PRINT("in = " << in);

            if (!UI::UIContext::commandLineParse(in))
                return false;
            QString arg;
            try
            {
                while (!in.isEmpty())
                {
                    in >> arg;

                    // Parse the options.
                    if (qApp->translate("djv::ViewLib::ViewContext", "-combine") == arg)
                    {
                        _p->commandLineOptions.combine = true;
                    }

                    // Parse the file options.
                    else if (
                        qApp->translate("djv::ViewLib::ViewContext", "-file_layer") == arg)
                    {
                        int value = 0;
                        in >> value;
                        _p->commandLineOptions.fileLayer.reset(new int(value));
                    }
                    else if (
                        qApp->translate("djv::ViewLib::ViewContext", "-file_proxy") == arg)
                    {
                        AV::PixelDataInfo::PROXY value = static_cast<AV::PixelDataInfo::PROXY>(0);
                        in >> value;
                        _p->commandLineOptions.fileProxy.reset(new AV::PixelDataInfo::PROXY(value));
                    }
                    else if (
                        qApp->translate("djv::ViewLib::ViewContext", "-file_cache") == arg)
                    {
                        bool value = false;
                        in >> value;
                        _p->commandLineOptions.fileCacheEnable.reset(new bool(value));
                    }

                    // Parse the window options.
                    else if (
                        qApp->translate("djv::ViewLib::ViewContext", "-window_full_screen") == arg)
                    {
                        _p->commandLineOptions.windowFullScreen.reset(new bool(true));
                    }

                    // Parse the playback options.
                    else if (
                        qApp->translate("djv::ViewLib::ViewContext", "-playback") == arg)
                    {
                        Enum::PLAYBACK value = static_cast<Enum::PLAYBACK>(0);
                        in >> value;
                        _p->commandLineOptions.playback.reset(new Enum::PLAYBACK(value));
                    }
                    else if (
                        qApp->translate("djv::ViewLib::ViewContext", "-playback_frame") == arg)
                    {
                        int value = 0;
                        in >> value;
                        _p->commandLineOptions.playbackFrame.reset(new int(value));
                    }
                    else if (
                        qApp->translate("djv::ViewLib::ViewContext", "-playback_speed") == arg)
                    {
                        Core::Speed::FPS value = static_cast<Core::Speed::FPS>(0);
                        in >> value;
                        _p->commandLineOptions.playbackSpeed.reset(new Core::Speed(value));
                    }

                    // Parse the arguments.
                    else
                    {
                        _p->commandLineOptions.input += arg;
                    }
                }
            }
            catch (const QString &)
            {
                throw arg;
            }
            return true;
        }

        QString ViewContext::commandLineHelp() const
        {
            static const QString label = qApp->translate("djv::ViewLib::ViewContext",
                "djv_view\n"
                "\n"
                "    Real-time image sequence and movie playback."
                "\n"
                "Usage\n"
                "\n"
                "    djv_view [image]... [option]...\n"
                "\n"
                "    image  - One or more images, image sequences, or movies\n"
                "\n"
                "Options\n"
                "\n"
                "    -combine\n"
                "        Combine multiple command line arguments into a single sequence.\n"
                "\n"
                "File Options\n"
                "\n"
                "    -file_layer (value)\n"
                "        Set the input layer.\n"
                "    -file_proxy (value)\n"
                "        Set the proxy scale: %1.\n"
                "    -file_cache (value)\n"
                "        Set whether the file cache is enabled: %2.\n"
                "\n"
                "Window Options\n"
                "\n"
                "    -window_full_screen\n"
                "        Set the window full screen.\n"
                "\n"
                "Playback Options\n"
                "\n"
                "    -playback (value)\n"
                "        Set the playback: %3.\n"
                "    -playback_frame (value)\n"
                "        Set the playback frame.\n"
                "    -playback_speed (value)\n"
                "        Set the playback speed: %4.\n"
                "%5");
            return QString(label).
                arg(AV::PixelDataInfo::proxyLabels().join(", ")).
                arg(Core::StringUtil::boolLabels().join(", ")).
                arg(Enum::playbackLabels().join(", ")).
                arg(Core::Speed::fpsLabels().join(", ")).
                arg(UI::UIContext::commandLineHelp());
        }

    } // namespace ViewLib
} // namespace djv
