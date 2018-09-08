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

#include <djvUIContext.h>

#include <djvAboutDialog.h>
#include <djvDebugLogDialog.h>
#include <djvFileBrowser.h>
#include <djvFileBrowserCache.h>
#include <djvFileBrowserPrefs.h>
#include <djvHelpPrefs.h>
#include <djvIconLibrary.h>
#include <djvImageIOPrefs.h>
#include <djvImageIOWidget.h>
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
// djvUIContext::Private
//------------------------------------------------------------------------------

struct djvUIContext::Private
{
    bool                                    valid = false;
    QScopedPointer<djvIconLibrary>          iconLibrary;
    QScopedPointer<djvStyle>                style;
    QScopedPointer<djvFileBrowserPrefs>     fileBrowserPrefs;
    QScopedPointer<djvHelpPrefs>            helpPrefs;
    QScopedPointer<djvImagePrefs>           imagePrefs;
    QScopedPointer<djvImageIOPrefs>         imageIOPrefs;
    QScopedPointer<djvSequencePrefs>        sequencePrefs;
    QScopedPointer<djvTimePrefs>            timePrefs;
    QScopedPointer<djvFileBrowserCache>     fileBrowserCache;
    QScopedPointer<djvImageIOWidgetFactory> imageIOWidgetFactory;
    QScopedPointer<djvPrefsDialog>          prefsDialog;
    QScopedPointer<djvInfoDialog>           infoDialog;
    QScopedPointer<djvAboutDialog>          aboutDialog;
    QScopedPointer<djvMessagesDialog>       messagesDialog;
    QScopedPointer<djvDebugLogDialog>       debugLogDialog;
    QScopedPointer<djvFileBrowser>          fileBrowser;
};

//------------------------------------------------------------------------------
// djvUIContext
//------------------------------------------------------------------------------

djvUIContext::djvUIContext(QObject * parent) :
    djvGraphicsContext(parent),
    _p(new Private)
{
    //DJV_DEBUG("djvUIContext::djvUIContext");

    Q_INIT_RESOURCE(djvUI);
    
    // Register meta types.
    qRegisterMetaType<djvPlaybackUtil::PLAYBACK>("djvPlaybackUtil::PLAYBACK");

    // Load translators.
    loadTranslator("djvUI");
    
    //! \todo Check the command line for whether to reset the preferences.
    Q_FOREACH(const QString & arg, qApp->arguments())
    {
        if (qApp->translate("djvUIContext", "-reset_prefs") == arg)
        {
            DJV_LOG(debugLog(), "djvUIContext", "Reset the preferences...");
            djvPrefs::setReset(true);
        }
    }

    // Load preferences.
    DJV_LOG(debugLog(), "djvAbstractApplication", "Load the preferences...");
    _p->fileBrowserPrefs.reset(new djvFileBrowserPrefs(this));
    _p->helpPrefs.reset(new djvHelpPrefs);
    _p->imagePrefs.reset(new djvImagePrefs);
    _p->imageIOPrefs.reset(new djvImageIOPrefs(this));
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

djvUIContext::~djvUIContext()
{
    //DJV_DEBUG("djvUIContext::~djvUIContext");
    QThreadPool::globalInstance()->waitForDone();    
}

bool djvUIContext::isValid() const
{
    return _p->valid;
}

void djvUIContext::setValid(bool in)
{
    //DJV_DEBUG("djvUIContext::setValid");
    //DJV_DEBUG_PRINT("in = " << in);
    _p->valid = in;
}

void djvUIContext::help()
{
    //DJV_DEBUG("djvUIContext::help");
    //DJV_DEBUG_PRINT("url = " << doc());
    QDesktopServices::openUrl(QUrl::fromLocalFile(doc()));
}

djvFileBrowser * djvUIContext::fileBrowser(const QString & title) const
{
    if (! _p->fileBrowser)
    {
        djvUIContext * that = const_cast<djvUIContext *>(this);
        _p->fileBrowser.reset(new djvFileBrowser(that));
    }
    _p->fileBrowser->close();
    _p->fileBrowser->setWindowTitle(
        ! title.isEmpty() ?
            title :
            qApp->translate("djvUIContext", "File Browser"));
    _p->fileBrowser->setPinnable(false);
    _p->fileBrowser->disconnect(SIGNAL(fileInfoChanged(const djvFileInfo &)));
    return _p->fileBrowser.data();
}

djvImageIOWidgetFactory * djvUIContext::imageIOWidgetFactory() const
{
    if (! _p->imageIOWidgetFactory)
    {
        djvUIContext * that = const_cast<djvUIContext *>(this);        
        _p->imageIOWidgetFactory.reset(new djvImageIOWidgetFactory(that));
    }
    return _p->imageIOWidgetFactory.data();
}

djvPrefsDialog * djvUIContext::prefsDialog() const
{
    if (! _p->prefsDialog)
    {
        djvUIContext * that = const_cast<djvUIContext *>(this);
        _p->prefsDialog.reset(new djvPrefsDialog(that));
    }
    return _p->prefsDialog.data();
}

djvInfoDialog * djvUIContext::infoDialog() const
{
    if (! _p->infoDialog)
    {
        djvUIContext * that = const_cast<djvUIContext *>(this);
        _p->infoDialog.reset(new djvInfoDialog(info(), that));
    }
    return _p->infoDialog.data();
}

djvAboutDialog * djvUIContext::aboutDialog() const
{
    if (! _p->aboutDialog)
    {
        djvUIContext * that = const_cast<djvUIContext *>(this);
        _p->aboutDialog.reset(new djvAboutDialog(about(), that));
    }
    return _p->aboutDialog.data();
}

djvMessagesDialog * djvUIContext::messagesDialog() const
{
    if (! _p->messagesDialog)
    {
        djvUIContext * that = const_cast<djvUIContext *>(this);        
        _p->messagesDialog.reset(new djvMessagesDialog(that));
    }
    return _p->messagesDialog.data();
}

djvDebugLogDialog * djvUIContext::debugLogDialog() const
{
    if (! _p->debugLogDialog)
    {
        djvUIContext * that = const_cast<djvUIContext *>(this);
        _p->debugLogDialog.reset(new djvDebugLogDialog(that));
    }
    return _p->debugLogDialog.data();
}

djvFileBrowserPrefs * djvUIContext::fileBrowserPrefs() const
{
    return _p->fileBrowserPrefs.data();
}

djvHelpPrefs * djvUIContext::helpPrefs() const
{
    return _p->helpPrefs.data();
}

djvImagePrefs * djvUIContext::imagePrefs() const
{
    return _p->imagePrefs.data();
}

djvImageIOPrefs * djvUIContext::imageIOPrefs() const
{
    return _p->imageIOPrefs.data();
}

djvSequencePrefs * djvUIContext::sequencePrefs() const
{
    return _p->sequencePrefs.data();
}

djvTimePrefs * djvUIContext::timePrefs() const
{
    return _p->timePrefs.data();
}
    
djvIconLibrary * djvUIContext::iconLibrary() const
{
    return _p->iconLibrary.data();
}

djvStyle * djvUIContext::style() const
{
    return _p->style.data();
}

djvFileBrowserCache * djvUIContext::fileBrowserCache() const
{
    return _p->fileBrowserCache.data();
}

QString djvUIContext::info() const
{
    static const QString label = qApp->translate("djvUIContext",
"%1"
"\n"
"Image I/O Widgets\n"
"\n"
"    Plugins: %2\n"
"\n"
"Preferences\n"
"\n"
"    User: %3\n"
"    System: %4\n");
    return QString(label).
        arg(djvGraphicsContext::info()).
        arg(imageIOWidgetFactory()->names().join(", ")).
        arg(djvPrefs(QString(), djvPrefs::USER).fileName()).
        arg(djvPrefs(QString(), djvPrefs::SYSTEM).fileName());
}

bool djvUIContext::commandLineParse(QStringList & in) throw (QString)
{
    //DJV_DEBUG("djvUIContext::commandLineParse");
    //DJV_DEBUG_PRINT("in = " << in);
    if (! djvGraphicsContext::commandLineParse(in))
        return false;
    QStringList tmp;
    QString     arg;
    try
    {
        while (!in.isEmpty())
        {
            in >> arg;
            if (qApp->translate("djvUIContext", "-reset_prefs") == arg)
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

QString djvUIContext::commandLineHelp() const
{
    static const QString label = qApp->translate("djvUIContext",
"\n"
"User Interface Options\n"
"\n"
"    -reset_prefs\n"
"        Do not load the preference at start up.\n"
"%2");
    return QString(label).
        arg(djvGraphicsContext::commandLineHelp());
}

void djvUIContext::print(const QString & string)
{
    if (_p->valid)
    {
        messagesDialog()->message(string);
    }
    else
    {
        djvGraphicsContext::print(string);
    }
}
