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

//! \file djvGuiContext.cpp

#include <djvGuiContext.h>

#include <djvAboutDialog.h>
#include <djvDebugLogDialog.h>
#include <djvFileBrowser.h>
#include <djvFileBrowserCache.h>
#include <djvFileBrowserPrefs.h>
#include <djvHelpPrefs.h>
#include <djvIconLibrary.h>
#include <djvImageIoPrefs.h>
#include <djvImageIoWidget.h>
#include <djvImagePrefs.h>
#include <djvInfoDialog.h>
#include <djvMessagesDialog.h>
#include <djvPlaybackUtil.h>
#include <djvPrefs.h>
#include <djvPrefsDialog.h>
#include <djvSequencePrefs.h>
#include <djvStyle.h>
#include <djvTimePrefs.h>

#include <djvDebug.h>
#include <djvDebugLog.h>

#include <QApplication>
#include <QDesktopServices>
#include <QScopedPointer>
#include <QThreadPool>
#include <QUrl>

//------------------------------------------------------------------------------
// djvGuiContextPrivate
//------------------------------------------------------------------------------

struct djvGuiContextPrivate
{
    djvGuiContextPrivate() :
        valid(false)
    {}

    bool                                    valid;
    QScopedPointer<djvIconLibrary>          iconLibrary;
    QScopedPointer<djvStyle>                style;
    QScopedPointer<djvFileBrowserPrefs>     fileBrowserPrefs;
    QScopedPointer<djvHelpPrefs>            helpPrefs;
    QScopedPointer<djvImagePrefs>           imagePrefs;
    QScopedPointer<djvImageIoPrefs>         imageIoPrefs;
    QScopedPointer<djvSequencePrefs>        sequencePrefs;
    QScopedPointer<djvTimePrefs>            timePrefs;
    QScopedPointer<djvFileBrowserCache>     fileBrowserCache;
    QScopedPointer<djvImageIoWidgetFactory> imageIoWidgetFactory;
    QScopedPointer<djvPrefsDialog>          prefsDialog;
    QScopedPointer<djvInfoDialog>           infoDialog;
    QScopedPointer<djvAboutDialog>          aboutDialog;
    QScopedPointer<djvMessagesDialog>       messagesDialog;
    QScopedPointer<djvDebugLogDialog>       debugLogDialog;
    QScopedPointer<djvFileBrowser>          fileBrowser;
};

//------------------------------------------------------------------------------
// djvGuiContext
//------------------------------------------------------------------------------

djvGuiContext::djvGuiContext(QObject * parent) :
    djvImageContext(parent),
    _p(new djvGuiContextPrivate)
{
    //DJV_DEBUG("djvGuiContext::djvGuiContext");

    Q_INIT_RESOURCE(djvGui);
    
    // Register meta types.
    
    qRegisterMetaType<djvPlaybackUtil::PLAYBACK>("djvPlaybackUtil::PLAYBACK");

    // Load translators.

    loadTranslator("djvGui");
    
    //! \todo Check the command line for whether to reset the preferences.
    
    Q_FOREACH(const QString & arg, qApp->arguments())
    {
        if (qApp->translate("djvGuiContext", "-reset_prefs") == arg)
        {
            DJV_LOG(debugLog(), "djvGuiContext", "Reset the preferences...");
            
            djvPrefs::setReset(true);
        }
    }

    // Load preferences.
    
    DJV_LOG(debugLog(), "djvAbstractApplication", "Load the preferences...");
    
    _p->fileBrowserPrefs.reset(new djvFileBrowserPrefs(this));
    _p->helpPrefs.reset(new djvHelpPrefs);
    _p->imagePrefs.reset(new djvImagePrefs);
    _p->imageIoPrefs.reset(new djvImageIoPrefs(this));
    _p->sequencePrefs.reset(new djvSequencePrefs);
    _p->timePrefs.reset(new djvTimePrefs);

    DJV_LOG(debugLog(), "djvAbstractImageApplication", "");

    // Initialize objects.
    
    _p->fileBrowserCache.reset(new djvFileBrowserCache);
    _p->fileBrowserCache->setMaxCost(fileBrowserPrefs()->thumbnailsCache());
    
    _p->iconLibrary.reset(new djvIconLibrary);
    _p->style.reset(new djvStyle);

    DJV_LOG(debugLog(), "djvAbstractApplication", "Information:");
    DJV_LOG(debugLog(), "djvAbstractApplication", "");
    DJV_LOG(debugLog(), "djvAbstractApplication", info());
}

djvGuiContext::~djvGuiContext()
{
    //DJV_DEBUG("djvGuiContext::~djvGuiContext");
    
    QThreadPool::globalInstance()->waitForDone();
    
    delete _p;
}

bool djvGuiContext::isValid() const
{
    return _p->valid;
}

void djvGuiContext::setValid(bool in)
{
    //DJV_DEBUG("djvGuiContext::setValid");
    //DJV_DEBUG_PRINT("in = " << in);

    _p->valid = in;
}

void djvGuiContext::help()
{
    //DJV_DEBUG("djvGuiContext::help");
    //DJV_DEBUG_PRINT("url = " << doc());
    
    QDesktopServices::openUrl(QUrl::fromLocalFile(doc()));
}

djvFileBrowser * djvGuiContext::fileBrowser(const QString & title) const
{
    if (! _p->fileBrowser)
    {
        djvGuiContext * that = const_cast<djvGuiContext *>(this);
        
        _p->fileBrowser.reset(new djvFileBrowser(that));
    }
    
    _p->fileBrowser->close();

    _p->fileBrowser->setWindowTitle(
        ! title.isEmpty() ?
            title :
            qApp->translate("djvGuiContext", "File Browser"));
    
    _p->fileBrowser->setPinnable(false);
    
    _p->fileBrowser->disconnect(SIGNAL(fileInfoChanged(const djvFileInfo &)));

    return _p->fileBrowser.data();
}

djvImageIoWidgetFactory * djvGuiContext::imageIoWidgetFactory() const
{
    if (! _p->imageIoWidgetFactory)
    {
        djvGuiContext * that = const_cast<djvGuiContext *>(this);
        
        _p->imageIoWidgetFactory.reset(new djvImageIoWidgetFactory(that));
    }
    
    return _p->imageIoWidgetFactory.data();
}

djvPrefsDialog * djvGuiContext::prefsDialog() const
{
    if (! _p->prefsDialog)
    {
        djvGuiContext * that = const_cast<djvGuiContext *>(this);
        
        _p->prefsDialog.reset(new djvPrefsDialog(that));
    }
    
    return _p->prefsDialog.data();
}

djvInfoDialog * djvGuiContext::infoDialog() const
{
    if (! _p->infoDialog)
    {
        djvGuiContext * that = const_cast<djvGuiContext *>(this);
        
        _p->infoDialog.reset(new djvInfoDialog(info(), that));
    }
    
    return _p->infoDialog.data();
}

djvAboutDialog * djvGuiContext::aboutDialog() const
{
    if (! _p->aboutDialog)
    {
        djvGuiContext * that = const_cast<djvGuiContext *>(this);
        
        _p->aboutDialog.reset(new djvAboutDialog(about(), that));
    }
    
    return _p->aboutDialog.data();
}

djvMessagesDialog * djvGuiContext::messagesDialog() const
{
    if (! _p->messagesDialog)
    {
        djvGuiContext * that = const_cast<djvGuiContext *>(this);
        
        _p->messagesDialog.reset(new djvMessagesDialog(that));
    }
    
    return _p->messagesDialog.data();
}

djvDebugLogDialog * djvGuiContext::debugLogDialog() const
{
    if (! _p->debugLogDialog)
    {
        djvGuiContext * that = const_cast<djvGuiContext *>(this);
        
        _p->debugLogDialog.reset(new djvDebugLogDialog(that));
    }
    
    return _p->debugLogDialog.data();
}

djvFileBrowserPrefs * djvGuiContext::fileBrowserPrefs() const
{
    return _p->fileBrowserPrefs.data();
}

djvHelpPrefs * djvGuiContext::helpPrefs() const
{
    return _p->helpPrefs.data();
}

djvImagePrefs * djvGuiContext::imagePrefs() const
{
    return _p->imagePrefs.data();
}

djvImageIoPrefs * djvGuiContext::imageIoPrefs() const
{
    return _p->imageIoPrefs.data();
}

djvSequencePrefs * djvGuiContext::sequencePrefs() const
{
    return _p->sequencePrefs.data();
}

djvTimePrefs * djvGuiContext::timePrefs() const
{
    return _p->timePrefs.data();
}
    
djvIconLibrary * djvGuiContext::iconLibrary() const
{
    return _p->iconLibrary.data();
}

djvStyle * djvGuiContext::style() const
{
    return _p->style.data();
}

djvFileBrowserCache * djvGuiContext::fileBrowserCache() const
{
    return _p->fileBrowserCache.data();
}

QString djvGuiContext::info() const
{
    static const QString label = qApp->translate("djvGuiContext",
"%1"
"\n"
"Image I/O Widgets\n"
"\n"
"    %2\n"
"\n"
"Preferences\n"
"\n"
"    User: %3\n"
"    System: %4\n");

    return QString(label).
        arg(djvImageContext::info()).
        arg(imageIoWidgetFactory()->names().join(", ")).
        arg(djvPrefs(QString(), djvPrefs::USER).fileName()).
        arg(djvPrefs(QString(), djvPrefs::SYSTEM).fileName());
}

bool djvGuiContext::commandLineParse(QStringList & in) throw (QString)
{
    //DJV_DEBUG("djvGuiContext::commandLineParse");
    //DJV_DEBUG_PRINT("in = " << in);

    if (! djvImageContext::commandLineParse(in))
        return false;

    QStringList tmp;
    QString     arg;

    try
    {
        while (!in.isEmpty())
        {
            in >> arg;

            if (qApp->translate("djvGuiContext", "-reset_prefs") == arg)
            {}
            else
            {
                tmp << arg;
            }
        }
    }
    catch (const QString &)
    {
        in = tmp;
        
        throw QString(arg);
    }

    in = tmp;
    
    return true;
}

QString djvGuiContext::commandLineHelp() const
{
    static const QString label = qApp->translate("djvGuiContext",
"\n"
"User Interface Options\n"
"\n"
"    -reset_prefs\n"
"        Do not load the preference at start up.\n"
"%2");

    return QString(label).
        arg(djvImageContext::commandLineHelp());
}

void djvGuiContext::print(const QString & string)
{
    if (_p->valid)
    {
        messagesDialog()->message(string);
    }
    else
    {
        djvImageContext::print(string);
    }
}
