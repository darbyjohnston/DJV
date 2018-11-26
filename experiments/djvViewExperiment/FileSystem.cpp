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

#include <FileSystem.h>

#include <Context.h>
#include <FileBrowser.h>
#include <Workspace.h>

#include <QAction>
#include <QDockWidget>
#include <QMenu>

#include <iostream>

namespace djv
{
    namespace ViewExperiment
    {
        struct FileSystem::Private
        {
            std::map<QString, QPointer<QAction> > actions;
        };
        
        FileSystem::FileSystem(const QPointer<Context> & context, QObject * parent) :
            IUISystem("FileSystem", context, parent),
            _p(new Private)
        {
            _p->actions["New"] = new QAction("New", this);
            _p->actions["New"]->setShortcut(QKeySequence("Ctrl+N"));
            _p->actions["Open"] = new QAction("Open", this);
            _p->actions["Open"]->setShortcut(QKeySequence("Ctrl+O"));
            _p->actions["Close"] = new QAction("Close", this);
            _p->actions["Close"]->setShortcut(QKeySequence("Ctrl+W"));
            _p->actions["Save"] = new QAction("Save", this);
            _p->actions["Save"]->setShortcut(QKeySequence("Ctrl+S"));
            _p->actions["Save As"] = new QAction("Save As", this);
            _p->actions["Save As"]->setShortcut(QKeySequence("Ctrl+Shift+S"));

            connect(
                _p->actions["New"],
                &QAction::triggered,
                [context]
            {
                auto workspaceSystem = context->getSystem<WorkspaceSystem>();
                if (auto workspace = workspaceSystem->getCurrentWorkspace())
                {
                    workspace->newProject();
                }
            });
            connect(
                _p->actions["Open"],
                &QAction::triggered,
                [context]
            {
            });
            connect(
                _p->actions["Close"],
                &QAction::triggered,
                [context]
            {
                auto workspaceSystem = context->getSystem<WorkspaceSystem>();
                if (auto workspace = workspaceSystem->getCurrentWorkspace())
                {
                    workspace->closeProject(workspace->getCurrentProject());
                }
            });
            connect(
                _p->actions["Save"],
                &QAction::triggered,
                [context]
            {
            });
            connect(
                _p->actions["Save As"],
                &QAction::triggered,
                [context]
            {
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
            menu->addAction(_p->actions["New"]);
            menu->addAction(_p->actions["Open"]);
            menu->addAction(_p->actions["Close"]);
            menu->addAction(_p->actions["Save"]);
            menu->addAction(_p->actions["Save As"]);
            return menu;
        }

        QPointer<QDockWidget> FileSystem::createDockWidget()
        {
            auto out = new QDockWidget("File Browser");
            out->setWidget(new FileBrowserWidget(getContext()));
            return out;
        }

        QString FileSystem::getDockWidgetSortKey() const
        {
            return "0";
        }

        Qt::DockWidgetArea FileSystem::getDockWidgetArea() const
        {
            return Qt::DockWidgetArea::LeftDockWidgetArea;
        }

    } // namespace ViewExperiment
} // namespace djv

