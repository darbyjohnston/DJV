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

//! \file djvHelpPrefs.h

#ifndef DJV_HELPe_PREFS_H
#define DJV_HELP_PREFS_H

#include <djvGuiExport.h>

#include <QObject>

struct djvHelpPrefsPrivate;

//! \addtogroup djvGuiMisc
//@{

//------------------------------------------------------------------------------
//! \class djvHelpPrefs
//!
//! This class provides help preferences.
//------------------------------------------------------------------------------

class DJV_GUI_EXPORT djvHelpPrefs : public QObject
{
    Q_OBJECT
    
    //! This property holds whether tool tips are enabled.
    
    Q_PROPERTY(
        bool   toolTips
        READ   hasToolTips
        WRITE  setToolTips
        NOTIFY toolTipsChanged)
    
public:

    //! Constructor.

    explicit djvHelpPrefs(QObject * parent = 0);

    //! Destructor.

    ~djvHelpPrefs();
    
    //! The default for whether tool tips are enabled.
    
    static bool toolTipsDefault();

    //! Get whether tool tips are enabled.

    bool hasToolTips() const;

public Q_SLOTS:

    //! Set whether tool tips are enabled.

    void setToolTips(bool);

Q_SIGNALS:

    //! This signal is emitted when the tool tips are changed.
    
    void toolTipsChanged(bool);

private:

    void toolTipsUpdate();
    
    djvHelpPrefsPrivate * _p;
};

//@} // djvGuiMisc

#endif // DJV_HELP_PREFS_H

