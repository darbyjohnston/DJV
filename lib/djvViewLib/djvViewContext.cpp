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

#include <djvViewContext.h>

#include <djvViewFileCache.h>
#include <djvViewFilePrefsWidget.h>
#include <djvViewFileSave.h>
#include <djvViewFilePrefs.h>
#include <djvViewImagePrefs.h>
#include <djvViewImagePrefsWidget.h>
#include <djvViewInputPrefs.h>
#include <djvViewInputPrefsWidget.h>
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

#include <QApplication>

//------------------------------------------------------------------------------
// djvViewContext::Private
//------------------------------------------------------------------------------

struct djvViewContext::Private
{
    Private() :
        combine      (false),
        sequence     (djvSequence::COMPRESS_RANGE),
        autoSequence (true),
        filePrefs    (0),
        imagePrefs   (0),
        inputPrefs   (0),
        playbackPrefs(0),
        shortcutPrefs(0),
        viewPrefs    (0),
        windowPrefs  (0),
        fileCache    (0),
        fileSave     (0)
    {}
    
    QStringList                             input;
    bool                                    combine;
    djvSequence::COMPRESS                   sequence;
    bool                                    autoSequence;
    QScopedPointer<int>                     fileLayer;
    QScopedPointer<djvPixelDataInfo::PROXY> fileProxy;
    QScopedPointer<bool>                    fileCacheEnable;
    QScopedPointer<bool>                    windowFullScreen;
    QScopedPointer<djvViewUtil::PLAYBACK>   playback;
    QScopedPointer<int>                     playbackFrame;
    QScopedPointer<djvSpeed>                playbackSpeed;
    
    djvViewFilePrefs *                      filePrefs;
    djvViewImagePrefs *                     imagePrefs;
    djvViewInputPrefs *                     inputPrefs;
    djvViewPlaybackPrefs *                  playbackPrefs;
    djvViewShortcutPrefs *                  shortcutPrefs;
    djvViewViewPrefs *                      viewPrefs;
    djvViewWindowPrefs *                    windowPrefs;

    djvViewFileCache *                      fileCache;
    djvViewFileSave *                       fileSave;
};

//------------------------------------------------------------------------------
// djvViewContext
//------------------------------------------------------------------------------

djvViewContext::djvViewContext(QObject * parent) :
    djvGuiContext(parent),
    _p(new Private)
{
    //DJV_DEBUG("djvViewContext::djvViewContext");
    
    // Load translators.
    loadTranslator("djvViewLib");
    
    // Load preferences.
    DJV_LOG(debugLog(), "djvViewContext", "Load the preferences...");
    _p->filePrefs = new djvViewFilePrefs(this);
    _p->imagePrefs = new djvViewImagePrefs(this);
    _p->inputPrefs = new djvViewInputPrefs(this);
    _p->playbackPrefs = new djvViewPlaybackPrefs(this);
    _p->shortcutPrefs = new djvViewShortcutPrefs(this);
    _p->viewPrefs = new djvViewViewPrefs(this);
    _p->windowPrefs = new djvViewWindowPrefs(this);
    DJV_LOG(debugLog(), "djvViewContext", "");
    
    // Initialize objects.
    DJV_LOG(debugLog(), "djvViewContext", "Initialize objects...");
    _p->fileCache = new djvViewFileCache(this);
    _p->fileSave = new djvViewFileSave(this);
    DJV_LOG(debugLog(), "djvViewContext", "");
}

djvViewContext::~djvViewContext()
{
    //DJV_DEBUG("djvViewContext::djvViewContext");
    
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

const QStringList & djvViewContext::input() const
{
    return _p->input;
}

bool djvViewContext::hasCombine() const
{
    return _p->combine;
}

djvSequence::COMPRESS djvViewContext::sequence() const
{
    return _p->sequence;
}

bool djvViewContext::hasAutoSequence() const
{
    return _p->autoSequence;
}

const QScopedPointer<int> & djvViewContext::fileLayer() const
{
    return _p->fileLayer;
}

const QScopedPointer<djvPixelDataInfo::PROXY> & djvViewContext::fileProxy() const
{
    return _p->fileProxy;
}

const QScopedPointer<bool> & djvViewContext::hasFileCache() const
{
    return _p->fileCacheEnable;
}

const QScopedPointer<bool> & djvViewContext::isWindowFullScreen() const
{
    return _p->windowFullScreen;
}

const QScopedPointer<djvViewUtil::PLAYBACK> & djvViewContext::playback() const
{
    return _p->playback;
}

const QScopedPointer<int> & djvViewContext::playbackFrame() const
{
    return _p->playbackFrame;
}

const QScopedPointer<djvSpeed> & djvViewContext::playbackSpeed() const
{
    return _p->playbackSpeed;
}
    
djvViewFilePrefs * djvViewContext::filePrefs() const
{
    return _p->filePrefs;
}

djvViewImagePrefs * djvViewContext::imagePrefs() const
{
    return _p->imagePrefs;
}

djvViewInputPrefs * djvViewContext::inputPrefs() const
{
    return _p->inputPrefs;
}

djvViewPlaybackPrefs * djvViewContext::playbackPrefs() const
{
    return _p->playbackPrefs;
}

djvViewShortcutPrefs * djvViewContext::shortcutPrefs() const
{
    return _p->shortcutPrefs;
}

djvViewViewPrefs * djvViewContext::viewPrefs() const
{
    return _p->viewPrefs;
}

djvViewWindowPrefs * djvViewContext::windowPrefs() const
{
    return _p->windowPrefs;
}

djvViewFileCache * djvViewContext::fileCache() const
{
    return _p->fileCache;
}

djvViewFileSave * djvViewContext::fileSave() const
{
    return _p->fileSave;
}

void djvViewContext::setValid(bool valid)
{
    djvGuiContext::setValid(true);    
    if (isValid())
    {
        prefsDialog()->addWidget(
            new djvViewFilePrefsWidget(this), "djv_view");
        prefsDialog()->addWidget(
            new djvViewWindowPrefsWidget(this), "djv_view");
        prefsDialog()->addWidget(
            new djvViewViewPrefsWidget(this), "djv_view");
        prefsDialog()->addWidget(
            new djvViewImagePrefsWidget(this), "djv_view");
        prefsDialog()->addWidget(
            new djvViewPlaybackPrefsWidget(this), "djv_view");
        prefsDialog()->addWidget(
            new djvViewInputPrefsWidget(this), "djv_view");
        prefsDialog()->addWidget(
            new djvViewShortcutPrefsWidget(this), "djv_view");
    }
}

bool djvViewContext::commandLineParse(QStringList & in) throw (QString)
{
    //DJV_DEBUG("djvViewContext::commandLineParse");
    //DJV_DEBUG_PRINT("in = " << in);

    if (! djvGuiContext::commandLineParse(in))
        return false;
    QString arg;
    try
    {
        while (! in.isEmpty())
        {
            in >> arg;

            // Parse the options.
            if (qApp->translate("djvViewContext", "-combine") == arg)
            {
                _p->combine = true;
            }
            else if (
                qApp->translate("djvViewContext", "-seq") == arg ||
                qApp->translate("djvViewContext", "-q") == arg)
            {
                in >> _p->sequence;
            }
            else if (
                qApp->translate("djvViewContext", "-auto_seq") == arg)
            {
                in >> _p->autoSequence;
            }

            // Parse the file options.
            else if (
                qApp->translate("djvViewContext", "-file_layer") == arg)
            {
                int value = 0;
                in >> value;
                _p->fileLayer.reset(new int(value));
            }
            else if (
                qApp->translate("djvViewContext", "-file_proxy") == arg)
            {
                djvPixelDataInfo::PROXY value =
                    static_cast<djvPixelDataInfo::PROXY>(0);
                in >> value;
                _p->fileProxy.reset(new djvPixelDataInfo::PROXY(value));
            }
            else if (
                qApp->translate("djvViewContext", "-file_cache") == arg)
            {
                bool value = false;
                in >> value;
                _p->fileCacheEnable.reset(new bool(value));
            }

            // Parse the window options.
            else if (
                qApp->translate("djvViewContext", "-window_full_screen") == arg)
            {
                _p->windowFullScreen.reset(new bool(true));
            }

            // Parse the playback options.
            else if (
                qApp->translate("djvViewContext", "-playback") == arg)
            {
                djvViewUtil::PLAYBACK value =
                    static_cast<djvViewUtil::PLAYBACK>(0);
                in >> value;
                _p->playback.reset(new djvViewUtil::PLAYBACK(value));
            }
            else if (
                qApp->translate("djvViewContext", "-playback_frame") == arg)
            {
                int value = 0;
                in >> value;
                _p->playbackFrame.reset(new int(value));
            }
            else if (
                qApp->translate("djvViewContext", "-playback_speed") == arg)
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
    return true;
}

QString djvViewContext::commandLineHelp() const
{
    static const QString label = qApp->translate("djvViewContext",
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
    return QString(label).
        arg(djvSequence::compressLabels().join(", ")).
        arg(djvStringUtil::label(_p->sequence).join(", ")).
        arg(djvStringUtil::boolLabels().join(", ")).
        arg(djvStringUtil::label(_p->autoSequence).join(", ")).
        arg(djvPixelDataInfo::proxyLabels().join(", ")).
        arg(djvStringUtil::boolLabels().join(", ")).
        arg(djvViewUtil::playbackLabels().join(", ")).
        arg(djvSpeed::fpsLabels().join(", ")).
        arg(djvGuiContext::commandLineHelp());
}
