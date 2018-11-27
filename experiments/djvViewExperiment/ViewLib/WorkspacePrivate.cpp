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

#include <ViewLib/WorkspacePrivate.h>

#include <ViewLib/Context.h>
#include <ViewLib/ImageView.h>
#include <ViewLib/Project.h>
#include <ViewLib/WorkspaceSystem.h>
#include <ViewLib/Workspace.h>

#include <QEvent>
#include <QMdiArea>
#include <QStackedLayout>
#include <QTabBar>
#include <QVBoxLayout>

#include <iostream>

namespace djv
{
    namespace ViewLib
    {
        struct WorkspaceMDISubWindow::Private
        {
            QPointer<Context> context;
            QPointer<Workspace> workspace;
            QPointer<Project> project;
        };

        WorkspaceMDISubWindow::WorkspaceMDISubWindow(const QPointer<Workspace> &workspace, const QPointer<Project> & project, const QPointer<Context> & context) :
            _p(new Private)
        {
            _p->context = context;
            _p->workspace = workspace;
            _p->project = project;
        }

        QSize WorkspaceMDISubWindow::sizeHint() const
        {
            return QSize(300, 200);
        }

        struct WorkspaceMDI::Private
        {
            QPointer<Context> context;
            QPointer<Workspace> workspace;
            QPointer<QMdiArea> mdiArea;
            std::map<QPointer<Project>, QPointer<QMdiSubWindow> > projectToWindow;
            std::map<QPointer<QMdiSubWindow >, QPointer<Project> > windowToProject;
        };

        WorkspaceMDI::WorkspaceMDI(const QPointer<Workspace> & workspace, const QPointer<Context> & context, QWidget * parent) :
            QWidget(parent),
            _p(new Private)
        {
            _p->context = context;
            _p->workspace = workspace;

            _p->mdiArea = new QMdiArea;
            _p->mdiArea->setViewMode(workspace->getViewMode());

            auto layout = new QVBoxLayout(this);
            layout->addWidget(_p->mdiArea);

            for (auto project : workspace->getProjects())
            {
                _addProject(project);
            }

            connect(
                workspace,
                &Workspace::projectAdded,
                [this](const QPointer<Project> & project)
            {
                _addProject(project);
            });
            connect(
                workspace,
                &Workspace::projectRemoved,
                [this](const QPointer<Project> & project)
            {
                _removeProject(project);
            });
            connect(
                workspace,
                &Workspace::currentProjectChanged,
                [this](const QPointer<Project> & project)
            {
                auto i = _p->projectToWindow.find(project);
                if (i != _p->projectToWindow.end())
                {
                    _p->mdiArea->setActiveSubWindow(i->second);
                }
            });
            connect(
                workspace,
                &Workspace::viewModeChanged,
                [this](QMdiArea::ViewMode value)
            {
                _p->mdiArea->setViewMode(value);
            });
            connect(
                workspace,
                &Workspace::windowStateChanged,
                [this](Enum::WindowState value)
            {
                if (auto window = _p->mdiArea->activeSubWindow())
                {
                    switch (value)
                    {
                    case Enum::WindowState::Normal: window->showNormal(); break;
                    case Enum::WindowState::Maximized: window->showMaximized(); break;
                    case Enum::WindowState::Minimized: window->showMinimized(); break;
                    default: break;
                    }
                }
            });

            connect(
                _p->mdiArea,
                &QMdiArea::subWindowActivated,
                [this, workspace](QMdiSubWindow * window)
            {
                auto i = _p->windowToProject.find(window);
                if (i != _p->windowToProject.end())
                {
                    workspace->setCurrentProject(i->second);
                }
            });
        }

        WorkspaceMDI::~WorkspaceMDI()
        {}

        bool WorkspaceMDI::eventFilter(QObject * object, QEvent * event)
        {
            switch (event->type())
            {
            case QEvent::Close:
            {
                auto i = _p->windowToProject.find(qobject_cast<WorkspaceMDISubWindow *>(object));
                if (i != _p->windowToProject.end())
                {
                    _p->workspace->closeProject(i->second);
                }
                break;
            }
            case QEvent::WindowStateChange:
            {
                auto i = _p->windowToProject.find(qobject_cast<WorkspaceMDISubWindow *>(object));
                if (i != _p->windowToProject.end())
                {
                    Enum::WindowState windowState = Enum::WindowState::Normal;
                    if (i->first->windowState() & Qt::WindowMaximized)
                    {
                        windowState = Enum::WindowState::Maximized;
                    }
                    else if (i->first->windowState() & Qt::WindowMinimized)
                    {
                        windowState = Enum::WindowState::Minimized;
                    }
                    _p->workspace->setWindowState(i->second, windowState);
                }
                break;
            }
            default:
                break;
            }
            return QWidget::eventFilter(object, event);
        }

        void WorkspaceMDI::_addProject(const QPointer<Project> & project)
        {
            auto imageView = new ImageView(_p->context);
            auto window = new WorkspaceMDISubWindow(_p->workspace, project, _p->context);
            _p->projectToWindow[project] = window;
            _p->windowToProject[window] = project;
            window->setWindowTitle(project->getFileInfo().fileName());
            window->setAttribute(Qt::WA_DeleteOnClose);
            window->setWidget(imageView);
            window->installEventFilter(this);
            _p->mdiArea->addSubWindow(window);
            window->show();
        }

        void WorkspaceMDI::_removeProject(const QPointer<Project> & project)
        {
            auto i = _p->projectToWindow.find(project);
            if (i != _p->projectToWindow.end())
            {
                auto window = i->second;
                _p->projectToWindow.erase(i);
                auto j = _p->windowToProject.find(window);
                if (j != _p->windowToProject.end())
                {
                    _p->windowToProject.erase(j);
                }
                window->close();
            }
        }

        struct WorkspaceTabs::Private
        {
            QPointer<Context> context;
            QPointer<QTabBar> tabBar;
            QPointer<QStackedLayout> stackedLayout;
            std::map<QPointer<Workspace>, QPointer<WorkspaceMDI> > workspaceToMDI;
            std::map<QPointer<WorkspaceMDI >, QPointer<Workspace> > mdiToWorkspace;
            std::map<int, QPointer<WorkspaceMDI> > tabToMDI;
            std::map<QPointer<WorkspaceMDI>, int> mdiToTab;
        };

        WorkspaceTabs::WorkspaceTabs(const QPointer<Context> & context, QWidget * parent) :
            QWidget(parent),
            _p(new Private)
        {
            _p->context = context;

            _p->tabBar = new QTabBar;

            _p->stackedLayout = new QStackedLayout;
            _p->stackedLayout->setMargin(0);

            auto workspaceSystem = context->getSystemT<WorkspaceSystem>();
            for (auto i : workspaceSystem->getWorkspaces())
            {
                _addWorkspace(i);
            }

            auto layout = new QVBoxLayout(this);
            layout->setMargin(0);
            layout->setSpacing(0);
            layout->addWidget(_p->tabBar);
            layout->addLayout(_p->stackedLayout, 1);

            connect(
                workspaceSystem,
                &WorkspaceSystem::workspaceAdded,
                [this](const QPointer<Workspace> & workspace)
            {
                _addWorkspace(workspace);
            });
            connect(
                workspaceSystem,
                &WorkspaceSystem::workspaceRemoved,
                [this](const QPointer<Workspace> & workspace)
            {
                _removeWorkspace(workspace);
            });
            connect(
                workspaceSystem,
                &WorkspaceSystem::currentWorkspaceChanged,
                [this](const QPointer<Workspace> & workspace)
            {
                const auto i = _p->workspaceToMDI.find(workspace);
                if (i != _p->workspaceToMDI.end())
                {
                    auto j = _p->mdiToTab.find(i->second);
                    if (j != _p->mdiToTab.end())
                    {
                        _p->tabBar->setCurrentIndex(j->second);
                    }
                }
            });

            connect(
                _p->tabBar,
                &QTabBar::currentChanged,
                [this, workspaceSystem](int index)
            {
                const auto i = _p->tabToMDI.find(index);
                if (i != _p->tabToMDI.end())
                {
                    _p->stackedLayout->setCurrentWidget(i->second);
                    const auto j = _p->mdiToWorkspace.find(i->second);
                    if (j != _p->mdiToWorkspace.end())
                    {
                        workspaceSystem->setCurrentWorkspace(j->second);
                    }
                }
            });
        }

        WorkspaceTabs::~WorkspaceTabs()
        {}

        void WorkspaceTabs::_addWorkspace(const QPointer<Workspace> & workspace)
        {
            auto mdi = new WorkspaceMDI(workspace, _p->context);
            _p->workspaceToMDI[workspace] = mdi;
            _p->mdiToWorkspace[mdi] = workspace;
            const int index = _p->tabBar->addTab(workspace->getName());
            _p->tabToMDI[index] = mdi;
            _p->mdiToTab[mdi] = index;
            _p->stackedLayout->addWidget(mdi);
            //! \todo Why do we need to set the margin here?
            mdi->layout()->setMargin(0);
        }

        void WorkspaceTabs::_removeWorkspace(const QPointer<Workspace> & workspace)
        {
            auto i = _p->workspaceToMDI.find(workspace);
            if (i != _p->workspaceToMDI.end())
            {
                auto mdi = i->second;
                _p->workspaceToMDI.erase(i);
                auto j = _p->mdiToWorkspace.find(mdi);
                if (j != _p->mdiToWorkspace.end())
                {
                    _p->mdiToWorkspace.erase(j);
                }
                auto k = _p->mdiToTab.find(mdi);
                if (k != _p->mdiToTab.end())
                {
                    const auto l = _p->tabToMDI.find(k->second);
                    if (l != _p->tabToMDI.end())
                    {
                        _p->tabToMDI.erase(l);
                    }
                    _p->tabBar->removeTab(k->second);
                    _p->mdiToTab.erase(k);
                }
                delete mdi;
            }
        }

    } // namespace ViewLib
} // namespace djv

