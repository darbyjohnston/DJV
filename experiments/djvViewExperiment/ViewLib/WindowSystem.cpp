//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <ViewLib/WindowSystem.h>

#include <ViewLib/Context.h>
#include <ViewLib/Project.h>

#include <QAction>
#include <QDockWidget>
#include <QMenu>

#include <iostream>

namespace djv
{
    namespace ViewLib
    {
        struct WindowSystem::Private
        {
            std::map<QString, QPointer<QAction> > actions;
            std::map<QString, QPointer<QActionGroup> > actionGroups;
            std::vector<QPointer<QMenu> > menus;
            QPointer<Workspace> currentWorkspace;
        };
        
        WindowSystem::WindowSystem(const QPointer<Context> & context, QObject * parent) :
            IViewSystem("WindowSystem", context, parent),
            _p(new Private)
        {
            _p->actions["Next"] = new QAction("Next", this);
            _p->actions["Next"]->setShortcut(QKeySequence("Page Up"));
            _p->actions["Previous"] = new QAction("Previous", this);
            _p->actions["Previous"]->setShortcut(QKeySequence("Page Down"));
            _p->actions["Normal"] = new QAction("Normal", this);
            _p->actions["Normal"]->setCheckable(true);
            _p->actions["Normal"]->setShortcut(QKeySequence("Ctrl+Meta+M"));
            _p->actions["Maximize"] = new QAction("Maximize", this);
            _p->actions["Maximize"]->setCheckable(true);
            _p->actions["Maximize"]->setShortcut(QKeySequence("Ctrl+M"));
            _p->actions["Minimize"] = new QAction("Minimize", this);
            _p->actions["Minimize"]->setCheckable(true);
            _p->actions["Minimize"]->setShortcut(QKeySequence("Ctrl+Shift+M"));
            _p->actionGroups["WindowState"] = new QActionGroup(this);
            _p->actionGroups["WindowState"]->setExclusive(true);
            _p->actionGroups["WindowState"]->addAction(_p->actions["Normal"]);
            _p->actionGroups["WindowState"]->addAction(_p->actions["Maximize"]);
            _p->actionGroups["WindowState"]->addAction(_p->actions["Minimize"]);

            _updateActions();

            connect(
                _p->actions["Next"],
                &QAction::triggered,
                [this]
            {
                nextProject();
            });
            connect(
                _p->actions["Previous"],
                &QAction::triggered,
                [this]
            {
                prevProject();
            });
            connect(
                _p->actionGroups["WindowState"],
                &QActionGroup::triggered,
                [this](QAction * action)
            {
                if (_p->currentWorkspace)
                {
                    if (_p->actions["Normal"] == action)
                    {
                        _p->currentWorkspace->setWindowState(Enum::WindowState::Normal);
                    }
                    else if (_p->actions["Maximize"] == action)
                    {
                        _p->currentWorkspace->setWindowState(
                            Enum::WindowState::Maximized == _p->currentWorkspace->getWindowState() ?
                            Enum::WindowState::Normal :
                            Enum::WindowState::Maximized);
                    }
                    else if (_p->actions["Minimize"] == action)
                    {
                        _p->currentWorkspace->setWindowState(
                            Enum::WindowState::Minimized == _p->currentWorkspace->getWindowState() ?
                            Enum::WindowState::Normal :
                            Enum::WindowState::Minimized);
                    }
                }
            });
        }
        
        WindowSystem::~WindowSystem()
        {}

        QPointer<QMenu> WindowSystem::createMenu()
        {
            auto menu = new QMenu("Window");
            menu->addAction(_p->actions["Next"]);
            menu->addAction(_p->actions["Previous"]);
            menu->addSeparator();
            menu->addAction(_p->actions["Normal"]);
            menu->addAction(_p->actions["Minimize"]);
            menu->addAction(_p->actions["Maximize"]);
            return menu;
        }

        QString WindowSystem::getMenuSortKey() const
        {
            return "2";
        }

        void WindowSystem::nextProject()
        {
            if (_p->currentWorkspace)
            {
                _p->currentWorkspace->nextProject();
            }
        }

        void WindowSystem::prevProject()
        {
            if (_p->currentWorkspace)
            {
                _p->currentWorkspace->prevProject();
            }
        }

        void WindowSystem::setCurrentWorkspace(const QPointer<Workspace> & workspace)
        {
            if (workspace == _p->currentWorkspace)
                return;
            if (_p->currentWorkspace)
            {
                disconnect(
                    _p->currentWorkspace,
                    SIGNAL(projectAdded(const QPointer<Project> &)),
                    this,
                    SLOT(_updateActions()));
                disconnect(
                    _p->currentWorkspace,
                    SIGNAL(projectRemoved(const QPointer<Project> &)),
                    this,
                    SLOT(_updateActions()));
                disconnect(
                    _p->currentWorkspace,
                    SIGNAL(viewModeChanged(QMdiArea::ViewMode)),
                    this,
                    SLOT(_updateActions()));
                disconnect(
                    _p->currentWorkspace,
                    SIGNAL(windowStateChanged(Enum::WindowState)),
                    this,
                    SLOT(_updateActions()));
            }
            _p->currentWorkspace = workspace;
            _updateActions();
            if (_p->currentWorkspace)
            {
                connect(
                    _p->currentWorkspace,
                    SIGNAL(projectAdded(const QPointer<Project> &)),
                    SLOT(_updateActions()));
                connect(
                    _p->currentWorkspace,
                    SIGNAL(projectRemoved(const QPointer<Project> &)),
                    SLOT(_updateActions()));
                connect(
                    _p->currentWorkspace,
                    SIGNAL(viewModeChanged(QMdiArea::ViewMode)),
                    SLOT(_updateActions()));
                connect(
                    _p->currentWorkspace,
                    SIGNAL(windowStateChanged(Enum::WindowState)),
                    SLOT(_updateActions()));
            }
        }

        void WindowSystem::_updateActions()
        {
            const bool multipleProjects = _p->currentWorkspace && _p->currentWorkspace->getProjects().size() > 1;
            _p->actions["Next"]->setEnabled(multipleProjects);
            _p->actions["Previous"]->setEnabled(multipleProjects);

            const bool hasProjects =
                _p->currentWorkspace &&
                _p->currentWorkspace->getProjects().size() > 0 &&
                QMdiArea::SubWindowView == _p->currentWorkspace->getViewMode();
            _p->actionGroups["WindowState"]->setEnabled(hasProjects);
            _p->actions["Normal"]->setChecked(_p->currentWorkspace && _p->currentWorkspace->getWindowState() == Enum::WindowState::Normal);
            _p->actions["Maximize"]->setChecked(_p->currentWorkspace && _p->currentWorkspace->getWindowState() == Enum::WindowState::Maximized);
            _p->actions["Minimize"]->setChecked(_p->currentWorkspace && _p->currentWorkspace->getWindowState() == Enum::WindowState::Minimized);
        }

    } // namespace ViewLib
} // namespace djv

