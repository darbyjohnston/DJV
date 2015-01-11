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

//! \file djvMiscPrefs.cpp

#include <djvMiscPrefs.h>

#include <djvPrefs.h>

#include <djvDebug.h>
#include <djvSequence.h>

#include <QApplication>

//------------------------------------------------------------------------------
// djvMiscPrefs
//------------------------------------------------------------------------------

djvMiscPrefs::djvMiscPrefs(QObject * parent) :
    QObject(parent)
{
    //DJV_DEBUG("djvMiscPrefs::djvMiscPrefs");

    djvPrefs prefs("djvMiscPrefs", djvPrefs::SYSTEM);

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

    qint64 sequenceMaxFrames = djvSequence::maxFrames();

    if (prefs.get("sequenceMaxFrames", sequenceMaxFrames))
    {
        djvSequence::setMaxFrames(sequenceMaxFrames);
    }
}

djvMiscPrefs::~djvMiscPrefs()
{
    //DJV_DEBUG("djvMiscPrefs::~djvMiscPrefs");

    djvPrefs prefs("djvMiscPrefs", djvPrefs::SYSTEM);

    prefs.set("timeUnits", djvTime::units());
    prefs.set("speed", djvSpeed::speed());
    prefs.set("sequenceMaxFrames", djvSequence::maxFrames());
}

djvTime::UNITS djvMiscPrefs::timeUnits() const
{
    return djvTime::units();
}

djvSpeed::FPS djvMiscPrefs::speed() const
{
    return djvSpeed::speed();
}

qint64 djvMiscPrefs::sequenceMaxFrames() const
{
    return djvSequence::maxFrames();
}

djvMiscPrefs * djvMiscPrefs::global()
{
    static djvMiscPrefs * global = 0;
    
    if (! global)
    {
        global = new djvMiscPrefs(qApp);
    }
    
    return global;
}

void djvMiscPrefs::setTimeUnits(djvTime::UNITS units)
{
    if (units == this->timeUnits())
        return;

    djvTime::setUnits(units);

    Q_EMIT timeUnitsChanged(this->timeUnits());
}

void djvMiscPrefs::setSpeed(djvSpeed::FPS speed)
{
    if (speed == this->speed())
        return;

    djvSpeed::setSpeed(speed);

    Q_EMIT speedChanged(this->speed());
}

void djvMiscPrefs::setSequenceMaxFrames(qint64 size)
{
    if (size == this->sequenceMaxFrames())
        return;

    djvSequence::setMaxFrames(size);

    Q_EMIT sequenceMaxFramesChanged(this->sequenceMaxFrames());
}
