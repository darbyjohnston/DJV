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

#include <djvViewLib/WindowObject.h>

#include <djvViewLib/Context.h>
#include <djvViewLib/Media.h>

#include <QAction>
#include <QDockWidget>
#include <QMenu>

namespace djv
{
    namespace ViewLib
    {
        struct WindowObject::Private
        {
            std::map<QString, QPointer<QAction> > actions;
            std::map<QString, QPointer<QActionGroup> > actionGroups;
            std::vector<QPointer<QMenu> > menus;
            QPointer<Workspace> currentWorkspace;
        };
        
        WindowObject::WindowObject(const std::shared_ptr<Context> & context, QObject * parent) :
            IViewObject("WindowObject", context, parent),
            _p(new Private)
        {
            DJV_PRIVATE_PTR();
            p.actions["Next"] = new QAction("Next", this);
            p.actions["Next"]->setShortcut(QKeySequence("Page Up"));
            p.actions["Previous"] = new QAction("Previous", this);
            p.actions["Previous"]->setShortcut(QKeySequence("Page Down"));
            p.actions["Normal"] = new QAction("Normal", this);
            p.actions["Normal"]->setCheckable(true);
            p.actions["Normal"]->setShortcut(QKeySequence("Ctrl+Meta+M"));
            p.actions["Maximize"] = new QAction("Maximize", this);
            p.actions["Maximize"]->setCheckable(true);
            p.actions["Maximize"]->setShortcut(QKeySequence("Ctrl+M"));
            p.actions["Minimize"] = new QAction("Minimize", this);
            p.actions["Minimize"]->setCheckable(true);
            p.actions["Minimize"]->setShortcut(QKeySequence("Ctrl+Shift+M"));
            p.actionGroups["WindowState"] = new QActionGroup(this);
            p.actionGroups["WindowState"]->setExclusive(true);
            p.actionGroups["WindowState"]->addAction(p.actions["Normal"]);
            p.actionGroups["WindowState"]->addAction(p.actions["Maximize"]);
            p.actionGroups["WindowState"]->addAction(p.actions["Minimize"]);

            _updateActions();

            connect(
                p.actions["Next"],
                &QAction::triggered,
                [this]
            {
                nextMedia();
            });
            connect(
                p.actions["Previous"],
                &QAction::triggered,
                [this]
            {
                prevMedia();
            });
            connect(
                p.actionGroups["WindowState"],
                &QActionGroup::triggered,
                [this](QAction * action)
            {
                DJV_PRIVATE_PTR();
                if (p.currentWorkspace)
                {
                    if (p.actions["Normal"] == action)
                    {
                        p.currentWorkspace->setWindowState(Enum::WindowState::Normal);
                    }
                    else if (p.actions["Maximize"] == action)
                    {
                        p.currentWorkspace->setWindowState(
                            Enum::WindowState::Maximized == p.currentWorkspace->getWindowState() ?
                            Enum::WindowState::Normal :
                            Enum::WindowState::Maximized);
                    }
                    else if (p.actions["Minimize"] == action)
                    {
                        p.currentWorkspace->setWindowState(
                            Enum::WindowState::Minimized == p.currentWorkspace->getWindowState() ?
                            Enum::WindowState::Normal :
                            Enum::WindowState::Minimized);
                    }
                }
            });
        }
        
        WindowObject::~WindowObject()
        {}

        QPointer<QMenu> WindowObject::createMenu()
        {
            DJV_PRIVATE_PTR();
            auto menu = new QMenu("Window");
            menu->addAction(p.actions["Next"]);
            menu->addAction(p.actions["Previous"]);
            menu->addSeparator();
            menu->addAction(p.actions["Normal"]);
            menu->addAction(p.actions["Minimize"]);
            menu->addAction(p.actions["Maximize"]);
            return menu;
        }

        std::string WindowObject::getMenuSortKey() const
        {
            return "2";
        }

        void WindowObject::nextMedia()
        {
            DJV_PRIVATE_PTR();
            if (p.currentWorkspace)
            {
                p.currentWorkspace->nextMedia();
            }
        }

        void WindowObject::prevMedia()
        {
            DJV_PRIVATE_PTR();
            if (p.currentWorkspace)
            {
                p.currentWorkspace->prevMedia();
            }
        }

        void WindowObject::setCurrentWorkspace(const QPointer<Workspace> & workspace)
        {
            DJV_PRIVATE_PTR();
            if (workspace == p.currentWorkspace)
                return;
            if (p.currentWorkspace)
            {
                disconnect(
                    p.currentWorkspace,
                    SIGNAL(mediaAdded(const std::shared_ptr<Media> &)),
                    this,
                    SLOT(_updateActions()));
                disconnect(
                    p.currentWorkspace,
                    SIGNAL(mediaRemoved(const std::shared_ptr<Media> &)),
                    this,
                    SLOT(_updateActions()));
                disconnect(
                    p.currentWorkspace,
                    SIGNAL(viewModeChanged(QMdiArea::ViewMode)),
                    this,
                    SLOT(_updateActions()));
                disconnect(
                    p.currentWorkspace,
                    SIGNAL(windowStateChanged(Enum::WindowState)),
                    this,
                    SLOT(_updateActions()));
            }
            p.currentWorkspace = workspace;
            _updateActions();
            if (p.currentWorkspace)
            {
                connect(
                    p.currentWorkspace,
                    SIGNAL(mediaAdded(const std::shared_ptr<Media> &)),
                    SLOT(_updateActions()));
                connect(
                    p.currentWorkspace,
                    SIGNAL(mediaRemoved(const std::shared_ptr<Media> &)),
                    SLOT(_updateActions()));
                connect(
                    p.currentWorkspace,
                    SIGNAL(viewModeChanged(QMdiArea::ViewMode)),
                    SLOT(_updateActions()));
                connect(
                    p.currentWorkspace,
                    SIGNAL(windowStateChanged(Enum::WindowState)),
                    SLOT(_updateActions()));
            }
        }

        void WindowObject::_updateActions()
        {
            DJV_PRIVATE_PTR();
            const bool multipleMedia = p.currentWorkspace && p.currentWorkspace->getMedia().size() > 1;
            p.actions["Next"]->setEnabled(multipleMedia);
            p.actions["Previous"]->setEnabled(multipleMedia);

            const bool hasMedia =
                p.currentWorkspace &&
                p.currentWorkspace->getMedia().size() > 0 &&
                QMdiArea::SubWindowView == p.currentWorkspace->getViewMode();
            p.actionGroups["WindowState"]->setEnabled(hasMedia);
            p.actions["Normal"]->setChecked(p.currentWorkspace && p.currentWorkspace->getWindowState() == Enum::WindowState::Normal);
            p.actions["Maximize"]->setChecked(p.currentWorkspace && p.currentWorkspace->getWindowState() == Enum::WindowState::Maximized);
            p.actions["Minimize"]->setChecked(p.currentWorkspace && p.currentWorkspace->getWindowState() == Enum::WindowState::Minimized);
        }

    } // namespace ViewLib
} // namespace djv

