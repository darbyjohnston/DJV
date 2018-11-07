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

#include <djvViewLib/WindowGroup.h>

#include <djvViewLib/ControlsWindow.h>
#include <djvViewLib/ImageView.h>
#include <djvViewLib/MainWindow.h>
#include <djvViewLib/Session.h>
#include <djvViewLib/ViewContext.h>
#include <djvViewLib/WindowActions.h>
#include <djvViewLib/WindowMenu.h>
#include <djvViewLib/WindowPrefs.h>
#include <djvViewLib/WindowToolBar.h>

namespace djv
{
    namespace ViewLib
    {
        struct WindowGroup::Private
        {
            Private(const QPointer<ViewContext> & context) :
                uiComponentVisible(context->windowPrefs()->uiComponentVisible()),
                controlsWindow(context->windowPrefs()->hasControlsWindow())
            {}

            bool                           fullScreen = false;
            bool                           uiVisible = true;
            bool                           uiVisiblePrev = true;
            QMap<Enum::UI_COMPONENT, bool> uiComponentVisible;
            bool                           controlsWindow = false;
            bool                           controlsWindowPrev = false;

            QPointer<WindowActions> actions;
        };

        WindowGroup::WindowGroup(
            const QPointer<WindowGroup> & copy,
            const QPointer<Session> & session,
            const QPointer<ViewContext> & context) :
            AbstractGroup(session, context),
            _p(new Private(context))
        {
            //DJV_DEBUG("WindowGroup::WindowGroup");

            if (copy)
            {
                _p->uiVisible = copy->_p->uiVisible;
                _p->uiComponentVisible = copy->_p->uiComponentVisible;
            }

            // Create the actions.
            _p->actions = new WindowActions(context, this);

            // Initialize.
            update();

            // Setup the action callbacks.
            connect(
                _p->actions->action(WindowActions::NEW),
                SIGNAL(triggered()),
                SLOT(newCallback()));
            connect(
                _p->actions->action(WindowActions::DUPLICATE),
                SIGNAL(triggered()),
                SLOT(duplicateCallback()));
            connect(
                _p->actions->action(WindowActions::CLOSE),
                SIGNAL(triggered()),
                SLOT(closeCallback()));
            connect(
                _p->actions->action(WindowActions::FIT),
                SIGNAL(triggered()),
                SLOT(fitCallback()));
            connect(
                _p->actions->action(WindowActions::FULL_SCREEN),
                SIGNAL(toggled(bool)),
                SLOT(setFullScreen(bool)));
            connect(
                _p->actions->action(WindowActions::UI_VISIBLE),
                SIGNAL(toggled(bool)),
                SLOT(setUIVisible(bool)));
            connect(
                _p->actions->group(WindowActions::UI_COMPONENT_VISIBLE_GROUP),
                SIGNAL(triggered(QAction *)),
                SLOT(uiComponentVisibleCallback(QAction *)));
            connect(
                _p->actions->action(WindowActions::DETACH_CONTROLS),
                SIGNAL(toggled(bool)),
                SLOT(setControlsWindow(bool)));

            // Setup the preferences callbacks.
            connect(
                context->windowPrefs(),
                SIGNAL(uiComponentVisibleChanged(const QMap<djv::ViewLib::Enum::UI_COMPONENT, bool> &)),
                SLOT(setUIComponentVisible(const QMap<djv::ViewLib::Enum::UI_COMPONENT, bool> &)));
            connect(
                context->windowPrefs(),
                SIGNAL(controlsWindowChanged(bool)),
                SLOT(setControlsWindow(bool)));
        }

        WindowGroup::~WindowGroup()
        {
            //DJV_DEBUG("WindowGroup::~WindowGroup");
        }

        bool WindowGroup::hasFullScreen() const
        {
            return _p->fullScreen;
        }

        bool WindowGroup::isUIVisible() const
        {
            return _p->uiVisible;
        }

        const QMap<Enum::UI_COMPONENT, bool> & WindowGroup::uiComponentVisible() const
        {
            return _p->uiComponentVisible;
        }

        bool WindowGroup::hasControlsWindow() const
        {
            return _p->controlsWindow;
        }

        QPointer<QMenu> WindowGroup::createMenu() const
        {
            return new WindowMenu(_p->actions.data());
        }

        QPointer<QToolBar> WindowGroup::createToolBar() const
        {
            return new WindowToolBar(_p->actions.data(), context());
        }

        void WindowGroup::setFullScreen(bool fullScreen)
        {
            if (fullScreen == _p->fullScreen)
                return;
            _p->fullScreen = fullScreen;
            auto mainWindow = session()->mainWindow();
            if (_p->fullScreen)
            {
                if (!mainWindow->isFullScreen())
                {
                    mainWindow->showFullScreen();
                    _p->uiVisiblePrev = _p->uiVisible;
                    _p->controlsWindowPrev = _p->controlsWindow;
                    switch (context()->windowPrefs()->fullScreenUI())
                    {
                    case Enum::FULL_SCREEN_UI_HIDE:
                        setUIVisible(false);
                        break;
                    case Enum::FULL_SCREEN_UI_SHOW:
                        setUIVisible(true);
                        break;
                    case Enum::FULL_SCREEN_UI_DETACH:
                        setUIVisible(false);
                        setControlsWindow(true);
                        break;
                    default: break;
                    }
                }
            }
            else
            {
                if (mainWindow->isFullScreen())
                {
                    mainWindow->showNormal();
                    switch (context()->windowPrefs()->fullScreenUI())
                    {
                    case Enum::FULL_SCREEN_UI_HIDE:
                    case Enum::FULL_SCREEN_UI_SHOW:
                        setUIVisible(_p->uiVisiblePrev);
                        break;
                    case Enum::FULL_SCREEN_UI_DETACH:
                        setUIVisible(_p->uiVisiblePrev);
                        setControlsWindow(_p->controlsWindowPrev);
                        break;
                    default: break;
                    }
                }
            }
            update();
            Q_EMIT fullScreenChanged(_p->fullScreen);
        }

        void WindowGroup::setControlsWindow(bool value)
        {
            if (value == _p->controlsWindow)
                return;
            _p->controlsWindow = value;
            update();
            Q_EMIT controlsWindowChanged(_p->controlsWindow);
        }

        void WindowGroup::setUIVisible(bool visible)
        {
            if (visible == _p->uiVisible)
                return;
            _p->uiVisible = visible;
            update();
            Q_EMIT uiVisibleChanged(_p->uiVisible);
        }

        void WindowGroup::setUIComponentVisible(const QMap<Enum::UI_COMPONENT, bool> & visible)
        {
            if (visible == _p->uiComponentVisible)
                return;
            _p->uiComponentVisible = visible;
            update();
            Q_EMIT uiComponentVisibleChanged(_p->uiComponentVisible);
        }

        void WindowGroup::newCallback()
        {
            auto session = new Session(nullptr, context());
            session->mainWindow()->show();
        }

        void WindowGroup::duplicateCallback()
        {
            auto session = new Session(this->session(), context());
            session->mainWindow()->show();
        }

        void WindowGroup::closeCallback()
        {
            session()->mainWindow()->close();
        }

        void WindowGroup::fitCallback()
        {
            session()->mainWindow()->fitWindow();
        }

        void WindowGroup::uiComponentVisibleCallback(QAction * action)
        {
            auto tmp = _p->uiComponentVisible;
            const Enum::UI_COMPONENT i = static_cast<Enum::UI_COMPONENT>(
                _p->actions->group(WindowActions::UI_COMPONENT_VISIBLE_GROUP)->actions().indexOf(action));
            tmp[i] = !tmp[i];
            setUIComponentVisible(tmp);
        }

        void WindowGroup::update()
        {
            _p->actions->action(WindowActions::FULL_SCREEN)->setChecked(_p->fullScreen);
            _p->actions->action(WindowActions::UI_VISIBLE)->setChecked(_p->uiVisible);
            for (int i = 0; i < Enum::UI_COMPONENT_COUNT; ++i)
            {
                _p->actions->group(WindowActions::UI_COMPONENT_VISIBLE_GROUP)->actions()[i]->setChecked(
                    _p->uiComponentVisible[static_cast<Enum::UI_COMPONENT>(i)]);
            }
            _p->actions->action(WindowActions::DETACH_CONTROLS)->setChecked(_p->controlsWindow);
        }

    } // namespace ViewLib
} // namespace djv
