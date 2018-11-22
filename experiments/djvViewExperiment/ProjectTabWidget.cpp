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

#include <ProjectTabWidget.h>

#include <Context.h>
#include <ImageView.h>
#include <Project.h>

#include <QTabWidget>
#include <QVBoxLayout>

#include <iostream>
#include <map>

namespace djv
{
    namespace ViewExperiment
    {
        struct ProjectTabWidget::Private
        {
            QPointer<Context> context;
            QPointer<QTabWidget> tabWidget;
            std::map<QPointer<Project>, QPointer<ImageView> > projectToWidget;
            std::map<QPointer<ImageView>, QPointer<Project> > widgetToProject;
        };

        ProjectTabWidget::ProjectTabWidget(const QPointer<Context> & context, QWidget * parent) :
            QWidget(parent),
            _p(new Private)
        {
            _p->context = context;
            
            _p->tabWidget = new QTabWidget;
            
            auto layout = new QVBoxLayout(this);
            layout->addWidget(_p->tabWidget);
            
            auto projectSystem = context->getSystem<ProjectSystem>().data();
            for (auto project : projectSystem->getProjects())
            {
                _addTab(project);
            }

            connect(
                projectSystem,
                &ProjectSystem::projectAdded,
                [this, context](const QPointer<Project> & project)
            {
                _addTab(project);
            });
            connect(
                projectSystem,
                &ProjectSystem::projectRemoved,
                [this, context](const QPointer<Project> & project)
            {
                _removeTab(project);
            });
            connect(
                projectSystem,
                &ProjectSystem::currentProjectChanged,
                [this, context](const QPointer<Project> & project)
            {
                auto i = _p->projectToWidget.find(project);
                if (i != _p->projectToWidget.end())
                {
                    _p->tabWidget->setCurrentIndex(_p->tabWidget->indexOf(i->second));
                }
            });

            connect(
                _p->tabWidget,
                &QTabWidget::currentChanged,
                [this, context](int index)
            {
                context->getSystem<ProjectSystem>()->setCurrentProject(index);
            });
            connect(
                _p->tabWidget,
                &QTabWidget::tabCloseRequested,
                [this, context](int index)
            {
                context->getSystem<ProjectSystem>()->closeProject(index);
            });
        }
        
        ProjectTabWidget::~ProjectTabWidget()
        {}

        void ProjectTabWidget::_addTab(const QPointer<Project> & project)
        {
            auto imageView = new ImageView(_p->context);
            _p->tabWidget->addTab(imageView, project->getFileInfo().fileName());
            _p->tabWidget->setTabsClosable(_p->tabWidget->count() > 1);
            _p->projectToWidget[project] = imageView;
            _p->widgetToProject[imageView] = project;
        }

        void ProjectTabWidget::_removeTab(const QPointer<Project> & project)
        {
            auto i = _p->projectToWidget.find(project);
            if (i != _p->projectToWidget.end())
            {
                auto imageView = i->second;
                _p->projectToWidget.erase(i);
                _p->tabWidget->removeTab(_p->tabWidget->indexOf(imageView));
                _p->tabWidget->setTabsClosable(_p->tabWidget->count() > 1);
                auto j = _p->widgetToProject.find(imageView);
                if (j != _p->widgetToProject.end())
                {
                    _p->widgetToProject.erase(j);
                }
                delete imageView;
            }
        }

    } // namespace ViewExperiment
} // namespace djv

