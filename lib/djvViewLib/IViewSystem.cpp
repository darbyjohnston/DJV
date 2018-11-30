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

#include <djvViewLib/IViewSystem.h>

#include <djvViewLib/Context.h>

#include <QDockWidget>
#include <QMenu>

namespace djv
{
    namespace ViewLib
    {
        struct IViewSystem::Private
        {
            std::weak_ptr<Context> context;
            std::string name;
        };
        
        IViewSystem::IViewSystem(const std::string & name, const std::shared_ptr<Context> & context, QObject * parent) :
            QObject(parent),
            _p(new Private)
        {
            _p->context = context;
            _p->name = name;
        }
        
        IViewSystem::~IViewSystem()
        {}

        const std::weak_ptr<Context> & IViewSystem::getContext() const
        {
            return _p->context;
        }

        const std::string & IViewSystem::getName() const
        {
            return _p->name;
        }

        QPointer<QMenu> IViewSystem::createMenu()
        {
            return nullptr;
        }

        std::string IViewSystem::getMenuSortKey() const
        {
            return _p->name;
        }

        QPointer<QDockWidget> IViewSystem::createDockWidget()
        {
            return nullptr;
        }

        std::string IViewSystem::getDockWidgetSortKey() const
        {
            return _p->name;
        }

        Qt::DockWidgetArea IViewSystem::getDockWidgetArea() const
        {
            return Qt::NoDockWidgetArea;
        }

        bool IViewSystem::isDockWidgetVisible() const
        {
            return false;
        }

        void IViewSystem::setCurrentWorkspace(const QPointer<Workspace> &)
        {}

        void IViewSystem::setCurrentProject(const QPointer<Project> &)
        {}

    } // namespace ViewLib
} // namespace djv

