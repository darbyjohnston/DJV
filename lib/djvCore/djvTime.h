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

//! \file djvTime.h

#ifndef DJV_TIME_H
#define DJV_TIME_H

#include <djvSpeed.h>

#include <QMetaType>

#if ! defined(DJV_WINDOWS)
#include <sys/time.h>
#else
#include <time.h>
#endif

//! \addtogroup djvCoreMisc
//@{

//------------------------------------------------------------------------------
//! \class djvTime
//!
//! This class provides time utilities.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvTime
{
    Q_GADGET
    Q_ENUMS(UNITS)
    
public:

    //! Destructor.
    
    virtual ~djvTime() = 0;

    //! Get the current time.

    static ::time_t current();

    //! Sleep for the given number of seconds.

    static void sleep(unsigned long seconds);

    //! Sleep for the given number of milliseconds.

    static void msleep(unsigned long msecs);

    //! Sleep for the given number of microseconds.

    static void usleep(unsigned long usecs);

    //! Convert the given number of seconds to hours, minutes, and seconds.

    static void secondsToTime(
        double,
        int &    hours,
        int &    minutes,
        double & seconds);

    //! Convert a time to a string.

    static QString timeToString(::time_t);

    //! Convert seconds to a human readable string.

    static QString labelTime(double seconds);

    //! Convert timecode to time.

    static void timecodeToTime(
        quint32,
        int & hour,
        int & minute,
        int & second,
        int & frame);

    //! Convert time to timecode.

    static quint32 timeToTimecode(
        int hour,
        int minute,
        int second,
        int frame);

    //! Convert timecode to a frame.

    static qint64 timecodeToFrame(quint32 timecode, const djvSpeed &);

    //! Convert a frame to timecode.

    static quint32 frameToTimecode(qint64 frame, const djvSpeed &);

    //! Convert timecode to a string.

    static QString timecodeToString(quint32);

    //! Convert a string to timecode.

    static quint32 stringToTimecode(const QString &, bool * ok = 0);

    //! Convert a keycode to a string.

    static QString keycodeToString(
        int id,
        int type,
        int prefix,
        int count,
        int offset);

    //! Convert a string to a keycode.

    static bool stringToKeycode(
        const QString &,
        int & id,
        int & type,
        int & prefix,
        int & count,
        int & offset);

    //! This enumeration provides the time units.

    enum UNITS
    {
        UNITS_TIMECODE,
        UNITS_FRAMES,

        UNITS_COUNT
    };

    //! Get the time units labels.

    static const QStringList & unitsLabels();

    //! Get the time units default.
    
    static UNITS unitsDefault();
    
    //! Get the global time units.

    static UNITS units();

    //! Set the global time units.

    static void setUnits(UNITS);
    
    //! Convert a frame to a string using the global time units.

    static QString frameToString(qint64 frame, const djvSpeed &);

    //! Convert a string to a frame using the global time units.

    static qint64 stringToFrame(const QString &, const djvSpeed &, bool * ok = 0);
};

//------------------------------------------------------------------------------

DJV_STRING_OPERATOR(DJV_CORE_EXPORT, djvTime::UNITS);

//@} // djvCoreMisc

#endif // DJV_TIME_H

