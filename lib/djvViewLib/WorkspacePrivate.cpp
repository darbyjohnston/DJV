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

#include <djvViewLib/WorkspacePrivate.h>

#include <djvViewLib/Context.h>
#include <djvViewLib/ImageView.h>
#include <djvViewLib/Media.h>
#include <djvViewLib/WorkspaceObject.h>
#include <djvViewLib/Workspace.h>

#include <QEvent>
#include <QMdiArea>
#include <QStackedLayout>
#include <QTabBar>
#include <QVBoxLayout>

namespace djv
{
    namespace ViewLib
    {
        struct WorkspaceMDISubWindow::Private
        {
            std::weak_ptr<Context> context;
            QPointer<Workspace> workspace;
            std::shared_ptr<Media> media;
            QPointer<ImageView> imageView;
            std::shared_ptr<Core::ValueObserver<std::shared_ptr<AV::Image> > > imageObserver;
        };

        WorkspaceMDISubWindow::WorkspaceMDISubWindow(const QPointer<Workspace> & workspace, const std::shared_ptr<Media> & media, const std::shared_ptr<Context> & context) :
            _p(new Private)
        {
            _p->context = context;
            _p->workspace = workspace;
            _p->media = media;

            setWindowTitle(QString::fromStdString(media->getFileName()));
            setAttribute(Qt::WA_DeleteOnClose);

            _p->imageView = new ImageView(context);
            setWidget(_p->imageView);

            _p->imageObserver = Core::ValueObserver<std::shared_ptr<AV::Image> >::create(
                media->getCurrentImage(),
                [this](const std::shared_ptr<AV::Image> & image)
            {
                _p->imageView->setImage(image);
            });
        }

        WorkspaceMDISubWindow::~WorkspaceMDISubWindow()
        {}
        
        QSize WorkspaceMDISubWindow::sizeHint() const
        {
            return QSize(300, 200);
        }

        struct WorkspaceMDI::Private
        {
            std::weak_ptr<Context> context;
            QPointer<Workspace> workspace;
            QPointer<QMdiArea> mdiArea;
            std::map<std::shared_ptr<Media>, QPointer<QMdiSubWindow> > mediaToWindow;
            std::map<QPointer<QMdiSubWindow>, std::shared_ptr<Media> > windowToMedia;
        };

        WorkspaceMDI::WorkspaceMDI(const QPointer<Workspace> & workspace, const std::shared_ptr<Context> & context, QWidget * parent) :
            QWidget(parent),
            _p(new Private)
        {
            _p->context = context;
            _p->workspace = workspace;

            _p->mdiArea = new QMdiArea;
            _p->mdiArea->setViewMode(workspace->getViewMode());

            auto layout = new QVBoxLayout(this);
            layout->addWidget(_p->mdiArea);

            for (auto media : workspace->getMedia())
            {
                _addMedia(media);
            }

            connect(
                workspace,
                &Workspace::mediaAdded,
                [this](const std::shared_ptr<Media> & media)
            {
                _addMedia(media);
            });
            connect(
                workspace,
                &Workspace::mediaRemoved,
                [this](const std::shared_ptr<Media> & media)
            {
                _removeMedia(media);
            });
            connect(
                workspace,
                &Workspace::currentMediaChanged,
                [this](const std::shared_ptr<Media> & media)
            {
                auto i = _p->mediaToWindow.find(media);
                if (i != _p->mediaToWindow.end())
                {
                    _p->mdiArea->setActiveSubWindow(i->second);
                }
            });
            connect(
                workspace,
                &Workspace::viewModeChanged,
                [this](QMdiArea::ViewMode value)
            {
                _p->mdiArea->setViewMode(value);
            });
            connect(
                workspace,
                &Workspace::windowStateChanged,
                [this](Enum::WindowState value)
            {
                if (auto window = _p->mdiArea->activeSubWindow())
                {
                    switch (value)
                    {
                    case Enum::WindowState::Normal: window->showNormal(); break;
                    case Enum::WindowState::Maximized: window->showMaximized(); break;
                    case Enum::WindowState::Minimized: window->showMinimized(); break;
                    default: break;
                    }
                }
            });

            connect(
                _p->mdiArea,
                &QMdiArea::subWindowActivated,
                [this, workspace](QMdiSubWindow * window)
            {
                auto i = _p->windowToMedia.find(window);
                if (i != _p->windowToMedia.end())
                {
                    workspace->setCurrentMedia(i->second);
                }
            });
        }

        WorkspaceMDI::~WorkspaceMDI()
        {}

        bool WorkspaceMDI::eventFilter(QObject * object, QEvent * event)
        {
            switch (event->type())
            {
            case QEvent::Close:
            {
                auto i = _p->windowToMedia.find(qobject_cast<WorkspaceMDISubWindow *>(object));
                if (i != _p->windowToMedia.end())
                {
                    _p->workspace->closeMedia(i->second);
                }
                break;
            }
            case QEvent::WindowStateChange:
            {
                auto i = _p->windowToMedia.find(qobject_cast<WorkspaceMDISubWindow *>(object));
                if (i != _p->windowToMedia.end())
                {
                    Enum::WindowState windowState = Enum::WindowState::Normal;
                    if (i->first->windowState() & Qt::WindowMaximized)
                    {
                        windowState = Enum::WindowState::Maximized;
                    }
                    else if (i->first->windowState() & Qt::WindowMinimized)
                    {
                        windowState = Enum::WindowState::Minimized;
                    }
                    _p->workspace->setWindowState(i->second, windowState);
                }
                break;
            }
            default:
                break;
            }
            return QWidget::eventFilter(object, event);
        }

        void WorkspaceMDI::_addMedia(const std::shared_ptr<Media> & media)
        {
            if (auto context = _p->context.lock())
            {
                auto window = new WorkspaceMDISubWindow(_p->workspace, media, context);
                _p->mediaToWindow[media] = window;
                _p->windowToMedia[window] = media;
                window->installEventFilter(this);
                _p->mdiArea->addSubWindow(window);
                window->show();
            }
        }

        void WorkspaceMDI::_removeMedia(const std::shared_ptr<Media> & media)
        {
            auto i = _p->mediaToWindow.find(media);
            if (i != _p->mediaToWindow.end())
            {
                auto window = i->second;
                _p->mediaToWindow.erase(i);
                auto j = _p->windowToMedia.find(window);
                if (j != _p->windowToMedia.end())
                {
                    _p->windowToMedia.erase(j);
                }
                window->close();
            }
        }

        struct WorkspaceTabs::Private
        {
            std::weak_ptr<Context> context;
            QPointer<QTabBar> tabBar;
            QPointer<QStackedLayout> stackedLayout;
            std::map<QPointer<Workspace>, QPointer<WorkspaceMDI> > workspaceToMDI;
            std::map<QPointer<WorkspaceMDI >, QPointer<Workspace> > mdiToWorkspace;
            std::map<int, QPointer<WorkspaceMDI> > tabToMDI;
            std::map<QPointer<WorkspaceMDI>, int> mdiToTab;
        };

        WorkspaceTabs::WorkspaceTabs(const std::shared_ptr<Context> & context, QWidget * parent) :
            QWidget(parent),
            _p(new Private)
        {
            _p->context = context;

            _p->tabBar = new QTabBar;

            _p->stackedLayout = new QStackedLayout;
            _p->stackedLayout->setMargin(0);

            auto workspaceObject = context->getObjectT<WorkspaceObject>();
            for (auto i : workspaceObject->getWorkspaces())
            {
                _addWorkspace(i);
            }

            auto layout = new QVBoxLayout(this);
            layout->setMargin(0);
            layout->setSpacing(0);
            layout->addWidget(_p->tabBar);
            layout->addLayout(_p->stackedLayout, 1);

            connect(
                workspaceObject,
                &WorkspaceObject::workspaceAdded,
                [this](const QPointer<Workspace> & workspace)
            {
                _addWorkspace(workspace);
            });
            connect(
                workspaceObject,
                &WorkspaceObject::workspaceRemoved,
                [this](const QPointer<Workspace> & workspace)
            {
                _removeWorkspace(workspace);
            });
            connect(
                workspaceObject,
                &WorkspaceObject::currentWorkspaceChanged,
                [this](const QPointer<Workspace> & workspace)
            {
                const auto i = _p->workspaceToMDI.find(workspace);
                if (i != _p->workspaceToMDI.end())
                {
                    auto j = _p->mdiToTab.find(i->second);
                    if (j != _p->mdiToTab.end())
                    {
                        _p->tabBar->setCurrentIndex(j->second);
                    }
                }
            });

            connect(
                _p->tabBar,
                &QTabBar::currentChanged,
                [this, workspaceObject](int index)
            {
                const auto i = _p->tabToMDI.find(index);
                if (i != _p->tabToMDI.end())
                {
                    _p->stackedLayout->setCurrentWidget(i->second);
                    const auto j = _p->mdiToWorkspace.find(i->second);
                    if (j != _p->mdiToWorkspace.end())
                    {
                        workspaceObject->setCurrentWorkspace(j->second);
                    }
                }
            });
        }

        WorkspaceTabs::~WorkspaceTabs()
        {}

        void WorkspaceTabs::_addWorkspace(const QPointer<Workspace> & workspace)
        {
            if (auto context = _p->context.lock())
            {
                auto mdi = new WorkspaceMDI(workspace, context);
                _p->workspaceToMDI[workspace] = mdi;
                _p->mdiToWorkspace[mdi] = workspace;
                const int index = _p->tabBar->addTab(QString::fromStdString(workspace->getName()));
                _p->tabToMDI[index] = mdi;
                _p->mdiToTab[mdi] = index;
                _p->stackedLayout->addWidget(mdi);
                //! \todo Why do we need to set the margin here?
                mdi->layout()->setMargin(0);
            }
        }

        void WorkspaceTabs::_removeWorkspace(const QPointer<Workspace> & workspace)
        {
            auto i = _p->workspaceToMDI.find(workspace);
            if (i != _p->workspaceToMDI.end())
            {
                auto mdi = i->second;
                _p->workspaceToMDI.erase(i);
                auto j = _p->mdiToWorkspace.find(mdi);
                if (j != _p->mdiToWorkspace.end())
                {
                    _p->mdiToWorkspace.erase(j);
                }
                auto k = _p->mdiToTab.find(mdi);
                if (k != _p->mdiToTab.end())
                {
                    const auto l = _p->tabToMDI.find(k->second);
                    if (l != _p->tabToMDI.end())
                    {
                        _p->tabToMDI.erase(l);
                    }
                    _p->tabBar->removeTab(k->second);
                    _p->mdiToTab.erase(k);
                }
                delete mdi;
            }
        }

    } // namespace ViewLib
} // namespace djv

