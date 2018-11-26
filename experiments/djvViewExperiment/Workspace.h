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

#pragma once

#include <IUISystem.h>

#include <QFileInfo>
#include <QMdiArea>
#include <QWidget>

namespace djv
{
    namespace ViewExperiment
    {
        class Project;

        class Workspace : public QObject
        {
            Q_OBJECT

        public:
            Workspace(const QPointer<Context> &, QObject * parent = nullptr);
            ~Workspace() override;

            const QString & getName() const;
            const std::vector<QPointer<Project> > & getProjects() const;
            const QPointer<Project> & getCurrentProject() const;

            QMdiArea::ViewMode getViewMode() const;

        public Q_SLOTS:
            void setName(const QString &);
            void newProject();
            void openProject(const QFileInfo &);
            void closeProject(const QPointer<Project> &);
            void setCurrentProject(const QPointer<Project> &);
            void nextProject();
            void prevProject();
            void setViewMode(QMdiArea::ViewMode);

        Q_SIGNALS:
            void nameChanged(const QString &);
            void projectAdded(const QPointer<Project> &);
            void projectRemoved(const QPointer<Project> &);
            void currentProjectChanged(const QPointer<Project> &);
            void viewModeChanged(QMdiArea::ViewMode);

        private:
            DJV_PRIVATE();
        };

        class WorkspaceMDI : public QWidget
        {
            Q_OBJECT

        public:
            WorkspaceMDI(const QPointer<Workspace> &, const QPointer<Context> &, QWidget * parent = nullptr);
            ~WorkspaceMDI() override;

        private Q_SLOTS:
            void _addProject(const QPointer<Project> &);
            void _removeProject(const QPointer<Project> &);

        private:
            DJV_PRIVATE();
        };

        class WorkspaceTabs : public QWidget
        {
            Q_OBJECT

        public:
            WorkspaceTabs(const QPointer<Context> &, QWidget * parent = nullptr);
            ~WorkspaceTabs() override;

        private Q_SLOTS:
            void _addWorkspace(const QPointer<Workspace> &);
            void _removeWorkspace(const QPointer<Workspace> &);

        private:
            DJV_PRIVATE();
        };

        class WorkspaceSystem : public IUISystem
        {
            Q_OBJECT

        public:
            WorkspaceSystem(const QPointer<Context> &, QObject * parent = nullptr);
            ~WorkspaceSystem() override;

            const std::vector<QPointer<Workspace> > & getWorkspaces() const;
            const QPointer<Workspace> & getCurrentWorkspace() const;
            QPointer<WorkspaceTabs> createWorkspaceTabs();
            
            QPointer<QMenu> createMenu() override;
            QString getMenuSortKey() const override;

        public Q_SLOTS:
            void newWorkspace();
            void closeWorkspace(const QPointer<Workspace> &);
            void setCurrentWorkspace(const QPointer<Workspace> &);
            void nextWorkspace();
            void prevWorkspace();

        Q_SIGNALS:
            void workspaceAdded(const QPointer<Workspace> &);
            void workspaceRemoved(const QPointer<Workspace> &);
            void currentWorkspaceChanged(const QPointer<Workspace> &);

        private:
            void _updateMenus();

            DJV_PRIVATE();
        };

    } // namespace ViewExperiment
} // namespace djv

