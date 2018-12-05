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

#include <djvViewLib/WorkspaceObject.h>

#include <djvViewLib/Context.h>
#include <djvViewLib/Media.h>
#include <djvViewLib/WorkspacePrivate.h>

#include <QAction>
#include <QDockWidget>
#include <QMenu>

namespace djv
{
    namespace ViewLib
    {
        struct WorkspaceObject::Private
        {
            std::vector<QPointer<Workspace> > workspaces;
            QPointer<Workspace> currentWorkspace;
            std::map<std::string, QPointer<QAction> > actions;
            std::map<std::string, QPointer<QActionGroup> > actionGroups;
            std::vector<QPointer<QMenu> > menus;
        };
        
        WorkspaceObject::WorkspaceObject(const std::shared_ptr<Context> & context, QObject * parent) :
            IViewObject("WorkspaceObject", context, parent),
            _p(new Private)
        {
            _p->actions["New"] = new QAction("New", this);
            _p->actions["Close"] = new QAction("Close", this);
            _p->actions["Next"] = new QAction("Next", this);
            _p->actions["Next"]->setShortcut(QKeySequence("Ctrl+Page Up"));
            _p->actions["Previous"] = new QAction("Previous", this);
            _p->actions["Previous"]->setShortcut(QKeySequence("Ctrl+Page Down"));

            _p->actions["Tabs"] = new QAction("Tabs", this);
            _p->actions["Tabs"]->setCheckable(true);
            _p->actions["Tabs"]->setShortcut(QKeySequence("Ctrl+Meta+T"));
            _p->actions["Windows"] = new QAction("Windows", this);
            _p->actions["Windows"]->setCheckable(true);
            _p->actions["Windows"]->setShortcut(QKeySequence("Ctrl+Meta+W"));
            _p->actionGroups["ViewMode"] = new QActionGroup(this);
            _p->actionGroups["ViewMode"]->setExclusive(true);
            _p->actionGroups["ViewMode"]->addAction(_p->actions["Tabs"]);
            _p->actionGroups["ViewMode"]->addAction(_p->actions["Windows"]);

            connect(
                _p->actions["New"],
                &QAction::triggered,
                [this]
            {
                newWorkspace();
            });
            connect(
                _p->actions["Close"],
                &QAction::triggered,
                [this]
            {
                closeWorkspace(getCurrentWorkspace());
            });
            connect(
                _p->actions["Next"],
                &QAction::triggered,
                [this]
            {
                nextWorkspace();
            });
            connect(
                _p->actions["Previous"],
                &QAction::triggered,
                [this]
            {
                prevWorkspace();
            });
            connect(
                _p->actionGroups["ViewMode"],
                &QActionGroup::triggered,
                [this](QAction * action)
            {
                if (auto workspace = getCurrentWorkspace())
                {
                    if (_p->actions["Tabs"] == action)
                    {
                        workspace->setViewMode(QMdiArea::TabbedView);
                    }
                    else if (_p->actions["Windows"] == action)
                    {
                        workspace->setViewMode(QMdiArea::SubWindowView);
                    }
                }
            });

            newWorkspace();
        }
        
        WorkspaceObject::~WorkspaceObject()
        {}

        const std::vector<QPointer<Workspace> > & WorkspaceObject::getWorkspaces() const
        {
            return _p->workspaces;
        }

        const QPointer<Workspace> & WorkspaceObject::getCurrentWorkspace() const
        {
            return _p->currentWorkspace;
        }

        std::shared_ptr<Media> WorkspaceObject::getCurrentMedia() const
        {
            return _p->currentWorkspace ? _p->currentWorkspace->getCurrentMedia() : nullptr;
        }

        QPointer<QWidget> WorkspaceObject::createWorkspaceTabs()
        {
            WorkspaceTabs * out = nullptr;
            if (auto context = std::dynamic_pointer_cast<Context>(getContext().lock()))
            {
                out = new WorkspaceTabs(context);
            }
            return out;
        }

        QPointer<QMenu> WorkspaceObject::createMenu()
        {
            auto menu = new QMenu("Workspace");
            _p->menus.push_back(menu);
            _updateMenus();
            return menu;
        }

        std::string WorkspaceObject::getMenuSortKey() const
        {
            return "1";
        }

        void WorkspaceObject::newWorkspace()
        {
            if (auto context = std::dynamic_pointer_cast<Context>(getContext().lock()))
            {
                auto workspace = new Workspace(context);
                _p->workspaces.push_back(workspace);
                _updateMenus();
                Q_EMIT workspaceAdded(workspace);
                setCurrentWorkspace(workspace);
            }
        }

        void WorkspaceObject::closeWorkspace(const QPointer<Workspace> & workspace)
        {
            auto i = std::find(_p->workspaces.begin(), _p->workspaces.end(), workspace);
            if (i != _p->workspaces.end())
            {
                //! \todo Save
                int index = static_cast<int>(i - _p->workspaces.begin());
                auto workspace = *i;
                _p->workspaces.erase(i);
                _updateMenus();
                Q_EMIT workspaceRemoved(workspace);
                if (workspace == _p->currentWorkspace)
                {
                    index = std::min(index, static_cast<int>(_p->workspaces.size()) - 1);
                    setCurrentWorkspace(index != -1 ? _p->workspaces[index] : nullptr);
                }
                delete workspace.data();
            }
        }

        void WorkspaceObject::setCurrentWorkspace(const QPointer<Workspace> & workspace)
        {
            if (workspace == _p->currentWorkspace)
                return;
            if (_p->currentWorkspace)
            {
                disconnect(
                    _p->currentWorkspace,
                    SIGNAL(mediaAdded(const std::shared_ptr<Media> &)),
                    this,
                    SLOT(_updateMenus()));
                disconnect(
                    _p->currentWorkspace,
                    SIGNAL(mediaRemoved(const std::shared_ptr<Media> &)),
                    this,
                    SLOT(_updateMenus()));
                disconnect(
                    _p->currentWorkspace,
                    SIGNAL(currentMediaChanged(const std::shared_ptr<Media> &)),
                    this,
                    SIGNAL(currentMediaChanged(const std::shared_ptr<Media> &)));
            }
            _p->currentWorkspace = workspace;
            _updateMenus();
            if (_p->currentWorkspace)
            {
                connect(
                    _p->currentWorkspace,
                    SIGNAL(mediaAdded(const std::shared_ptr<Media> &)),
                    SLOT(_updateMenus()));
                connect(
                    _p->currentWorkspace,
                    SIGNAL(mediaRemoved(const std::shared_ptr<Media> &)),
                    SLOT(_updateMenus()));
                connect(
                    _p->currentWorkspace,
                    SIGNAL(currentMediaChanged(const std::shared_ptr<Media> &)),
                    SIGNAL(currentMediaChanged(const std::shared_ptr<Media> &)));
            }
            else
            {
                Q_EMIT currentMediaChanged(nullptr);
            }
            Q_EMIT currentWorkspaceChanged(_p->currentWorkspace);
        }

        void WorkspaceObject::nextWorkspace()
        {
            auto i = std::find(_p->workspaces.begin(), _p->workspaces.end(), _p->currentWorkspace);
            if (i != _p->workspaces.end())
            {
                ++i;
                if (i == _p->workspaces.end())
                {
                    i = _p->workspaces.begin();
                }
                setCurrentWorkspace(*i);
            }
        }

        void WorkspaceObject::prevWorkspace()
        {
            auto i = std::find(_p->workspaces.begin(), _p->workspaces.end(), _p->currentWorkspace);
            if (i != _p->workspaces.end())
            {
                if (i == _p->workspaces.begin())
                {
                    i = _p->workspaces.end();
                }
                --i;
                setCurrentWorkspace(*i);
            }
        }

        void WorkspaceObject::_updateMenus()
        {
            const bool currentWorkspace = _p->currentWorkspace;
            _p->actions["Close"]->setEnabled(currentWorkspace);
            _p->actionGroups["ViewMode"]->setEnabled(currentWorkspace);

            const bool multipleWorkspaces = _p->workspaces.size() > 1;
            _p->actions["Next"]->setEnabled(multipleWorkspaces);
            _p->actions["Previous"]->setEnabled(multipleWorkspaces);

            QMdiArea::ViewMode viewMode = QMdiArea::TabbedView;
            if (_p->currentWorkspace)
            {
                viewMode = _p->currentWorkspace->getViewMode();
            }
            switch (viewMode)
            {
            case QMdiArea::TabbedView: _p->actions["Tabs"]->setChecked(true); break;
            case QMdiArea::SubWindowView: _p->actions["Windows"]->setChecked(true); break;
            default: break;
            }

            for (auto menu : _p->menus)
            {
                menu->clear();
                menu->addAction(_p->actions["New"]);
                menu->addAction(_p->actions["Close"]);
                menu->addAction(_p->actions["Next"]);
                menu->addAction(_p->actions["Previous"]);
                menu->addSeparator();
                menu->addAction(_p->actions["Tabs"]);
                menu->addAction(_p->actions["Windows"]);
            }
        }

    } // namespace ViewLib
} // namespace djv

