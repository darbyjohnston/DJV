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

//! \file djvApplication.cpp

#include <djvApplication.h>

#include <djvApplicationMessageDialog.h>
#include <djvFileBrowser.h>
#include <djvFileBrowserPrefs.h>
#include <djvImageIoPrefs.h>
#include <djvMiscPrefs.h>
#include <djvOpenGlPrefs.h>
#include <djvPrefs.h>
#include <djvStyle.h>

#include <djvDebugLog.h>
#include <djvError.h>
#include <djvFileInfoUtil.h>
#include <djvSystem.h>

#include <QDesktopServices>
#if QT_VERSION < 0x050000
#include <QPlastiqueStyle>
#endif
#include <QScopedPointer>
#include <QUrl>

#include <algorithm>

//------------------------------------------------------------------------------
// djvAbstractApplication::P
//------------------------------------------------------------------------------

namespace
{

class ToolTipFilter : public QObject
{
protected:

    bool eventFilter(QObject * object, QEvent * event)
    {
        if (event->type() == QEvent::ToolTip)
            return true;
        
        return QObject::eventFilter(object, event);
    }
};

} // namespace

struct djvAbstractApplication::P
{
    P() :
        valid        (false),
        toolTips     (toolTipsDefault()),
        toolTipFilter(new ToolTipFilter)
    {}
        
    bool                          valid;
    bool                          toolTips;
    QScopedPointer<ToolTipFilter> toolTipFilter;
};

//------------------------------------------------------------------------------
// djvAbstractApplication
//------------------------------------------------------------------------------

djvAbstractApplication::djvAbstractApplication(
    const QString & name,
    int &           argc,
    char **         argv) throw (djvError) :
    djvAbstractImageApplication(name, argc, argv),
    _p(new P)
{
    //DJV_DEBUG("djvAbstractApplication::djvAbstractApplication");

    loadTranslator("djvGui");

    // Parse the command line for the reset preferences option.

    try
    {
        resetPreferencesCommandLine(_commandLineArgs);
    }
    catch (const QString & error)
    {
        printError(djvError(errorLabels()[ERROR_COMMAND_LINE].arg(error)));

        setExitValue(djvApplicationEnum::EXIT_ERROR);
    }

    // Load the preferences.
    
    DJV_LOG("djvAbstractApplication", "Load the preferences...");
    
    djvFileBrowserPrefs::global();
    djvImageIoPrefs::global();
    djvMiscPrefs::global();
    djvOpenGlPrefs::global();

    djvPrefs prefs("djvAbstractApplication", djvPrefs::SYSTEM);
    prefs.get("toolTips", _p->toolTips);

    DJV_LOG("djvAbstractImageApplication", "");

    // Initialize.

    djvStyle::global();

    toolTipsUpdate();
    
    DJV_LOG("djvAbstractApplication", "Information:");
    DJV_LOG("djvAbstractApplication", "");
    DJV_LOG("djvAbstractApplication", info());
}

djvAbstractApplication::~djvAbstractApplication()
{
    //DJV_DEBUG("djvAbstractApplication::~djvAbstractApplication");
    
    delete djvFileBrowser::global();

    djvPrefs prefs("djvAbstractApplication", djvPrefs::SYSTEM);
    prefs.set("toolTips", _p->toolTips);

    delete _p;
}

bool djvAbstractApplication::isValid() const
{
    return _p->valid;
}

void djvAbstractApplication::setValid(bool in)
{
    //DJV_DEBUG("djvAbstractApplication::setValid");
    //DJV_DEBUG_PRINT("in = " << in);

    _p->valid = in;
}

bool djvAbstractApplication::toolTipsDefault()
{
    return true;
}

bool djvAbstractApplication::hasToolTips() const
{
    return _p->toolTips;
}

void djvAbstractApplication::setToolTips(bool toolTips)
{
    if (toolTips == _p->toolTips)
        return;

    _p->toolTips = toolTips;

    toolTipsUpdate();
}

void djvAbstractApplication::help() const
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(doc()));
}

int djvAbstractApplication::run()
{
    //DJV_DEBUG("djvAbstractApplication::run");
    //DJV_DEBUG_PRINT("valid = " << _valid);

    return djvAbstractImageApplication::run();
}

QString djvAbstractApplication::info() const
{
    static const QString label = qApp->translate("djvAbstractApplication",
"%1"
"\n"
"Preferences\n"
"\n"
"    User: %2\n"
"    System: %3\n");

    return QString(label).
        arg(djvAbstractImageApplication::info()).
        arg(djvPrefs(QString(), djvPrefs::USER).fileName()).
        arg(djvPrefs(QString(), djvPrefs::SYSTEM).fileName());
}

void djvAbstractApplication::printMessage(const QString & message) const
{
    if (_p->valid)
    {
        djvApplicationMessageDialog::global()->message(message);
    }
    else
    {
        djvAbstractImageApplication::printMessage(message);
    }
}

void djvAbstractApplication::printError(const djvError & in) const
{
    if (_p->valid)
    {
        djvApplicationMessageDialog::global()->message(in);
    }
    else
    {
        djvAbstractImageApplication::printError(in);
    }
}

QString djvAbstractApplication::commandLineHelp() const
{
    static const QString label = qApp->translate("djvAbstractApplication",
"\n"
"User Interface Options\n"
"\n"
"    -reset_prefs\n"
"        Do not load the preference at start up.\n"
"%2");

    return QString(label).
        arg(djvAbstractImageApplication::commandLineHelp());
}

void djvAbstractApplication::resetPreferencesCommandLine(QStringList & in)
    throw (QString)
{
    //DJV_DEBUG("djvAbstractApplication::resetPreferencesCommandLine");
    //DJV_DEBUG_PRINT("in = " << in);

    djvAbstractCoreApplication::commandLine(in);

    if (djvAbstractCoreApplication::exitValue() !=
        djvApplicationEnum::EXIT_DEFAULT)
        return;

    QStringList tmp;
    QString     arg;

    try
    {
        while (!in.isEmpty())
        {
            in >> arg;

            if (qApp->translate("djvAbstractApplication", "-reset_prefs") == arg)
            {
                DJV_LOG("djvAbstractApplication", "Reset the preferences...");
                
                djvPrefs::setReset(true);
            }
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
}

void djvAbstractApplication::toolTipsUpdate()
{
    if (! _p->toolTips)
    {
        qApp->installEventFilter(_p->toolTipFilter.data());
    }
    else
    {
        qApp->removeEventFilter(_p->toolTipFilter.data());
    }
}

//------------------------------------------------------------------------------
// djvApplication
//------------------------------------------------------------------------------

djvApplication::djvApplication(const QString & name, int & argc, char ** argv)
    throw (djvError) :
    QApplication(argc, argv),
    djvAbstractApplication(name, argc, argv)
{
#   if QT_VERSION < 0x050000
    setStyle(new QPlastiqueStyle);
#   else
    setStyle("fusion");
#   endif
    
    setOrganizationName("djv.sourceforge.net");
    
    setApplicationName(name);
}

int djvApplication::run()
{
    djvAbstractApplication::run();

    if (djvApplicationEnum::EXIT_DEFAULT == exitValue())
    {
        QApplication::exec();
    }
    
    return exitValue();
}

