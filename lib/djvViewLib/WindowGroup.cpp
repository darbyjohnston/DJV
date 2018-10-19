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

#include <djvViewLib/Context.h>
#include <djvViewLib/ImageView.h>
#include <djvViewLib/MainWindow.h>
#include <djvViewLib/WindowActions.h>
#include <djvViewLib/WindowMenu.h>
#include <djvViewLib/WindowPrefs.h>
#include <djvViewLib/WindowToolBar.h>

#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QPointer>
#include <QToolBar>
#include <QToolButton>

namespace djv
{
    namespace ViewLib
    {
        struct WindowGroup::Private
        {
            Private(const QPointer<Context> & context) :
                uiComponentVisible(context->windowPrefs()->uiComponentVisible())
            {}

            bool          fullScreen = false;
            bool          uiVisible = true;
            QVector<bool> uiComponentVisible;

            QPointer<WindowActions> actions;
            QPointer<WindowMenu>    menu;
            QPointer<WindowToolBar> toolBar;
        };

        WindowGroup::WindowGroup(
            const QPointer<WindowGroup> & copy,
            const QPointer<MainWindow> & mainWindow,
            const QPointer<Context> & context) :
            AbstractGroup(mainWindow, context),
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

            // Create the menus.
            _p->menu = new WindowMenu(_p->actions.data(), mainWindow->menuBar());
            mainWindow->menuBar()->addMenu(_p->menu);

            // Create the widgets.
            _p->toolBar = new WindowToolBar(_p->actions.data(), context);
            mainWindow->addToolBar(_p->toolBar);

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

            // Setup the preferences callbacks.
            connect(
                context->windowPrefs(),
                SIGNAL(uiComponentVisibleChanged(const QVector<bool> &)),
                SLOT(setUIComponentVisible(const QVector<bool> &)));
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

        const QVector<bool> & WindowGroup::uiComponentVisible() const
        {
            return _p->uiComponentVisible;
        }

        QPointer<QToolBar> WindowGroup::toolBar() const
        {
            return _p->toolBar.data();
        }

        void WindowGroup::setFullScreen(bool fullScreen)
        {
            if (fullScreen == _p->fullScreen)
                return;
            _p->fullScreen = fullScreen;
            if (_p->fullScreen)
            {
                mainWindow()->showFullScreen();
                if (!context()->windowPrefs()->hasFullScreenUI())
                {
                    setUIVisible(false);
                }
            }
            else
            {
                mainWindow()->showNormal();

                if (!context()->windowPrefs()->hasFullScreenUI())
                {
                    setUIVisible(true);
                }
            }
            update();
            Q_EMIT fullScreenChanged(_p->fullScreen);
        }

        void WindowGroup::setUIVisible(bool visible)
        {
            if (visible == _p->uiVisible)
                return;
            _p->uiVisible = visible;
            update();
            Q_EMIT uiVisibleChanged(_p->uiVisible);
        }

        void WindowGroup::setUIComponentVisible(const QVector<bool> & visible)
        {
            if (visible == _p->uiComponentVisible)
                return;
            _p->uiComponentVisible = visible;
            update();
            Q_EMIT uiComponentVisibleChanged(_p->uiComponentVisible);
        }

        void WindowGroup::newCallback()
        {
            (new MainWindow(0, context()))->show();
        }

        void WindowGroup::duplicateCallback()
        {
            (new MainWindow(mainWindow(), context()))->show();
        }

        void WindowGroup::closeCallback()
        {
            mainWindow()->close();
        }

        void WindowGroup::fitCallback()
        {
            mainWindow()->fitWindow();
        }

        void WindowGroup::uiComponentVisibleCallback(QAction * action)
        {
            QVector<bool> tmp = _p->uiComponentVisible;
            const int index = _p->actions->group(WindowActions::UI_COMPONENT_VISIBLE_GROUP)->actions().indexOf(action);
            tmp[index] = !tmp[index];
            setUIComponentVisible(tmp);
        }

        void WindowGroup::update()
        {
            _p->actions->action(WindowActions::FULL_SCREEN)->setChecked(_p->fullScreen);
            _p->actions->action(WindowActions::UI_VISIBLE)->setChecked(_p->uiVisible);
            for (int i = 0; i < Enum::UI_COMPONENT_COUNT; ++i)
            {
                _p->actions->group(WindowActions::UI_COMPONENT_VISIBLE_GROUP)->
                    actions()[i]->setChecked(_p->uiComponentVisible[i]);
            }
        }

    } // namespace ViewLib
} // namespace djv
