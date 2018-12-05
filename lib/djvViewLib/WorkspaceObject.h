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

#include <djvViewLib/IViewObject.h>
#include <djvViewLib/Workspace.h>

namespace djv
{
    namespace ViewLib
    {
        class WorkspaceObject : public IViewObject
        {
            Q_OBJECT

        public:
            WorkspaceObject(const std::shared_ptr<Context> &, QObject * parent = nullptr);
            ~WorkspaceObject() override;

            const std::vector<QPointer<Workspace> > & getWorkspaces() const;
            const QPointer<Workspace> & getCurrentWorkspace() const;
            std::shared_ptr<Media> getCurrentMedia() const;

            QPointer<QWidget> createWorkspaceTabs();
            
            QPointer<QMenu> createMenu() override;
            std::string getMenuSortKey() const override;

        public Q_SLOTS:
            void newWorkspace();
            void closeWorkspace(const QPointer<Workspace> &);
            void nextWorkspace();
            void prevWorkspace();

            void setCurrentWorkspace(const QPointer<Workspace> &) override;

        Q_SIGNALS:
            void workspaceAdded(const QPointer<Workspace> &);
            void workspaceRemoved(const QPointer<Workspace> &);
            void currentWorkspaceChanged(const QPointer<Workspace> &);
            void currentMediaChanged(const std::shared_ptr<Media> &);

        private Q_SLOTS:
            void _updateMenus();

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewLib
} // namespace djv

