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

#include <MainWindow.h>

#include <Context.h>
#include <IUISystem.h>
#include <ProjectTabWidget.h>
#include <WindowSystem.h>

#include <QDockWidget>
#include <QLayout>
#include <QMenu>
#include <QMenuBar>

namespace djv
{
    namespace ViewExperiment
    {
        struct MainWindow::Private
        {
            QPointer<Context> context;
            QPointer<ProjectTabWidget> projectTabWidget;
        };
        
        MainWindow::MainWindow(const QPointer<Context> & context) :
            _p(new Private)
        {
            _p->context = context;

            std::map<QString, QPointer<QMenu> > menus;
            for (auto i : _p->context->getSystems())
            {
                if (auto uiSystem = qobject_cast<IUISystem *>(i))
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

            std::map<QString, std::pair<QPointer<QDockWidget>, Qt::DockWidgetArea> > dockWidgetMap;
            for (auto i : _p->context->getSystems())
            {
                if (auto uiSystem = qobject_cast<IUISystem *>(i))
                {
                    if (auto dockWidget = uiSystem->createDockWidget())
                    {
                        dockWidgetMap[uiSystem->getDockWidgetSortKey()] = std::make_pair(dockWidget, uiSystem->getDockWidgetArea());
                    }
                }
            }
            std::vector<QPointer<QDockWidget> > dockWidgetList;
            for (auto i : dockWidgetMap)
            {
                addDockWidget(i.second.second, i.second.first);
                dockWidgetList.push_back(i.second.first);
            }
            context->getSystem<WindowSystem>()->setDockWidgets(dockWidgetList);

            _p->projectTabWidget = new ProjectTabWidget(context);
            _p->projectTabWidget->layout()->setContentsMargins(0, 0, 0, 0);
            setCentralWidget(_p->projectTabWidget);
        }
        
        MainWindow::~MainWindow()
        {}
        
        QMenu * MainWindow::createPopupMenu()
        {
            auto out = new QMenu;
            QMap<QString, QPointer<QMenu> > map;
            Q_FOREACH(auto i, _p->context->getSystems())
            {
                if (auto uiSystem = qobject_cast<IUISystem *>(i))
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
        
    } // namespace ViewExperiment
} // namespace djv

