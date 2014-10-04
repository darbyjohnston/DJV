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

//! \file djvMiscPrefs.h

#ifndef DJV_MISC_PREFS_H
#define DJV_MISC_PREFS_H

#include <djvGuiExport.h>

#include <djvTime.h>

#include <QObject>

//! \addtogroup djvGuiMisc
//@{

//------------------------------------------------------------------------------
//! \class djvMiscPrefs
//!
//! This class provides miscellaneous preferences.
//------------------------------------------------------------------------------

class DJV_GUI_EXPORT djvMiscPrefs : public QObject
{
    Q_OBJECT
    
    //! This property holds the default speed.
    
    Q_PROPERTY(
        djvSpeed::FPS speed
        READ          speed
        WRITE         setSpeed
        NOTIFY        speedChanged)
    
    //! This property holds the time units.
    
    Q_PROPERTY(
        djvTime::UNITS units
        READ           units
        WRITE          setUnits
        NOTIFY         unitsChanged)
    
public:

    //! Constructor.

    explicit djvMiscPrefs(QObject * parent = 0);

    //! Destructor.

    ~djvMiscPrefs();

    //! Get the efault speed.

    djvSpeed::FPS speed() const;

    //! Get the time units.

    djvTime::UNITS units() const;

    //! Get the global preferences.

    static djvMiscPrefs * global();
    
public Q_SLOTS:

    //! Set the efault speed.

    void setSpeed(djvSpeed::FPS);

    //! Set the time units.

    void setUnits(djvTime::UNITS);

Q_SIGNALS:

    //! This signal is emitted when the efault speed is changed.
    
    void speedChanged(djvSpeed::FPS);
    
    //! This signal is emitted when the time units are changed.
    
    void unitsChanged(djvTime::UNITS);
};

//@} // djvGuiMisc

#endif // DJV_MISC_PREFS_H

