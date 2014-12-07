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

//! \file djvViewViewPrefs.cpp

#include <djvViewViewPrefs.h>

#include <djvViewApplication.h>

#include <djvPrefs.h>

//------------------------------------------------------------------------------
// djvViewViewPrefs
//------------------------------------------------------------------------------

djvViewViewPrefs::djvViewViewPrefs(QObject * parent) :
    djvViewAbstractPrefs(parent),
    _viewSize          (viewSizeDefault()),
    _background        (backgroundDefault()),
    _resize            (resizeDefault()),
    _grid              (gridDefault()),
    _gridColor         (gridColorDefault()),
    _hudEnabled        (hudEnabledDefault()),
    _hudVisible        (hudVisibleDefault()),
    _hudColor          (hudColorDefault()),
    _hudBackground     (hudBackgroundDefault()),
    _hudBackgroundColor(hudBackgroundColorDefault())
{
    //DJV_DEBUG("djvViewViewPrefs::djvViewViewPrefs");

    djvPrefs prefs("djvViewViewPrefs");
    prefs.get("background", _background);
    prefs.get("viewSize", _viewSize);
    prefs.get("resize", _resize);
    prefs.get("grid", _grid);
    prefs.get("gridColor", _gridColor);
    prefs.get("hudEnabled", _hudEnabled);
    if (prefs.contains("hudVisible"))
    {
        _hudVisible.clear();
        prefs.get("hudVisible", _hudVisible);
        _hudVisible.resize(djvView::HUD_COUNT);
    }
    prefs.get("hudColor", _hudColor);
    prefs.get("hudBackground", _hudBackground);
    prefs.get("hudBackgroundColor", _hudBackgroundColor);

    //DJV_DEBUG_PRINT("hudVisible = " << _hudVisible);
}

djvViewViewPrefs::~djvViewViewPrefs()
{
    //DJV_DEBUG("djvViewViewPrefs::~djvViewViewPrefs");

    //DJV_DEBUG_PRINT("hudVisible = " << _hudVisible);

    djvPrefs prefs("djvViewViewPrefs");
    prefs.set("background", _background);
    prefs.set("viewSize", _viewSize);
    prefs.set("resize", _resize);
    prefs.set("grid", _grid);
    prefs.set("gridColor", _gridColor);
    prefs.set("hudEnabled", _hudEnabled);
    prefs.set("hudVisible", _hudVisible);
    prefs.set("hudColor", _hudColor);
    prefs.set("hudBackground", _hudBackground);
    prefs.set("hudBackgroundColor", _hudBackgroundColor);
}

djvColor djvViewViewPrefs::backgroundDefault()
{
    return djvColor(0.0);
}

const djvColor & djvViewViewPrefs::background() const
{
    return _background;
}

const djvVector2i & djvViewViewPrefs::viewSizeDefault()
{
    static const djvVector2i viewSize(640, 300);
    
    return viewSize;
}

const djvVector2i & djvViewViewPrefs::viewSize() const
{
    return _viewSize;
}

djvView::VIEW_RESIZE djvViewViewPrefs::resizeDefault()
{
    return djvView::VIEW_RESIZE_NOTHING;
}

djvView::VIEW_RESIZE djvViewViewPrefs::resize() const
{
    return _resize;
}

djvView::GRID djvViewViewPrefs::gridDefault()
{
    return djvView::GRID_NONE;
}

djvView::GRID djvViewViewPrefs::grid() const
{
    return _grid;
}

djvColor djvViewViewPrefs::gridColorDefault()
{
    return djvColor(0.25);
}

const djvColor & djvViewViewPrefs::gridColor() const
{
    return _gridColor;
}

bool djvViewViewPrefs::hudEnabledDefault()
{
    return false;
}

bool djvViewViewPrefs::isHudEnabled() const
{
    return _hudEnabled;
}

QVector<bool> djvViewViewPrefs::hudVisibleDefault()
{
    return QVector<bool>(djvView::HUD_COUNT, true);
}

QVector<bool> djvViewViewPrefs::hudVisible() const
{
    return _hudVisible;
}

bool djvViewViewPrefs::isHudVisible(djvView::HUD in) const
{
    return _hudVisible[in] != 0 ? true : false;
}

djvColor djvViewViewPrefs::hudColorDefault()
{
    return djvColor(1.0);
}

const djvColor & djvViewViewPrefs::hudColor() const
{
    return _hudColor;
}

djvView::HUD_BACKGROUND djvViewViewPrefs::hudBackgroundDefault()
{
    return djvView::HUD_BACKGROUND_SHADOW;
}

djvView::HUD_BACKGROUND djvViewViewPrefs::hudBackground() const
{
    return _hudBackground;
}

djvColor djvViewViewPrefs::hudBackgroundColorDefault()
{
    return djvColor(0.0, 0.5);
}

const djvColor & djvViewViewPrefs::hudBackgroundColor() const
{
    return _hudBackgroundColor;
}

djvViewViewPrefs * djvViewViewPrefs::global()
{
    static djvViewViewPrefs * prefs = 0;

    if (! prefs)
    {
        prefs = new djvViewViewPrefs(DJV_VIEW_APP);
    }

    return prefs;
}

void djvViewViewPrefs::setBackground(const djvColor & in)
{
    if (in == _background)
        return;

    _background = in;

    Q_EMIT backgroundChanged(_background);
    Q_EMIT prefChanged();
}

void djvViewViewPrefs::setViewSize(const djvVector2i & size)
{
    if (size == _viewSize)
        return;
    
    _viewSize = size;
    
    Q_EMIT viewSizeChanged(_viewSize);
    Q_EMIT prefChanged();
}

void djvViewViewPrefs::setResize(djvView::VIEW_RESIZE in)
{
    if (in == _resize)
        return;

    _resize = in;

    Q_EMIT resizeChanged(_resize);
    Q_EMIT prefChanged();
}

void djvViewViewPrefs::setGrid(djvView::GRID in)
{
    if (in == _grid)
        return;

    _grid = in;

    Q_EMIT gridChanged(_grid);
    Q_EMIT prefChanged();
}

void djvViewViewPrefs::setGridColor(const djvColor & in)
{
    if (in == _gridColor)
        return;

    _gridColor = in;

    Q_EMIT gridColorChanged(_gridColor);
    Q_EMIT prefChanged();
}

void djvViewViewPrefs::setHudEnabled(bool in)
{
    if (in == _hudEnabled)
        return;

    _hudEnabled = in;

    Q_EMIT hudEnabledChanged(_hudEnabled);
    Q_EMIT prefChanged();
}

void djvViewViewPrefs::setHudVisible(const QVector<bool> & visible)
{
    if (visible == _hudVisible)
        return;

    //DJV_DEBUG("djvViewViewPrefs::setHudVisible");
    //DJV_DEBUG_PRINT("visible = " << visible);

    _hudVisible = visible;

    Q_EMIT hudVisibleChanged(_hudVisible);
    Q_EMIT prefChanged();
}

void djvViewViewPrefs::setHudVisible(djvView::HUD visible, bool in)
{
    const bool tmp = _hudVisible[visible] != 0 ? true : false;

    if (in == tmp)
        return;

    _hudVisible[visible] = in;

    Q_EMIT hudVisibleChanged(_hudVisible);
    Q_EMIT prefChanged();
}

void djvViewViewPrefs::setHudColor(const djvColor & in)
{
    if (in == _hudColor)
        return;

    _hudColor = in;

    Q_EMIT hudColorChanged(_hudColor);
    Q_EMIT prefChanged();
}

void djvViewViewPrefs::setHudBackground(djvView::HUD_BACKGROUND in)
{
    if (in == _hudBackground)
        return;

    _hudBackground = in;

    Q_EMIT hudBackgroundChanged(_hudBackground);
    Q_EMIT prefChanged();
}

void djvViewViewPrefs::setHudBackgroundColor(const djvColor & in)
{
    if (in == _hudBackgroundColor)
        return;

    _hudBackgroundColor = in;
    
    Q_EMIT hudBackgroundColorChanged(_hudBackgroundColor);
    Q_EMIT prefChanged();
}

