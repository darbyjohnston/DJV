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

#include <ViewLib/PlaybackSystem.h>

#include <ViewLib/Context.h>
#include <ViewLib/Project.h>

#include <QAction>
#include <QDockWidget>
#include <QMenu>

#include <iostream>

namespace djv
{
    namespace ViewLib
    {
        struct PlaybackSystem::Private
        {
            std::map<QString, QPointer<QAction> > actions;
        };
        
        PlaybackSystem::PlaybackSystem(const QPointer<Context> & context, QObject * parent) :
            IViewSystem("PlaybackSystem", context, parent),
            _p(new Private)
        {
            _p->actions["Stop"] = new QAction("Stop", this);
            _p->actions["Stop"]->setShortcut(QKeySequence("Ctrl+K"));
            _p->actions["Forward"] = new QAction("Forward", this);
            _p->actions["Forward"]->setShortcut(QKeySequence("Ctrl+L"));
            _p->actions["Reverse"] = new QAction("Reverse", this);
            _p->actions["Reverse"]->setShortcut(QKeySequence("Ctrl+J"));

            _p->actions["Timeline"] = new QAction("Timeline", this);
            _p->actions["Timeline"]->setCheckable(true);
            _p->actions["Timeline"]->setChecked(true);
        }
        
        PlaybackSystem::~PlaybackSystem()
        {}

        QString PlaybackSystem::getMenuSortKey() const
        {
            return "5";
        }
        
        QPointer<QMenu> PlaybackSystem::createMenu()
        {
            auto menu = new QMenu("Playback");
            menu->addAction(_p->actions["Stop"]);
            menu->addAction(_p->actions["Forward"]);
            menu->addAction(_p->actions["Reverse"]);
            menu->addSeparator();
            menu->addAction(_p->actions["Timeline"]);
            return menu;
        }

        QPointer<QDockWidget> PlaybackSystem::createDockWidget()
        {
            auto out = new QDockWidget("Timeline");
            connect(
                _p->actions["Timeline"],
                &QAction::toggled,
                [out](bool value)
            {
                out->setVisible(value);
            });
            connect(
                out,
                &QDockWidget::visibilityChanged,
                [this](bool value)
            {
                _p->actions["Timeline"]->setChecked(value);
            });
            return out;
        }

        Qt::DockWidgetArea PlaybackSystem::getDockWidgetArea() const
        {
            return Qt::DockWidgetArea::BottomDockWidgetArea;
        }

        bool PlaybackSystem::isDockWidgetVisible() const
        {
            return true;
        }

        void PlaybackSystem::setCurrentProject(const QPointer<Project> & project)
        {
            _p->actions["Stop"]->setEnabled(project);
            _p->actions["Forward"]->setEnabled(project);
            _p->actions["Reverse"]->setEnabled(project);
        }

    } // namespace ViewLib
} // namespace djv

