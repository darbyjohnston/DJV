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

#include <Workspace.h>

#include <Context.h>
#include <ImageView.h>
#include <Project.h>

#include <QAction>
#include <QDockWidget>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMenu>
#include <QVBoxLayout>

#include <iostream>

namespace djv
{
    namespace ViewExperiment
    {
        namespace
        {
            size_t workspaceCount = 0;
        
        } // namespace

        struct Workspace::Private
        {
            QPointer<Context> context;
            QString name;
            std::vector<QPointer<Project> > projects;
            QPointer<Project> currentProject;
            QMdiArea::ViewMode viewMode = QMdiArea::TabbedView;
        };

        Workspace::Workspace(const QPointer<Context> & context, QObject * parent) :
            QObject(parent),
            _p(new Private)
        {
            _p->context = context;
            ++workspaceCount;
            _p->name = QString("Workspace %1").arg(workspaceCount);
        }

        Workspace::~Workspace()
        {
            for (auto i : _p->projects)
            {
                //! \todo Save
                delete i.data();
            }
        }

        const QString & Workspace::getName() const
        {
            return _p->name;
        }

        const std::vector<QPointer<Project> > & Workspace::getProjects() const
        {
            return _p->projects;
        }

        const QPointer<Project> & Workspace::getCurrentProject() const
        {
            return _p->currentProject;
        }

        QMdiArea::ViewMode Workspace::getViewMode() const
        {
            return _p->viewMode;
        }

        void Workspace::setName(const QString & value)
        {
            if (value == _p->name)
                return;
            _p->name = value;
            Q_EMIT nameChanged(_p->name);
        }

        void Workspace::newProject()
        {
            auto project = new Project(_p->context);
            _p->projects.push_back(project);
            _p->currentProject = project;
            Q_EMIT projectAdded(project);
            Q_EMIT currentProjectChanged(_p->currentProject);
        }

        void Workspace::openProject(const QFileInfo & fileInfo)
        {
            auto project = new Project(_p->context);
            project->open(fileInfo);
            _p->projects.push_back(project);
            _p->currentProject = project;
            Q_EMIT projectAdded(project);
            Q_EMIT currentProjectChanged(_p->currentProject);
        }

        void Workspace::closeProject(const QPointer<Project> & project)
        {
            auto i = std::find(_p->projects.begin(), _p->projects.end(), project);
            if (i != _p->projects.end())
            {
                //! \todo Save
                int index = static_cast<int>(i - _p->projects.begin());
                auto project = *i;
                _p->projects.erase(i);
                Q_EMIT projectRemoved(project);
                if (project == _p->currentProject)
                {
                    index = std::min(index, static_cast<int>(_p->projects.size()) - 1);
                    _p->currentProject = index != -1 ? _p->projects[index] : nullptr;
                    Q_EMIT currentProjectChanged(_p->currentProject);
                }
                delete project.data();
            }
        }

        void Workspace::setCurrentProject(const QPointer<Project> & project)
        {
            if (project == _p->currentProject)
                return;
            _p->currentProject = project;
            Q_EMIT currentProjectChanged(_p->currentProject);
        }

        void Workspace::nextProject()
        {}

        void Workspace::prevProject()
        {}

        void Workspace::setViewMode(QMdiArea::ViewMode value)
        {
            if (value == _p->viewMode)
                return;
            _p->viewMode = value;
            Q_EMIT viewModeChanged(_p->viewMode);
        }

        namespace
        {
            class MDISubWindow : public QMdiSubWindow
            {
            public:
                MDISubWindow(const QPointer<Workspace> &workspace, const QPointer<Project> & project, const QPointer<Context> & context) :
                    _workspace(workspace),
                    _project(project),
                    _context(context)
                {}

                virtual QSize sizeHint() const override { return QSize(300, 200); }

            protected:
                void closeEvent(QCloseEvent *) override
                {
                    _workspace->closeProject(_project);
                }

            private:
                QPointer<Workspace> _workspace;
                QPointer<Project> _project;
                QPointer<Context> _context;
            };

        } // namespace

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

        void WorkspaceMDI::_addProject(const QPointer<Project> & project)
        {
            auto imageView = new ImageView(_p->context);
            auto window = new MDISubWindow(_p->workspace, project, _p->context);
            window->setWindowTitle(project->getFileInfo().fileName());
            window->setAttribute(Qt::WA_DeleteOnClose);
            window->setWidget(imageView);
            _p->mdiArea->addSubWindow(window);
            window->show();
            _p->projectToWindow[project] = window;
            _p->windowToProject[window] = project;
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
            QPointer<QTabWidget> tabWidget;
            std::map<QPointer<Workspace>, QPointer<WorkspaceMDI> > workspaceToMDI;
            std::map<QPointer<WorkspaceMDI >, QPointer<Workspace> > mdiToWorkspace;
        };

        WorkspaceTabs::WorkspaceTabs(const QPointer<Context> & context, QWidget * parent) :
            QWidget(parent),
            _p(new Private)
        {
            _p->context = context;

            _p->tabWidget = new QTabWidget;

            auto workspaceSystem = context->getSystem<WorkspaceSystem>();
            for (auto i : workspaceSystem->getWorkspaces())
            {
                _addWorkspace(i);
            }

            auto layout = new QVBoxLayout(this);
            layout->setMargin(0);
            layout->addWidget(_p->tabWidget);

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
                auto i = _p->workspaceToMDI.find(workspace);
                if (i != _p->workspaceToMDI.end())
                {
                    _p->tabWidget->setCurrentWidget(i->second);
                }
            });

            connect(
                _p->tabWidget,
                &QTabWidget::currentChanged,
                [workspaceSystem](int index)
            {
                const auto & workspaces = workspaceSystem->getWorkspaces();
                if (index >= 0 && index < workspaces.size())
                {
                    workspaceSystem->setCurrentWorkspace(workspaces[index]);
                }
            });
        }

        WorkspaceTabs::~WorkspaceTabs()
        {}

        void WorkspaceTabs::_addWorkspace(const QPointer<Workspace> & workspace)
        {
            auto mdi = new WorkspaceMDI(workspace, _p->context);
            _p->tabWidget->addTab(mdi, workspace->getName());
            mdi->layout()->setContentsMargins(0, 0, 0, 0);
            _p->workspaceToMDI[workspace] = mdi;
            _p->mdiToWorkspace[mdi] = workspace;
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
                _p->tabWidget->removeTab(_p->tabWidget->indexOf(mdi));
                delete mdi;
            }
        }

        struct WorkspaceSystem::Private
        {
            std::vector<QPointer<Workspace> > workspaces;
            QPointer<Workspace> currentWorkspace;
            std::map<QString, QPointer<QAction> > actions;
            std::map<QString, QPointer<QActionGroup> > actionGroups;

            std::vector<QPointer<QMenu> > menus;
        };
        
        WorkspaceSystem::WorkspaceSystem(const QPointer<Context> & context, QObject * parent) :
            IUISystem("WorkspaceSystem", context, parent),
            _p(new Private)
        {
            _p->actions["New"] = new QAction("New", this);
            _p->actions["Close"] = new QAction("Close", this);
            _p->actions["Tabs"] = new QAction("Tabs", this);
            _p->actions["Tabs"]->setCheckable(true);
            _p->actions["Windows"] = new QAction("Windows", this);
            _p->actions["Windows"]->setCheckable(true);
            _p->actionGroups["ViewMode"] = new QActionGroup(this);
            _p->actionGroups["ViewMode"]->setExclusive(true);
            _p->actionGroups["ViewMode"]->addAction(_p->actions["Tabs"]);
            _p->actionGroups["ViewMode"]->addAction(_p->actions["Windows"]);

            connect(
                _p->actions["New"],
                &QAction::triggered,
                [context]
            {
                auto workspaceSystem = context->getSystem<WorkspaceSystem>();
                workspaceSystem->newWorkspace();
            });
            connect(
                _p->actions["Close"],
                &QAction::triggered,
                [context]
            {
                auto workspaceSystem = context->getSystem<WorkspaceSystem>();
                workspaceSystem->closeWorkspace(workspaceSystem->getCurrentWorkspace());
            });
            connect(
                _p->actionGroups["ViewMode"],
                &QActionGroup::triggered,
                [this, context](QAction * action)
            {
                auto workspaceSystem = context->getSystem<WorkspaceSystem>();
                if (auto workspace = workspaceSystem->getCurrentWorkspace())
                {
                    if (_p->actions["Tabs"] == action)
                    {
                        workspace->setViewMode(QMdiArea::TabbedView);
                    }
                    else if (_p->actions["Windows"])
                    {
                        workspace->setViewMode(QMdiArea::SubWindowView);
                    }
                }
            });

            newWorkspace();
        }
        
        WorkspaceSystem::~WorkspaceSystem()
        {}

        const std::vector<QPointer<Workspace> > & WorkspaceSystem::getWorkspaces() const
        {
            return _p->workspaces;
        }

        const QPointer<Workspace> & WorkspaceSystem::getCurrentWorkspace() const
        {
            return _p->currentWorkspace;
        }

        QPointer<WorkspaceTabs> WorkspaceSystem::createWorkspaceTabs()
        {
            auto out = new WorkspaceTabs(getContext());

            return out;
        }

        QPointer<QMenu> WorkspaceSystem::createMenu()
        {
            auto menu = new QMenu("Workspace");
            _p->menus.push_back(menu);
            _updateMenus();
            return menu;
        }

        QString WorkspaceSystem::getMenuSortKey() const
        {
            return "1";
        }

        void WorkspaceSystem::newWorkspace()
        {
            auto workspace = new Workspace(getContext());
            _p->workspaces.push_back(workspace);
            _p->currentWorkspace = workspace;
            _updateMenus();
            Q_EMIT workspaceAdded(workspace);
            Q_EMIT currentWorkspaceChanged(_p->currentWorkspace);
        }

        void WorkspaceSystem::closeWorkspace(const QPointer<Workspace> & workspace)
        {
            auto i = std::find(_p->workspaces.begin(), _p->workspaces.end(), workspace);
            if (i != _p->workspaces.end())
            {
                //! \todo Save
                int index = static_cast<int>(i - _p->workspaces.begin());
                auto workspace = *i;
                _p->workspaces.erase(i);
                _updateMenus();
                Q_EMIT workspaceRemoved(workspace);
                if (workspace == _p->currentWorkspace)
                {
                    index = std::min(index, static_cast<int>(_p->workspaces.size()) - 1);
                    _p->currentWorkspace = index != -1 ? _p->workspaces[index] : nullptr;
                    _updateMenus();
                    Q_EMIT currentWorkspaceChanged(_p->currentWorkspace);
                }
                delete workspace.data();
            }
        }

        void WorkspaceSystem::setCurrentWorkspace(const QPointer<Workspace> & workspace)
        {
            if (workspace == _p->currentWorkspace)
                return;
            _p->currentWorkspace = workspace;
            _updateMenus();
            Q_EMIT currentWorkspaceChanged(_p->currentWorkspace);
        }

        void WorkspaceSystem::nextWorkspace()
        {}

        void WorkspaceSystem::prevWorkspace()
        {}

        void WorkspaceSystem::_updateMenus()
        {
            bool enabled = _p->currentWorkspace;
            QMdiArea::ViewMode viewMode = QMdiArea::TabbedView;
            if (_p->currentWorkspace)
            {
                viewMode = _p->currentWorkspace->getViewMode();
            }
            _p->actionGroups["ViewMode"]->setEnabled(enabled);
            switch (viewMode)
            {
            case QMdiArea::TabbedView: _p->actions["Tabs"]->setChecked(true); break;
            case QMdiArea::SubWindowView: _p->actions["Windows"]->setChecked(true); break;
            default: break;
            }

            for (auto menu : _p->menus)
            {
                menu->clear();
                menu->addAction(_p->actions["New"]);
                menu->addAction(_p->actions["Close"]);
                menu->addSeparator();
                menu->addAction(_p->actions["Tabs"]);
                menu->addAction(_p->actions["Windows"]);
            }
        }

    } // namespace ViewExperiment
} // namespace djv

