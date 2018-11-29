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

#pragma once

#include <ViewLib/Enum.h>

#include <QMdiArea>
#include <QPointer>
#include <QString>

namespace djv
{
    namespace ViewLib
    {
        class Context;
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
            Enum::WindowState getWindowState() const;

        public Q_SLOTS:
            void setName(const QString &);
            void newProject();
            void openProject(const QString &);
            void closeProject(const QPointer<Project> &);
            void setCurrentProject(const QPointer<Project> &);
            void nextProject();
            void prevProject();
            void setViewMode(QMdiArea::ViewMode);
            void setWindowState(Enum::WindowState);
            void setWindowState(const QPointer<Project> &, Enum::WindowState);

        Q_SIGNALS:
            void nameChanged(const QString &);
            void projectAdded(const QPointer<Project> &);
            void projectRemoved(const QPointer<Project> &);
            void currentProjectChanged(const QPointer<Project> &);
            void viewModeChanged(QMdiArea::ViewMode);
            void windowStateChanged(Enum::WindowState);

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewLib
} // namespace djv

