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

#include <djvViewLib/MainWindow.h>

#include <djvViewLib/ControlsWindow.h>
#include <djvViewLib/FileCache.h>
#include <djvViewLib/FileExport.h>
#include <djvViewLib/FileGroup.h>
#include <djvViewLib/FilePrefs.h>
#include <djvViewLib/HelpGroup.h>
#include <djvViewLib/HudInfo.h>
#include <djvViewLib/ImageGroup.h>
#include <djvViewLib/ImagePrefs.h>
#include <djvViewLib/ImageView.h>
#include <djvViewLib/PlaybackGroup.h>
#include <djvViewLib/PlaybackPrefs.h>
#include <djvViewLib/StatusBar.h>
#include <djvViewLib/ToolGroup.h>
#include <djvViewLib/ViewContext.h>
#include <djvViewLib/ViewGroup.h>
#include <djvViewLib/ViewPrefs.h>
#include <djvViewLib/ViewPrefsWidget.h>
#include <djvViewLib/WindowGroup.h>
#include <djvViewLib/WindowPrefs.h>

#include <djvUI/OpenGLPrefs.h>

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

namespace djv
{
    namespace ViewLib
    {
        struct MainWindow::Private
        {
            Private(const QPointer<ViewContext> & context) :
                context(context)
            {}

            Enum::MOUSE_WHEEL mouseWheel = static_cast<Enum::MOUSE_WHEEL>(0);
            QScopedPointer<FileGroup> fileGroup;
            QScopedPointer<WindowGroup> windowGroup;
            QScopedPointer<ViewGroup> viewGroup;
            QScopedPointer<ImageGroup> imageGroup;
            QScopedPointer<PlaybackGroup> playbackGroup;
            QMap<Enum::UI_COMPONENT, QPointer<QToolBar> > toolBars;
            QPointer<QToolBar> playbackControls;
            qint64 playbackFrameTmp = 0;
            float playbackSpeedTmp = 0.f;
            QScopedPointer<ToolGroup> toolGroup;
            QScopedPointer<HelpGroup> helpGroup;
            std::shared_ptr<Graphics::Image> image;
            std::shared_ptr<Graphics::Image> frameStoreImage;
            QPointer<ImageView> viewWidget;
            int menuBarHeight = 0;
            QScopedPointer<ControlsWindow> controlsWindow;
            QPointer<ViewContext> context;
        };

        namespace
        {
            QVector<QPointer<MainWindow> > _mainWindowList;

        } // namespace

        MainWindow::MainWindow(
            const QPointer<MainWindow> & copy,
            const QPointer<ViewContext> & context) :
            _p(new Private(context))
        {
            //DJV_DEBUG("MainWindow::MainWindow");

            _mainWindowList.append(this);

            menuBar()->setNativeMenuBar(false);

            // Create the view widget.
            _p->viewWidget = new ImageView(context);

            // Create the groups.
            _p->fileGroup.reset(new FileGroup(copy ? copy->_p->fileGroup.data() : nullptr, this, context));
            _p->windowGroup.reset(new WindowGroup(copy ? copy->_p->windowGroup.data() : nullptr, this, context));
            _p->viewGroup.reset(new ViewGroup(copy ? copy->_p->viewGroup.data() : nullptr, this, context));
            _p->imageGroup.reset(new ImageGroup(copy ? copy->_p->imageGroup.data() : nullptr, this, context));
            _p->playbackGroup.reset(new PlaybackGroup(copy ? copy->_p->playbackGroup.data() : nullptr, this, context));
            _p->toolGroup.reset(new ToolGroup(copy ? copy->_p->toolGroup.data() : nullptr, this, context));
            _p->helpGroup.reset(new HelpGroup(copy ? copy->_p->helpGroup.data() : nullptr, this, context));

            // Create the menus.
            menuBar()->addMenu(_p->fileGroup->createMenu());
            menuBar()->addMenu(_p->windowGroup->createMenu());
            menuBar()->addMenu(_p->viewGroup->createMenu());
            menuBar()->addMenu(_p->imageGroup->createMenu());
            menuBar()->addMenu(_p->playbackGroup->createMenu());
            menuBar()->addMenu(_p->toolGroup->createMenu());
            menuBar()->addMenu(_p->helpGroup->createMenu());

            // Create the tool bars.
            _p->toolBars[Enum::UI_FILE_TOOL_BAR] = _p->fileGroup->createToolBar();
            _p->toolBars[Enum::UI_WINDOW_TOOL_BAR] = _p->windowGroup->createToolBar();
            _p->toolBars[Enum::UI_VIEW_TOOL_BAR] = _p->viewGroup->createToolBar();
            _p->toolBars[Enum::UI_IMAGE_TOOL_BAR] = _p->imageGroup->createToolBar();
            _p->toolBars[Enum::UI_TOOLS_TOOL_BAR] = _p->toolGroup->createToolBar();
            Q_FOREACH(auto toolBar, _p->toolBars)
            {
                addToolBar(toolBar);
            }
            _p->playbackControls = _p->playbackGroup->createToolBar();
            addToolBar(Qt::ToolBarArea::BottomToolBarArea, _p->playbackControls);

            // Create the status bar.
            setStatusBar(new StatusBar(this, context));

            // Layout the widgets.
            setCentralWidget(_p->viewWidget);

            // Initialize.
            setWindowTitle(qApp->applicationName());
            setAttribute(Qt::WA_DeleteOnClose);
            if (copy)
            {
                _p->frameStoreImage = copy->_p->frameStoreImage;
                _p->viewWidget->setViewPos(copy->_p->viewWidget->viewPos());
                _p->viewWidget->setViewZoom(copy->_p->viewWidget->viewZoom());
                resize(copy->size());
            }
            fileUpdate();
            imageUpdate();
            windowUpdate();
            playbackUpdate();

            // Setup the file group callbacks.
            connect(
                _p->fileGroup.data(),
                SIGNAL(imageChanged()),
                SLOT(imageUpdate()));
            connect(
                _p->fileGroup.data(),
                SIGNAL(setFrameStore()),
                SLOT(setFrameStoreCallback()));
            connect(
                _p->fileGroup.data(),
                SIGNAL(reloadFrame()),
                SLOT(reloadFrameCallback()));
            connect(
                _p->fileGroup.data(),
                SIGNAL(exportSequence(const djv::Core::FileInfo &)),
                SLOT(exportSequenceCallback(const djv::Core::FileInfo &)));
            connect(
                _p->fileGroup.data(),
                SIGNAL(exportFrame(const djv::Core::FileInfo &)),
                SLOT(exportFrameCallback(const djv::Core::FileInfo &)));

            // Setup the image group callbacks.
            connect(
                _p->imageGroup.data(),
                SIGNAL(showFrameStoreChanged(bool)),
                SLOT(imageUpdate()));
            connect(
                _p->imageGroup.data(),
                SIGNAL(setFrameStore()),
                SLOT(setFrameStoreCallback()));
            connect(
                _p->imageGroup.data(),
                SIGNAL(displayProfileChanged(const djv::ViewLib::DisplayProfile &)),
                SLOT(imageUpdate()));
            connect(
                _p->imageGroup.data(),
                SIGNAL(redrawNeeded()),
                SLOT(imageUpdate()));
            connect(
                _p->imageGroup.data(),
                SIGNAL(resizeNeeded()),
                SLOT(windowResizeCallback()));

            // Setup the window group callbacks.
            connect(
                _p->windowGroup.data(),
                SIGNAL(uiVisibleChanged(bool)),
                SLOT(windowUpdate()));
            connect(
                _p->windowGroup.data(),
                SIGNAL(uiComponentVisibleChanged(const QMap<djv::ViewLib::Enum::UI_COMPONENT, bool> &)),
                SLOT(windowUpdate()));
            connect(
                _p->windowGroup.data(),
                SIGNAL(controlsWindowChanged(bool)),
                SLOT(windowUpdate()));

            // Setup the playback group callbacks.
            connect(
                _p->playbackGroup.data(),
                SIGNAL(playbackChanged(djv::ViewLib::Enum::PLAYBACK)),
                SLOT(playbackUpdate()));
            connect(
                _p->playbackGroup.data(),
                SIGNAL(frameChanged(qint64)),
                SLOT(imageUpdate()));
            connect(
                _p->playbackGroup.data(),
                SIGNAL(sequenceChanged(const djv::Core::Sequence &)),
                SLOT(viewOverlayUpdate()));
            connect(
                _p->playbackGroup.data(),
                SIGNAL(frameChanged(qint64)),
                SLOT(viewOverlayUpdate()));
            connect(
                _p->playbackGroup.data(),
                SIGNAL(speedChanged(const djv::Core::Speed &)),
                SLOT(viewOverlayUpdate()));
            connect(
                _p->playbackGroup.data(),
                SIGNAL(actualSpeedChanged(float)),
                SLOT(viewOverlayUpdate()));
            connect(
                _p->playbackGroup.data(),
                SIGNAL(droppedFramesChanged(bool)),
                SLOT(viewOverlayUpdate()));
            connect(
                _p->playbackGroup.data(),
                SIGNAL(layoutChanged(djv::ViewLib::Enum::LAYOUT)),
                SLOT(windowResizeCallback()));

            // Setup the view callbacks.
            connect(
                _p->viewWidget,
                SIGNAL(mouseWheelChanged(djv::ViewLib::Enum::MOUSE_WHEEL)),
                SLOT(mouseWheelCallback(djv::ViewLib::Enum::MOUSE_WHEEL)));
            connect(
                _p->viewWidget,
                SIGNAL(mouseWheelValueChanged(int)),
                SLOT(mouseWheelValueCallback(int)));
            connect(
                _p->viewWidget,
                SIGNAL(fileDropped(const djv::Core::FileInfo &)),
                SLOT(fileOpen(const djv::Core::FileInfo &)));

            // Setup the preferences callbacks.
            connect(
                context->UIContext::openGLPrefs(),
                SIGNAL(filterChanged(const djv::Graphics::OpenGLImageFilter &)),
                SLOT(imageUpdate()));
            connect(
                context->viewPrefs(),
                SIGNAL(backgroundChanged(const djv::Graphics::Color &)),
                SLOT(imageUpdate()));
        }

        MainWindow::~MainWindow()
        {
            //DJV_DEBUG("MainWindow::~MainWindow");
        }

        QPointer<FileGroup> MainWindow::fileGroup() const
        {
            return _p->fileGroup.data();
        }

        QPointer<WindowGroup> MainWindow::windowGroup() const
        {
            return _p->windowGroup.data();
        }

        QPointer<ViewGroup> MainWindow::viewGroup() const
        {
            return _p->viewGroup.data();
        }

        QPointer<ImageGroup> MainWindow::imageGroup() const
        {
            return _p->imageGroup.data();
        }

        QPointer<PlaybackGroup> MainWindow::playbackGroup() const
        {
            return _p->playbackGroup.data();
        }

        QPointer<ToolGroup> MainWindow::toolGroup() const
        {
            return _p->toolGroup.data();
        }

        QPointer<HelpGroup> MainWindow::helpGroup() const
        {
            return _p->helpGroup.data();
        }

        const QPointer<ImageView> & MainWindow::viewWidget() const
        {
            return _p->viewWidget;
        }

        QVector<QPointer<MainWindow> > MainWindow::mainWindowList()
        {
            return _mainWindowList;
        }

        QPointer<MainWindow> MainWindow::createWindow(const QPointer<ViewContext> & context)
        {
            auto out = QPointer<MainWindow>(new MainWindow(0, context));

            // Apply command line file options.
            if (context->commandLineOptions().fileLayer.data())
            {
                out->setFileLayer(*context->commandLineOptions().fileLayer);
            }
            if (context->commandLineOptions().fileProxy.data())
            {
                out->setFileProxy(*context->commandLineOptions().fileProxy);
            }
            if (context->commandLineOptions().fileCacheEnable.data())
            {
                out->setFileCacheEnabled(*context->commandLineOptions().fileCacheEnable);
            }

            // Apply command line window options.
            if (context->commandLineOptions().windowFullScreen.data() && *context->commandLineOptions().windowFullScreen)
            {
                out->showFullScreen();
            }

            // Apply command line playback options.
            if (context->commandLineOptions().playback.data())
            {
                out->setPlayback(*context->commandLineOptions().playback);
            }
            if (context->commandLineOptions().playbackFrame.data())
            {
                out->setPlaybackFrame(*context->commandLineOptions().playbackFrame);
            }
            if (context->commandLineOptions().playbackSpeed.data())
            {
                out->setPlaybackSpeed(*context->commandLineOptions().playbackSpeed);
            }

            return out;
        }

        QMenu * MainWindow::createPopupMenu()
        {
            auto out = new QMenu;
            out->addMenu(_p->fileGroup->createMenu());
            out->addMenu(_p->windowGroup->createMenu());
            out->addMenu(_p->viewGroup->createMenu());
            out->addMenu(_p->imageGroup->createMenu());
            out->addMenu(_p->playbackGroup->createMenu());
            out->addMenu(_p->toolGroup->createMenu());
            out->addMenu(_p->helpGroup->createMenu());
            return out;
        }

        void MainWindow::fileOpen(const Core::FileInfo & fileInfo, bool init)
        {
            //DJV_DEBUG("MainWindow::fileOpen");
            //DJV_DEBUG_PRINT("fileInfo = " << fileInfo);
            //DJV_DEBUG_PRINT("init = " << init);

            DJV_LOG(_p->context->debugLog(), "djv::ViewLib::MainWindow",
                QString("Open file = \"%1\"").arg(fileInfo));

            // Initialize.
            _p->image.reset();
            _p->viewWidget->setData(nullptr);

            // Open the file.
            {
                _p->fileGroup->open(fileInfo);

                // Set playback.
                const Core::Sequence & sequence = _p->fileGroup->imageIOInfo().sequence;
                //DJV_DEBUG_PRINT("sequence = " << sequence);
                _p->playbackGroup->setSequence(sequence);
                if (init)
                {
                    _p->playbackGroup->setFrame(0);
                    _p->playbackGroup->setPlayback(
                        (sequence.frames.count() > 1 && _p->context->playbackPrefs()->hasAutoStart()) ?
                        Enum::FORWARD :
                        Enum::STOP);
                }
            }

            // Update.
            fileUpdate();
            imageUpdate();
            if (init && isVisible())
            {
                if (_p->context->windowPrefs()->hasAutoFit())
                {
                    if (!isFullScreen())
                    {
                        _p->viewWidget->viewZero();
                        fitWindow();
                    }
                    else
                    {
                        _p->viewWidget->viewFit();
                    }
                }
                else
                {
                    _p->viewWidget->viewFit();
                }
            }
        }

        void MainWindow::setFileLayer(int in)
        {
            _p->fileGroup->setLayer(in);
        }

        void MainWindow::setFileProxy(Graphics::PixelDataInfo::PROXY in)
        {
            _p->fileGroup->setProxy(in);
        }

        void MainWindow::setFileCacheEnabled(bool in)
        {
            _p->fileGroup->setCacheEnabled(in);
        }

        void MainWindow::fitWindow(bool move)
        {
            if (isFullScreen())
                return;
            //DJV_DEBUG("MainWindow::fitWindow");
            const glm::ivec2 frame(frameGeometry().width(), frameGeometry().height());
            //DJV_DEBUG_PRINT("frame = " << frame);
            //DJV_DEBUG_PRINT("view size = " <<
            //    Core::VectorUtil::fromQSize(_p->viewWidget->size()));
            _p->viewWidget->updateGeometry();
            resize(sizeHint());
            //DJV_DEBUG_PRINT("new view size = " <<
            //    Core::VectorUtil::fromQSize(_p->viewWidget->size()));
            if (move && isVisible())
            {
                //DJV_DEBUG_PRINT("move");
                this->move(
                    x() - (frameGeometry().width() / 2 - frame.x / 2),
                    y() - (frameGeometry().height() / 2 - frame.y / 2));
            }
            _p->viewWidget->viewFit();
        }

        void MainWindow::setPlayback(Enum::PLAYBACK in)
        {
            _p->playbackGroup->setPlayback(in);
        }

        void MainWindow::setPlaybackFrame(qint64 in)
        {
            _p->playbackGroup->setFrame(in);
        }

        void MainWindow::setPlaybackSpeed(const Core::Speed & in)
        {
            _p->playbackGroup->setSpeed(in);
        }

        void MainWindow::showEvent(QShowEvent * event)
        {
            //DJV_DEBUG("MainWindow::showEvent");    
            QMainWindow::showEvent(event);
            const QSize& sizeHint = this->sizeHint();
            const glm::ivec2 size(sizeHint.width(), sizeHint.height());
            //DJV_DEBUG_PRINT("size = " << size);
            resize(size.x, size.y);
            _p->viewWidget->viewFit();
            const Core::Box2i screen = Core::BoxUtil::fromQt(QApplication::desktop()->availableGeometry());
            //DJV_DEBUG_PRINT("screen = " << screen);
            move(
                screen.x + screen.w / 2 - size.x / 2,
                screen.y + screen.h / 2 - size.y / 2);
        }

        void MainWindow::closeEvent(QCloseEvent * event)
        {
            QMainWindow::closeEvent(event);
            const int i = _mainWindowList.indexOf(this);
            if (i != -1)
            {
                _mainWindowList.remove(i);
                if (0 == _mainWindowList.count())
                {
                    Q_FOREACH(QWidget * widget, qApp->topLevelWidgets())
                    {
                        if (widget != this)
                        {
                            widget->close();
                        }
                    }
                }
            }
        }

        void MainWindow::keyPressEvent(QKeyEvent * event)
        {
            QMainWindow::keyPressEvent(event);
            switch (event->key())
            {
            case Qt::Key_Escape:
                if (!_p->windowGroup->isUIVisible() && !_p->windowGroup->hasControlsWindow())
                {
                    _p->windowGroup->setUIVisible(true);
                }
                else if (isFullScreen())
                {
                    _p->windowGroup->setFullScreen(false);
                    _p->viewWidget->viewFit();
                }
                break;
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
                    _p->viewWidget->viewFit();
                }
            }

            QTimer::singleShot(0, this, SLOT(enableUpdatesCallback()));
        }

        void MainWindow::enableUpdatesCallback()
        {
            setUpdatesEnabled(true);
        }

        void MainWindow::reloadFrameCallback()
        {
            //DJV_DEBUG("MainWindow::reloadFrameCallback");
            const qint64 frame = _p->playbackGroup->frame();
            //DJV_DEBUG_PRINT("frame = " << frame);
            _p->context->fileCache()->removeItem(FileCacheKey(this, frame));
        }

        void MainWindow::exportSequenceCallback(const Core::FileInfo & in)
        {
            //DJV_DEBUG("MainWindow::exportSequenceCallback");
            //DJV_DEBUG_PRINT("in = " << in);
            Core::Sequence sequence;
            const Core::FrameList & frames = _p->playbackGroup->sequence().frames;
            if (frames.count())
            {
                sequence.frames = frames.mid(
                    _p->playbackGroup->inPoint(),
                    _p->playbackGroup->outPoint() - _p->playbackGroup->inPoint() + 1);
            }
            sequence.speed = _p->playbackGroup->speed();
            const FileExportInfo info(
                _p->fileGroup->fileInfo(),
                in,
                _p->fileGroup->imageIOInfo()[_p->fileGroup->layer()],
                sequence,
                _p->fileGroup->layer(),
                _p->fileGroup->proxy(),
                _p->fileGroup->hasU8Conversion(),
                _p->imageGroup->hasColorProfile(),
                imageOptions());
            _p->context->fileExport()->start(info);
        }

        void MainWindow::exportFrameCallback(const Core::FileInfo & in)
        {
            //DJV_DEBUG("MainWindow::exportFrameCallback");
            //DJV_DEBUG_PRINT("in = " << in);
            Core::Sequence sequence;
            const Core::FrameList & frames = _p->playbackGroup->sequence().frames;
            if (frames.count())
            {
                sequence.frames += frames[_p->playbackGroup->frame()];
            }
            sequence.speed = _p->playbackGroup->speed();
            //DJV_DEBUG_PRINT("sequence = " << sequence);
            const FileExportInfo info(
                _p->fileGroup->fileInfo(),
                in,
                _p->fileGroup->imageIOInfo()[_p->fileGroup->layer()],
                sequence,
                _p->fileGroup->layer(),
                _p->fileGroup->proxy(),
                _p->fileGroup->hasU8Conversion(),
                _p->imageGroup->hasColorProfile(),
                imageOptions());
            _p->context->fileExport()->start(info);
        }

        void MainWindow::setFrameStoreCallback()
        {
            //DJV_DEBUG("MainWindow::setFrameStoreCallback");
            if (_p->image)
            {
                _p->frameStoreImage = _p->image;
            }
        }

        void MainWindow::mouseWheelCallback(Enum::MOUSE_WHEEL in)
        {
            _p->mouseWheel = in;
            switch (_p->mouseWheel)
            {
            case Enum::MOUSE_WHEEL_PLAYBACK_SHUTTLE:
                _p->playbackGroup->setPlayback(Enum::STOP);
                _p->playbackFrameTmp = _p->playbackGroup->frame();
                break;
            case Enum::MOUSE_WHEEL_PLAYBACK_SPEED:
                _p->playbackSpeedTmp = Core::Speed::speedToFloat(_p->playbackGroup->speed());
                break;
            default: break;
            }
        }

        void MainWindow::mouseWheelValueCallback(int in)
        {
            switch (_p->mouseWheel)
            {
            case Enum::MOUSE_WHEEL_PLAYBACK_SHUTTLE:
                _p->playbackGroup->setFrame(_p->playbackFrameTmp + in);
                break;
            case Enum::MOUSE_WHEEL_PLAYBACK_SPEED:
                _p->playbackGroup->setSpeed(Core::Speed::floatToSpeed(_p->playbackSpeedTmp + static_cast<float>(in)));
                break;
            default: break;
            }
        }
        
        void MainWindow::controlsWindowClosedCallback()
        {
            _p->windowGroup->setControlsWindow(false);
        }

        void MainWindow::fileUpdate()
        {
            // Update the window title.
            const Core::FileInfo & fileInfo = _p->fileGroup->fileInfo();
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

            // Update the image.
            const qint64 frame = _p->playbackGroup->frame();
            _p->image = _p->fileGroup->image(frame);
            if (_p->image)
            {
                //DJV_DEBUG_PRINT("image = " << *_p->image);
            }

            // Update the view.
            _p->viewWidget->setOptions(imageOptions());
            _p->viewWidget->setData(_p->image ? &*_p->image : nullptr);
            viewOverlayUpdate();
            _p->viewWidget->update();

            //! Update the file group.
            _p->fileGroup->setPreloadFrame(frame);

            Q_EMIT imageChanged(image());
            Q_EMIT imageOptionsChanged(imageOptions());
        }

        void MainWindow::windowUpdate()
        {
            //DJV_DEBUG("MainWindow::windowUpdate");

            // Temporarily disable updates to try and minimize flickering.
            setUpdatesEnabled(false);

            const bool detach = _p->windowGroup->hasControlsWindow();
            const bool visible = _p->windowGroup->isUIVisible() && !detach;
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
            const auto & componentVisible = _p->windowGroup->uiComponentVisible();
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
                    auto controlsWindow = new ControlsWindow(this, _p->context, this);
                    connect(
                        controlsWindow,
                        SIGNAL(closed()),
                        SLOT(controlsWindowClosedCallback()));
                    _p->controlsWindow.reset(controlsWindow);
                }
                _p->controlsWindow->show();
                _p->controlsWindow->raise();
            }
            else if (_p->controlsWindow)
            {
                _p->controlsWindow->hide();
            }

            QTimer::singleShot(0, this, SLOT(enableUpdatesCallback()));
        }

        void MainWindow::viewOverlayUpdate()
        {
            //DJV_DEBUG("MainWindow::viewOverlayUpdate");

            //! Update the HUD.
            HudInfo hudInfo;
            auto image = this->image();
            if (image)
            {
                hudInfo.info = image->info();
                hudInfo.tags = image->tags;
            }
            const Core::Sequence & sequence = _p->playbackGroup->sequence();
            hudInfo.frame = 0;
            if (sequence.frames.count() &&
                _p->playbackGroup->frame() <
                static_cast<int64_t>(sequence.frames.count()))
            {
                hudInfo.frame = sequence.frames[_p->playbackGroup->frame()];
            }
            hudInfo.speed = _p->playbackGroup->speed();
            hudInfo.actualSpeed = _p->playbackGroup->actualSpeed();
            hudInfo.droppedFrames = _p->playbackGroup->hasDroppedFrames();
            hudInfo.visible = _p->context->viewPrefs()->hudInfo();
            _p->viewWidget->setHudInfo(hudInfo);
        }

        void MainWindow::playbackUpdate()
        {
            switch (_p->playbackGroup->playback())
            {
            case Enum::FORWARD:
            case Enum::REVERSE:
                _p->fileGroup->setPreloadActive(false);
                break;
            case Enum::STOP:
                _p->fileGroup->setPreloadActive(true);
                break;
            default: break;
            }
        }

        const std::shared_ptr<Graphics::Image> & MainWindow::image() const
        {
            return _p->imageGroup->isFrameStoreVisible() ? _p->frameStoreImage : _p->image;
        }

        Graphics::OpenGLImageOptions MainWindow::imageOptions() const
        {
            Graphics::OpenGLImageOptions out;
            out.xform.mirror = _p->imageGroup->mirror();
            auto image = this->image();
            if (image)
            {
                out.xform.scale = Enum::imageScale(_p->imageGroup->scale(), image->size());
            }
            out.xform.rotate = Enum::imageRotate(_p->imageGroup->rotate());
            out.premultipliedAlpha = _p->imageGroup->hasPremultipliedAlpha();
            if (image && _p->imageGroup->hasColorProfile())
            {
                out.colorProfile = image->colorProfile;
            }
            out.displayProfile = _p->imageGroup->displayProfile();
            out.channel = _p->imageGroup->channel();
            out.background = _p->context->viewPrefs()->background();
            return out;
        }

    } // namespace ViewLib
} // namespace djv
