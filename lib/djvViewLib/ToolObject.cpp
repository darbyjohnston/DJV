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

#include <djvViewLib/ToolObject.h>

#include <djvViewLib/Context.h>

#include <QAction>
#include <QDockWidget>
#include <QMenu>

namespace djv
{
    namespace ViewLib
    {
        struct IToolObject::Private
        {
        };

        IToolObject::IToolObject(const std::string & name, const std::shared_ptr<Context> & context, QObject * parent) :
            IViewObject(name, context, parent),
            _p(new Private)
        {}

        IToolObject::~IToolObject()
        {}

        struct ToolObject::Private
        {
            std::map<std::string, QPointer<QAction> > dockWidgetsActions;
            std::vector<QPointer<QMenu> > menus;
        };
        
        ToolObject::ToolObject(const std::shared_ptr<Context> & context, QObject * parent) :
            IViewObject("ToolObject", context, parent),
            _p(new Private)
        {}
        
        ToolObject::~ToolObject()
        {}

        void ToolObject::addDockWidget(const QPointer<IToolObject> & object, const QPointer<QDockWidget> & dockWidget)
        {
            const auto & key = object->getDockWidgetSortKey();
            _p->dockWidgetsActions[key] = new QAction(dockWidget->windowTitle(), this);
            auto action = _p->dockWidgetsActions[key];
            action->setCheckable(true);
            action->setChecked(dockWidget->isVisible());
            connect(
                action,
                &QAction::toggled,
                [dockWidget](bool value)
            {
                dockWidget->setVisible(value);
            });
            connect(
                dockWidget,
                &QDockWidget::visibilityChanged,
                [action](bool value)
            {
                action->setChecked(value);
            });
            _updateMenus();
        }

        std::string ToolObject::getMenuSortKey() const
        {
            return "6";
        }
        
        QPointer<QMenu> ToolObject::createMenu()
        {
            auto menu = new QMenu("Tools");
            _p->menus.push_back(menu);
            _updateMenus();
            return menu;
        }

        void ToolObject::_updateMenus()
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

    } // namespace ViewLib
} // namespace djv

