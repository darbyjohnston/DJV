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

#pragma once

#include <djvUI/Core.h>

#include <djvGraphics/GraphicsContext.h>

#include <djvCore/Util.h>

#include <memory>

class djvAboutDialog;
class djvDebugLogDialog;
class djvFileBrowser;
class djvFileBrowserCache;
class djvFileBrowserPrefs;
class djvHelpPrefs;
class djvIconLibrary;
class djvImageIOPrefs;
class djvImageIOWidgetFactory;
class djvImagePrefs;
class djvInfoDialog;
class djvMessagesDialog;
class djvPrefsDialog;
class djvSequencePrefs;
class djvStyle;
class djvTimePrefs;

//! \addtogroup djvUIMisc
//@{

//------------------------------------------------------------------------------
//! \class djvUIContext
//!
//! This class provides global functionality for the library.
//------------------------------------------------------------------------------

class djvUIContext : public djvGraphicsContext
{
    Q_OBJECT
    
public:
    explicit djvUIContext(QObject * parent = nullptr);

    virtual ~djvUIContext();
    
    //! Get whether the user-interface has started.
    bool isValid() const;

    //! Set whether the user-interface has started.
    virtual void setValid(bool);
    
    //! Open the documentation.
    void help();
    
    //! Get the file browser.
    djvFileBrowser * fileBrowser(const QString & title = QString()) const;
    
    //! Get the image I/O widget plugin factory.
    djvImageIOWidgetFactory * imageIOWidgetFactory() const;
    
    //! Get the preferences dialog.
    djvPrefsDialog * prefsDialog() const;

    //! Get the information dialog.
    djvInfoDialog * infoDialog() const;

    //! Get the about dialog.
    djvAboutDialog * aboutDialog() const;

    //! Get the messages dialog.
    djvMessagesDialog * messagesDialog() const;
    
    //! Get the debugging dialog.
    djvDebugLogDialog * debugLogDialog() const;
    
    //! Get the file browser preferences.
    djvFileBrowserPrefs * fileBrowserPrefs() const;

    //! Get the help preferences.
    djvHelpPrefs * helpPrefs() const;

    //! Get the image preferences.
    djvImagePrefs * imagePrefs() const;

    //! Get the image I/O preferences.
    djvImageIOPrefs * imageIOPrefs() const;

    //! Get the sequence preferences.
    djvSequencePrefs * sequencePrefs() const;

    //! Get the time preferences.
    djvTimePrefs * timePrefs() const;

    //! Get the file browser cache.
    djvFileBrowserCache * fileBrowserCache() const;

    //! Get the icon library.
    djvIconLibrary * iconLibrary() const;

    //! Get the style.
    djvStyle * style() const;

    virtual QString info() const;

protected:
    virtual bool commandLineParse(QStringList &) throw (QString);

    virtual QString commandLineHelp() const;
    
    virtual void print(const QString &);
    
private:    
    DJV_PRIVATE_COPY(djvUIContext);
    
    struct Private;
    std::unique_ptr<Private> _p;
};

//@} // djvUIMisc

