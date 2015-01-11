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
    
    //! This property holds the global time units.
    
    Q_PROPERTY(
        djvTime::UNITS timeUnits
        READ           timeUnits
        WRITE          setTimeUnits
        NOTIFY         timeUnitsChanged)
    
    //! This property holds the global speed.
    
    Q_PROPERTY(
        djvSpeed::FPS speed
        READ          speed
        WRITE         setSpeed
        NOTIFY        speedChanged)
    
    //! This property holds the maximum number of frames a sequence can hold.
    
    Q_PROPERTY(
        qint64 sequenceMaxFrames
        READ   sequenceMaxFrames
        WRITE  setSequenceMaxFrames
        NOTIFY sequenceMaxFramesChanged)
    
public:

    //! Constructor.

    explicit djvMiscPrefs(QObject * parent = 0);

    //! Destructor.

    ~djvMiscPrefs();

    //! Get the global time units.

    djvTime::UNITS timeUnits() const;

    //! Get the global speed.

    djvSpeed::FPS speed() const;

    //! Get the maximum number of frames a sequence can hold.

    qint64 sequenceMaxFrames() const;

    //! Get the global preferences.

    static djvMiscPrefs * global();
    
public Q_SLOTS:

    //! Set the global time units.

    void setTimeUnits(djvTime::UNITS);

    //! Set the global speed.

    void setSpeed(djvSpeed::FPS);

    //! Set the maximum number of frames a sequence can hold.

    void setSequenceMaxFrames(qint64);

Q_SIGNALS:

    //! This signal is emitted when the global time units are changed.
    
    void timeUnitsChanged(djvTime::UNITS);

    //! This signal is emitted when the global speed is changed.

    void speedChanged(djvSpeed::FPS);

    //! This signal is emitted when the maximum number of frames in a sequence
    //! is changed.

    void sequenceMaxFramesChanged(qint64);
};

//@} // djvGuiMisc

#endif // DJV_MISC_PREFS_H

