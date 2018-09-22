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
            QStringList                                    input;
            bool                                           combine = false;
            Core::Sequence::COMPRESS                       sequence = Core::Sequence::COMPRESS_RANGE;
            bool                                           autoSequence = true;
            QScopedPointer<int>                            fileLayer;
            QScopedPointer<Graphics::PixelDataInfo::PROXY> fileProxy;
            QScopedPointer<bool>                           fileCacheEnable;
            QScopedPointer<bool>                           windowFullScreen;
            QScopedPointer<Util::PLAYBACK>                 playback;
            QScopedPointer<int>                            playbackFrame;
            QScopedPointer<Core::Speed>                    playbackSpeed;

            FilePrefs *     filePrefs = nullptr;
            ImagePrefs *    imagePrefs = nullptr;
            InputPrefs *    inputPrefs = nullptr;
            PlaybackPrefs * playbackPrefs = nullptr;
            ShortcutPrefs * shortcutPrefs = nullptr;
            ViewPrefs *     viewPrefs = nullptr;
            WindowPrefs *   windowPrefs = nullptr;

            FileCache * fileCache = nullptr;
            FileSave *  fileSave = nullptr;
        };

        Context::Context(QObject * parent) :
            UI::UIContext(parent),
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

        const QStringList & Context::input() const
        {
            return _p->input;
        }

        bool Context::hasCombine() const
        {
            return _p->combine;
        }

        Core::Sequence::COMPRESS Context::sequence() const
        {
            return _p->sequence;
        }

        bool Context::hasAutoSequence() const
        {
            return _p->autoSequence;
        }

        const QScopedPointer<int> & Context::fileLayer() const
        {
            return _p->fileLayer;
        }

        const QScopedPointer<Graphics::PixelDataInfo::PROXY> & Context::fileProxy() const
        {
            return _p->fileProxy;
        }

        const QScopedPointer<bool> & Context::hasFileCache() const
        {
            return _p->fileCacheEnable;
        }

        const QScopedPointer<bool> & Context::isWindowFullScreen() const
        {
            return _p->windowFullScreen;
        }

        const QScopedPointer<Util::PLAYBACK> & Context::playback() const
        {
            return _p->playback;
        }

        const QScopedPointer<int> & Context::playbackFrame() const
        {
            return _p->playbackFrame;
        }

        const QScopedPointer<Core::Speed> & Context::playbackSpeed() const
        {
            return _p->playbackSpeed;
        }

        FilePrefs * Context::filePrefs() const
        {
            return _p->filePrefs;
        }

        ImagePrefs * Context::imagePrefs() const
        {
            return _p->imagePrefs;
        }

        InputPrefs * Context::inputPrefs() const
        {
            return _p->inputPrefs;
        }

        PlaybackPrefs * Context::playbackPrefs() const
        {
            return _p->playbackPrefs;
        }

        ShortcutPrefs * Context::shortcutPrefs() const
        {
            return _p->shortcutPrefs;
        }

        ViewPrefs * Context::viewPrefs() const
        {
            return _p->viewPrefs;
        }

        WindowPrefs * Context::windowPrefs() const
        {
            return _p->windowPrefs;
        }

        FileCache * Context::fileCache() const
        {
            return _p->fileCache;
        }

        FileSave * Context::fileSave() const
        {
            return _p->fileSave;
        }

        void Context::setValid(bool valid)
        {
            UI::UIContext::setValid(true);
            if (isValid())
            {
                prefsDialog()->addWidget(new FilePrefsWidget(this), "djv_view");
                prefsDialog()->addWidget(new WindowPrefsWidget(this), "djv_view");
                prefsDialog()->addWidget(new ViewPrefsWidget(this), "djv_view");
                prefsDialog()->addWidget(new ImagePrefsWidget(this), "djv_view");
                prefsDialog()->addWidget(new PlaybackPrefsWidget(this), "djv_view");
                prefsDialog()->addWidget(new InputPrefsWidget(this), "djv_view");
                prefsDialog()->addWidget(new ShortcutPrefsWidget(this), "djv_view");
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
                        _p->combine = true;
                    }
                    else if (
                        qApp->translate("djv::ViewLib::Context", "-seq") == arg ||
                        qApp->translate("djv::ViewLib::Context", "-q") == arg)
                    {
                        in >> _p->sequence;
                    }
                    else if (
                        qApp->translate("djv::ViewLib::Context", "-auto_seq") == arg)
                    {
                        in >> _p->autoSequence;
                    }

                    // Parse the file options.
                    else if (
                        qApp->translate("djv::ViewLib::Context", "-file_layer") == arg)
                    {
                        int value = 0;
                        in >> value;
                        _p->fileLayer.reset(new int(value));
                    }
                    else if (
                        qApp->translate("djv::ViewLib::Context", "-file_proxy") == arg)
                    {
                        Graphics::PixelDataInfo::PROXY value = static_cast<Graphics::PixelDataInfo::PROXY>(0);
                        in >> value;
                        _p->fileProxy.reset(new Graphics::PixelDataInfo::PROXY(value));
                    }
                    else if (
                        qApp->translate("djv::ViewLib::Context", "-file_cache") == arg)
                    {
                        bool value = false;
                        in >> value;
                        _p->fileCacheEnable.reset(new bool(value));
                    }

                    // Parse the window options.
                    else if (
                        qApp->translate("djv::ViewLib::Context", "-window_full_screen") == arg)
                    {
                        _p->windowFullScreen.reset(new bool(true));
                    }

                    // Parse the playback options.
                    else if (
                        qApp->translate("djv::ViewLib::Context", "-playback") == arg)
                    {
                        Util::PLAYBACK value =
                            static_cast<Util::PLAYBACK>(0);
                        in >> value;
                        _p->playback.reset(new Util::PLAYBACK(value));
                    }
                    else if (
                        qApp->translate("djv::ViewLib::Context", "-playback_frame") == arg)
                    {
                        int value = 0;
                        in >> value;
                        _p->playbackFrame.reset(new int(value));
                    }
                    else if (
                        qApp->translate("djv::ViewLib::Context", "-playback_speed") == arg)
                    {
                        Core::Speed::FPS value = static_cast<Core::Speed::FPS>(0);
                        in >> value;
                        _p->playbackSpeed.reset(new Core::Speed(value));
                    }

                    // Parse the arguments.
                    else
                    {
                        _p->input += arg;
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
            sequenceLabel << _p->sequence;
            QStringList autoSequenceLabel;
            autoSequenceLabel << _p->autoSequence;
            return QString(label).
                arg(Core::Sequence::compressLabels().join(", ")).
                arg(sequenceLabel.join(", ")).
                arg(Core::StringUtil::boolLabels().join(", ")).
                arg(autoSequenceLabel.join(", ")).
                arg(Graphics::PixelDataInfo::proxyLabels().join(", ")).
                arg(Core::StringUtil::boolLabels().join(", ")).
                arg(Util::playbackLabels().join(", ")).
                arg(Core::Speed::fpsLabels().join(", ")).
                arg(UI::UIContext::commandLineHelp());
        }

    } // namespace ViewLib
} // namespace djv
