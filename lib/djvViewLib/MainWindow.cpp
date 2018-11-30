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
#include <djvViewLib/ToolObject.h>
#include <djvViewLib/WorkspaceObject.h>

#include <QDockWidget>
#include <QLayout>
#include <QMenu>
#include <QMenuBar>

namespace djv
{
    namespace ViewLib
    {
        struct MainWindow::Private
        {
            std::weak_ptr<Context> context;
        };
        
        MainWindow::MainWindow(const std::shared_ptr<Context> & context) :
            _p(new Private)
        {
            _p->context = context;

            std::map<std::string, QPointer<QMenu> > menus;
            for (auto i : context->getObjects())
            {
                if (auto viewObject = qobject_cast<IViewObject *>(i))
                {
                    if (auto menu = viewObject->createContextMenu())
                    {
                        menus[viewObject->getContextMenuSortKey()] = menu;
                    }
                }
            }
            for (auto i : menus)
            {
                menuBar()->addMenu(i.second);
            }

            auto workspaceObject = context->getObjectT<WorkspaceObject>();
            setCentralWidget(workspaceObject->createWorkspaceTabs());

            struct DockWidget
            {
                QPointer<IViewObject> object;
                QPointer<QDockWidget> widget;
                Qt::DockWidgetArea area;
                bool visible = false;
            };
            std::map<std::string, DockWidget> dockWidgetMap;
            std::map<std::string, DockWidget> toolDockWidgetMap;
            for (auto i : context->getObjects())
            {
                if (auto toolObject = qobject_cast<IToolObject *>(i))
                {
                    if (auto dockWidget = toolObject->createDockWidget())
                    {
                        const auto & key = toolObject->getDockWidgetSortKey();
                        toolDockWidgetMap[key].object = toolObject;
                        toolDockWidgetMap[key].widget = dockWidget;
                        toolDockWidgetMap[key].area = toolObject->getDockWidgetArea();
                        toolDockWidgetMap[key].visible = toolObject->isDockWidgetVisible();
                    }
                }
                else if (auto viewObject = qobject_cast<IViewObject *>(i))
                {
                    if (auto dockWidget = viewObject->createDockWidget())
                    {
                        const auto & key = viewObject->getDockWidgetSortKey();
                        dockWidgetMap[key].object = viewObject;
                        dockWidgetMap[key].widget = dockWidget;
                        dockWidgetMap[key].area = viewObject->getDockWidgetArea();
                        dockWidgetMap[key].visible = viewObject->isDockWidgetVisible();
                    }
                }
            }
            for (auto i : dockWidgetMap)
            {
                addDockWidget(i.second.area, i.second.widget);
                i.second.widget->setVisible(i.second.visible);
            }
            auto toolObject = context->getObjectT<ToolObject>();
            for (auto i : toolDockWidgetMap)
            {
                addDockWidget(i.second.area, i.second.widget);
                i.second.widget->setVisible(i.second.visible);
                toolObject->addDockWidget(qobject_cast<IToolObject *>(i.second.object.data()), i.second.widget);
            }
        }
        
        MainWindow::~MainWindow()
        {}
        
        QMenu * MainWindow::createPopupMenu()
        {
            QMenu * out = nullptr;
            if (auto context = _p->context.lock())
            {
                auto out = new QMenu;
                std::map<std::string, QPointer<QMenu> > map;
                for (auto i : context->getObjects())
                {
                    if (auto viewObject = qobject_cast<IViewObject *>(i))
                    {
                        if (auto menu = viewObject->createContextMenu())
                        {
                            map[viewObject->getContextMenuSortKey()] = menu;
                        }
                    }
                }
                for (auto i : map)
                {
                    out->addMenu(i.second);
                }
            }
            return out;
        }
        
    } // namespace ViewLib
} // namespace djv

