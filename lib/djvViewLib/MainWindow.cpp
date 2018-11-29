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

#include <djvViewLib/MainWindow.h>

#include <djvViewLib/Context.h>
#include <djvViewLib/ToolSystem.h>
#include <djvViewLib/WorkspaceSystem.h>

#include <QDockWidget>
#include <QLayout>
#include <QMenu>
#include <QMenuBar>

#include <iostream>

namespace djv
{
    namespace ViewLib
    {
        struct MainWindow::Private
        {
            QPointer<Context> context;
        };
        
        MainWindow::MainWindow(const QPointer<Context> & context) :
            _p(new Private)
        {
            _p->context = context;

            std::map<QString, QPointer<QMenu> > menus;
            for (auto i : context->getSystems())
            {
                if (auto uiSystem = qobject_cast<IViewSystem *>(i))
                {
                    if (auto menu = uiSystem->createContextMenu())
                    {
                        menus[uiSystem->getContextMenuSortKey()] = menu;
                    }
                }
            }
            for (auto i : menus)
            {
                menuBar()->addMenu(i.second);
            }

            auto workspaceSystem = context->getSystemT<WorkspaceSystem>();
            setCentralWidget(workspaceSystem->createWorkspaceTabs());

            struct DockWidget
            {
                QPointer<IViewSystem> system;
                QPointer<QDockWidget> widget;
                Qt::DockWidgetArea area;
                bool visible = false;
            };
            std::map<QString, DockWidget> dockWidgetMap;
            std::map<QString, DockWidget> toolDockWidgetMap;
            for (auto i : context->getSystems())
            {
                if (auto toolSystem = qobject_cast<IToolSystem *>(i))
                {
                    if (auto dockWidget = toolSystem->createDockWidget())
                    {
                        const auto & key = toolSystem->getDockWidgetSortKey();
                        toolDockWidgetMap[key].system = toolSystem;
                        toolDockWidgetMap[key].widget = dockWidget;
                        toolDockWidgetMap[key].area = toolSystem->getDockWidgetArea();
                        toolDockWidgetMap[key].visible = toolSystem->isDockWidgetVisible();
                    }
                }
                else if (auto uiSystem = qobject_cast<IViewSystem *>(i))
                {
                    if (auto dockWidget = uiSystem->createDockWidget())
                    {
                        const auto & key = uiSystem->getDockWidgetSortKey();
                        dockWidgetMap[key].system = uiSystem;
                        dockWidgetMap[key].widget = dockWidget;
                        dockWidgetMap[key].area = uiSystem->getDockWidgetArea();
                        dockWidgetMap[key].visible = uiSystem->isDockWidgetVisible();
                    }
                }
            }
            for (auto i : dockWidgetMap)
            {
                addDockWidget(i.second.area, i.second.widget);
                i.second.widget->setVisible(i.second.visible);
            }
            auto toolSystem = context->getSystemT<ToolSystem>();
            for (auto i : toolDockWidgetMap)
            {
                addDockWidget(i.second.area, i.second.widget);
                i.second.widget->setVisible(i.second.visible);
                toolSystem->addDockWidget(qobject_cast<IToolSystem *>(i.second.system.data()), i.second.widget);
            }
        }
        
        MainWindow::~MainWindow()
        {}
        
        QMenu * MainWindow::createPopupMenu()
        {
            auto out = new QMenu;
            QMap<QString, QPointer<QMenu> > map;
            Q_FOREACH(auto i, _p->context->getSystems())
            {
                if (auto uiSystem = qobject_cast<IViewSystem *>(i))
                {
                    if (auto menu = uiSystem->createContextMenu())
                    {
                        map[uiSystem->getContextMenuSortKey()] = menu;
                    }
                }
            }
            Q_FOREACH(auto i, map)
            {
                out->addMenu(i);
            }
            return out;
        }
        
    } // namespace ViewLib
} // namespace djv

