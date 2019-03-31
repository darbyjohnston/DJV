//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvViewLib/AnnotationsGroup.h>
#include <djvViewLib/ControlsWindow.h>
#include <djvViewLib/FileCache.h>
#include <djvViewLib/FileExport.h>
#include <djvViewLib/FileGroup.h>
#include <djvViewLib/HelpGroup.h>
#include <djvViewLib/HudInfo.h>
#include <djvViewLib/ImageGroup.h>
#include <djvViewLib/ImagePrefs.h>
#include <djvViewLib/ImageView.h>
#include <djvViewLib/PlaybackGroup.h>
#include <djvViewLib/PlaybackPrefs.h>
#include <djvViewLib/Session.h>
#include <djvViewLib/StatusBar.h>
#include <djvViewLib/ToolGroup.h>
#include <djvViewLib/ViewContext.h>
#include <djvViewLib/ViewGroup.h>
#include <djvViewLib/ViewPrefs.h>
#include <djvViewLib/ViewPrefsWidget.h>
#include <djvViewLib/WindowGroup.h>
#include <djvViewLib/WindowPrefs.h>

#include <djvCore/Debug.h>
#include <djvCore/DebugLog.h>
#include <djvCore/Error.h>

#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QMenu>
#include <QMenuBar>
#include <QPointer>
#include <QScopedPointer>
#include <QTimer>
#include <QToolBar>

#if defined(DJV_WINDOWS)
#include <QWindowsWindowFunctions>
#endif // DJV_WINDOWS

namespace djv
{
    namespace ViewLib
    {
        struct MainWindow::Private
        {
            Private(const QPointer<ViewContext> & context, const QPointer<Session> & session) :
                context(context),
                session(session)
            {}

            Enum::MOUSE_WHEEL_ACTION mouseWheelAction = Enum::MOUSE_WHEEL_ACTION_NONE;
            QMap<Enum::UI_COMPONENT, QPointer<QToolBar> > toolBars;
            QPointer<QToolBar> playbackControls;
            qint64 playbackFrameTmp = 0;
            float playbackSpeedTmp = 0.f;
            int menuBarHeight = 0;
            QScopedPointer<ControlsWindow> controlsWindow;
            QPointer<Session> session;
            QPointer<ViewContext> context;
        };

        MainWindow::MainWindow(
            const QPointer<Session> & copy,
            const QPointer<Session> & session,
            const QPointer<ViewContext> & context) :
            _p(new Private(context, session))
        {
            //DJV_DEBUG("MainWindow::MainWindow");

            // Create the menus.
            menuBar()->setNativeMenuBar(false);
            menuBar()->addMenu(session->fileGroup()->createMenu());
            menuBar()->addMenu(session->windowGroup()->createMenu());
            menuBar()->addMenu(session->viewGroup()->createMenu());
            menuBar()->addMenu(session->imageGroup()->createMenu());
            menuBar()->addMenu(session->playbackGroup()->createMenu());
            menuBar()->addMenu(session->annotationsGroup()->createMenu());
            menuBar()->addMenu(session->toolGroup()->createMenu());
            menuBar()->addMenu(session->helpGroup()->createMenu());

            // Create the tool bars.
            _p->toolBars[Enum::UI_FILE_TOOL_BAR] = session->fileGroup()->createToolBar();
            _p->toolBars[Enum::UI_WINDOW_TOOL_BAR] = session->windowGroup()->createToolBar();
            _p->toolBars[Enum::UI_VIEW_TOOL_BAR] = session->viewGroup()->createToolBar();
            _p->toolBars[Enum::UI_IMAGE_TOOL_BAR] = session->imageGroup()->createToolBar();
            _p->toolBars[Enum::UI_ANNOTATIONS_TOOL_BAR] = session->annotationsGroup()->createToolBar();
            _p->toolBars[Enum::UI_TOOLS_TOOL_BAR] = session->toolGroup()->createToolBar();
            Q_FOREACH(auto toolBar, _p->toolBars)
            {
                addToolBar(toolBar);
            }
            _p->playbackControls = session->playbackGroup()->createToolBar();
            addToolBar(Qt::ToolBarArea::BottomToolBarArea, _p->playbackControls);

            // Create the status bar.
            setStatusBar(new StatusBar(session, context));

            // Layout the widgets.
            auto viewWidget = session->viewWidget();
            setCentralWidget(viewWidget);

            // Initialize.
            setWindowTitle(qApp->applicationName());
            setAttribute(Qt::WA_DeleteOnClose);
            if (copy)
            {
                viewWidget->setViewPos(copy->viewWidget()->viewPos());
                viewWidget->setViewZoom(copy->viewWidget()->viewZoom());
                resize(copy->mainWindow()->size());
            }
            fileUpdate();
            imageUpdate();
            windowUpdate();
            viewHUDUpdate();

            // Setup the session callbacks.
            connect(
                session.data(),
                SIGNAL(fileChanged(bool)),
                SLOT(fileCallback(bool)));
            connect(
                session.data(),
                SIGNAL(imageChanged(const std::shared_ptr<djv::AV::Image> &)),
                SLOT(imageUpdate()));

            // Setup theimage group callbacks.
            connect(
                session->imageGroup(),
                SIGNAL(resizeNeeded()),
                SLOT(windowResizeCallback()));

            // Setup the window group callbacks.
            connect(
                session->windowGroup(),
                SIGNAL(uiVisibleChanged(bool)),
                SLOT(windowUpdate()));
            connect(
                session->windowGroup(),
                SIGNAL(uiComponentVisibleChanged(const QMap<djv::ViewLib::Enum::UI_COMPONENT, bool> &)),
                SLOT(windowUpdate()));
            connect(
                session->windowGroup(),
                SIGNAL(controlsWindowChanged(bool)),
                SLOT(windowUpdate()));

            // Setup the playback group callbacks.
            connect(
                session->playbackGroup(),
                SIGNAL(sequenceChanged(const djv::Core::Sequence &)),
                SLOT(viewHUDUpdate()));
            connect(
                session->playbackGroup(),
                SIGNAL(frameChanged(qint64)),
                SLOT(viewHUDUpdate()));
            connect(
                session->playbackGroup(),
                SIGNAL(speedChanged(const djv::Core::Speed &)),
                SLOT(viewHUDUpdate()));
            connect(
                session->playbackGroup(),
                SIGNAL(actualSpeedChanged(float)),
                SLOT(viewHUDUpdate()));
            connect(
                session->playbackGroup(),
                SIGNAL(droppedFramesChanged(bool)),
                SLOT(viewHUDUpdate()));
            connect(
                session->playbackGroup(),
                SIGNAL(layoutChanged(djv::ViewLib::Enum::LAYOUT)),
                SLOT(windowResizeCallback()));

            // Setup the view callbacks.
            connect(
                viewWidget,
                &ImageView::contextMenuRequested,
                [this](const QPoint & value)
            {
                auto menu = createPopupMenu();
                menu->popup(value);
            });
            connect(
                viewWidget,
                SIGNAL(mouseWheelActionChanged(djv::ViewLib::Enum::MOUSE_WHEEL_ACTION)),
                SLOT(mouseWheelActionCallback(djv::ViewLib::Enum::MOUSE_WHEEL_ACTION)));
            connect(
                viewWidget,
                SIGNAL(mouseWheelValueChanged(int)),
                SLOT(mouseWheelValueCallback(int)));
        }

        MainWindow::~MainWindow()
        {
            //DJV_DEBUG("MainWindow::~MainWindow");
        }

        QMenu * MainWindow::createPopupMenu()
        {
            auto out = new QMenu;
            out->addMenu(_p->session->fileGroup()->createMenu());
            out->addMenu(_p->session->windowGroup()->createMenu());
            out->addMenu(_p->session->viewGroup()->createMenu());
            out->addMenu(_p->session->imageGroup()->createMenu());
            out->addMenu(_p->session->playbackGroup()->createMenu());
            out->addMenu(_p->session->annotationsGroup()->createMenu());
            out->addMenu(_p->session->toolGroup()->createMenu());
            out->addMenu(_p->session->helpGroup()->createMenu());
            return out;
        }

        void MainWindow::fileCallback(bool init)
        {
            auto viewWidget = _p->session->viewWidget();
            viewWidget->setData(nullptr);
            fileUpdate();
            imageUpdate();
            if (init && isVisible())
            {
                if (_p->context->windowPrefs()->hasAutoFit())
                {
                    if (!isFullScreen())
                    {
                        viewWidget->viewZero();
                        fitWindow();
                    }
                    else
                    {
                        viewWidget->viewFit();
                    }
                }
                else
                {
                    viewWidget->viewFit();
                }
            }
        }

        void MainWindow::fitWindow(bool move)
        {
            if (isFullScreen())
                return;
            //DJV_DEBUG("MainWindow::fitWindow");
            const glm::ivec2 frame(frameGeometry().width(), frameGeometry().height());
            //DJV_DEBUG_PRINT("frame = " << frame);
            auto viewWidget = _p->session->viewWidget();
            //DJV_DEBUG_PRINT("view size = " << Core::VectorUtil::fromQSize(viewWidget->size()));
            viewWidget->updateGeometry();
            resize(sizeHint());
            //DJV_DEBUG_PRINT("new view size = " << Core::VectorUtil::fromQSize(viewWidget->size()));
            if (move && isVisible())
            {
                //DJV_DEBUG_PRINT("move");
                this->move(
                    x() - (frameGeometry().width() / 2 - frame.x / 2),
                    y() - (frameGeometry().height() / 2 - frame.y / 2));
            }
            viewWidget->viewFit();
        }

        void MainWindow::showEvent(QShowEvent * event)
        {
            //DJV_DEBUG("MainWindow::showEvent");    
            QMainWindow::showEvent(event);

#if defined(DJV_WINDOWS)
            //! \bug https://doc.qt.io/qt-5/windows-issues.html#fullscreen-opengl-based-windows
            QWindowsWindowFunctions::setHasBorderInFullScreen(windowHandle(), true);
#endif // DJV_WINDOWS

            const QSize& sizeHint = this->sizeHint();
            const glm::ivec2 size(sizeHint.width(), sizeHint.height());
            //DJV_DEBUG_PRINT("size = " << size);
            resize(size.x, size.y);
            auto viewWidget = _p->session->viewWidget();
            viewWidget->viewFit();
            const Core::Box2i screen = Core::BoxUtil::fromQt(QApplication::desktop()->availableGeometry());
            //DJV_DEBUG_PRINT("screen = " << screen);
            move(
                screen.x + screen.w / 2 - size.x / 2,
                screen.y + screen.h / 2 - size.y / 2);
        }

        void MainWindow::closeEvent(QCloseEvent * event)
        {
            QMainWindow::closeEvent(event);
            _p->session->close();
        }

        void MainWindow::keyPressEvent(QKeyEvent * event)
        {
            QMainWindow::keyPressEvent(event);
            switch (event->key())
            {
            case Qt::Key_Escape:
            {
                auto windowGroup = _p->session->windowGroup();
                if (!windowGroup->isUIVisible() && !windowGroup->hasControlsWindow())
                {
                    windowGroup->setUIVisible(true);
                }
                else if (windowGroup->hasFullScreen())
                {
                    windowGroup->setFullScreen(false);
                    auto viewWidget = _p->session->viewWidget();
                    viewWidget->viewFit();
                }
                break;
            }
            }
        }

        void MainWindow::windowResizeCallback()
        {
            //DJV_DEBUG("MainWindow::windowResizeCallback");
            //DJV_DEBUG_PRINT("size = " << width() << " " << height());

            // Temporarily disable updates to try and minimize flickering.
            setUpdatesEnabled(false);
            if (_p->context->windowPrefs()->hasAutoFit())
            {
                if (!isFullScreen())
                {
                    //DJV_DEBUG_PRINT("fit window");
                    fitWindow();
                }
                else
                {
                    //DJV_DEBUG_PRINT("fit view");
                    auto viewWidget = _p->session->viewWidget();
                    viewWidget->viewFit();
                }
            }

            QTimer::singleShot(0, this, SLOT(enableUpdatesCallback()));
        }

        void MainWindow::enableUpdatesCallback()
        {
            setUpdatesEnabled(true);
        }

        void MainWindow::mouseWheelActionCallback(Enum::MOUSE_WHEEL_ACTION in)
        {
            _p->mouseWheelAction = in;
            auto playbackGroup = _p->session->playbackGroup();
            switch (_p->mouseWheelAction)
            {
            case Enum::MOUSE_WHEEL_ACTION_PLAYBACK_SHUTTLE:
                playbackGroup->setPlayback(Enum::STOP);
                _p->playbackFrameTmp = playbackGroup->frame();
                break;
            case Enum::MOUSE_WHEEL_ACTION_PLAYBACK_SPEED:
                _p->playbackSpeedTmp = Core::Speed::speedToFloat(playbackGroup->speed());
                break;
            default: break;
            }
        }

        void MainWindow::mouseWheelValueCallback(int in)
        {
            auto playbackGroup = _p->session->playbackGroup();
            switch (_p->mouseWheelAction)
            {
            case Enum::MOUSE_WHEEL_ACTION_PLAYBACK_SHUTTLE:
                playbackGroup->setFrame(_p->playbackFrameTmp + in);
                break;
            case Enum::MOUSE_WHEEL_ACTION_PLAYBACK_SPEED:
                playbackGroup->setSpeed(Core::Speed::floatToSpeed(_p->playbackSpeedTmp + static_cast<float>(in)));
                break;
            default: break;
            }
        }
        
        void MainWindow::controlsWindowClosedCallback()
        {
            _p->session->windowGroup()->setControlsWindow(false);
        }

        void MainWindow::fileUpdate()
        {
            const Core::FileInfo & fileInfo = _p->session->fileGroup()->fileInfo();
            const QString title = !fileInfo.fileName().isEmpty() ?
                qApp->translate("djv::ViewLib::MainWindow", "%1 - %2").
                arg(qApp->applicationName()).
                arg(QDir::toNativeSeparators(fileInfo)) :
                QString("%1").arg(qApp->applicationName());
            setWindowTitle(title);
        }

        void MainWindow::imageUpdate()
        {
            //DJV_DEBUG("MainWindow::imageUpdate");

            auto viewWidget = _p->session->viewWidget();
            viewWidget->setOptions(_p->session->imageOptions());
            viewWidget->setData(_p->session->image());
            viewHUDUpdate();
            viewWidget->update();
        }

        void MainWindow::windowUpdate()
        {
            //DJV_DEBUG("MainWindow::windowUpdate");

            // Temporarily disable updates to try and minimize flickering.
            setUpdatesEnabled(false);

            auto windowGroup = _p->session->windowGroup();
            const bool detach = windowGroup->hasControlsWindow();
            const bool visible = windowGroup->isUIVisible() && !detach;
            if (visible)
            {
                if (_p->menuBarHeight)
                {
                    menuBar()->setFixedHeight(_p->menuBarHeight);
                }
            }
            else
            {
                const int height = menuBar()->height();
                if (height)
                {
                    _p->menuBarHeight = height;
                    //DJV_DEBUG_PRINT("menu bar height = " << _p->menuBarHeight);
                    menuBar()->setFixedHeight(0);
                }
            }
            const auto & componentVisible = windowGroup->uiComponentVisible();
            Q_FOREACH(auto key, _p->toolBars.keys())
            {
                _p->toolBars[key]->setVisible(visible && componentVisible[key]);
            }
            _p->playbackControls->setVisible(visible && componentVisible[Enum::UI_PLAYBACK_CONTROLS]);
            statusBar()->setVisible(visible && componentVisible[Enum::UI_STATUS_BAR]);

            if (detach)
            {
                if (!_p->controlsWindow)
                {
                    auto controlsWindow = new ControlsWindow(_p->session, _p->context, this);
                    connect(
                        controlsWindow,
                        SIGNAL(closed()),
                        SLOT(controlsWindowClosedCallback()));
                    _p->controlsWindow.reset(controlsWindow);
                }
                _p->controlsWindow->show();
                _p->controlsWindow->raise();
                _p->controlsWindow->activateWindow();
            }
            else if (_p->controlsWindow)
            {
                _p->controlsWindow->hide();
            }

            QTimer::singleShot(0, this, SLOT(enableUpdatesCallback()));
        }

        void MainWindow::viewHUDUpdate()
        {
            //DJV_DEBUG("MainWindow::viewHUDUpdate");

            HudInfo hudInfo;
            if (auto image = _p->session->image())
            {
                hudInfo.info = image->info();
                hudInfo.tags = image->tags;
            }
            auto playbackGroup = _p->session->playbackGroup();
            const Core::Sequence & sequence = playbackGroup->sequence();
            hudInfo.frame = 0;
            const qint64 frame = playbackGroup->frame();
            const auto & frames = sequence.frames;
            if (frames.count() &&
                frame >= 0 &&
                frame < static_cast<int64_t>(frames.count()))
            {
                hudInfo.frame = frames[frame];
            }
            hudInfo.speed = playbackGroup->speed();
            hudInfo.actualSpeed = playbackGroup->actualSpeed();
            hudInfo.droppedFrames = playbackGroup->hasDroppedFrames();
            hudInfo.visible = _p->context->viewPrefs()->hudInfo();
            auto viewWidget = _p->session->viewWidget();
            viewWidget->setHudInfo(hudInfo);
        }

    } // namespace ViewLib
} // namespace djv
