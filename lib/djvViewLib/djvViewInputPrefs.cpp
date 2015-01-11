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

//! \file djvViewInputPrefs.cpp

#include <djvViewInputPrefs.h>

#include <djvViewApplication.h>

#include <djvPrefs.h>

//------------------------------------------------------------------------------
// djvViewInputPrefs
//------------------------------------------------------------------------------

djvViewInputPrefs::djvViewInputPrefs(QObject * parent) :
    djvViewAbstractPrefs(parent),
    _mouseWheel     (mouseWheelDefault()),
    _mouseWheelShift(mouseWheelShiftDefault()),
    _mouseWheelCtrl (mouseWheelCtrlDefault())
{
    //DJV_DEBUG("djvViewInputPrefs::djvViewInputPrefs");

    djvPrefs prefs("djvViewInputPrefs");
    prefs.get("mouseWheel", _mouseWheel);
    prefs.get("mouseWheelShift", _mouseWheelShift);
    prefs.get("mouseWheelCtrl", _mouseWheelCtrl);
}

djvViewInputPrefs::~djvViewInputPrefs()
{
    //DJV_DEBUG("Prefs::~Prefs");

    djvPrefs prefs("djvViewInputPrefs");
    prefs.set("mouseWheel", _mouseWheel);
    prefs.set("mouseWheelShift", _mouseWheelShift);
    prefs.set("mouseWheelCtrl", _mouseWheelCtrl);
}

djvViewUtil::MOUSE_WHEEL djvViewInputPrefs::mouseWheelDefault()
{
    return djvViewUtil::MOUSE_WHEEL_VIEW_ZOOM;
}

djvViewUtil::MOUSE_WHEEL djvViewInputPrefs::mouseWheel() const
{
    return _mouseWheel;
}

djvViewUtil::MOUSE_WHEEL djvViewInputPrefs::mouseWheelShiftDefault()
{
    return djvViewUtil::MOUSE_WHEEL_PLAYBACK_SHUTTLE;
}

djvViewUtil::MOUSE_WHEEL djvViewInputPrefs::mouseWheelShift() const
{
    return _mouseWheelShift;
}

djvViewUtil::MOUSE_WHEEL djvViewInputPrefs::mouseWheelCtrlDefault()
{
    return djvViewUtil::MOUSE_WHEEL_PLAYBACK_SPEED;
}

djvViewUtil::MOUSE_WHEEL djvViewInputPrefs::mouseWheelCtrl() const
{
    return _mouseWheelCtrl;
}

djvViewInputPrefs * djvViewInputPrefs::global()
{
    static djvViewInputPrefs * prefs = 0;

    if (!prefs)
    {
        prefs = new djvViewInputPrefs(DJV_VIEW_APP);
    }

    return prefs;
}

void djvViewInputPrefs::setMouseWheel(djvViewUtil::MOUSE_WHEEL in)
{
    if (in == _mouseWheel)
        return;

    _mouseWheel = in;

    Q_EMIT prefChanged();
}

void djvViewInputPrefs::setMouseWheelShift(djvViewUtil::MOUSE_WHEEL in)
{
    if (in == _mouseWheelShift)
        return;

    _mouseWheelShift = in;

    Q_EMIT prefChanged();
}

void djvViewInputPrefs::setMouseWheelCtrl(djvViewUtil::MOUSE_WHEEL in)
{
    if (in == _mouseWheelCtrl)
        return;

    _mouseWheelCtrl = in;

    Q_EMIT prefChanged();
}

