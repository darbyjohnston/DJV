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

#include <djvTimePrefs.h>

#include <djvPrefs.h>

#include <djvDebug.h>

#include <QApplication>

//------------------------------------------------------------------------------
// djvTimePrefs
//------------------------------------------------------------------------------

djvTimePrefs::djvTimePrefs(QObject * parent) :
    QObject(parent)
{
    //DJV_DEBUG("djvTimePrefs::djvTimePrefs");

    djvPrefs prefs("djvTimePrefs", djvPrefs::SYSTEM);
    djvTime::UNITS timeUnits = djvTime::units();
    if (prefs.get("timeUnits", timeUnits))
    {
        djvTime::setUnits(timeUnits);
    }
    djvSpeed::FPS speed = djvSpeed::speed();
    if (prefs.get("speed", speed))
    {
        djvSpeed::setSpeed(speed);
    }
}

djvTimePrefs::~djvTimePrefs()
{
    //DJV_DEBUG("djvTimePrefs::~djvTimePrefs");

    djvPrefs prefs("djvTimePrefs", djvPrefs::SYSTEM);
    prefs.set("timeUnits", djvTime::units());
    prefs.set("speed", djvSpeed::speed());
}

djvTime::UNITS djvTimePrefs::timeUnits() const
{
    return djvTime::units();
}

djvSpeed::FPS djvTimePrefs::speed() const
{
    return djvSpeed::speed();
}

void djvTimePrefs::setTimeUnits(djvTime::UNITS units)
{
    if (units == this->timeUnits())
        return;
    djvTime::setUnits(units);
    Q_EMIT timeUnitsChanged(this->timeUnits());
}

void djvTimePrefs::setSpeed(djvSpeed::FPS speed)
{
    if (speed == this->speed())
        return;
    djvSpeed::setSpeed(speed);
    Q_EMIT speedChanged(this->speed());
}

