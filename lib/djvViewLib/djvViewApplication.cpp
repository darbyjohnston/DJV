//------------------------------------------------------------------------------
// Copyright (c) 2004-2014 Darby Johnston
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

//! \file djvViewApplication.cpp

#include <djvViewApplication.h>

#include <djvViewFileCache.h>
#include <djvViewFileSave.h>
#include <djvViewFilePrefs.h>
#include <djvViewFilePrefsWidget.h>
#include <djvViewImagePrefs.h>
#include <djvViewImagePrefsWidget.h>
#include <djvViewInputPrefs.h>
#include <djvViewInputPrefsWidget.h>
#include <djvViewMainWindow.h>
#include <djvViewPlaybackPrefs.h>
#include <djvViewPlaybackPrefsWidget.h>
#include <djvViewShortcutPrefs.h>
#include <djvViewShortcutPrefsWidget.h>
#include <djvViewViewPrefs.h>
#include <djvViewViewPrefsWidget.h>
#include <djvViewWindowPrefs.h>
#include <djvViewWindowPrefsWidget.h>

#include <djvPrefsDialog.h>

#include <djvError.h>
#include <djvFileInfoUtil.h>
#include <djvStringUtil.h>

//------------------------------------------------------------------------------
// djvViewApplication::P
//------------------------------------------------------------------------------

struct djvViewApplication::P
{
    QStringList input;

    struct CommandLine
    {
        QScopedPointer<bool>                    fileSequenceAuto;
        QScopedPointer<int>                     fileLayer;
        QScopedPointer<djvPixelDataInfo::PROXY> fileProxy;
        QScopedPointer<bool>                    fileCacheEnabled;
        QScopedPointer<bool>                    windowFullScreen;
        QScopedPointer<djvView::PLAYBACK>       playback;
        QScopedPointer<int>                     playbackFrame;
        QScopedPointer<djvSpeed>                playbackSpeed;
    };
    
    CommandLine commandLine;
};

//------------------------------------------------------------------------------
// djvViewApplication
//------------------------------------------------------------------------------

djvViewApplication::djvViewApplication(int & argc, char ** argv) throw (djvError) :
    djvApplication("djv_view", argc, argv),
    _p(new P)
{
    //DJV_DEBUG("djvViewApplication::djvViewApplication");
    
    //
    // Initialize the preferences.
    //

    djvViewFilePrefs::global();
    djvViewImagePrefs::global();
    djvViewInputPrefs::global();
    djvViewPlaybackPrefs::global();
    djvViewShortcutPrefs::global();
    djvViewViewPrefs::global();
    djvViewWindowPrefs::global();

    //
    // Initialize the objects.
    //

    djvViewFileCache::global();
    djvViewFileSave::global();

    // Parse the command line.

    try
    {
        commandLine(_commandLineArgs);
    }
    catch (const djvError & error)
    {
        printError(djvError(QString(errorCommandLine).arg(error.string())));
        
        setExitValue(EXIT_VALUE_ERROR);
    }

    if (exitValue() != EXIT_VALUE_DEFAULT)
        return;

    // Initialize the user interface.

    setValid(true);
    
    setWindowIcon(QPixmap(":projector32x32.png"));

    prefsDialog()->addWidget(new djvViewFilePrefsWidget, "djv_view");
    prefsDialog()->addWidget(new djvViewWindowPrefsWidget, "djv_view");
    prefsDialog()->addWidget(new djvViewViewPrefsWidget, "djv_view");
    prefsDialog()->addWidget(new djvViewImagePrefsWidget, "djv_view");
    prefsDialog()->addWidget(new djvViewPlaybackPrefsWidget, "djv_view");
    prefsDialog()->addWidget(new djvViewInputPrefsWidget, "djv_view");
    prefsDialog()->addWidget(new djvViewShortcutPrefsWidget, "djv_view");

    // Show main window(s).

    if (_p->input.count())
    {
        // Combine command line arguments.

        if (djvViewFilePrefs::global()->hasCombineCommandLine())
        {
            djvFileInfo fileInfo(_p->input[0]);

            if (fileInfo.isSequenceValid())
            {
                fileInfo.setType(djvFileInfo::SEQUENCE);
            }

            for (int i = 1; i < _p->input.count(); ++i)
            {
                fileInfo.addSequence(_p->input[i]);
            }

            fileInfo.sortSequence();

            _p->input.clear();
            _p->input += fileInfo;
        }

        // Create and show a window for each input.

        for (int i = 0; i < _p->input.count(); ++i)
        {
            djvFileInfo file(_p->input[i]);

            if (file.isSequenceValid())
            {
                file.setType(djvFileInfo::SEQUENCE);
            }

            djvViewMainWindow * window = this->window();

            window->fileOpen(file);

            if (_p->commandLine.fileLayer.data())
            {
                window->setFileLayer(*_p->commandLine.fileLayer);
            }

            if (_p->commandLine.playback.data())
            {
                window->setPlayback(*_p->commandLine.playback);
            }

            if (_p->commandLine.playbackFrame.data())
            {
                window->setPlaybackFrame(*_p->commandLine.playbackFrame);
            }

            if (_p->commandLine.playbackSpeed.data())
            {
                window->setPlaybackSpeed(*_p->commandLine.playbackSpeed);
            }

            window->show();
        }
    }
    else
    {
        // Create and show an empty window.

        window()->show();
    }
}

djvViewApplication::~djvViewApplication()
{
    //DJV_DEBUG("djvViewApplication::~djvViewApplication");

    delete _p;
}

void djvViewApplication::commandLine(QStringList & in) throw (djvError)
{
    //DJV_DEBUG("djvViewApplication::commandLine");
    //DJV_DEBUG_PRINT("in = " << in);

    djvApplication::commandLine(in);

    if (exitValue() != EXIT_VALUE_DEFAULT)
        return;

    QString arg;

    try
    {
        while (! in.isEmpty())
        {
            in >> arg;

            // File options.

            if ("-file_seq_auto" == arg)
            {
                bool value = 0;
                in >> value;
                _p->commandLine.fileSequenceAuto.reset(new bool(value));
            }
            else if ("-file_layer" == arg)
            {
                int value = 0;
                in >> value;
                _p->commandLine.fileLayer.reset(new int(value));
            }
            else if ("-file_proxy" == arg)
            {
                djvPixelDataInfo::PROXY value = static_cast<djvPixelDataInfo::PROXY>(0);
                in >> value;
                _p->commandLine.fileProxy.reset(new djvPixelDataInfo::PROXY(value));
            }
            else if ("-file_cache" == arg)
            {
                bool value = false;
                in >> value;
                _p->commandLine.fileCacheEnabled.reset(new bool(value));
            }

            // Window options.

            else if ("-window_full_screen" == arg)
            {
                _p->commandLine.windowFullScreen.reset(new bool(true));
            }

            // Playback options.

            else if ("-playback" == arg)
            {
                djvView::PLAYBACK value = static_cast<djvView::PLAYBACK>(0);
                in >> value;
                _p->commandLine.playback.reset(new djvView::PLAYBACK(value));
            }
            else if ("-playback_frame" == arg)
            {
                int value = 0;
                in >> value;
                _p->commandLine.playbackFrame.reset(new int(value));
            }
            else if ("-playback_speed" == arg)
            {
                djvSpeed::FPS value = static_cast<djvSpeed::FPS>(0);
                in >> value;
                _p->commandLine.playbackSpeed.reset(new djvSpeed(value));
            }

#           if defined(DJV_WINDOWS)

            //! \todo Remove these arguments on Windows which only happens
            //! when running djv_view after its been installed?

            else if (0 == QString("Files\\%1\\bin\\djv_view.exe\"").
                arg(DJV_PACKAGE_NAME).compare(arg, Qt::CaseInsensitive))
            {}
            else if (0 == QString("(x86)\\%1\\bin\\djv_view.exe\"").
                arg(DJV_PACKAGE_NAME).compare(arg, Qt::CaseInsensitive))
            {}
            else if (0 == QString("Files").
                arg(DJV_PACKAGE_NAME).compare(arg, Qt::CaseInsensitive))
            {}

#           endif // DJV_WINDOWS

            // Arguments.

            else
            {
                _p->input += arg;
            }
        }
    }
    catch (const QString &)
    {
        throw djvError(arg);
    }
}

namespace
{

const QString commandLineHelpLabel =
"\n"
" djv_view\n"
"\n"
"     The djv_view application provides real-time image sequence and movie "
"playback. It is intended as a professional tool for reviewing computer "
"animation, film, and video footage.\n"
"\n"
" Usage\n"
"\n"
"     djv_view [image]... [option]...\n"
"\n"
" File Options\n"
"\n"
"     -file_seq_auto (value)\n"
"         Automatically detect sequences when opening files. Options = %1.\n"
"     -file_layer (value)\n"
"         Set the input layer. Default = %2.\n"
"     -file_proxy (value)\n"
"         Set the proxy scale. Options = %3.\n"
"     -file_cache (value)\n"
"         Set whether the file cache is enabled. Options = %4.\n"
"\n"
" Window Options\n"
"\n"
"     -window_full_screen\n"
"         Set the window full screen.\n"
"\n"
" Playback Options\n"
"\n"
"     -playback (value)\n"
"         Set the playback. Options = %5.\n"
"     -playback_frame (value)\n"
"         Set the playback frame.\n"
"     -playback_speed (value)\n"
"         Set the playback speed. Options = %6.\n"
"%7";

} // namespace

QString djvViewApplication::commandLineHelp() const
{
    return QString(commandLineHelpLabel).
        arg(djvStringUtil::boolLabels().join(", ")).
        arg(0).
        arg(djvPixelDataInfo::proxyLabels().join(", ")).
        arg(djvStringUtil::boolLabels().join(", ")).
        arg(djvView::playbackLabels().join(", ")).
        arg(djvSpeed::fpsLabels().join(", ")).
        arg(djvApplication::commandLineHelp());
}

djvViewMainWindow * djvViewApplication::window() const
{
    djvViewMainWindow * out = new djvViewMainWindow;

    // Apply command line file options.

    if (_p->commandLine.fileSequenceAuto.data())
    {
        out->setAutoSequence(*_p->commandLine.fileSequenceAuto);
    }

    if (_p->commandLine.fileLayer.data())
    {
        out->setFileLayer(*_p->commandLine.fileLayer);
    }

    if (_p->commandLine.fileProxy.data())
    {
        out->setFileProxy(*_p->commandLine.fileProxy);
    }

    if (_p->commandLine.fileCacheEnabled.data())
    {
        out->setFileCacheEnabled(*_p->commandLine.fileCacheEnabled);
    }

    // Apply command line window options.

    if (_p->commandLine.windowFullScreen.data() &&
        *_p->commandLine.windowFullScreen)
    {
        out->showFullScreen();
    }

    // Apply command line playback options.

    if (_p->commandLine.playback.data())
    {
        out->setPlayback(*_p->commandLine.playback);
    }

    if (_p->commandLine.playbackFrame.data())
    {
        out->setPlaybackFrame(*_p->commandLine.playbackFrame);
    }

    if (_p->commandLine.playbackSpeed.data())
    {
        out->setPlaybackSpeed(*_p->commandLine.playbackSpeed);
    }

    return out;
}
