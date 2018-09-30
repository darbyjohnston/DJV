//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
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

#include <djvViewLib/ViewPrefs.h>

#include <djvUI/Prefs.h>

namespace djv
{
    namespace ViewLib
    {
        ViewPrefs::ViewPrefs(const QPointer<Context> & context, QObject * parent) :
            AbstractPrefs(context, parent),
            _zoomFactor(zoomFactorDefault()),
            _background(backgroundDefault()),
            _grid(gridDefault()),
            _gridColor(gridColorDefault()),
            _hudEnabled(hudEnabledDefault()),
            _hudInfo(hudInfoDefault()),
            _hudColor(hudColorDefault()),
            _hudBackground(hudBackgroundDefault()),
            _hudBackgroundColor(hudBackgroundColorDefault())
        {
            //DJV_DEBUG("ViewPrefs::ViewPrefs");
            UI::Prefs prefs("djv::ViewLib::ViewPrefs");
            prefs.get("zoomFactor", _zoomFactor);
            prefs.get("background", _background);
            prefs.get("grid", _grid);
            prefs.get("gridColor", _gridColor);
            prefs.get("hudEnabled", _hudEnabled);
            if (prefs.contains("hudInfo"))
            {
                _hudInfo.clear();
                prefs.get("hudInfo", _hudInfo);
                _hudInfo.resize(Enum::HUD_COUNT);
            }
            prefs.get("hudColor", _hudColor);
            prefs.get("hudBackground", _hudBackground);
            prefs.get("hudBackgroundColor", _hudBackgroundColor);
        }

        ViewPrefs::~ViewPrefs()
        {
            //DJV_DEBUG("ViewPrefs::~ViewPrefs");
            UI::Prefs prefs("djv::ViewLib::ViewPrefs");
            prefs.set("zoomFactor", _zoomFactor);
            prefs.set("background", _background);
            prefs.set("grid", _grid);
            prefs.set("gridColor", _gridColor);
            prefs.set("hudEnabled", _hudEnabled);
            prefs.set("hudInfo", _hudInfo);
            prefs.set("hudColor", _hudColor);
            prefs.set("hudBackground", _hudBackground);
            prefs.set("hudBackgroundColor", _hudBackgroundColor);
        }

        Enum::ZOOM_FACTOR ViewPrefs::zoomFactorDefault()
        {
            return Enum::ZOOM_FACTOR_10;
        }

        Enum::ZOOM_FACTOR ViewPrefs::zoomFactor() const
        {
            return _zoomFactor;
        }

        Graphics::Color ViewPrefs::backgroundDefault()
        {
            return Graphics::Color(0.f);
        }

        const Graphics::Color & ViewPrefs::background() const
        {
            return _background;
        }

        Enum::GRID ViewPrefs::gridDefault()
        {
            return Enum::GRID_NONE;
        }

        Enum::GRID ViewPrefs::grid() const
        {
            return _grid;
        }

        Graphics::Color ViewPrefs::gridColorDefault()
        {
            return Graphics::Color(0.25);
        }

        const Graphics::Color & ViewPrefs::gridColor() const
        {
            return _gridColor;
        }

        bool ViewPrefs::hudEnabledDefault()
        {
            return false;
        }

        bool ViewPrefs::isHudEnabled() const
        {
            return _hudEnabled;
        }

        QVector<bool> ViewPrefs::hudInfoDefault()
        {
            return QVector<bool>(Enum::HUD_COUNT, true);
        }

        QVector<bool> ViewPrefs::hudInfo() const
        {
            return _hudInfo;
        }

        bool ViewPrefs::isHudInfo(Enum::HUD in) const
        {
            return _hudInfo[in] != 0 ? true : false;
        }

        Graphics::Color ViewPrefs::hudColorDefault()
        {
            return Graphics::Color(1.f);
        }

        const Graphics::Color & ViewPrefs::hudColor() const
        {
            return _hudColor;
        }

        Enum::HUD_BACKGROUND ViewPrefs::hudBackgroundDefault()
        {
            return Enum::HUD_BACKGROUND_SHADOW;
        }

        Enum::HUD_BACKGROUND ViewPrefs::hudBackground() const
        {
            return _hudBackground;
        }

        Graphics::Color ViewPrefs::hudBackgroundColorDefault()
        {
            return Graphics::Color(0.f, 0.5f);
        }

        const Graphics::Color & ViewPrefs::hudBackgroundColor() const
        {
            return _hudBackgroundColor;
        }

        void ViewPrefs::setZoomFactor(Enum::ZOOM_FACTOR in)
        {
            if (in == _zoomFactor)
                return;
            _zoomFactor = in;
            Q_EMIT zoomFactorChanged(_zoomFactor);
            Q_EMIT prefChanged();

        }

        void ViewPrefs::setBackground(const Graphics::Color & in)
        {
            if (in == _background)
                return;
            _background = in;
            Q_EMIT backgroundChanged(_background);
            Q_EMIT prefChanged();
        }

        void ViewPrefs::setGrid(Enum::GRID in)
        {
            if (in == _grid)
                return;
            _grid = in;
            Q_EMIT gridChanged(_grid);
            Q_EMIT prefChanged();
        }

        void ViewPrefs::setGridColor(const Graphics::Color & in)
        {
            if (in == _gridColor)
                return;
            _gridColor = in;
            Q_EMIT gridColorChanged(_gridColor);
            Q_EMIT prefChanged();
        }

        void ViewPrefs::setHudEnabled(bool in)
        {
            if (in == _hudEnabled)
                return;
            _hudEnabled = in;
            Q_EMIT hudEnabledChanged(_hudEnabled);
            Q_EMIT prefChanged();
        }

        void ViewPrefs::setHudInfo(const QVector<bool> & info)
        {
            if (info == _hudInfo)
                return;
            //DJV_DEBUG("ViewPrefs::setHudInfo");
            //DJV_DEBUG_PRINT("info = " << info);
            _hudInfo = info;
            Q_EMIT hudInfoChanged(_hudInfo);
            Q_EMIT prefChanged();
        }

        void ViewPrefs::setHudInfo(Enum::HUD info, bool in)
        {
            const bool tmp = _hudInfo[info] != 0 ? true : false;
            if (in == tmp)
                return;
            _hudInfo[info] = in;
            Q_EMIT hudInfoChanged(_hudInfo);
            Q_EMIT prefChanged();
        }

        void ViewPrefs::setHudColor(const Graphics::Color & in)
        {
            if (in == _hudColor)
                return;
            _hudColor = in;
            Q_EMIT hudColorChanged(_hudColor);
            Q_EMIT prefChanged();
        }

        void ViewPrefs::setHudBackground(Enum::HUD_BACKGROUND in)
        {
            if (in == _hudBackground)
                return;
            _hudBackground = in;
            Q_EMIT hudBackgroundChanged(_hudBackground);
            Q_EMIT prefChanged();
        }

        void ViewPrefs::setHudBackgroundColor(const Graphics::Color & in)
        {
            if (in == _hudBackgroundColor)
                return;
            _hudBackgroundColor = in;
            Q_EMIT hudBackgroundColorChanged(_hudBackgroundColor);
            Q_EMIT prefChanged();
        }

    } // namespace ViewLib
} // namespace djv
