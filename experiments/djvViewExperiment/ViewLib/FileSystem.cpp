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

#include <ViewLib/FileSystem.h>

#include <ViewLib/Context.h>
#include <ViewLib/Project.h>
#include <ViewLib/WorkspaceSystem.h>
#include <ViewLib/Workspace.h>

#include <UI/FileBrowser.h>

#include <QAction>
#include <QApplication>
#include <QDockWidget>
#include <QMenu>

#include <iostream>

namespace djv
{
    namespace ViewLib
    {
        struct FileSystem::Private
        {
            std::map<QString, QPointer<QAction> > actions;
            std::vector<QPointer<QDockWidget> > dockWidgets;
        };
        
        FileSystem::FileSystem(const QPointer<Context> & context, QObject * parent) :
            IViewSystem("FileSystem", context, parent),
            _p(new Private)
        {
            _p->actions["Open"] = new QAction("Open", this);
            _p->actions["Open"]->setShortcut(QKeySequence("Ctrl+O"));
            _p->actions["Close"] = new QAction("Close", this);
            _p->actions["Close"]->setShortcut(QKeySequence("Ctrl+W"));
            _p->actions["Export"] = new QAction("Export", this);
            _p->actions["Export"]->setShortcut(QKeySequence("Ctrl+E"));
            _p->actions["Exit"] = new QAction("Exit", this);
            _p->actions["Exit"]->setShortcut(QKeySequence("Ctrl+Q"));

            connect(
                _p->actions["Open"],
                &QAction::triggered,
                [this, context]
            {
                auto workspaceSystem = context->getSystemT<WorkspaceSystem>();
                if (auto workspace = workspaceSystem->getCurrentWorkspace())
                {
                    workspace->newProject();
                }
                for (const auto & i : _p->dockWidgets)
                {
                    i->show();
                }
            });
            connect(
                _p->actions["Close"],
                &QAction::triggered,
                [context]
            {
                auto workspaceSystem = context->getSystemT<WorkspaceSystem>();
                if (auto workspace = workspaceSystem->getCurrentWorkspace())
                {
                    workspace->closeProject(workspace->getCurrentProject());
                }
            });
            connect(
                _p->actions["Export"],
                &QAction::triggered,
                [context]
            {
            });
            connect(
                _p->actions["Exit"],
                &QAction::triggered,
                [this]
            {
                qApp->quit();
            });
        }
        
        FileSystem::~FileSystem()
        {}

        QString FileSystem::getMenuSortKey() const
        {
            return "0";
        }
        
        QPointer<QMenu> FileSystem::createMenu()
        {
            auto menu = new QMenu("File");
            menu->addAction(_p->actions["Open"]);
            menu->addAction(_p->actions["Close"]);
            menu->addAction(_p->actions["Export"]);
            menu->addSeparator();
            menu->addAction(_p->actions["Exit"]);
            return menu;
        }

        QPointer<QDockWidget> FileSystem::createDockWidget()
        {
            auto out = new QDockWidget("File Browser");
            out->setWidget(new UI::FileBrowserWidget(qobject_cast<UI::Context *>(getContext().data())));
            _p->dockWidgets.push_back(out);
            return out;
        }

        Qt::DockWidgetArea FileSystem::getDockWidgetArea() const
        {
            return Qt::DockWidgetArea::LeftDockWidgetArea;
        }

        bool FileSystem::isDockWidgetVisible() const
        {
            return true;
        }

        void FileSystem::setCurrentWorkspace(const QPointer<Workspace> & workspace)
        {
            _p->actions["Open"]->setEnabled(workspace);
        }

        void FileSystem::setCurrentProject(const QPointer<Project> & project)
        {
            _p->actions["Close"]->setEnabled(project);
            _p->actions["Export"]->setEnabled(project);
        }

    } // namespace ViewLib
} // namespace djv
