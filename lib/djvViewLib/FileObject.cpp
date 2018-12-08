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
#include <djvViewLib/Media.h>
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
            QPointer<Workspace> currentWorkspace;
        };
        
        FileObject::FileObject(const std::shared_ptr<Context> & context, QObject * parent) :
            IViewObject("FileObject", context, parent),
            _p(new Private)
        {
            DJV_PRIVATE_PTR();
            p.actions["Open"] = new QAction("Open", this);
            p.actions["Open"]->setShortcut(QKeySequence("Ctrl+O"));
            p.actions["Close"] = new QAction("Close", this);
            p.actions["Close"]->setShortcut(QKeySequence("Ctrl+W"));
            p.actions["Export"] = new QAction("Export", this);
            p.actions["Export"]->setShortcut(QKeySequence("Ctrl+E"));
            p.actions["Exit"] = new QAction("Exit", this);
            p.actions["Exit"]->setShortcut(QKeySequence("Ctrl+Q"));

            connect(
                p.actions["Open"],
                &QAction::triggered,
                [this, context]
            {
                DJV_PRIVATE_PTR();
                for (const auto & i : p.dockWidgets)
                {
                    i->show();
                }
            });
            connect(
                p.actions["Close"],
                &QAction::triggered,
                [context]
            {
                auto workspaceObject = context->getObjectT<WorkspaceObject>();
                if (auto workspace = workspaceObject->getCurrentWorkspace())
                {
                    workspace->closeMedia(workspace->getCurrentMedia());
                }
            });
            connect(
                p.actions["Export"],
                &QAction::triggered,
                [context]
            {
            });
            connect(
                p.actions["Exit"],
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
            DJV_PRIVATE_PTR();
            auto menu = new QMenu("File");
            menu->addAction(p.actions["Open"]);
            menu->addAction(p.actions["Close"]);
            menu->addAction(p.actions["Export"]);
            menu->addSeparator();
            menu->addAction(p.actions["Exit"]);
            return menu;
        }

        QPointer<QDockWidget> FileObject::createDockWidget()
        {
            DJV_PRIVATE_PTR();
            QPointer<QDockWidget> out;
            if (auto context = std::dynamic_pointer_cast<Context>(getContext().lock()))
            {
                out = new QDockWidget("File Browser");
                auto fileBrowser = new UIQt::FileBrowser(context);
                fileBrowser->setPath("C:/Users/darby/Desktop");
                out->setWidget(fileBrowser);
                p.dockWidgets.push_back(out);
                connect(
                    fileBrowser,
                    &UIQt::FileBrowser::opened,
                    [this](const std::string & fileName)
                {
                    DJV_PRIVATE_PTR();
                    if (p.currentWorkspace)
                    {
                        p.currentWorkspace->openMedia(fileName);
                    }
                });
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
            DJV_PRIVATE_PTR();
            p.actions["Open"]->setEnabled(workspace);
            p.currentWorkspace = workspace;
        }

        void FileObject::setCurrentMedia(const std::shared_ptr<Media> & project)
        {
            DJV_PRIVATE_PTR();
            p.actions["Close"]->setEnabled(project ? true : false);
            p.actions["Export"]->setEnabled(project ? true : false);
        }

    } // namespace ViewLib
} // namespace djv

