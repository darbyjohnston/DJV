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

#include <djvViewLib/Workspace.h>

#include <djvViewLib/Context.h>
#include <djvViewLib/Project.h>

#include <iostream>

namespace djv
{
    namespace ViewLib
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
            std::map<QPointer<Project>, Enum::WindowState> windowState;
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

        Enum::WindowState Workspace::getWindowState() const
        {
            const auto i = _p->windowState.find(_p->currentProject);
            return i != _p->windowState.end() ? i->second : Enum::WindowState::Normal;
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

        void Workspace::openProject(const QString & fileName)
        {
            auto project = new Project(_p->context);
            project->open(fileName);
            _p->projects.push_back(project);
            _p->currentProject = project;
            Q_EMIT projectAdded(project);
            Q_EMIT currentProjectChanged(_p->currentProject);
        }

        void Workspace::closeProject(const QPointer<Project> & project)
        {
            const auto i = std::find(_p->projects.begin(), _p->projects.end(), project);
            if (i != _p->projects.end())
            {
                //! \todo Save
                int index = static_cast<int>(i - _p->projects.begin());
                auto project = *i;
                _p->projects.erase(i);
                Q_EMIT projectRemoved(project);
                const auto j = _p->windowState.find(project);
                if (j != _p->windowState.end())
                {
                    _p->windowState.erase(j);
                }
                if (project == _p->currentProject)
                {
                    index = std::min(index, static_cast<int>(_p->projects.size()) - 1);
                    setCurrentProject(index != -1 ? _p->projects[index] : nullptr);
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
            auto windowState = Enum::WindowState::Normal;
            const auto i = _p->windowState.find(project);
            if (i != _p->windowState.end())
            {
                windowState = i->second;
            }
            Q_EMIT windowStateChanged(windowState);
        }

        void Workspace::nextProject()
        {
            auto i = std::find(_p->projects.begin(), _p->projects.end(), _p->currentProject);
            if (i != _p->projects.end())
            {
                ++i;
                if (i == _p->projects.end())
                {
                    i = _p->projects.begin();
                }
                setCurrentProject(*i);
            }
        }

        void Workspace::prevProject()
        {
            auto i = std::find(_p->projects.begin(), _p->projects.end(), _p->currentProject);
            if (i != _p->projects.end())
            {
                if (i == _p->projects.begin())
                {
                    i = _p->projects.end();
                }
                --i;
                setCurrentProject(*i);
            }
        }

        void Workspace::setViewMode(QMdiArea::ViewMode value)
        {
            if (value == _p->viewMode)
                return;
            _p->viewMode = value;
            Q_EMIT viewModeChanged(_p->viewMode);
        }

        void Workspace::setWindowState(Enum::WindowState value)
        {
            setWindowState(_p->currentProject, value);
        }

        void Workspace::setWindowState(const QPointer<Project> & project, Enum::WindowState value)
        {
            const auto i = _p->windowState.find(project);
            if (i == _p->windowState.end())
            {
                _p->windowState[project] = Enum::WindowState::Normal;
            }
            if (value == _p->windowState[project])
                return;
            _p->windowState[project] = value;
            Q_EMIT windowStateChanged(value);
        }

    } // namespace ViewLib
} // namespace djv

