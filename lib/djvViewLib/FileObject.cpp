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

#include <djvViewLib/FileObject.h>

#include <djvViewLib/Context.h>
#include <djvViewLib/Project.h>
#include <djvViewLib/WorkspaceObject.h>
#include <djvViewLib/Workspace.h>

#include <djvUIQt/FileBrowser.h>

#include <QAction>
#include <QApplication>
#include <QDockWidget>
#include <QMenu>

namespace djv
{
    namespace ViewLib
    {
        struct FileObject::Private
        {
            std::map<QString, QPointer<QAction> > actions;
            std::vector<QPointer<QDockWidget> > dockWidgets;
        };
        
        FileObject::FileObject(const std::shared_ptr<Context> & context, QObject * parent) :
            IViewObject("FileObject", context, parent),
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
                auto workspaceObject = context->getObjectT<WorkspaceObject>();
                if (auto workspace = workspaceObject->getCurrentWorkspace())
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
                auto workspaceObject = context->getObjectT<WorkspaceObject>();
                if (auto workspace = workspaceObject->getCurrentWorkspace())
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
        
        FileObject::~FileObject()
        {}

        std::string FileObject::getMenuSortKey() const
        {
            return "0";
        }
        
        QPointer<QMenu> FileObject::createMenu()
        {
            auto menu = new QMenu("File");
            menu->addAction(_p->actions["Open"]);
            menu->addAction(_p->actions["Close"]);
            menu->addAction(_p->actions["Export"]);
            menu->addSeparator();
            menu->addAction(_p->actions["Exit"]);
            return menu;
        }

        QPointer<QDockWidget> FileObject::createDockWidget()
        {
            QPointer<QDockWidget> out;
            if (auto context = std::dynamic_pointer_cast<Context>(getContext().lock()))
            {
                out = new QDockWidget("File Browser");
                out->setWidget(new UIQt::FileBrowserWidget(context));
                _p->dockWidgets.push_back(out);
            }
            return out;
        }

        Qt::DockWidgetArea FileObject::getDockWidgetArea() const
        {
            return Qt::DockWidgetArea::LeftDockWidgetArea;
        }

        bool FileObject::isDockWidgetVisible() const
        {
            return true;
        }

        void FileObject::setCurrentWorkspace(const QPointer<Workspace> & workspace)
        {
            _p->actions["Open"]->setEnabled(workspace);
        }

        void FileObject::setCurrentProject(const std::shared_ptr<Project> & project)
        {
            _p->actions["Close"]->setEnabled(project ? true : false);
            _p->actions["Export"]->setEnabled(project ? true : false);
        }

    } // namespace ViewLib
} // namespace djv

