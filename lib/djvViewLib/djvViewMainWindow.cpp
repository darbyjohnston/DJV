//------------------------------------------------------------------------------
// Copyright (c) 2004-2014 Darby Johnston
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

//! \file djvViewMainWindow.cpp

#include <djvViewMainWindow.h>

#include <djvViewApplication.h>
#include <djvViewFileCache.h>
#include <djvViewFileGroup.h>
#include <djvViewFilePrefs.h>
#include <djvViewFileSave.h>
#include <djvViewHelpGroup.h>
#include <djvViewHudInfo.h>
#include <djvViewImageGroup.h>
#include <djvViewImageView.h>
#include <djvViewPlaybackGroup.h>
#include <djvViewPlaybackPrefs.h>
#include <djvViewToolGroup.h>
#include <djvViewViewGroup.h>
#include <djvViewViewPrefs.h>
#include <djvViewViewPrefsWidget.h>
#include <djvViewWindowGroup.h>
#include <djvViewWindowPrefs.h>

#include <djvColorSwatch.h>

#include <djvDebug.h>
#include <djvError.h>
#include <djvOpenGlImage.h>
#include <djvOpenGlOffscreenBuffer.h>
#include <djvOpenGlPrefs.h>
#include <djvStringUtil.h>

#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QScopedPointer>
#include <QStatusBar>
#include <QTimer>
#include <QToolBar>

//------------------------------------------------------------------------------
// djvViewMainWindow::P
//------------------------------------------------------------------------------

struct djvViewMainWindow::P
{
    P() :
        mouseWheel      (static_cast<djvViewInput::MOUSE_WHEEL>(0)),
        fileGroup       (0),
        windowGroup     (0),
        viewGroup       (0),
        imageGroup      (0),
        playbackGroup   (0),
        playbackFrameTmp(0),
        playbackSpeedTmp(0.0),
        toolGroup       (0),
        helpGroup       (0),
        imageP          (0),
        sampleInit      (false),
        viewWidget      (0),
        viewWidgetFit   (false),
        infoSwatch      (0),
        infoPixelLabel  (0),
        menuBarHeight   (0)
    {}

    djvViewInput::MOUSE_WHEEL                mouseWheel;
    djvViewFileGroup *                       fileGroup;
    djvViewWindowGroup *                     windowGroup;
    djvViewViewGroup *                       viewGroup;
    djvViewImageGroup *                      imageGroup;
    djvViewPlaybackGroup *                   playbackGroup;
    qint64                                   playbackFrameTmp;
    double                                   playbackSpeedTmp;
    djvViewToolGroup *                       toolGroup;
    djvViewHelpGroup *                       helpGroup;
    const djvImage *                         imageP;
    djvImage                                 imageTmp;
    djvVector2i                              imagePick;
    djvColor                                 imageSample;
    bool                                     sampleInit;
    QScopedPointer<djvOpenGlOffscreenBuffer> sampleBuffer;
    djvOpenGlImageState                      sampleState;
    djvViewImageView *                       viewWidget;
    bool                                     viewWidgetFit;
    djvColorSwatch *                         infoSwatch;
    QLabel *                                 infoPixelLabel;
    QLabel *                                 infoImageLabel;
    QLabel *                                 infoCacheLabel;
    int                                      menuBarHeight;
};

//------------------------------------------------------------------------------
// djvViewMainWindow
//------------------------------------------------------------------------------

namespace
{

QVector<djvViewMainWindow *> _mainWindowList;

} // namespace

djvViewMainWindow::djvViewMainWindow(const djvViewMainWindow * copy) :
    _p(new P)
{
    //DJV_DEBUG("djvViewMainWindow::djvViewMainWindow");

    _mainWindowList.append(this);

    //! \todo The native menu bar on OS X causes issues with keyboard shortcuts.
    
    //menuBar()->setNativeMenuBar(false);

    // Create the widgets.

    _p->viewWidget = new djvViewImageView;

    _p->infoSwatch = new djvColorSwatch;
    _p->infoSwatch->setFixedSize(20, 20);

    _p->infoPixelLabel = new QLabel;
    _p->infoPixelLabel->setToolTip(
        "Pixel information\n\nClick and drag inside the image.");

    _p->infoImageLabel = new QLabel;
    _p->infoImageLabel->setToolTip("Image information.");

    _p->infoCacheLabel = new QLabel;
    _p->infoCacheLabel->setToolTip(
        "File cache information.");

    // Create the groups.

    _p->fileGroup = new djvViewFileGroup(this, copy ? copy->_p->fileGroup : 0);
    _p->windowGroup = new djvViewWindowGroup(this, copy ? copy->_p->windowGroup : 0);
    _p->viewGroup = new djvViewViewGroup(this, copy ? copy->_p->viewGroup : 0);
    _p->imageGroup = new djvViewImageGroup(this, copy ? copy->_p->imageGroup : 0);
    _p->playbackGroup = new djvViewPlaybackGroup(this, copy ? copy->_p->playbackGroup : 0);
    _p->toolGroup = new djvViewToolGroup(this, copy ? copy->_p->toolGroup : 0);
    _p->helpGroup = new djvViewHelpGroup(this, copy ? copy->_p->helpGroup : 0);

    // Layout the widgets.
    
    setCentralWidget(_p->viewWidget);

    statusBar()->addWidget(_p->infoSwatch);
    statusBar()->addWidget(_p->infoPixelLabel);
    statusBar()->addWidget(new QWidget, 1);
    statusBar()->addWidget(_p->infoImageLabel);
    statusBar()->addWidget(_p->infoCacheLabel);

    // Initialize.

    if (copy)
    {
        _p->imageTmp  = copy->_p->imageTmp;
        _p->imagePick = copy->_p->imagePick;
    }

    setWindowTitle(DJV_APP->name());

    setAttribute(Qt::WA_DeleteOnClose);
    
    if (copy)
    {
        _p->viewWidget->setViewPos (copy->_p->viewWidget->viewPos());
        _p->viewWidget->setViewZoom(copy->_p->viewWidget->viewZoom());
    }

    if (copy)
    {
        resize(copy->size());
    }
    else
    {
        fitWindow();
    }
    
    _p->viewWidget->installEventFilter(this);

    fileUpdate();
    fileCacheUpdate();
    imageUpdate();
    controlsUpdate();

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
        SIGNAL(displayProfileChanged(const djvViewDisplayProfile &)),
        SLOT(imageUpdate()));

    connect(
        _p->imageGroup,
        SIGNAL(redrawNeeded()),
        SLOT(imageUpdate()));

    connect(
        _p->imageGroup,
        SIGNAL(resizeNeeded()),
        SLOT(windowResizeUpdate()));

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
        SIGNAL(realSpeedChanged(double)),
        SLOT(viewOverlayUpdate()));

    connect(
        _p->playbackGroup,
        SIGNAL(droppedFramesChanged(bool)),
        SLOT(viewOverlayUpdate()));

    connect(
        _p->playbackGroup,
        SIGNAL(layoutChanged(djvView::LAYOUT)),
        SLOT(windowResizeUpdate()));

    // Setup the view callbacks.

    connect(
        _p->viewWidget,
        SIGNAL(pickChanged(const djvVector2i &)),
        SLOT(pickCallback(const djvVector2i &)));

    connect(
        _p->viewWidget,
        SIGNAL(mouseWheelChanged(djvViewInput::MOUSE_WHEEL)),
        SLOT(mouseWheelCallback(djvViewInput::MOUSE_WHEEL)));

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
        djvViewFileCache::global(),
        SIGNAL(cacheChanged()),
        SLOT(fileCacheUpdate()));

    connect(
        djvOpenGlPrefs::global(),
        SIGNAL(filterChanged(const djvOpenGlImageFilter &)),
        SLOT(imageUpdate()));

    connect(
        djvViewViewPrefs::global(),
        SIGNAL(backgroundChanged(const djvColor &)),
        SLOT(imageUpdate()));
}

djvViewMainWindow::~djvViewMainWindow()
{
    //DJV_DEBUG("djvViewMainWindow::~djvViewMainWindow");

    _p->imageP = 0;

    _p->viewWidget->setData(0);

    _p->viewWidget->context()->makeCurrent();

    //! \todo Hopefully this ensures that the view widget context is deleted
    //! after the child widgets.

    _p->viewWidget->setParent(0);
    _p->viewWidget->deleteLater();

    delete _p;
}

const djvImageIoInfo & djvViewMainWindow::imageIoInfo() const
{
    return _p->fileGroup->imageIoInfo();
}

djvViewImageView * djvViewMainWindow::viewWidget() const
{
    return _p->viewWidget;
}

QVector<djvViewMainWindow *> djvViewMainWindow::mainWindowList()
{
    return _mainWindowList;
}

void djvViewMainWindow::fileOpen(const djvFileInfo & in, bool init)
{
    //DJV_DEBUG("djvViewMainWindow::fileOpen");
    //DJV_DEBUG_PRINT("in = " << in);
    //DJV_DEBUG_PRINT("init = " << init);

    // Initialize.

    _p->imageP = 0;

    _p->viewWidget->setData(0);

    // Open the file.

    {
        _p->fileGroup->open(in);

        // Set playback.

        const djvSequence & sequence = _p->fileGroup->imageIoInfo().sequence;

        _p->playbackGroup->setSequence(sequence);

        if (init)
        {
            _p->playbackGroup->setFrame(0);

            _p->playbackGroup->setPlayback(
                (sequence.frames.count() > 1 &&
                    djvViewPlaybackPrefs::global()->hasAutoStart()) ?
                    djvView::FORWARD :
                    djvView::STOP);
        }
    }

    // Update.

    fileUpdate();
    imageUpdate();

    if (init)
    {
        if (djvViewWindowPrefs::global()->hasResizeFit())
        {
            if (! isFullScreen())
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
            viewResizeUpdate();
        }
    }
}

void djvViewMainWindow::setAutoSequence(bool in)
{
    _p->fileGroup->setAutoSequence(in);
}

void djvViewMainWindow::setFileLayer(int in)
{
    _p->fileGroup->setLayer(in);
}

void djvViewMainWindow::setFileProxy(djvPixelDataInfo::PROXY in)
{
    _p->fileGroup->setProxy(in);
}

void djvViewMainWindow::setFileCacheEnabled(bool in)
{
    _p->fileGroup->setCacheEnabled(in);
}

void djvViewMainWindow::fitWindow()
{
    //DJV_DEBUG("djvViewMainWindow::fitWindow");

    // Calculate image size.

    const djvBox2f bbox = _p->viewWidget->bbox().size;

    //DJV_DEBUG_PRINT("bbox = " << bbox);

    djvVector2i imageSize = djvVectorUtil::ceil<double, int>(bbox.size);

    if (! djvVectorUtil::isSizeValid(imageSize))
    {
        imageSize = djvVector2i(600, 300);
    }

    //DJV_DEBUG_PRINT("image size = " << imageSize);

    // Adjust to screen size.

    const double resizeMax = djvView::windowResizeMax(
        djvViewWindowPrefs::global()->hasResizeFit() ?
        djvViewWindowPrefs::global()->resizeMax() :
        djvView::WINDOW_RESIZE_MAX_UNLIMITED);

    const djvVector2i max(
        static_cast<int>(qApp->desktop()->width () * resizeMax),
        static_cast<int>(qApp->desktop()->height() * resizeMax));

    if (imageSize.x > max.x || imageSize.y > max.y)
    {
        imageSize =
            imageSize.x > imageSize.y ?
            djvVector2i(
                max.x,
                djvMath::ceil<int>(
                    imageSize.y / static_cast<double>(imageSize.x) * max.x)) :
            djvVector2i(
                djvMath::ceil<int>(
                    imageSize.x / static_cast<double>(imageSize.y) * max.y),
                max.y);
    }

    // Adjust to size hint.

    const djvVector2i uiSize =
        djvVector2i(sizeHint().width(), sizeHint().height()) -
        djvVector2i(0, _p->viewWidget->sizeHint().height());

    //DJV_DEBUG_PRINT("ui size = " << uiSize);

    const djvVector2i size = djvVector2i(
        djvMath::max(imageSize.x, uiSize.x),
        imageSize.y + uiSize.y);

    //DJV_DEBUG_PRINT("size = " << size);

    // Set size.

    showNormal();

    resize(size.x, size.y);

    _p->viewWidget->viewFit();
}

void djvViewMainWindow::setPlayback(djvView::PLAYBACK in)
{
    _p->playbackGroup->setPlayback(in);
}

void djvViewMainWindow::setPlaybackFrame(qint64 in)
{
    _p->playbackGroup->setFrame(in);
}

void djvViewMainWindow::setPlaybackSpeed(const djvSpeed & in)
{
    _p->playbackGroup->setSpeed(in);
}

void djvViewMainWindow::changeEvent(QEvent * event)
{
    switch (event->type())
    {
        case QEvent::WindowStateChange:
        {
            //DJV_DEBUG("djvViewMainWindow::changeEvent");
        
            //
            //! \todo It seems that changing full screen mode happens
            //! asynchronously so we add this extra flag to trigger the
            //! proper resizing behavior.
            //
            
            _p->viewWidgetFit = true;
            
        } break;
        
        default: break;
    }
}

void djvViewMainWindow::closeEvent(QCloseEvent * event)
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

void djvViewMainWindow::keyPressEvent(QKeyEvent * event)
{
    //DJV_DEBUG("djvViewMainWindow::keyPressEvent");
    
    QMainWindow::keyPressEvent(event);

    switch (event->key())
    {
        case Qt::Key_Escape:
        
            if (! _p->windowGroup->hasControlsVisible())
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

bool djvViewMainWindow::eventFilter(QObject * object, QEvent * event)
{
    //DJV_DEBUG("djvViewMainWindow::eventFilter");
    //DJV_DEBUG_PRINT("event = " << event->type());
    
    switch (event->type())
    {
        case QEvent::UpdateRequest:

            if (_p->viewWidgetFit)
            {
                //DJV_DEBUG("djvViewMainWindow::eventFilter");
                //DJV_DEBUG_PRINT("view fit");
                
                _p->viewWidget->viewFit();
                
                _p->viewWidgetFit = false;
            }
        
            break;
        
        default: break;
    }
    
    return QMainWindow::eventFilter(object, event);
}

void djvViewMainWindow::reloadFrameCallback()
{
    //DJV_DEBUG("djvViewMainWindow::reloadFrameCallback");
    
    const qint64 frame = _p->playbackGroup->frame();
    
    //DJV_DEBUG_PRINT("frame = " << frame);
    
    djvViewFileCache::global()->del(this, frame);
}

void djvViewMainWindow::saveCallback(const djvFileInfo & in)
{
    //DJV_DEBUG("djvViewMainWindow::saveCallback");
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

    const djvViewFileSaveInfo info(
        _p->fileGroup->fileInfo(),
        in,
        _p->fileGroup->imageIoInfo()[_p->fileGroup->layer()],
        sequence,
        _p->fileGroup->layer(),
        _p->fileGroup->proxy(),
        _p->fileGroup->hasU8Conversion(),
        _p->imageGroup->hasColorProfile(),
        imageOptions());

    djvViewFileSave::global()->save(info);
}

void djvViewMainWindow::saveFrameCallback(const djvFileInfo & in)
{
    //DJV_DEBUG("djvViewMainWindow::saveFrameCallback");
    //DJV_DEBUG_PRINT("in = " << in);

    djvSequence sequence;

    const djvFrameList & frames = _p->playbackGroup->sequence().frames;

    if (frames.count())
    {
        sequence.frames += frames[_p->playbackGroup->frame()];
    }

    sequence.speed = _p->playbackGroup->speed();

    //DJV_DEBUG_PRINT("sequence = " << sequence);

    const djvViewFileSaveInfo info(
        _p->fileGroup->fileInfo(),
        in,
        _p->fileGroup->imageIoInfo()[_p->fileGroup->layer()],
        sequence,
        _p->fileGroup->layer(),
        _p->fileGroup->proxy(),
        _p->fileGroup->hasU8Conversion(),
        _p->imageGroup->hasColorProfile(),
        imageOptions());

    djvViewFileSave::global()->save(info);
}

void djvViewMainWindow::pickCallback(const djvVector2i & pick)
{
    _p->imagePick = pick;

    if (! _p->sampleInit)
    {
        _p->sampleInit = true;

        QTimer::singleShot(0, this, SLOT(viewPickUpdate()));
    }
}

void djvViewMainWindow::loadFrameStoreCallback()
{
    //DJV_DEBUG("djvViewMainWindow::loadFrameStoreCallback");

    if (_p->imageP)
    {
        _p->imageTmp = *_p->imageP;
    }
}

void djvViewMainWindow::mouseWheelCallback(djvViewInput::MOUSE_WHEEL in)
{
    _p->mouseWheel = in;

    switch (_p->mouseWheel)
    {
    case djvViewInput::MOUSE_WHEEL_PLAYBACK_SHUTTLE:

        _p->playbackGroup->setPlayback(djvView::STOP);

        _p->playbackFrameTmp = _p->playbackGroup->frame();

        break;

    case djvViewInput::MOUSE_WHEEL_PLAYBACK_SPEED:

        _p->playbackSpeedTmp =
            djvSpeed::speedToFloat(_p->playbackGroup->speed());

        break;

    default: break;
    }
}

void djvViewMainWindow::mouseWheelValueCallback(int in)
{
    switch (_p->mouseWheel)
    {
        case djvViewInput::MOUSE_WHEEL_PLAYBACK_SHUTTLE:

            _p->playbackGroup->setFrame(_p->playbackFrameTmp + in);

            break;

        case djvViewInput::MOUSE_WHEEL_PLAYBACK_SPEED:

            _p->playbackGroup->setSpeed(
                djvSpeed::floatToSpeed(
                _p->playbackSpeedTmp + static_cast<double>(in)));

            break;

        default: break;
    }
}

void djvViewMainWindow::enableUpdatesCallback()
{
    setUpdatesEnabled(true);
}

void djvViewMainWindow::fileUpdate()
{
    // Update the window title.

    const djvFileInfo & file = _p->fileGroup->fileInfo();

    const QString title = ! file.fileName().isEmpty() ?
        QString("%1 - %2").arg(DJV_APP->name()).arg(file) :
        QString("%1").arg(DJV_APP->name());

    setWindowTitle(title);
}

void djvViewMainWindow::fileCacheUpdate()
{
    //DJV_DEBUG("djvViewMainWindow::cacheUpdate");

    const double totalSize = djvViewFileCache::global()->size();
    const double maxSize   = djvViewFileCache::global()->maxSize();

    _p->infoCacheLabel->setText(QString("Cache: %1% %2/%3GB").
        arg(static_cast<int>(totalSize / maxSize * 100)).
        arg(totalSize, 0, 'f', 2).
        arg(maxSize,   0, 'f', 2));
}

void djvViewMainWindow::imageUpdate()
{
    //DJV_DEBUG("djvViewWindow::imageUpdate");

    // Update the image.

    _p->imageP = _p->fileGroup->image(_p->playbackGroup->frame());

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

    _p->infoImageLabel->setText(QString("Image: %1x%2:%3 %4").
        arg(info.size.x).
        arg(info.size.y).
        arg(djvVectorUtil::aspect(info.size), 0, 'f', 2).
        arg(djvStringUtil::label(info.pixel).join(" ")));

    //! Update the view.

    _p->viewWidget->setOptions(imageOptions());
    _p->viewWidget->setData(image);

    viewOverlayUpdate();

    _p->viewWidget->update();

    Q_EMIT imageChanged();
}

void djvViewMainWindow::controlsUpdate()
{
    //DJV_DEBUG("djvViewWindow::controlsUpdate");
    
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

    _p->fileGroup->toolBar()->setVisible(controls && visible[djvView::TOOL_BARS]);
    _p->windowGroup->toolBar()->setVisible(controls && visible[djvView::TOOL_BARS]);
    _p->viewGroup->toolBar()->setVisible(controls && visible[djvView::TOOL_BARS]);
    _p->imageGroup->toolBar()->setVisible(controls && visible[djvView::TOOL_BARS]);
    _p->toolGroup->toolBar()->setVisible(controls && visible[djvView::TOOL_BARS]);

    _p->playbackGroup->toolBar()->setVisible(controls && visible[djvView::PLAYBACK_BAR]);

    statusBar()->setVisible(controls && visible[djvView::INFO_BAR]);
    
    //windowResizeUpdate();
        
    //
    //! \todo It seems that changing the visibility of the controls is
    //! happening asynchronously so we add this extra flag to trigger the
    //! proper resizing behavior.
    //
    
    _p->viewWidgetFit = true;
    
    QTimer::singleShot(0, this, SLOT(enableUpdatesCallback()));
}

void djvViewMainWindow::viewOverlayUpdate()
{
    //DJV_DEBUG("djvViewWindow::viewOverlayUpdate");

    //! Update the HUD.

    djvViewHudInfo hudInfo;

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

    hudInfo.speed         = _p->playbackGroup->speed();
    hudInfo.realSpeed     = _p->playbackGroup->realSpeed();
    hudInfo.droppedFrames = _p->playbackGroup->hasDroppedFrames();

    _p->viewWidget->setHudInfo(hudInfo);
}

void djvViewMainWindow::viewPickUpdate()
{
    //DJV_DEBUG("djvViewWindow::viewPickUpdate");

    // Update the info bar with pixel information.

    const djvVector2i pick = djvVectorUtil::floor<double, int>(
        djvVector2f(_p->imagePick - _p->viewWidget->viewPos()) /
        _p->viewWidget->viewZoom());

    //DJV_DEBUG_PRINT("pick = " << pick);

    djvOpenGlImageOptions options = imageOptions();

    _p->imageSample = options.background;

    const djvImage * image = this->image();

    if (image && _p->windowGroup->toolBarVisible()[djvView::INFO_BAR])
    {
        //DJV_DEBUG_PRINT("sample");

        try
        {
            _p->viewWidget->makeCurrent();

            djvPixelData tmp(djvPixelDataInfo(1, image->pixel()));
            
            if (! _p->sampleBuffer || _p->sampleBuffer->info() != tmp.info())
            {
                _p->sampleBuffer.reset(
                    new djvOpenGlOffscreenBuffer(tmp.info()));
            }

            djvOpenGlImageOptions _options = options;
            _options.xform.position -= pick;

            djvOpenGlImage::copy(
                *image,
                tmp,
                _options,
                &_p->sampleState,
                _p->sampleBuffer.data());

            djvOpenGlImage::average(tmp, _p->imageSample);
        }
        catch (const djvError & error)
        {
            DJV_VIEW_APP->printError(error);
        }
    }

    //DJV_DEBUG_PRINT("sample = " << _p->imageSample);

    _p->infoSwatch->setColor(_p->imageSample);

    _p->infoPixelLabel->setText(QString("Pixel: %1, %2, %3").
        arg(pick.x).
        arg(pick.y).
        arg(djvStringUtil::label(_p->imageSample).join(" ")));

    _p->sampleInit = false;
}

void djvViewMainWindow::viewResizeUpdate()
{
    //DJV_DEBUG("djvViewMainWindow::viewResizeUpdate");

    switch (djvViewViewPrefs::global()->resize())
    {
        case djvView::VIEW_RESIZE_FIT:    _p->viewWidget->viewFit();    break;
        case djvView::VIEW_RESIZE_CENTER: _p->viewWidget->viewCenter(); break;

        default: break;
    }
}

void djvViewMainWindow::windowResizeUpdate()
{
    //DJV_DEBUG("djvViewMainWindow::windowResizeUpdate");
    //DJV_DEBUG_PRINT("size = " << width() << " " << height());
    
    // Temporarily disable updates to try and minimize flickering.
    
    setUpdatesEnabled(false);
    
    if (djvViewWindowPrefs::global()->hasResizeFit())
    {
        if (! isFullScreen())
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

const djvImage * djvViewMainWindow::image() const
{
    return _p->imageGroup->hasFrameStore() ? &_p->imageTmp : _p->imageP;
}

djvOpenGlImageOptions djvViewMainWindow::imageOptions() const
{
    // Image processing options.

    djvOpenGlImageOptions out;

    out.xform.mirror = _p->imageGroup->mirror();

    const djvImage * image = this->image();

    if (image)
    {
        out.xform.scale = djvView::imageScale(
            _p->imageGroup->scale(), image->size());
    }

    out.xform.rotate = djvView::imageRotate(_p->imageGroup->rotate());

    if (image && _p->imageGroup->hasColorProfile())
    {
        out.colorProfile = image->colorProfile;
    }

    out.displayProfile = _p->imageGroup->displayProfile();
    out.channel = _p->imageGroup->channel();
    out.background = djvViewViewPrefs::global()->background();

    return out;
}
