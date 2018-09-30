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

#include <djvViewLib/Context.h>

#include <djvViewLib/FileCache.h>
#include <djvViewLib/FilePrefsWidget.h>
#include <djvViewLib/FileSave.h>
#include <djvViewLib/FilePrefs.h>
#include <djvViewLib/ImagePrefs.h>
#include <djvViewLib/ImagePrefsWidget.h>
#include <djvViewLib/InputPrefs.h>
#include <djvViewLib/InputPrefsWidget.h>
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

#include <QApplication>

namespace djv
{
    namespace ViewLib
    {
        struct Context::Private
        {
            CommandLineOptions commandLineOptions;
            
            QPointer<FilePrefs>     filePrefs;
            QPointer<ImagePrefs>    imagePrefs;
            QPointer<InputPrefs>    inputPrefs;
            QPointer<PlaybackPrefs> playbackPrefs;
            QPointer<ShortcutPrefs> shortcutPrefs;
            QPointer<ViewPrefs>     viewPrefs;
            QPointer<WindowPrefs>   windowPrefs;

            QPointer<FileCache> fileCache;
            QPointer<FileSave>  fileSave;
        };

        Context::Context(int & argc, char ** argv, QObject * parent) :
            UI::UIContext(argc, argv, parent),
            _p(new Private)
        {
            //DJV_DEBUG("Context::Context");

            // Load translators.
            loadTranslator("djvViewLib");

            // Load preferences.
            DJV_LOG(debugLog(), "Context", "Load the preferences...");
            _p->filePrefs = new FilePrefs(this);
            _p->imagePrefs = new ImagePrefs(this);
            _p->inputPrefs = new InputPrefs(this);
            _p->playbackPrefs = new PlaybackPrefs(this);
            _p->shortcutPrefs = new ShortcutPrefs(this);
            _p->viewPrefs = new ViewPrefs(this);
            _p->windowPrefs = new WindowPrefs(this);
            DJV_LOG(debugLog(), "Context", "");

            // Initialize objects.
            DJV_LOG(debugLog(), "Context", "Initialize objects...");
            _p->fileCache = new FileCache(this);
            _p->fileSave = new FileSave(this);
            DJV_LOG(debugLog(), "Context", "");
        }

        Context::~Context()
        {
            //DJV_DEBUG("Context::Context");

            delete _p->windowPrefs;
            delete _p->viewPrefs;
            delete _p->shortcutPrefs;
            delete _p->playbackPrefs;
            delete _p->inputPrefs;
            delete _p->imagePrefs;
            delete _p->filePrefs;

            delete _p->fileSave;
            delete _p->fileCache;
        }

        const Context::CommandLineOptions & Context::commandLineOptions() const
        {
            return _p->commandLineOptions;
        }

        const QPointer<FilePrefs> & Context::filePrefs() const
        {
            return _p->filePrefs;
        }

        const QPointer<ImagePrefs> & Context::imagePrefs() const
        {
            return _p->imagePrefs;
        }

        const QPointer<InputPrefs> & Context::inputPrefs() const
        {
            return _p->inputPrefs;
        }

        const QPointer<PlaybackPrefs> & Context::playbackPrefs() const
        {
            return _p->playbackPrefs;
        }

        const QPointer<ShortcutPrefs> & Context::shortcutPrefs() const
        {
            return _p->shortcutPrefs;
        }

        const QPointer<ViewPrefs> & Context::viewPrefs() const
        {
            return _p->viewPrefs;
        }

        const QPointer<WindowPrefs> & Context::windowPrefs() const
        {
            return _p->windowPrefs;
        }

        const QPointer<FileCache> & Context::fileCache() const
        {
            return _p->fileCache;
        }

        const QPointer<FileSave> & Context::fileSave() const
        {
            return _p->fileSave;
        }

        void Context::setValid(bool valid)
        {
            UI::UIContext::setValid(true);
            if (isValid())
            {
                prefsDialog()->addWidget(new ShortcutPrefsWidget(this));
                prefsDialog()->addWidget(new InputPrefsWidget(this));
                prefsDialog()->addWidget(new PlaybackPrefsWidget(this));
                prefsDialog()->addWidget(new ImagePrefsWidget(this));
                prefsDialog()->addWidget(new ViewPrefsWidget(this));
                prefsDialog()->addWidget(new WindowPrefsWidget(this));
                prefsDialog()->addWidget(new FilePrefsWidget(this));
            }
        }

        bool Context::commandLineParse(QStringList & in)
        {
            //DJV_DEBUG("Context::commandLineParse");
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
                    if (qApp->translate("djv::ViewLib::Context", "-combine") == arg)
                    {
                        _p->commandLineOptions.combine = true;
                    }
                    else if (
                        qApp->translate("djv::ViewLib::Context", "-seq") == arg ||
                        qApp->translate("djv::ViewLib::Context", "-q") == arg)
                    {
                        in >> _p->commandLineOptions.sequence;
                    }
                    else if (
                        qApp->translate("djv::ViewLib::Context", "-auto_seq") == arg)
                    {
                        in >> _p->commandLineOptions.autoSequence;
                    }

                    // Parse the file options.
                    else if (
                        qApp->translate("djv::ViewLib::Context", "-file_layer") == arg)
                    {
                        int value = 0;
                        in >> value;
                        _p->commandLineOptions.fileLayer.reset(new int(value));
                    }
                    else if (
                        qApp->translate("djv::ViewLib::Context", "-file_proxy") == arg)
                    {
                        Graphics::PixelDataInfo::PROXY value = static_cast<Graphics::PixelDataInfo::PROXY>(0);
                        in >> value;
                        _p->commandLineOptions.fileProxy.reset(new Graphics::PixelDataInfo::PROXY(value));
                    }
                    else if (
                        qApp->translate("djv::ViewLib::Context", "-file_cache") == arg)
                    {
                        bool value = false;
                        in >> value;
                        _p->commandLineOptions.fileCacheEnable.reset(new bool(value));
                    }

                    // Parse the window options.
                    else if (
                        qApp->translate("djv::ViewLib::Context", "-window_full_screen") == arg)
                    {
                        _p->commandLineOptions.windowFullScreen.reset(new bool(true));
                    }

                    // Parse the playback options.
                    else if (
                        qApp->translate("djv::ViewLib::Context", "-playback") == arg)
                    {
                        Enum::PLAYBACK value = static_cast<Enum::PLAYBACK>(0);
                        in >> value;
                        _p->commandLineOptions.playback.reset(new Enum::PLAYBACK(value));
                    }
                    else if (
                        qApp->translate("djv::ViewLib::Context", "-playback_frame") == arg)
                    {
                        int value = 0;
                        in >> value;
                        _p->commandLineOptions.playbackFrame.reset(new int(value));
                    }
                    else if (
                        qApp->translate("djv::ViewLib::Context", "-playback_speed") == arg)
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

        QString Context::commandLineHelp() const
        {
            static const QString label = qApp->translate("djv::ViewLib::Context",
                "djv_view\n"
                "\n"
                "    The djv_view application provides real-time image sequence and movie "
                "playback. It is intended as a professional tool for reviewing computer "
                "animation, film, and video footage.\n"
                "\n"
                "Usage\n"
                "\n"
                "    djv_view [image]... [option]...\n"
                "\n"
                "    image  - One or more images, image sequences, or movies\n"
                "    option - Additional options (see below)\n"
                "\n"
                "Options\n"
                "\n"
                "    -combine\n"
                "        Combine multiple command line arguments into a single sequence.\n"
                "    -seq, -q (value)\n"
                "        Set command line file sequencing. Options = %1. Default = %2.\n"
                "    -auto_seq (value)\n"
                "        Automatically detect sequences when opening files. Options = %3. Default = %4.\n"
                "\n"
                "File Options\n"
                "\n"
                "    -file_layer (value)\n"
                "        Set the input layer.\n"
                "    -file_proxy (value)\n"
                "        Set the proxy scale. Options = %5.\n"
                "    -file_cache (value)\n"
                "        Set whether the file cache is enabled. Options = %6.\n"
                "\n"
                "Window Options\n"
                "\n"
                "    -window_full_screen\n"
                "        Set the window full screen.\n"
                "\n"
                "Playback Options\n"
                "\n"
                "    -playback (value)\n"
                "        Set the playback. Options = %7.\n"
                "    -playback_frame (value)\n"
                "        Set the playback frame.\n"
                "    -playback_speed (value)\n"
                "        Set the playback speed. Options = %8.\n"
                "%9");
            QStringList sequenceLabel;
            sequenceLabel << _p->commandLineOptions.sequence;
            QStringList autoSequenceLabel;
            autoSequenceLabel << _p->commandLineOptions.autoSequence;
            return QString(label).
                arg(Core::Sequence::compressLabels().join(", ")).
                arg(sequenceLabel.join(", ")).
                arg(Core::StringUtil::boolLabels().join(", ")).
                arg(autoSequenceLabel.join(", ")).
                arg(Graphics::PixelDataInfo::proxyLabels().join(", ")).
                arg(Core::StringUtil::boolLabels().join(", ")).
                arg(Enum::playbackLabels().join(", ")).
                arg(Core::Speed::fpsLabels().join(", ")).
                arg(UI::UIContext::commandLineHelp());
        }

    } // namespace ViewLib
} // namespace djv
