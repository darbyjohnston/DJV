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

//! \file djvViewWindowPrefs.cpp

#include <djvViewWindowPrefs.h>

#include <djvViewApplication.h>

#include <djvPrefs.h>

//------------------------------------------------------------------------------
// djvViewWindowPrefs
//------------------------------------------------------------------------------

djvViewWindowPrefs::djvViewWindowPrefs(QObject * parent) :
    djvViewAbstractPrefs(parent),
    _autoFit           (autoFitDefault()),
    _viewMax           (viewMaxDefault()),
    _viewMaxUser       (viewMaxUserDefault()),
    _fullScreenControls(fullScreenControlsDefault()),
    _toolBar           (toolBarDefault())
{
    //DJV_DEBUG("djvViewWindowPrefs");

    djvPrefs prefs("djvViewWindowPrefs");
    prefs.get("autoFit", _autoFit);
    prefs.get("viewMax", _viewMax);
    prefs.get("viewMaxUser", _viewMaxUser);
    prefs.get("fullScreenControls", _fullScreenControls);
    if (prefs.contains("toolBar"))
    {
        _toolBar.clear();
        prefs.get("toolBar", _toolBar);
        _toolBar.resize(djvViewUtil::TOOL_BAR_COUNT);
    }
}

djvViewWindowPrefs::~djvViewWindowPrefs()
{
    djvPrefs prefs("djvViewWindowPrefs");
    prefs.set("autoFit", _autoFit);
    prefs.set("viewMax", _viewMax);
    prefs.set("viewMaxUser", _viewMaxUser);
    prefs.set("fullScreenControls", _fullScreenControls);
    prefs.set("toolBar", _toolBar);
}

bool djvViewWindowPrefs::autoFitDefault()
{
    return true;
}

bool djvViewWindowPrefs::hasAutoFit() const
{
    return _autoFit;
}

djvViewUtil::VIEW_MAX djvViewWindowPrefs::viewMaxDefault()
{
    return djvViewUtil::VIEW_MAX_75;
}

djvViewUtil::VIEW_MAX djvViewWindowPrefs::viewMax() const
{
    return _viewMax;
}

const djvVector2i & djvViewWindowPrefs::viewMaxUserDefault()
{
    static const djvVector2i size(640, 300);
    
    return size;
}

const djvVector2i & djvViewWindowPrefs::viewMaxUser() const
{
    return _viewMaxUser;
}

bool djvViewWindowPrefs::fullScreenControlsDefault()
{
    return false;
}

bool djvViewWindowPrefs::hasFullScreenControls() const
{
    return _fullScreenControls;
}

QVector<bool> djvViewWindowPrefs::toolBarDefault()
{
    return QVector<bool>(djvViewUtil::TOOL_BAR_COUNT, true);
}

const QVector<bool> & djvViewWindowPrefs::toolBar() const
{
    return _toolBar;
}

djvViewWindowPrefs * djvViewWindowPrefs::global()
{
    static djvViewWindowPrefs * prefs = 0;

    if (!prefs)
    {
        prefs = new djvViewWindowPrefs(DJV_VIEW_APP);
    }

    return prefs;
}

void djvViewWindowPrefs::setAutoFit(bool in)
{
    if (in == _autoFit)
        return;
    
    _autoFit = in;

    Q_EMIT autoFitChanged(_autoFit);
    Q_EMIT prefChanged();
}

void djvViewWindowPrefs::setViewMax(djvViewUtil::VIEW_MAX in)
{
    if (in == _viewMax)
        return;

    _viewMax = in;

    Q_EMIT viewMaxChanged(_viewMax);
    Q_EMIT prefChanged();
}

void djvViewWindowPrefs::setViewMaxUser(const djvVector2i & size)
{
    if (size == _viewMaxUser)
        return;
    
    _viewMaxUser = size;
    
    Q_EMIT viewMaxUserChanged(_viewMaxUser);
    Q_EMIT prefChanged();
}

void djvViewWindowPrefs::setFullScreenControls(bool in)
{
    if (in == _fullScreenControls)
        return;

    _fullScreenControls = in;

    Q_EMIT fullScreenControlsChanged(_fullScreenControls);
    Q_EMIT prefChanged();
}

void djvViewWindowPrefs::setToolBar(const QVector<bool> & in)
{
    if (in == _toolBar)
        return;

    _toolBar = in;

    Q_EMIT toolBarChanged(_toolBar);
    Q_EMIT prefChanged();
}
