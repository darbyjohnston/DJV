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

#include <QAction>

namespace djv
{
    namespace ViewExperiment
    {
        struct Project::Private
        {
            QFileInfo fileInfo;
            bool changes = false;
        };
        
        Project::Project(const QPointer<Context> &, QObject * parent) :
            QObject(parent),
            _p(new Private)
        {}
        
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
            QVector<QPointer<Project> > projects;
            QPointer<Project> currentProject;
            QMap<QString, QAction *> actions;
        };
        
        ProjectSystem::ProjectSystem(const QPointer<Context> & context, QObject * parent) :
            IUISystem(context, parent),
            _p(new Private)
        {
            _p->actions["New"] = new QAction("New", this);
            _p->actions["Open"] = new QAction("Open", this);
            _p->actions["Close"] = new QAction("Close", this);
            _p->actions["Save"] = new QAction("Save", this);
            _p->actions["Save As"] = new QAction("Save As", this);
            
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
       
        QVector<QPointer<Project> > ProjectSystem::getProjects() const
        {
            return _p->projects;
        }
        
        QPointer<Project> ProjectSystem::getCurrentProject() const
        {
            return _p->currentProject;
        }
            
        QString ProjectSystem::menuSortKey() const
        {
            return "0";
        }
        
        QPointer<QMenu> ProjectSystem::createMenu() const
        {
            auto menu = new QMenu("File");
            menu->addAction(_p->actions["New"]);
            menu->addAction(_p->actions["Open"]);
            menu->addAction(_p->actions["Close"]);
            menu->addAction(_p->actions["Save"]);
            menu->addAction(_p->actions["Save As"]);
            return menu;
        }

        void ProjectSystem::newProject()
        {
            QPointer<Project> project = new Project(getContext());
            _p->projects.append(project);
            _p->currentProject = project;
            Q_EMIT projectAdded(project);
            Q_EMIT currentProjectChanged(project);
        }
        
        void ProjectSystem::openProject(const QFileInfo & fileInfo)
        {
            QPointer<Project> project = new Project(getContext());
            project->open(fileInfo);
            _p->projects.append(project);
            _p->currentProject = project;
            Q_EMIT projectAdded(project);
            Q_EMIT currentProjectChanged(project);
        }
        
        void ProjectSystem::closeProject(const QPointer<Project> & project)
        {
            int i = _p->projects.indexOf(project);
            if (i != -1)
            {
                //! \todo Save
                //! \todo Current project
                _p->projects.remove(i);
                Q_EMIT projectRemoved(project);
            }
        }
        
        void ProjectSystem::setCurrentProject(const QPointer<Project> & project)
        {
            if (project ==  _p->currentProject)
                return;
            _p->currentProject = project;
            Q_EMIT currentProjectChanged(_p->currentProject);
        }

    } // namespace ViewExperiment
} // namespace djv

