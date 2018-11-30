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

#include <djvCore/Core.h>

#include <QObject>
#include <QPointer>

class QDockWidget;
class QMenu;

namespace djv
{
    namespace ViewLib
    {
        class Context;
        class Project;
        class Workspace;
        
        class IViewObject : public QObject
        {
            Q_OBJECT

        public:
            IViewObject(const std::string & name, const std::shared_ptr<Context> &, QObject * parent = nullptr);
            virtual ~IViewObject() = 0;

            const std::weak_ptr<Context> & getContext() const;
            const std::string & getName() const;

            virtual QPointer<QMenu> createMenu();
            virtual std::string getMenuSortKey() const;

            virtual QPointer<QMenu> createContextMenu() { return createMenu(); }
            virtual std::string getContextMenuSortKey() const { return getMenuSortKey(); }

            virtual QPointer<QDockWidget> createDockWidget();
            virtual std::string getDockWidgetSortKey() const;
            virtual Qt::DockWidgetArea getDockWidgetArea() const;
            virtual bool isDockWidgetVisible() const;

        public Q_SLOTS:
            virtual void setCurrentWorkspace(const QPointer<Workspace> &);
            virtual void setCurrentProject(const QPointer<Project> &);

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewLib
} // namespace djv

