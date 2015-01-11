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

//! \file djvTime.cpp

#include <djvTime.h>

#include <djvAssert.h>

#include <QCoreApplication>
#include <QThread>

#if defined(DJV_WINDOWS)
#include <windows.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif
#include <float.h>
#include <time.h>

//------------------------------------------------------------------------------
// djvTime
//------------------------------------------------------------------------------

djvTime::~djvTime()
{}

::time_t djvTime::current()
{
    return ::time(0);
}

namespace
{

class Thread : public QThread
{
public:

    static void sleep (unsigned long seconds) { QThread::sleep (seconds); }
    static void msleep(unsigned long msecs)   { QThread::msleep(msecs);   }
    static void usleep(unsigned long usecs)   { QThread::usleep(usecs);   }
};

} // namespace

void djvTime::sleep(unsigned long seconds)
{
    Thread::sleep(seconds);
}

void djvTime::msleep(unsigned long msecs)
{
    Thread::msleep(msecs);
}

void djvTime::usleep(unsigned long usecs)
{
    Thread::usleep(usecs);
}

void djvTime::secondsToTime(double in, int & hour, int & minute, double & second)
{
    hour = static_cast<int>(in) / (60 * 60);
    in -= hour * 60 * 60;

    minute = static_cast<int>(in) / 60;
    in -= minute * 60;

    second = in;
}

QString djvTime::timeToString(::time_t in)
{
    QString out;
    
#   if defined(DJV_WINDOWS)

    char buf[djvStringUtil::cStringLength];

    ::ctime_s(buf, djvStringUtil::cStringLength, &in);

    out = buf;

#   else // DJV_WINDOWS

    out = ::ctime(&in);

#   endif // DJV_WINDOWS

    // Remove the newline.

    if (out.length())
    {
        out.chop(1);
    }

    return out;
}

QString djvTime::labelTime(double in)
{
    int    hour   = 0;
    int    minute = 0;
    double second = 0.0;
    
    secondsToTime(in, hour, minute, second);

    return QString("%1:%2:%3").
        arg(hour,   2, 10, QChar('0')).
        arg(minute, 2, 10, QChar('0')).
        arg(static_cast<int>(second), 2, 10, QChar('0'));
}

//! \todo All of the timecode/keycode conversion functionality is probably
//! wrong.

void djvTime::timecodeToTime(
    quint32 in,
    int &   hour,
    int &   minute,
    int &   second,
    int &   frame)
{
    hour   = (in >> 28 & 0x0f) * 10 + (in >> 24 & 0x0f);
    minute = (in >> 20 & 0x0f) * 10 + (in >> 16 & 0x0f);
    second = (in >> 12 & 0x0f) * 10 + (in >> 8  & 0x0f);
    frame  = (in >> 4  & 0x0f) * 10 + (in >> 0  & 0x0f);
}

quint32 djvTime::timeToTimecode(
    int hour,
    int minute,
    int second,
    int frame)
{
    return
        (hour   / 10 & 0x0f) << 28 | (hour   % 10 & 0x0f) << 24 |
        (minute / 10 & 0x0f) << 20 | (minute % 10 & 0x0f) << 16 |
        (second / 10 & 0x0f) << 12 | (second % 10 & 0x0f) << 8  |
        (frame  / 10 & 0x0f) << 4  | (frame  % 10 & 0x0f) << 0;
}

qint64 djvTime::timecodeToFrame(quint32 in, const djvSpeed & speed)
{
    if (! speed.isValid())
        return 0;

    int hour   = 0;
    int minute = 0;
    int second = 0;
    int frame  = 0;
    
    timecodeToTime(in, hour, minute, second, frame);

    return
        static_cast<qint64>(
            (hour * 60 * 60 + minute * 60 + second) *
            djvSpeed::speedToFloat(speed)) +
        frame;
}

quint32 djvTime::frameToTimecode(qint64 frame, const djvSpeed & speed)
{
    //DJV_DEBUG("djvTime::frameToTimecode");
    //DJV_DEBUG_PRINT("frame = " << frame);
    //DJV_DEBUG_PRINT("speed = " << speed);

    if (! speed.isValid())
        return 0;

    const double speed_float = djvSpeed::speedToFloat(speed);

    const int hour = static_cast<int>(frame / (speed_float * 60 * 60));
    frame -= static_cast<int>(hour * speed_float * 60 * 60);

    const int minute = static_cast<int>(frame / (speed_float * 60));
    frame -= static_cast<int>(minute * speed_float * 60);

    const int second = static_cast<int>(frame / speed_float);
    frame -= static_cast<int>(second * speed_float);

    //DJV_DEBUG_PRINT("hour = " << hour);
    //DJV_DEBUG_PRINT("minute = " << minute);
    //DJV_DEBUG_PRINT("second = " << second);
    //DJV_DEBUG_PRINT("frame = " << frame);

    const quint32 out = timeToTimecode(hour, minute, second, frame);

    //DJV_DEBUG_PRINT("out = " << timecodeToString(out));

    return out;
}

QString djvTime::timecodeToString(quint32 in)
{
    int hour   = 0;
    int minute = 0;
    int second = 0;
    int frame  = 0;
    
    timecodeToTime(in, hour, minute, second, frame);

    return QString("%1:%2:%3:%4").
        arg(hour,   2, 10, QChar('0')).
        arg(minute, 2, 10, QChar('0')).
        arg(second, 2, 10, QChar('0')).
        arg(frame,  2, 10, QChar('0'));
}

quint32 djvTime::stringToTimecode(const QString & in, bool * ok)
{
    if (ok)
    {
        *ok = true;
    }
    
    int hour   = 0;
    int minute = 0;
    int second = 0;
    int frame  = 0;

    QStringList tmp = in.split(':', QString::SkipEmptyParts);
    
    if (tmp.count() != 4 && ok)
    {
        *ok = false;
    }
    
    int i = 0;

    switch (tmp.count())
    {
        case 4: hour   = tmp[i++].toInt(ok);
        case 3: minute = tmp[i++].toInt(ok);
        case 2: second = tmp[i++].toInt(ok);
        case 1: frame  = tmp[i++].toInt(ok);
    }

    return timeToTimecode(hour, minute, second, frame);
}

QString djvTime::keycodeToString(
    int id,
    int type,
    int prefix,
    int count,
    int offset)
{
    return QString("%1:%2:%3:%4:%5").
        arg(id).
        arg(type).
        arg(prefix).
        arg(count).
        arg(offset);
}

bool djvTime::stringToKeycode(
    const QString & string,
    int &           id,
    int &           type,
    int &           prefix,
    int &           count,
    int &           offset)
{         
    const QStringList tmp = string.split(':', QString::SkipEmptyParts);

    if (tmp.count() != 5)
        return false;

    id     = tmp[0].toInt();
    type   = tmp[1].toInt();
    prefix = tmp[2].toInt();
    count  = tmp[3].toInt();
    offset = tmp[4].toInt();

    return true;
}

const QStringList & djvTime::unitsLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvTime", "Timecode") <<
        qApp->translate("djvTime", "Frames");

    DJV_ASSERT(data.count() == UNITS_COUNT);

    return data;
}

djvTime::UNITS djvTime::unitsDefault()
{
    return djvTime::UNITS_FRAMES;
}

namespace
{

djvTime::UNITS _units = djvTime::unitsDefault();

} // namespace

djvTime::UNITS djvTime::units()
{
    return _units;
}

void djvTime::setUnits(UNITS units)
{
    _units = units;
}

QString djvTime::frameToString(qint64 frame, const djvSpeed & speed)
{
    QString out;

    switch (units())
    {
        case UNITS_TIMECODE:
        
            out = timecodeToString(frameToTimecode(frame, speed));
            
            break;

        case UNITS_FRAMES:
        
            out = QString::number(frame);
            
            break;

        default: break;
    }

    return out;
}

qint64 djvTime::stringToFrame(
    const QString &  string,
    const djvSpeed & speed,
    bool *           ok)
{
    qint64 out = 0;

    switch (units())
    {
        case UNITS_TIMECODE:
        
            out = timecodeToFrame(stringToTimecode(string, ok), speed);
            
            break;

        case UNITS_FRAMES:
        
            out = string.toLongLong(ok);
            
            break;

        default: break;
    }

    return out;
}

//------------------------------------------------------------------------------

_DJV_STRING_OPERATOR_LABEL(djvTime::UNITS, djvTime::unitsLabels())

