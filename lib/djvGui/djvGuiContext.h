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

//! \file djvGuiContext.h

#ifndef DJV_GUI_CONTEXT_H
#define DJV_GUI_CONTEXT_H

#include <djvGuiExport.h>

#include <djvImageContext.h>
#include <djvUtil.h>

class  djvAboutDialog;
class  djvDebugLogDialog;
class  djvFileBrowser;
class  djvFileBrowserCache;
class  djvFileBrowserPrefs;
struct djvGuiContextPrivate;
class  djvHelpPrefs;
class  djvIconLibrary;
class  djvImageIoPrefs;
class  djvImageIoWidgetFactory;
class  djvImagePrefs;
class  djvInfoDialog;
class  djvMessagesDialog;
class  djvPrefsDialog;
class  djvSequencePrefs;
class  djvStyle;
class  djvTimePrefs;

//! \addtogroup djvGuiMisc
//@{

//------------------------------------------------------------------------------
//! \class djvGuiContext
//!
//! This class provides global functionality for the library.
//------------------------------------------------------------------------------

class DJV_GUI_EXPORT djvGuiContext : public djvImageContext
{
    Q_OBJECT
    
public:

    //! Constructor.

    explicit djvGuiContext(QObject * parent = 0);

    //! Destructor.

    virtual ~djvGuiContext();
    
    //! Get whether the user-interface has started.

    bool isValid() const;

    //! Set whether the user-interface has started.

    virtual void setValid(bool);
    
    //! Open the documentation.
    
    void help();
    
    //! Get the file browser.
    
    djvFileBrowser * fileBrowser(const QString & title = QString());
    
    //! Get the image I/O widget plugin factory.
    
    djvImageIoWidgetFactory * imageIoWidgetFactory();
    
    //! Get the preferences dialog.
    
    djvPrefsDialog * prefsDialog();

    //! Get the information dialog.

    djvInfoDialog * infoDialog();

    //! Get the about dialog.

    djvAboutDialog * aboutDialog();

    //! Get the messages dialog.

    djvMessagesDialog * messagesDialog();
    
    //! Get the debugging dialog.
    
    djvDebugLogDialog * debugLogDialog();
    
    //! Get the file browser preferences.
    
    djvFileBrowserPrefs * fileBrowserPrefs();

    //! Get the help preferences.
    
    djvHelpPrefs * helpPrefs();

    //! Get the image preferences.
    
    djvImagePrefs * imagePrefs();

    //! Get the image I/O preferences.
    
    djvImageIoPrefs * imageIoPrefs();

    //! Get the sequence preferences.
    
    djvSequencePrefs * sequencePrefs();

    //! Get the time preferences.
    
    djvTimePrefs * timePrefs();

    //! Get the file browser cache.
    
    djvFileBrowserCache * fileBrowserCache();

    //! Get the icon library.
    
    djvIconLibrary * iconLibrary();

    //! Get the style.
    
    djvStyle * style();

    virtual QString info() const;

protected:

    virtual bool commandLineParse(QStringList &) throw (QString);

    virtual QString commandLineHelp() const;
    
    virtual void print(const QString &);
    
private:
    
    DJV_PRIVATE_COPY(djvGuiContext);
    
    djvGuiContextPrivate * _p;
};

//@} // djvGuiMisc

#endif // DJV_GUI_CONTEXT_H

