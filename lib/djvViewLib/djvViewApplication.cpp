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

#include <djvDebugLog.h>
#include <djvError.h>
#include <djvFileInfoUtil.h>

#include <QFileOpenEvent>

//------------------------------------------------------------------------------
// djvViewApplication::P
//------------------------------------------------------------------------------

struct djvViewApplication::P
{
    P() :
        combine (false),
        sequence(djvSequence::COMPRESS_RANGE)
    {}
    
    QStringList                             input;
    bool                                    combine;
    djvSequence::COMPRESS                   sequence;
    QScopedPointer<bool>                    fileSequenceAuto;
    QScopedPointer<int>                     fileLayer;
    QScopedPointer<djvPixelDataInfo::PROXY> fileProxy;
    QScopedPointer<bool>                    fileCacheEnabled;
    QScopedPointer<bool>                    windowFullScreen;
    QScopedPointer<djvViewUtil::PLAYBACK>   playback;
    QScopedPointer<int>                     playbackFrame;
    QScopedPointer<djvSpeed>                playbackSpeed;
};

//------------------------------------------------------------------------------
// djvViewApplication
//------------------------------------------------------------------------------

djvViewApplication::djvViewApplication(int & argc, char ** argv) throw (djvError) :
    djvApplication("djv_view", argc, argv),
    _p(new P)
{
    //DJV_DEBUG("djvViewApplication::djvViewApplication");
    
    loadTranslator("djvViewLib");
    
    //
    // Load the preferences.
    //
    
    DJV_LOG("djvViewApplication", "Load the preferences...");

    djvViewFilePrefs::global();
    djvViewImagePrefs::global();
    djvViewInputPrefs::global();
    djvViewPlaybackPrefs::global();
    djvViewShortcutPrefs::global();
    djvViewViewPrefs::global();
    djvViewWindowPrefs::global();
    
    DJV_LOG("djvViewApplication", "");

    //
    // Initialize objects.
    //
    
    DJV_LOG("djvViewApplication", "Initialize objects...");

    djvViewFileCache::global();
    djvViewFileSave::global();
    
    DJV_LOG("djvViewApplication", "");

    // Parse the command line.

    try
    {
        commandLine(_commandLineArgs);
    }
    catch (const QString & error)
    {
        printError(
            djvApplication::errorLabels()[ERROR_COMMAND_LINE].arg(error));
        
        setExitValue(djvApplicationEnum::EXIT_ERROR);
    }

    if (hasDebugLog())
    {
        Q_FOREACH(const QString & message, djvDebugLog::global()->messages())
        {
            printMessage(message);
        }
        
        connect(
            djvDebugLog::global(),
            SIGNAL(message(const QString &)),
            SLOT(debugLogCallback(const QString &)));
    }

    if (exitValue() != djvApplicationEnum::EXIT_DEFAULT)
        return;

    // Initialize user interface.
    
    DJV_LOG("djvViewApplication", "Initialize user interface...");

    disconnect(
        djvDebugLog::global(),
        SIGNAL(message(const QString &)),
        this,
        SLOT(debugLogCallback(const QString &)));
    
    setValid(true);
        
    setWindowIcon(QPixmap(":projector32x32.png"));

    djvPrefsDialog::global()->addWidget(
        new djvViewFilePrefsWidget, "djv_view");
    djvPrefsDialog::global()->addWidget(
        new djvViewWindowPrefsWidget, "djv_view");
    djvPrefsDialog::global()->addWidget(
        new djvViewViewPrefsWidget, "djv_view");
    djvPrefsDialog::global()->addWidget(
        new djvViewImagePrefsWidget, "djv_view");
    djvPrefsDialog::global()->addWidget(
        new djvViewPlaybackPrefsWidget, "djv_view");
    djvPrefsDialog::global()->addWidget(
        new djvViewInputPrefsWidget, "djv_view");
    djvPrefsDialog::global()->addWidget(
        new djvViewShortcutPrefsWidget, "djv_view");
    
    DJV_LOG("djvViewApplication", "");

    // Show main window(s).

    if (_p->input.count())
    {
        // Combine command line arguments.

        if (_p->combine)
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
            // Parse the input.
            
            djvFileInfo fileInfo =
                djvFileInfoUtil::commandLine(_p->input[i], _p->sequence);
            
            DJV_LOG("djvViewApplication", QString("Input = \"%1\"").arg(fileInfo));
            
            // Initialize the window.
    
            djvViewMainWindow * window = this->window();

            window->fileOpen(fileInfo);

            if (_p->fileLayer.data())
            {
                window->setFileLayer(*_p->fileLayer);
            }

            if (_p->playback.data())
            {
                window->setPlayback(*_p->playback);
            }

            if (_p->playbackFrame.data())
            {
                window->setPlaybackFrame(*_p->playbackFrame);
            }

            if (_p->playbackSpeed.data())
            {
                window->setPlaybackSpeed(*_p->playbackSpeed);
            }
            
            DJV_LOG("djvViewApplication", "Show window...");

            window->show();
            
            DJV_LOG("djvViewApplication", "");
        }
    }
    else
    {
        // Create and show an empty window.
        
        DJV_LOG("djvViewApplication", "Show window...");

        window()->show();

        DJV_LOG("djvViewApplication", "");
    }
}

djvViewApplication::~djvViewApplication()
{
    //DJV_DEBUG("djvViewApplication::~djvViewApplication");

    delete _p;
}

QString djvViewApplication::doc() const
{
    return docPath() + "djv_view.html";
}

void djvViewApplication::commandLine(QStringList & in) throw (QString)
{
    //DJV_DEBUG("djvViewApplication::commandLine");
    //DJV_DEBUG_PRINT("in = " << in);

    djvApplication::commandLine(in);

    if (exitValue() != djvApplicationEnum::EXIT_DEFAULT)
        return;

    QString arg;

    try
    {
        while (! in.isEmpty())
        {
            in >> arg;

            // Parse the options.
            
            if (tr("-combine") == arg)
            {
                _p->combine = true;
            }
            else if (tr("-seq") == arg || tr("-q") == arg)
            {
                in >> _p->sequence;
            }

            // Parse the file options.

            else if (tr("-file_seq_auto") == arg)
            {
                bool value = 0;
                in >> value;
                _p->fileSequenceAuto.reset(new bool(value));
            }
            else if (tr("-file_layer") == arg)
            {
                int value = 0;
                in >> value;
                _p->fileLayer.reset(new int(value));
            }
            else if (tr("-file_proxy") == arg)
            {
                djvPixelDataInfo::PROXY value =
                    static_cast<djvPixelDataInfo::PROXY>(0);
                in >> value;
                _p->fileProxy.reset(new djvPixelDataInfo::PROXY(value));
            }
            else if (tr("-file_cache") == arg)
            {
                bool value = false;
                in >> value;
                _p->fileCacheEnabled.reset(new bool(value));
            }

            // Parse the window options.

            else if (tr("-window_full_screen") == arg)
            {
                _p->windowFullScreen.reset(new bool(true));
            }

            // Parse the playback options.

            else if (tr("-playback") == arg)
            {
                djvViewUtil::PLAYBACK value =
                    static_cast<djvViewUtil::PLAYBACK>(0);
                in >> value;
                _p->playback.reset(new djvViewUtil::PLAYBACK(value));
            }
            else if (tr("-playback_frame") == arg)
            {
                int value = 0;
                in >> value;
                _p->playbackFrame.reset(new int(value));
            }
            else if (tr("-playback_speed") == arg)
            {
                djvSpeed::FPS value = static_cast<djvSpeed::FPS>(0);
                in >> value;
                _p->playbackSpeed.reset(new djvSpeed(value));
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
}

QString djvViewApplication::commandLineHelp() const
{
    static const QString label = tr(
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
"Options\n"
"\n"
"    -combine\n"
"        Combine multiple command line arguments into a single sequence.\n"
"    -seq, -q (value)\n"
"        Set command line file sequencing. Options = %1. Default = %2.\n"
"\n"
"File Options\n"
"\n"
"    -file_seq_auto (value)\n"
"        Automatically detect sequences when opening files. Options = %3.\n"
"    -file_layer (value)\n"
"        Set the input layer. Default = %4.\n"
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

    return QString(label).
        arg(djvSequence::compressLabels().join(", ")).
        arg(djvStringUtil::label(_p->sequence).join(", ")).
        arg(djvStringUtil::boolLabels().join(", ")).
        arg(0).
        arg(djvPixelDataInfo::proxyLabels().join(", ")).
        arg(djvStringUtil::boolLabels().join(", ")).
        arg(djvViewUtil::playbackLabels().join(", ")).
        arg(djvSpeed::fpsLabels().join(", ")).
        arg(djvApplication::commandLineHelp());
}

bool djvViewApplication::event(QEvent * event)
{
    switch (event->type())
    {
        case QEvent::FileOpen:
        {
            QFileOpenEvent * e = static_cast<QFileOpenEvent *>(event);
            
            QVector<djvViewMainWindow *> mainWindowList =
                djvViewMainWindow::mainWindowList();
        
            if (mainWindowList.count())
            {
                mainWindowList[0]->fileOpen(e->file());
                mainWindowList[0]->raise();
            }
        
        } return true;
        
        default: break;
    }
    
    return djvApplication::event(event);
}

void djvViewApplication::debugLogCallback(const QString & in)
{
    printMessage(in);
}

djvViewMainWindow * djvViewApplication::window() const
{
    djvViewMainWindow * out = new djvViewMainWindow;

    // Apply command line file options.

    if (_p->fileSequenceAuto.data())
    {
        out->setAutoSequence(*_p->fileSequenceAuto);
    }

    if (_p->fileLayer.data())
    {
        out->setFileLayer(*_p->fileLayer);
    }

    if (_p->fileProxy.data())
    {
        out->setFileProxy(*_p->fileProxy);
    }

    if (_p->fileCacheEnabled.data())
    {
        out->setFileCacheEnabled(*_p->fileCacheEnabled);
    }

    // Apply command line window options.

    if (_p->windowFullScreen.data() && *_p->windowFullScreen)
    {
        out->showFullScreen();
    }

    // Apply command line playback options.

    if (_p->playback.data())
    {
        out->setPlayback(*_p->playback);
    }

    if (_p->playbackFrame.data())
    {
        out->setPlaybackFrame(*_p->playbackFrame);
    }

    if (_p->playbackSpeed.data())
    {
        out->setPlaybackSpeed(*_p->playbackSpeed);
    }

    return out;
}
