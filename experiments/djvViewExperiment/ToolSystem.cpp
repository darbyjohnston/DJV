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

#include <ToolSystem.h>

#include <Context.h>

#include <QAction>
#include <QDockWidget>
#include <QMenu>

#include <iostream>

namespace djv
{
    namespace ViewExperiment
    {
        struct ToolSystem::Private
        {
            std::map<QString, QPointer<QAction> > dockWidgetsActions;
            std::vector<QPointer<QMenu> > menus;
        };
        
        ToolSystem::ToolSystem(const QPointer<Context> & context, QObject * parent) :
            IUISystem("ToolSystem", context, parent),
            _p(new Private)
        {
        }
        
        ToolSystem::~ToolSystem()
        {}

        void ToolSystem::setDockWidgets(const std::vector<QPointer<QDockWidget> > & value)
        {
            for (auto i : value)
            {
                const auto & title = i->windowTitle();
                _p->dockWidgetsActions[title] = new QAction(title, this);
                auto action = _p->dockWidgetsActions[title];
                action->setCheckable(true);
                action->setChecked(i->isVisible());
                connect(
                    action,
                    &QAction::toggled,
                    [i](bool value)
                {
                    i->setVisible(value);
                });
                connect(
                    i,
                    &QDockWidget::visibilityChanged,
                    [action](bool value)
                {
                    action->setChecked(value);
                });
            }
            _updateMenus();
        }

        QString ToolSystem::getMenuSortKey() const
        {
            return "2";
        }
        
        QPointer<QMenu> ToolSystem::createMenu()
        {
            auto menu = new QMenu("Tools");
            _p->menus.push_back(menu);
            _updateMenus();
            return menu;
        }

        void ToolSystem::_updateMenus()
        {
            for (auto menu : _p->menus)
            {
                menu->clear();
                for (auto action : _p->dockWidgetsActions)
                {
                    menu->addAction(action.second);
                }
            }
        }

    } // namespace ViewExperiment
} // namespace djv

