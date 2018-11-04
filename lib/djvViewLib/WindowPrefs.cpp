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

#include <djvViewLib/WindowPrefs.h>

#include <djvUI/Prefs.h>

namespace djv
{
    namespace ViewLib
    {
        WindowPrefs::WindowPrefs(const QPointer<ViewContext> & context, QObject * parent) :
            AbstractPrefs(context, parent),
            _autoFit(autoFitDefault()),
            _viewMax(viewMaxDefault()),
            _viewMaxUser(viewMaxUserDefault()),
            _fullScreenUI(fullScreenUIDefault()),
            _uiComponentVisible(uiComponentVisibleDefault())
        {
            //DJV_DEBUG("WindowPrefs::WindowPrefs");
            //DJV_DEBUG_PRINT("ui components visible = " << _uiComponentVisible);
            UI::Prefs prefs("djv::ViewLib::WindowPrefs");
            prefs.get("autoFit", _autoFit);
            prefs.get("viewMax", _viewMax);
            prefs.get("viewMaxUser", _viewMaxUser);
            prefs.get("fullScreenUI", _fullScreenUI);
            if (prefs.contains("uiComponentVisible"))
            {
                _uiComponentVisible.clear();
                prefs.get("uiComponentVisible", _uiComponentVisible);
                _uiComponentVisible.resize(Enum::UI_COMPONENT_COUNT);
            }
        }

        WindowPrefs::~WindowPrefs()
        {
            //DJV_DEBUG("WindowPrefs::~WindowPrefs");
            //DJV_DEBUG_PRINT("ui components visible = " << _uiComponentVisible);
            UI::Prefs prefs("djv::ViewLib::WindowPrefs");
            prefs.set("autoFit", _autoFit);
            prefs.set("viewMax", _viewMax);
            prefs.set("viewMaxUser", _viewMaxUser);
            prefs.set("fullScreenUI", _fullScreenUI);
            prefs.set("uiComponentVisible", _uiComponentVisible);
        }

        bool WindowPrefs::autoFitDefault()
        {
            return true;
        }

        bool WindowPrefs::hasAutoFit() const
        {
            return _autoFit;
        }

        Enum::VIEW_MAX WindowPrefs::viewMaxDefault()
        {
            return Enum::VIEW_MAX_75;
        }

        Enum::VIEW_MAX WindowPrefs::viewMax() const
        {
            return _viewMax;
        }

        const glm::ivec2 & WindowPrefs::viewMaxUserDefault()
        {
            static const glm::ivec2 size(640, 300);
            return size;
        }

        const glm::ivec2 & WindowPrefs::viewMaxUser() const
        {
            return _viewMaxUser;
        }

        Enum::FULL_SCREEN_UI WindowPrefs::fullScreenUIDefault()
        {
            return Enum::FULL_SCREEN_UI_DETACH;
        }

        Enum::FULL_SCREEN_UI WindowPrefs::fullScreenUI() const
        {
            return _fullScreenUI;
        }

        QVector<bool> WindowPrefs::uiComponentVisibleDefault()
        {
            return QVector<bool>(Enum::UI_COMPONENT_COUNT, true);
        }

        const QVector<bool> & WindowPrefs::uiComponentVisible() const
        {
            return _uiComponentVisible;
        }

        void WindowPrefs::setAutoFit(bool in)
        {
            if (in == _autoFit)
                return;
            _autoFit = in;
            Q_EMIT autoFitChanged(_autoFit);
            Q_EMIT prefChanged();
        }

        void WindowPrefs::setViewMax(Enum::VIEW_MAX in)
        {
            if (in == _viewMax)
                return;
            _viewMax = in;
            Q_EMIT viewMaxChanged(_viewMax);
            Q_EMIT prefChanged();
        }

        void WindowPrefs::setViewMaxUser(const glm::ivec2 & size)
        {
            if (size == _viewMaxUser)
                return;
            _viewMaxUser = size;
            Q_EMIT viewMaxUserChanged(_viewMaxUser);
            Q_EMIT prefChanged();
        }

        void WindowPrefs::setFullScreenUI(Enum::FULL_SCREEN_UI in)
        {
            if (in == _fullScreenUI)
                return;
            _fullScreenUI = in;
            Q_EMIT fullScreenUIChanged(_fullScreenUI);
            Q_EMIT prefChanged();
        }

        void WindowPrefs::setUIComponentVisible(const QVector<bool> & in)
        {
            if (in == _uiComponentVisible)
                return;
            _uiComponentVisible = in;
            Q_EMIT uiComponentVisibleChanged(_uiComponentVisible);
            Q_EMIT prefChanged();
        }

    } // namespace ViewLib
} // namespace djv
