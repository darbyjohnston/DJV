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

#include <Project.h>

#include <Context.h>
#include <FileBrowser.h>

#include <QAction>
#include <QDockWidget>
#include <QMenu>

#include <iostream>

namespace djv
{
    namespace ViewExperiment
    {
        namespace
        {
            size_t projectCount = 0;

        } // namespace

        struct Project::Private
        {
            QFileInfo fileInfo;
            bool changes = false;
        };
        
        Project::Project(const QPointer<Context> &, QObject * parent) :
            QObject(parent),
            _p(new Private)
        {
            ++projectCount;
            _p->fileInfo = QString("Untitled %1").arg(projectCount);
        }
        
        Project::~Project()
        {}

        const QFileInfo & Project::getFileInfo() const
        {
            return _p->fileInfo;
        }

        bool Project::hasChanges() const
        {
            return _p->changes;
        }
            
        void Project::open(const QFileInfo & fileInfo)
        {
            _p->fileInfo = fileInfo;
            Q_EMIT fileInfoChanged(_p->fileInfo);
        }
        
        void Project::close()
        {
            _p->fileInfo = QFileInfo();
            Q_EMIT fileInfoChanged(_p->fileInfo);
        }
        
        void Project::save()
        {}
        
        void Project::saveAs(const QFileInfo & fileInfo)
        {
            _p->fileInfo = fileInfo;
            Q_EMIT fileInfoChanged(_p->fileInfo);
        }

        struct ProjectSystem::Private
        {
            std::vector<QPointer<Project> > projects;
            QPointer<Project> currentProject;
            std::map<QString, QAction *> actions;
        };
        
        ProjectSystem::ProjectSystem(const QPointer<Context> & context, QObject * parent) :
            IUISystem("ProjectSystem", context, parent),
            _p(new Private)
        {
            _p->actions["New"] = new QAction("New", this);
            _p->actions["New"]->setShortcut(QKeySequence("Ctrl+N"));
            _p->actions["Open"] = new QAction("Open", this);
            _p->actions["Open"]->setShortcut(QKeySequence("Ctrl+O"));
            _p->actions["Close"] = new QAction("Close", this);
            _p->actions["Close"]->setShortcut(QKeySequence("Ctrl+E"));
            _p->actions["Save"] = new QAction("Save", this);
            _p->actions["Save"]->setShortcut(QKeySequence("Ctrl+S"));
            _p->actions["Save As"] = new QAction("Save As", this);
            _p->actions["Save As"]->setShortcut(QKeySequence("Ctrl+Shift+S"));

            newProject();

            connect(
                _p->actions["New"],
                &QAction::triggered,
                [this]
            {
                newProject();
            });
            connect(
                _p->actions["Open"],
                &QAction::triggered,
                [this]
            {
            });
            connect(
                _p->actions["Close"],
                &QAction::triggered,
                [this]
            {
                if (_p->projects.size() > 1)
                {
                    closeProject(_p->currentProject);
                }
            });
            connect(
                _p->actions["Save"],
                &QAction::triggered,
                [this]
            {
            });
            connect(
                _p->actions["Save As"],
                &QAction::triggered,
                [this]
            {
            });
        }
        
        ProjectSystem::~ProjectSystem()
        {}
       
        const std::vector<QPointer<Project> > & ProjectSystem::getProjects() const
        {
            return _p->projects;
        }
        
        QPointer<Project> ProjectSystem::getProject(int index) const
        {
            return index >= 0 && index < static_cast<int>(_p->projects.size()) ? _p->projects[index].data() : nullptr;
        }

        const QPointer<Project> & ProjectSystem::getCurrentProject() const
        {
            return _p->currentProject;
        }

        QString ProjectSystem::getMenuSortKey() const
        {
            return "0";
        }
        
        QPointer<QMenu> ProjectSystem::createMenu()
        {
            auto menu = new QMenu("File");
            menu->addAction(_p->actions["New"]);
            menu->addAction(_p->actions["Open"]);
            menu->addAction(_p->actions["Close"]);
            menu->addAction(_p->actions["Save"]);
            menu->addAction(_p->actions["Save As"]);
            return menu;
        }

        QPointer<QDockWidget> ProjectSystem::createDockWidget()
        {
            auto out = new QDockWidget("File Browser");
            out->setWidget(new FileBrowserWidget(getContext()));
            //out->hide();
            return out;
        }

        QString ProjectSystem::getDockWidgetSortKey() const
        {
            return "0";
        }

        Qt::DockWidgetArea ProjectSystem::getDockWidgetArea() const
        {
            return Qt::DockWidgetArea::LeftDockWidgetArea;
        }

        void ProjectSystem::newProject()
        {
            auto project = new Project(getContext());
            _p->projects.push_back(project);
            _p->currentProject = project;
            Q_EMIT projectAdded(_p->currentProject);
            Q_EMIT currentProjectChanged(_p->currentProject);
        }
        
        void ProjectSystem::openProject(const QFileInfo & fileInfo)
        {
            auto project = new Project(getContext());
            project->open(fileInfo);
            _p->projects.push_back(project);
            _p->currentProject = project;
            Q_EMIT projectAdded(_p->currentProject);
            Q_EMIT currentProjectChanged(_p->currentProject);
        }
        
        void ProjectSystem::closeProject(const QPointer<Project> & project)
        {
            auto i = std::find(_p->projects.begin(), _p->projects.end(), project);
            if (i != _p->projects.end())
            {
                //! \todo Save
                auto project = *i;
                int index = static_cast<int>(i - _p->projects.begin());
                _p->projects.erase(i);
                Q_EMIT projectRemoved(project);
                delete project.data();
                if (project == _p->currentProject)
                {
                    index = std::min(index, static_cast<int>(_p->projects.size()) - 1);
                    _p->currentProject = index != -1 ? _p->projects[index] : nullptr;
                    Q_EMIT currentProjectChanged(_p->currentProject);
                }
                if (!_p->projects.size())
                {
                    newProject();
                }
            }
        }

        void ProjectSystem::closeProject(int index)
        {
            closeProject(getProject(index));
        }

        void ProjectSystem::setCurrentProject(const QPointer<Project> & project)
        {
            if (project == _p->currentProject)
                return;
            _p->currentProject = project;
            Q_EMIT currentProjectChanged(_p->currentProject);
        }

        void ProjectSystem::setCurrentProject(int index)
        {
            setCurrentProject(getProject(index));
        }

    } // namespace ViewExperiment
} // namespace djv

