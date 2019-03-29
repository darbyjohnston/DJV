//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
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

#include <djvUI/TimePrefs.h>

#include <djvUI/Prefs.h>

#include <djvCore/Debug.h>

#include <QApplication>

namespace djv
{
    namespace UI
    {
        struct TimePrefs::Private
        {
        };

        TimePrefs::TimePrefs(QObject * parent) :
            QObject(parent),
            _p(new Private)
        {
            //DJV_DEBUG("TimePrefs::TimePrefs");
            Prefs prefs("djv::UI::TimePrefs", Prefs::SYSTEM);
            Core::Time::UNITS units = Core::Time::units();
            if (prefs.get("units", units))
            {
                Core::Time::setUnits(units);
            }
            Core::Speed::FPS speed = Core::Speed::speed();
            if (prefs.get("speed", speed))
            {
                Core::Speed::setSpeed(speed);
            }
        }

        TimePrefs::~TimePrefs()
        {
            //DJV_DEBUG("TimePrefs::~TimePrefs");
            Prefs prefs("djv::UI::TimePrefs", Prefs::SYSTEM);
            prefs.set("units", Core::Time::units());
            prefs.set("speed", Core::Speed::speed());
        }

        Core::Time::UNITS TimePrefs::units() const
        {
            return Core::Time::units();
        }

        Core::Speed::FPS TimePrefs::speed() const
        {
            return Core::Speed::speed();
        }

        void TimePrefs::setUnits(Core::Time::UNITS units)
        {
            if (units == Core::Time::units())
                return;
            Core::Time::setUnits(units);
            Q_EMIT unitsChanged(Core::Time::units());
            Q_EMIT prefChanged();
        }

        void TimePrefs::setSpeed(Core::Speed::FPS speed)
        {
            if (speed == this->speed())
                return;
            Core::Speed::setSpeed(speed);
            Q_EMIT speedChanged(this->speed());
            Q_EMIT prefChanged();
        }

    } // namespace UI
} // namespace djv
