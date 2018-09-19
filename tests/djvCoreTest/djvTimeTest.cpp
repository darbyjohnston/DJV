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

#include <djvTimeTest.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>
#include <djvCore/Math.h>
#include <djvCore/Time.h>

#include <QStringList>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        void TimeTest::run(int &, char **)
        {
            DJV_DEBUG("TimeTest::run");
            current();
            sleep();
            convert();
        }

        void TimeTest::current()
        {
            DJV_DEBUG("TimeTest::current");
            DJV_DEBUG_PRINT(Time::timeToString(Time::current()));
        }

        void TimeTest::sleep()
        {
            DJV_DEBUG("TimeTest::sleep");
            Time::sleep(1);
            Time::msleep(1);
            Time::usleep(1);
        }

        void TimeTest::convert()
        {
            DJV_DEBUG("TimeTest::convert");
            {
                int   hours = 0;
                int   minutes = 0;
                float seconds = 0.f;
                Time::secondsToTime(0, hours, minutes, seconds);
                DJV_ASSERT(0 == hours);
                DJV_ASSERT(0 == minutes);
                DJV_ASSERT(Math::fuzzyCompare(0.f, seconds));
                Time::secondsToTime(59, hours, minutes, seconds);
                DJV_ASSERT(0 == hours);
                DJV_ASSERT(0 == minutes);
                DJV_ASSERT(Math::fuzzyCompare(59.f, seconds));
                Time::secondsToTime(60, hours, minutes, seconds);
                DJV_ASSERT(0 == hours);
                DJV_ASSERT(1 == minutes);
                DJV_ASSERT(Math::fuzzyCompare(0.f, seconds));
                Time::secondsToTime(3600, hours, minutes, seconds);
                DJV_ASSERT(1 == hours);
                DJV_ASSERT(0 == minutes);
                DJV_ASSERT(Math::fuzzyCompare(0.f, seconds));
            }
            {
                DJV_ASSERT("00:00:00" == Time::labelTime(0.f));
                DJV_ASSERT("00:00:59" == Time::labelTime(59.f));
                DJV_ASSERT("00:01:00" == Time::labelTime(60.f));
                DJV_ASSERT("01:00:00" == Time::labelTime(3600.f));
            }
            {
                const struct Data
                {
                    int hour;
                    int minute;
                    int seconds;
                    int frame;
                }
                data[] =
                {
                    {  0,  0,  0,  0 },
                    {  1,  1,  1,  1 },
                    { 10, 10, 10, 10 }
                };
                const int dataCount = sizeof(data) / sizeof(Data);
                for (int i = 0; i < dataCount; ++i)
                {
                    const quint32 timecode = Time::timeToTimecode(
                        data[i].hour,
                        data[i].minute,
                        data[i].seconds,
                        data[i].frame);
                    int hour = 0;
                    int minute = 0;
                    int seconds = 0;
                    int frame = 0;
                    Time::timecodeToTime(timecode, hour, minute, seconds, frame);
                    DJV_ASSERT(data[i].hour == hour);
                    DJV_ASSERT(data[i].minute == minute);
                    DJV_ASSERT(data[i].seconds == seconds);
                    DJV_ASSERT(data[i].frame == frame);
                }
            }
            {
                const struct Data
                {
                    QString a;
                    qint64  b;
                    Speed   speed;
                    bool    ok;
                }
                data[] =
                {
                    { "00:00:00:00",  0,                Speed::FPS_24, true  },
                    { "00:00:00:01",  1,                Speed::FPS_24, true  },
                    { "00:00:00:23", 23,                Speed::FPS_24, true  },
                    { "00:00:01:00", 24,                Speed::FPS_24, true  },
                    { "00:00:01:01", 24 + 1,            Speed::FPS_24, true  },
                    { "00:00:01:23", 24 + 23,           Speed::FPS_24, true  },
                    { "00:00:02:00", 24 + 24,           Speed::FPS_24, true  },
                    { "00:01:00:00", 60 * 24,           Speed::FPS_24, true  },
                    { "00:01:00:01", 60 * 24 + 1,       Speed::FPS_24, true  },
                    { "00:01:00:23", 60 * 24 + 23,      Speed::FPS_24, true  },
                    { "00:01:01:00", 60 * 24 + 24,      Speed::FPS_24, true  },
                    { "01:00:00:00", 60 * 60 * 24,      Speed::FPS_24, true  },
                    { "01:00:00:01", 60 * 60 * 24 + 1,  Speed::FPS_24, true  },
                    { "01:00:00:23", 60 * 60 * 24 + 23, Speed::FPS_24, true  },
                    { "01:00:01:00", 60 * 60 * 24 + 24, Speed::FPS_24, true  },
                    { "",            0,                 0,             false }
                };
                const int dataCount = sizeof(data) / sizeof(Data);
                for (int i = 0; i < dataCount; ++i)
                {
                    bool ok = false;
                    const qint64 a = Time::timecodeToFrame(Time::stringToTimecode(data[i].a, &ok), data[i].speed);
                    DJV_ASSERT(data[i].b == a);
                    DJV_ASSERT(data[i].ok == ok);
                    const QString b = Time::timecodeToString(Time::frameToTimecode(a, data[i].speed));
                    if (ok)
                    {
                        DJV_ASSERT(data[i].a == b);
                    }
                }
            }
            {
                const struct Data
                {
                    QString a, b;
                }
                data[] =
                {
                    { "00:00:00:00",  "00:00:00:00"  },
                    { "12:59:59:119", "12:59:59:119" },
                    { "1",            "00:00:00:01"  },
                    { "1:2",          "00:00:01:02"  },
                    { "1:2:3",        "00:01:02:03"  },
                    { "1:2:3:4",      "01:02:03:04"  }
                };
                const int dataCount = sizeof(data) / sizeof(Data);
                for (int i = 0; i < dataCount; ++i)
                {
                    const quint32 a(Time::stringToTimecode(data[i].a));
                    const quint32 b(Time::stringToTimecode(data[i].b));
                    DJV_ASSERT(a == b);
                    DJV_ASSERT(Time::timecodeToString(a) == data[i].b);
                    DJV_ASSERT(Time::timecodeToString(b) == data[i].b);
                }
            }
            {
                const struct Data
                {
                    QString keycode;
                    bool    ok;
                }
                data[] =
                {
                    { "0:0:0:0:0", true  },
                    { "",          false }
                };
                const int dataCount = sizeof(data) / sizeof(Data);
                for (int i = 0; i < dataCount; ++i)
                {
                    int id = 0;
                    int type = 0;
                    int prefix = 0;
                    int count = 0;
                    int offset = 0;
                    const bool ok = Time::stringToKeycode(
                        data[i].keycode,
                        id,
                        type,
                        prefix,
                        count,
                        offset);
                    DJV_ASSERT(data[i].ok == ok);
                    const QString keycode = Time::keycodeToString(
                        id,
                        type,
                        prefix,
                        count,
                        offset);
                    if (ok)
                    {
                        DJV_ASSERT(data[i].keycode == keycode);
                    }
                }
            }
            {
                const struct Data
                {
                    qint64 frame;
                    Speed  speed;
                    bool   ok;
                }
                data[] =
                {
                    { 0, Speed::FPS_24, true }
                };
                const int dataCount = sizeof(data) / sizeof(Data);
                for (int i = 0; i < dataCount; ++i)
                {
                    Q_FOREACH(
                        Time::UNITS units,
                        QList<Time::UNITS>() << Time::UNITS_TIMECODE <<
                        Time::UNITS_FRAMES)
                    {
                        Time::setUnits(units);
                        const QString s = Time::frameToString(data[i].frame, data[i].speed);
                        Speed speed;
                        bool ok = false;
                        const int frame = Time::stringToFrame(s, speed, &ok);
                        DJV_ASSERT(data[i].frame == frame);
                        DJV_ASSERT(data[i].speed == speed);
                        DJV_ASSERT(data[i].ok == ok);
                    }
                    Time::setUnits(Time::unitsDefault());
                }
            }
            {
                Time::UNITS units = Time::units();
                QStringList tmp;
                tmp << units;
                tmp >> units;
                DJV_ASSERT(Time::units() == units);
            }
        }

    } // namespace CoreTest
} // namespace djv
