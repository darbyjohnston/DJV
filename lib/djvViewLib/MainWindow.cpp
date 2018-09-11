//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
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

#include <djvViewLib/Context.h>
#include <djvViewLib/FileCache.h>
#include <djvViewLib/FileGroup.h>
#include <djvViewLib/FilePrefs.h>
#include <djvViewLib/FileSave.h>
#include <djvViewLib/HelpGroup.h>
#include <djvViewLib/HudInfo.h>
#include <djvViewLib/ImageGroup.h>
#include <djvViewLib/ImagePrefs.h>
#include <djvViewLib/ImageView.h>
#include <djvViewLib/PlaybackGroup.h>
#include <djvViewLib/PlaybackPrefs.h>
#include <djvViewLib/ToolGroup.h>
#include <djvViewLib/ViewGroup.h>
#include <djvViewLib/ViewPrefs.h>
#include <djvViewLib/ViewPrefsWidget.h>
#include <djvViewLib/WindowGroup.h>
#include <djvViewLib/WindowPrefs.h>

#include <djvUI/ColorSwatch.h>
#include <djvUI/ImagePrefs.h>

#include <djvGraphics/OpenGLImage.h>
#include <djvGraphics/OpenGLOffscreenBuffer.h>

#include <djvCore/Debug.h>
#include <djvCore/DebugLog.h>
#include <djvCore/Error.h>

#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QScopedPointer>
#include <QStatusBar>
#include <QTimer>
#include <QToolBar>

namespace djv
{
    namespace ViewLib
    {
        struct MainWindow::Private
        {
            Private(Context * context) :
                context(context)
            {}

            Util::MOUSE_WHEEL mouseWheel = static_cast<Util::MOUSE_WHEEL>(0);
            FileGroup * fileGroup = nullptr;
            WindowGroup * windowGroup = nullptr;
            ViewGroup * viewGroup = nullptr;
            ImageGroup * imageGroup = nullptr;
            PlaybackGroup * playbackGroup = nullptr;
            qint64 playbackFrameTmp = 0;
            float playbackSpeedTmp = 0.f;
            ToolGroup * toolGroup = nullptr;
            HelpGroup * helpGroup = nullptr;
            const djvImage * imageP = nullptr;
            djvImage imageTmp;
            glm::ivec2 imagePick = glm::ivec2(0, 0);
            djvColor imageSample;
            std::unique_ptr<djvOpenGLImage> openGLImage;
            bool sampleInit = false;
            ImageView * viewWidget = nullptr;
            djvColorSwatch * infoSwatch = nullptr;
            QLabel * infoPixelLabel = nullptr;
            QLabel * infoImageLabel = nullptr;
            QLabel * infoCacheLabel = nullptr;
            int menuBarHeight = 0;
            Context * context = nullptr;
        };

        namespace
        {
            QVector<MainWindow *> _mainWindowList;

        } // namespace

        MainWindow::MainWindow(
            const MainWindow * copy,
            Context *          context) :
            _p(new Private(context))
        {
            //DJV_DEBUG("MainWindow::MainWindow");

            _mainWindowList.append(this);

            //! \todo The native menu bar on OS X causes issues with keyboard shortcuts.
            //menuBar()->setNativeMenuBar(false);

            // Create the widgets.
            _p->viewWidget = new ImageView(context);

            _p->infoSwatch = new djvColorSwatch(context);
            _p->infoSwatch->setFixedSize(20, 20);

            _p->infoPixelLabel = new QLabel;
            _p->infoPixelLabel->setToolTip(
                qApp->translate("djv::ViewLib::MainWindow", "Pixel information\n\nClick and drag inside the image."));

            _p->infoImageLabel = new QLabel;
            _p->infoImageLabel->setToolTip(
                qApp->translate("djv::ViewLib::MainWindow", "Image information."));

            _p->infoCacheLabel = new QLabel;
            _p->infoCacheLabel->setToolTip(
                qApp->translate("djv::ViewLib::MainWindow", "File cache information."));

            // Create the groups.
            _p->fileGroup = new FileGroup(copy ? copy->_p->fileGroup : 0, this, context);
            _p->windowGroup = new WindowGroup(copy ? copy->_p->windowGroup : 0, this, context);
            _p->viewGroup = new ViewGroup(copy ? copy->_p->viewGroup : 0, this, context);
            _p->imageGroup = new ImageGroup(copy ? copy->_p->imageGroup : 0, this, context);
            _p->playbackGroup = new PlaybackGroup(copy ? copy->_p->playbackGroup : 0, this, context);
            _p->toolGroup = new ToolGroup(copy ? copy->_p->toolGroup : 0, this, context);
            _p->helpGroup = new HelpGroup(copy ? copy->_p->helpGroup : 0, this, context);

            // Layout the widgets.
            setCentralWidget(_p->viewWidget);

            statusBar()->addWidget(_p->infoSwatch);
            statusBar()->addWidget(_p->infoPixelLabel);
            statusBar()->addWidget(new QWidget, 1);
            statusBar()->addWidget(_p->infoImageLabel);
            statusBar()->addWidget(_p->infoCacheLabel);

            // Initialize.
            setWindowTitle(qApp->applicationName());
            setAttribute(Qt::WA_DeleteOnClose);
            if (copy)
            {
                _p->imageTmp = copy->_p->imageTmp;
                _p->imagePick = copy->_p->imagePick;

                _p->viewWidget->setViewPos(copy->_p->viewWidget->viewPos());
                _p->viewWidget->setViewZoom(copy->_p->viewWidget->viewZoom());

                resize(copy->size());
            }
            fileUpdate();
            fileCacheUpdate();
            imageUpdate();
            controlsUpdate();
            playbackUpdate();

            // Setup the file group callbacks.
            connect(
                _p->fileGroup,
                SIGNAL(imageChanged()),
                SLOT(imageUpdate()));
            connect(
                _p->fileGroup,
                SIGNAL(loadFrameStore()),
                SLOT(loadFrameStoreCallback()));
            connect(
                _p->fileGroup,
                SIGNAL(reloadFrame()),
                SLOT(reloadFrameCallback()));
            connect(
                _p->fileGroup,
                SIGNAL(save(const djvFileInfo &)),
                SLOT(saveCallback(const djvFileInfo &)));
            connect(
                _p->fileGroup,
                SIGNAL(saveFrame(const djvFileInfo &)),
                SLOT(saveFrameCallback(const djvFileInfo &)));

            // Setup the image group callbacks.
            connect(
                _p->imageGroup,
                SIGNAL(frameStoreChanged(bool)),
                SLOT(imageUpdate()));
            connect(
                _p->imageGroup,
                SIGNAL(loadFrameStore()),
                SLOT(loadFrameStoreCallback()));
            connect(
                _p->imageGroup,
                SIGNAL(displayProfileChanged(const djv::ViewLib::DisplayProfile &)),
                SLOT(imageUpdate()));
            connect(
                _p->imageGroup,
                SIGNAL(redrawNeeded()),
                SLOT(imageUpdate()));
            connect(
                _p->imageGroup,
                SIGNAL(resizeNeeded()),
                SLOT(windowResizeCallback()));

            // Setup the window group callbacks.
            connect(
                _p->windowGroup,
                SIGNAL(controlsVisibleChanged(bool)),
                SLOT(controlsUpdate()));
            connect(
                _p->windowGroup,
                SIGNAL(toolBarVisibleChanged(const QVector<bool> &)),
                SLOT(controlsUpdate()));

            // Setup the playback group callbacks.
            connect(
                _p->playbackGroup,
                SIGNAL(playbackChanged(djv::ViewLib::Util::PLAYBACK)),
                SLOT(playbackUpdate()));
            connect(
                _p->playbackGroup,
                SIGNAL(frameChanged(qint64)),
                SLOT(imageUpdate()));
            connect(
                _p->playbackGroup,
                SIGNAL(sequenceChanged(const djvSequence &)),
                SLOT(viewOverlayUpdate()));
            connect(
                _p->playbackGroup,
                SIGNAL(frameChanged(qint64)),
                SLOT(viewOverlayUpdate()));
            connect(
                _p->playbackGroup,
                SIGNAL(speedChanged(const djvSpeed &)),
                SLOT(viewOverlayUpdate()));
            connect(
                _p->playbackGroup,
                SIGNAL(realSpeedChanged(float)),
                SLOT(viewOverlayUpdate()));
            connect(
                _p->playbackGroup,
                SIGNAL(droppedFramesChanged(bool)),
                SLOT(viewOverlayUpdate()));
            connect(
                _p->playbackGroup,
                SIGNAL(layoutChanged(djv::ViewLib::Util::LAYOUT)),
                SLOT(windowResizeCallback()));

            // Setup the view callbacks.
            connect(
                _p->viewWidget,
                SIGNAL(pickChanged(const glm::ivec2 &)),
                SLOT(pickCallback(const glm::ivec2 &)));
            connect(
                _p->viewWidget,
                SIGNAL(mouseWheelChanged(djv::ViewLib::Util::MOUSE_WHEEL)),
                SLOT(mouseWheelCallback(djv::ViewLib::Util::MOUSE_WHEEL)));
            connect(
                _p->viewWidget,
                SIGNAL(mouseWheelValueChanged(int)),
                SLOT(mouseWheelValueCallback(int)));
            connect(
                _p->viewWidget,
                SIGNAL(fileDropped(const djvFileInfo &)),
                SLOT(fileOpen(const djvFileInfo &)));

            // Setup the preferences callbacks.
            connect(
                context->fileCache(),
                SIGNAL(cacheChanged()),
                SLOT(fileCacheUpdate()));
            connect(
                context->djvUIContext::imagePrefs(),
                SIGNAL(filterChanged(const djvOpenGLImageFilter &)),
                SLOT(imageUpdate()));
            connect(
                context->viewPrefs(),
                SIGNAL(backgroundChanged(const djvColor &)),
                SLOT(imageUpdate()));
        }

        MainWindow::~MainWindow()
        {
            //DJV_DEBUG("MainWindow::~MainWindow");
        }

        const djvImageIOInfo & MainWindow::imageIOInfo() const
        {
            return _p->fileGroup->imageIOInfo();
        }

        ImageView * MainWindow::viewWidget() const
        {
            return _p->viewWidget;
        }

        QVector<MainWindow *> MainWindow::mainWindowList()
        {
            return _mainWindowList;
        }

        MainWindow * MainWindow::createWindow(Context * context)
        {
            MainWindow * out = new MainWindow(0, context);

            // Apply command line file options.
            if (context->fileLayer().data())
            {
                out->setFileLayer(*context->fileLayer());
            }
            if (context->fileProxy().data())
            {
                out->setFileProxy(*context->fileProxy());
            }
            if (context->hasFileCache().data())
            {
                out->setFileCache(*context->hasFileCache());
            }

            // Apply command line window options.
            if (context->isWindowFullScreen().data() && *context->isWindowFullScreen())
            {
                out->showFullScreen();
            }

            // Apply command line playback options.
            if (context->playback().data())
            {
                out->setPlayback(*context->playback());
            }
            if (context->playbackFrame().data())
            {
                out->setPlaybackFrame(*context->playbackFrame());
            }
            if (context->playbackSpeed().data())
            {
                out->setPlaybackSpeed(*context->playbackSpeed());
            }

            return out;
        }

        void MainWindow::fileOpen(const djvFileInfo & fileInfo, bool init)
        {
            //DJV_DEBUG("MainWindow::fileOpen");
            //DJV_DEBUG_PRINT("fileInfo = " << fileInfo);
            //DJV_DEBUG_PRINT("init = " << init);

            DJV_LOG(_p->context->debugLog(), "MainWindow",
                QString("Open file = \"%1\"").arg(fileInfo));

            // Initialize.
            _p->imageP = 0;
            _p->viewWidget->setData(0);

            // Open the file.
            {
                _p->fileGroup->open(fileInfo);

                // Set playback.
                const djvSequence & sequence = _p->fileGroup->imageIOInfo().sequence;
                //DJV_DEBUG_PRINT("sequence = " << sequence);
                _p->playbackGroup->setSequence(sequence);
                if (init)
                {
                    _p->playbackGroup->setFrame(0);
                    _p->playbackGroup->setPlayback(
                        (sequence.frames.count() > 1 &&
                            _p->context->playbackPrefs()->hasAutoStart()) ?
                        Util::FORWARD :
                        Util::STOP);
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

        void MainWindow::setFileProxy(djvPixelDataInfo::PROXY in)
        {
            _p->fileGroup->setProxy(in);
        }

        void MainWindow::setFileCache(bool in)
        {
            _p->fileGroup->setCache(in);
        }

        void MainWindow::fitWindow(bool move)
        {
            if (isFullScreen())
                return;
            //DJV_DEBUG("MainWindow::fitWindow");
            const glm::ivec2 frame(frameGeometry().width(), frameGeometry().height());
            //DJV_DEBUG_PRINT("frame = " << frame);
            //DJV_DEBUG_PRINT("view size = " <<
            //    djvVectorUtil::fromQSize(_p->viewWidget->size()));
            _p->viewWidget->updateGeometry();
            resize(sizeHint());
            //DJV_DEBUG_PRINT("new view size = " <<
            //    djvVectorUtil::fromQSize(_p->viewWidget->size()));
            if (move && isVisible())
            {
                //DJV_DEBUG_PRINT("move");
                this->move(
                    x() - (frameGeometry().width() / 2 - frame.x / 2),
                    y() - (frameGeometry().height() / 2 - frame.y / 2));
            }
            _p->viewWidget->viewFit();
        }

        void MainWindow::setPlayback(Util::PLAYBACK in)
        {
            _p->playbackGroup->setPlayback(in);
        }

        void MainWindow::setPlaybackFrame(qint64 in)
        {
            _p->playbackGroup->setFrame(in);
        }

        void MainWindow::setPlaybackSpeed(const djvSpeed & in)
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
            const djvBox2i screen = djvBoxUtil::fromQt(QApplication::desktop()->availableGeometry());
            //DJV_DEBUG_PRINT("screen = " << screen);
            move(
                screen.x + screen.w / 2 - size.x / 2,
                screen.y + screen.h / 2 - size.y / 2);
        }

        void MainWindow::closeEvent(QCloseEvent * event)
        {
            QMainWindow::closeEvent(event);
            _mainWindowList.remove(_mainWindowList.indexOf(this));
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

        void MainWindow::keyPressEvent(QKeyEvent * event)
        {
            //DJV_DEBUG("MainWindow::keyPressEvent");
            QMainWindow::keyPressEvent(event);
            switch (event->key())
            {
            case Qt::Key_Escape:
                if (!_p->windowGroup->hasControlsVisible())
                {
                    _p->windowGroup->setControlsVisible(true);
                }
                else if (isFullScreen())
                {
                    //DJV_DEBUG_PRINT("show normal");

                    showNormal();

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
            _p->context->fileCache()->del(this, frame);
        }

        void MainWindow::saveCallback(const djvFileInfo & in)
        {
            //DJV_DEBUG("MainWindow::saveCallback");
            //DJV_DEBUG_PRINT("in = " << in);
            djvSequence sequence;
            const djvFrameList & frames = _p->playbackGroup->sequence().frames;
            if (frames.count())
            {
                sequence.frames = frames.mid(
                    _p->playbackGroup->inPoint(),
                    _p->playbackGroup->outPoint() - _p->playbackGroup->inPoint() + 1);
            }
            sequence.speed = _p->playbackGroup->speed();
            const FileSaveInfo info(
                _p->fileGroup->fileInfo(),
                in,
                _p->fileGroup->imageIOInfo()[_p->fileGroup->layer()],
                sequence,
                _p->fileGroup->layer(),
                _p->fileGroup->proxy(),
                _p->fileGroup->hasU8Conversion(),
                _p->imageGroup->hasColorProfile(),
                imageOptions());
            _p->context->fileSave()->save(info);
        }

        void MainWindow::saveFrameCallback(const djvFileInfo & in)
        {
            //DJV_DEBUG("MainWindow::saveFrameCallback");
            //DJV_DEBUG_PRINT("in = " << in);
            djvSequence sequence;
            const djvFrameList & frames = _p->playbackGroup->sequence().frames;
            if (frames.count())
            {
                sequence.frames += frames[_p->playbackGroup->frame()];
            }
            sequence.speed = _p->playbackGroup->speed();
            //DJV_DEBUG_PRINT("sequence = " << sequence);
            const FileSaveInfo info(
                _p->fileGroup->fileInfo(),
                in,
                _p->fileGroup->imageIOInfo()[_p->fileGroup->layer()],
                sequence,
                _p->fileGroup->layer(),
                _p->fileGroup->proxy(),
                _p->fileGroup->hasU8Conversion(),
                _p->imageGroup->hasColorProfile(),
                imageOptions());
            _p->context->fileSave()->save(info);
        }

        void MainWindow::loadFrameStoreCallback()
        {
            //DJV_DEBUG("MainWindow::loadFrameStoreCallback");
            if (_p->imageP)
            {
                _p->imageTmp = *_p->imageP;
            }
        }

        void MainWindow::pickCallback(const glm::ivec2 & pick)
        {
            _p->imagePick = pick;
            if (!_p->sampleInit)
            {
                _p->sampleInit = true;
                QTimer::singleShot(0, this, SLOT(viewPickUpdate()));
            }
        }

        void MainWindow::mouseWheelCallback(Util::MOUSE_WHEEL in)
        {
            _p->mouseWheel = in;
            switch (_p->mouseWheel)
            {
            case Util::MOUSE_WHEEL_PLAYBACK_SHUTTLE:
                _p->playbackGroup->setPlayback(Util::STOP);
                _p->playbackFrameTmp = _p->playbackGroup->frame();
                break;
            case Util::MOUSE_WHEEL_PLAYBACK_SPEED:
                _p->playbackSpeedTmp =
                    djvSpeed::speedToFloat(_p->playbackGroup->speed());
                break;
            default: break;
            }
        }

        void MainWindow::mouseWheelValueCallback(int in)
        {
            switch (_p->mouseWheel)
            {
            case Util::MOUSE_WHEEL_PLAYBACK_SHUTTLE:
                _p->playbackGroup->setFrame(_p->playbackFrameTmp + in);
                break;
            case Util::MOUSE_WHEEL_PLAYBACK_SPEED:
                _p->playbackGroup->setSpeed(
                    djvSpeed::floatToSpeed(
                        _p->playbackSpeedTmp + static_cast<float>(in)));
                break;
            default: break;
            }
        }

        void MainWindow::fileUpdate()
        {
            // Update the window title.
            const djvFileInfo & fileInfo = _p->fileGroup->fileInfo();
            const QString title = !fileInfo.fileName().isEmpty() ?
                qApp->translate("djv::ViewLib::MainWindow", "%1 - %2").
                arg(qApp->applicationName()).
                arg(QDir::toNativeSeparators(fileInfo)) :
                QString("%1").arg(qApp->applicationName());
            setWindowTitle(title);
        }

        void MainWindow::fileCacheUpdate()
        {
            //DJV_DEBUG("MainWindow::cacheUpdate");
            const float size = _p->context->fileCache()->size();
            const float maxSize = _p->context->fileCache()->maxSize();
            _p->infoCacheLabel->setText(
                qApp->translate("djv::ViewLib::MainWindow", "Cache: %1% %2/%3GB").
                arg(static_cast<int>(size / maxSize * 100)).
                arg(size, 0, 'f', 2).
                arg(maxSize, 0, 'f', 2));
        }

        void MainWindow::imageUpdate()
        {
            //DJV_DEBUG("MainWindow::imageUpdate");

            // Update the image.
            const qint64 frame = _p->playbackGroup->frame();
            _p->imageP = _p->fileGroup->image(frame);
            if (_p->imageP)
            {
                //DJV_DEBUG_PRINT("image = " << *_p->imageP);
            }
            const djvImage * image = this->image();

            // Update the information tool bar.
            djvPixelDataInfo info;
            if (image)
            {
                info = image->info();
            }
            //DJV_DEBUG_PRINT("info = " << info);
            _p->infoImageLabel->setText(
                qApp->translate("djv::ViewLib::MainWindow", "Image: %1x%2:%3 %4").
                arg(info.size.x).
                arg(info.size.y).
                arg(djvVectorUtil::aspect(info.size), 0, 'f', 2).
                arg(djvStringUtil::label(info.pixel).join(" ")));

            //! Update the view.
            _p->viewWidget->setOptions(imageOptions());
            _p->viewWidget->setData(image);
            viewOverlayUpdate();
            _p->viewWidget->update();

            //! Update the file group.
            _p->fileGroup->setPreloadFrame(frame);

            Q_EMIT imageChanged();
        }

        void MainWindow::controlsUpdate()
        {
            //DJV_DEBUG("MainWindow::controlsUpdate");

            // Temporarily disable updates to try and minimize flickering.
            setUpdatesEnabled(false);

            const bool controls = _p->windowGroup->hasControlsVisible();

            //! \todo It seems that when the menu bar is hidden the associated
            //! keyboard shortcuts no longer work. To work around this we resize
            //! the menu bar's height to zero instead of hiding it.
            //menuBar()->setVisible(controls);

            if (controls)
            {
                if (_p->menuBarHeight != 0)
                {
                    menuBar()->setFixedHeight(_p->menuBarHeight);
                }
            }
            else
            {
                _p->menuBarHeight = menuBar()->height();
                //DJV_DEBUG_PRINT("menu bar height = " << _p->menuBarHeight);
                menuBar()->setFixedHeight(0);
            }

            const QVector<bool> & visible = _p->windowGroup->toolBarVisible();
            _p->fileGroup->toolBar()->setVisible(controls && visible[Util::TOOL_BARS]);
            _p->windowGroup->toolBar()->setVisible(controls && visible[Util::TOOL_BARS]);
            _p->viewGroup->toolBar()->setVisible(controls && visible[Util::TOOL_BARS]);
            _p->imageGroup->toolBar()->setVisible(controls && visible[Util::TOOL_BARS]);
            _p->toolGroup->toolBar()->setVisible(controls && visible[Util::TOOL_BARS]);

            _p->playbackGroup->toolBar()->setVisible(
                controls && visible[Util::PLAYBACK_BAR]);

            statusBar()->setVisible(controls && visible[Util::INFO_BAR]);

            QTimer::singleShot(0, this, SLOT(enableUpdatesCallback()));
        }

        void MainWindow::viewOverlayUpdate()
        {
            //DJV_DEBUG("MainWindow::viewOverlayUpdate");

            //! Update the HUD.
            HudInfo hudInfo;
            const djvImage * image = this->image();
            if (image)
            {
                hudInfo.info = image->info();
                hudInfo.tags = image->tags;
            }
            const djvSequence & sequence = _p->playbackGroup->sequence();
            hudInfo.frame = 0;
            if (sequence.frames.count() &&
                _p->playbackGroup->frame() <
                static_cast<int64_t>(sequence.frames.count()))
            {
                hudInfo.frame = sequence.frames[_p->playbackGroup->frame()];
            }
            hudInfo.speed = _p->playbackGroup->speed();
            hudInfo.realSpeed = _p->playbackGroup->realSpeed();
            hudInfo.droppedFrames = _p->playbackGroup->hasDroppedFrames();
            hudInfo.visible = _p->context->viewPrefs()->hudInfo();
            _p->viewWidget->setHudInfo(hudInfo);
        }

        void MainWindow::viewPickUpdate()
        {
            //DJV_DEBUG("MainWindow::viewPickUpdate");

            _p->context->makeGLContextCurrent();
            if (!_p->openGLImage)
            {
                _p->openGLImage.reset(new djvOpenGLImage);
            }

            // Update the info bar with pixel information.
            const glm::ivec2 pick = djvVectorUtil::floor(
                glm::vec2(_p->imagePick - _p->viewWidget->viewPos()) /
                _p->viewWidget->viewZoom());
            //DJV_DEBUG_PRINT("pick = " << pick);
            djvOpenGLImageOptions options = imageOptions();
            _p->imageSample = options.background;
            const djvImage * image = this->image();
            if (image && _p->windowGroup->toolBarVisible()[Util::INFO_BAR])
            {
                //DJV_DEBUG_PRINT("sample");
                try
                {
                    _p->context->makeGLContextCurrent();
                    djvPixelData tmp(djvPixelDataInfo(glm::ivec2(1, 1), image->pixel()));
                    djvOpenGLImageOptions _options = options;
                    _options.xform.position -= pick;
                    _p->openGLImage->copy(*image, tmp, _options);
                    _p->openGLImage->average(tmp, _p->imageSample);
                }
                catch (djvError error)
                {
                    error.add(Util::errorLabels()[Util::ERROR_PICK_COLOR]);
                    _p->context->printError(error);
                }
            }
            //DJV_DEBUG_PRINT("sample = " << _p->imageSample);

            _p->infoSwatch->setColor(_p->imageSample);

            _p->infoPixelLabel->setText(
                qApp->translate("djv::ViewLib::MainWindow", "Pixel: %1, %2, %3").
                arg(pick.x).
                arg(pick.y).
                arg(djvStringUtil::label(_p->imageSample).join(" ")));

            _p->sampleInit = false;
        }

        void MainWindow::playbackUpdate()
        {
            switch (_p->playbackGroup->playback())
            {
            case Util::FORWARD:
            case Util::REVERSE:
                _p->fileGroup->setPreloadActive(false);
                break;
            case Util::STOP:
                _p->fileGroup->setPreloadActive(true);
                break;
            default: break;
            }
        }

        const djvImage * MainWindow::image() const
        {
            return _p->imageGroup->hasFrameStore() ? &_p->imageTmp : _p->imageP;
        }

        djvOpenGLImageOptions MainWindow::imageOptions() const
        {
            djvOpenGLImageOptions out;
            out.xform.mirror = _p->imageGroup->mirror();
            const djvImage * image = this->image();
            if (image)
            {
                out.xform.scale = Util::imageScale(
                    _p->imageGroup->scale(), image->size());
            }
            out.xform.rotate = Util::imageRotate(_p->imageGroup->rotate());
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
