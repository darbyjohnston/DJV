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
        WindowPrefs::WindowPrefs(Context * context, QObject * parent) :
            AbstractPrefs(context, parent),
            _autoFit(autoFitDefault()),
            _viewMax(viewMaxDefault()),
            _viewMaxUser(viewMaxUserDefault()),
            _fullScreenControls(fullScreenControlsDefault()),
            _toolBar(toolBarDefault())
        {
            //DJV_DEBUG("WindowPrefs::WindowPrefs");
            //DJV_DEBUG_PRINT("toolBar = " << _toolBar);
            UI::Prefs prefs("djv::ViewLib::WindowPrefs");
            prefs.get("autoFit", _autoFit);
            prefs.get("viewMax", _viewMax);
            prefs.get("viewMaxUser", _viewMaxUser);
            prefs.get("fullScreenControls", _fullScreenControls);
            if (prefs.contains("toolBar"))
            {
                _toolBar.clear();
                prefs.get("toolBar", _toolBar);
                _toolBar.resize(Util::TOOL_BAR_COUNT);
            }
        }

        WindowPrefs::~WindowPrefs()
        {
            //DJV_DEBUG("WindowPrefs::~WindowPrefs");
            //DJV_DEBUG_PRINT("toolBar = " << _toolBar);
            UI::Prefs prefs("djv::ViewLib::WindowPrefs");
            prefs.set("autoFit", _autoFit);
            prefs.set("viewMax", _viewMax);
            prefs.set("viewMaxUser", _viewMaxUser);
            prefs.set("fullScreenControls", _fullScreenControls);
            prefs.set("toolBar", _toolBar);
        }

        bool WindowPrefs::autoFitDefault()
        {
            return true;
        }

        bool WindowPrefs::hasAutoFit() const
        {
            return _autoFit;
        }

        Util::VIEW_MAX WindowPrefs::viewMaxDefault()
        {
            return Util::VIEW_MAX_75;
        }

        Util::VIEW_MAX WindowPrefs::viewMax() const
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

        bool WindowPrefs::fullScreenControlsDefault()
        {
            return false;
        }

        bool WindowPrefs::hasFullScreenControls() const
        {
            return _fullScreenControls;
        }

        QVector<bool> WindowPrefs::toolBarDefault()
        {
            return QVector<bool>(Util::TOOL_BAR_COUNT, true);
        }

        const QVector<bool> & WindowPrefs::toolBar() const
        {
            return _toolBar;
        }

        void WindowPrefs::setAutoFit(bool in)
        {
            if (in == _autoFit)
                return;
            _autoFit = in;
            Q_EMIT autoFitChanged(_autoFit);
            Q_EMIT prefChanged();
        }

        void WindowPrefs::setViewMax(Util::VIEW_MAX in)
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

        void WindowPrefs::setFullScreenControls(bool in)
        {
            if (in == _fullScreenControls)
                return;
            _fullScreenControls = in;
            Q_EMIT fullScreenControlsChanged(_fullScreenControls);
            Q_EMIT prefChanged();
        }

        void WindowPrefs::setToolBar(const QVector<bool> & in)
        {
            if (in == _toolBar)
                return;
            _toolBar = in;
            Q_EMIT toolBarChanged(_toolBar);
            Q_EMIT prefChanged();
        }

    } // namespace ViewLib
} // namespace djv
