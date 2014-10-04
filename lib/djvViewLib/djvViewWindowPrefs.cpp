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

//! \file djvViewWindowPrefs.cpp

#include <djvViewWindowPrefs.h>

#include <djvViewApplication.h>

#include <djvPrefs.h>

//------------------------------------------------------------------------------
// djvViewWindowPrefs
//------------------------------------------------------------------------------

djvViewWindowPrefs::djvViewWindowPrefs(QObject * parent) :
    djvViewAbstractPrefs(parent),
    _resizeFit(resizeFitDefault()),
    _resizeMax(resizeMaxDefault()),
    _toolBar  (toolBarDefault())
{
    //DJV_DEBUG("djvViewWindowPrefs");

    djvPrefs prefs("djvViewWindowPrefs");
    prefs.get("resizeFit", _resizeFit);
    prefs.get("resizeMax", _resizeMax);
    if (prefs.contains("toolBar"))
    {
        _toolBar.clear();
        prefs.get("toolBar", _toolBar);
        _toolBar.resize(djvView::TOOL_BAR_COUNT);
    }
}

djvViewWindowPrefs::~djvViewWindowPrefs()
{
    djvPrefs prefs("djvViewWindowPrefs");
    prefs.set("resizeFit", _resizeFit);
    prefs.set("resizeMax", _resizeMax);
    prefs.set("toolBar", _toolBar);
}

bool djvViewWindowPrefs::resizeFitDefault()
{
    return true;
}

bool djvViewWindowPrefs::hasResizeFit() const
{
    return _resizeFit;
}

djvView::WINDOW_RESIZE_MAX djvViewWindowPrefs::resizeMaxDefault()
{
    return djvView::WINDOW_RESIZE_MAX_75;
}

djvView::WINDOW_RESIZE_MAX djvViewWindowPrefs::resizeMax() const
{
    return _resizeMax;
}

QVector<bool> djvViewWindowPrefs::toolBarDefault()
{
    return QVector<bool>(djvView::TOOL_BAR_COUNT, true);
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

void djvViewWindowPrefs::setResizeFit(bool in)
{
    if (in == _resizeFit)
        return;
    
    _resizeFit = in;

    Q_EMIT resizeFitChanged(_resizeFit);
    Q_EMIT prefChanged();
}

void djvViewWindowPrefs::setResizeMax(djvView::WINDOW_RESIZE_MAX in)
{
    if (in == _resizeMax)
        return;
    
    _resizeMax = in;

    Q_EMIT resizeMaxChanged(_resizeMax);
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
