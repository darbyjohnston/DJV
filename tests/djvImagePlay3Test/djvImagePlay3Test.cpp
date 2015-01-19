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

//! \file djvImagePlay3Test.cpp

#include <djvImagePlay3Test.h>

#include <djvWindowUtil.h>

#include <djvError.h>
#include <djvOpenGlImage.h>
#include <djvPixel.h>
#include <djvTimer.h>

#include <QCursor>
#include <QFutureWatcher>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QVBoxLayout>
#if QT_VERSION < 0x050000
#include <QtConcurrentRun>
#else
#include <QtConcurrent/QtConcurrentRun>
#endif

//------------------------------------------------------------------------------
// djvImagePlay3TestView
//------------------------------------------------------------------------------

void djvImagePlay3TestView::showEvent(QShowEvent *)
{
    viewFit();
}

void djvImagePlay3TestView::mousePressEvent(QMouseEvent * event)
{
    const djvVector2i mouse(
        event->pos().x(),
        height() - 1 - event->pos().y());
    
    _viewPosTmp = viewPos();
    
    _mousePress = mouse;
}

void djvImagePlay3TestView::mouseMoveEvent(QMouseEvent * event)
{
    const djvVector2i mouse(
        event->pos().x(),
        height() - 1 - event->pos().y());
    
    setViewPos(_viewPosTmp + mouse - _mousePress);
}

void djvImagePlay3TestView::keyPressEvent(QKeyEvent * event)
{
    const QPoint pos = mapFromGlobal(QCursor::pos());
    
    const djvVector2i mouse(
        pos.x(),
        height() - 1 - pos.y());
    
    switch (event->key())
    {
        case Qt::Key_0:         viewZero(); break;
        case Qt::Key_Minus:     setViewZoom(viewZoom() * 0.5, mouse); break;
        case Qt::Key_Equal:     setViewZoom(viewZoom() * 2.0, mouse); break;
        case Qt::Key_Backspace: viewFit(); break;
    }
}

//------------------------------------------------------------------------------
// djvImagePlay3TestApplication
//------------------------------------------------------------------------------

djvImagePlay3TestCacheWidget::djvImagePlay3TestCacheWidget(
    qint64        frames,
    const Cache * cache,
    QWidget *     parent) :
    QWidget(parent),
    _frames(frames),
    _cache (cache),
    _frame (0)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    
    setAttribute(Qt::WA_OpaquePaintEvent);
}

void djvImagePlay3TestCacheWidget::setFrame(qint64 frame)
{
    if (frame == _frame)
        return;

    _frame = frame;
    
    update();
}

QSize djvImagePlay3TestCacheWidget::sizeHint() const
{
    return QSize(0, 20);
}

void djvImagePlay3TestCacheWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    
    const int w = width ();
    const int h = height();
    
    painter.fillRect(0, 0, w, h, Qt::black);
    
    double v = 0.0;
    
    Q_FOREACH(qint64 frame, _cache->keys())
    {
        v = frame / static_cast<double>(_frames - 1);

        painter.setPen(_cache->object(frame) ? Qt::blue : Qt::yellow);
        painter.drawLine(
            static_cast<int>(v * w),
            0,
            static_cast<int>(v * w),
            h / 2 - 1);
    }
    
    v = _frame / static_cast<double>(_frames - 1);
    
    painter.setPen(Qt::white);
    painter.drawLine(
        static_cast<int>(v * w),
        0,
        static_cast<int>(v * w),
        h - 1);
}

//------------------------------------------------------------------------------
// djvImagePlay3TestApplication
//------------------------------------------------------------------------------

djvImagePlay3TestApplication::djvImagePlay3TestApplication(
    int     argc,
    char ** argv) :
    djvApplication("djvImagePlay3Test", argc, argv),
    _frame      (0),
    _view       (0),
    _cacheWidget(0)
{
    // Parse the command line.

    if (argc != 2)
    {
        print("Usage: djvImagePlay3Test (input)");
        
        setExitValue(djvApplicationEnum::EXIT_ERROR);
        
        return;
    }

    _fileInfo.setFileName(argv[1]);

    if (_fileInfo.isSequenceValid())
    {
        _fileInfo.setType(djvFileInfo::SEQUENCE);
    }

    djvImageIoInfo               info;
    QScopedPointer<djvImageLoad> load;

    try
    {
        load.reset(djvImageIoFactory::global()->load(_fileInfo, info));
        
        if (! load)
            throw djvError(QString("Cannot open image \"%1\"").arg(_fileInfo));
    }
    catch (const djvError & error)
    {
        printError(error);
        
        setExitValue(djvApplicationEnum::EXIT_ERROR);
        
        return;
    }

    setValid(true);

    // Create the widgets.
    
    QWidget * widget = new QWidget;
    
    _view = new djvImagePlay3TestView;
    
    _cacheWidget = new djvImagePlay3TestCacheWidget(
        _fileInfo.sequence().frames.count(),
        &_cache);

    QVBoxLayout * layout = new QVBoxLayout(widget);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(_view);
    layout->addWidget(_cacheWidget);

    _window.reset(widget);

    // Initialize.
    
    const djvVector2i size = djvWindowUtil::resize(info.size);

    _window->setWindowTitle("djvImagePlay3Test");
    _window->resize(size.x, size.y);
    _window->show();
    
    _cache.setMaxCost(1000);
    
    cacheUpdate();
    
    startTimer(0);
}

void djvImagePlay3TestApplication::timerEvent(QTimerEvent * event)
{
    //DJV_DEBUG("djvImagePlay3TestApplication::timerEvent");
    //DJV_DEBUG_PRINT("frame = " << _frame);
    
    _window->setWindowTitle(
        QString("%1 Frame: %2").arg("djvImagePlay3Test").arg(_frame));

    static djvTimer t;
    static double   average = 0.0;
    static int      accum   = 0;
    
    t.check();
    
    const double fps = t.fps();
    
    t.start();

    if (fps < 1000.0)
    {
        average += fps;
        
        ++accum;
    }

    //print(QString("FPS = %1 (%2)").
    //    arg(fps).
    //    arg(accum ? (average / static_cast<double>(accum)) : 0.0));

    djvImage * imageP = _cache.object(_frame);
    
    if (imageP && imageP->isValid())
    {
        _view->setData(imageP);
         
        djvOpenGlImageOptions options;
        options.colorProfile = imageP->colorProfile;
        _view->setOptions(options);
        
        _view->updateGL();

        ++_frame;

        if (_frame >= _fileInfo.sequence().frames.count())
        {
            _frame = 0;
        }
        
        _cacheWidget->setFrame(_frame);
    }

    cacheUpdate();
}

namespace
{

struct Image
{
    Image() :
        frame(0),
        valid(false),
        image(0)
    {}
    
    qint64     frame;
    bool       valid;
    djvImage * image;
};

Image load(
    const djvFileInfo & fileInfo,
    qint64              frame)
{
    //DJV_DEBUG("load");
    //DJV_DEBUG_PRINT("fileInfo = " << fileInfo);
    //DJV_DEBUG_PRINT("frame = " << frame);

    Image out;
    out.frame = frame;
    out.image = new djvImage;

    try
    {
        djvImageIoInfo imageIoInfo;
        
        QScopedPointer<djvImageLoad> load(
            djvImageIoFactory::global()->load(fileInfo, imageIoInfo));
        
        load->read(
            *out.image,
            djvImageIoFrameInfo(fileInfo.sequence().frames[frame]));

        //DJV_DEBUG_PRINT("image = " << *out.image);

        out.valid = true;
    }
    catch (const djvError & error)
    {
        if (out.image->isValid())
        {
            out.valid = true;
        }
    }

    return out;
}

} // namespace

void djvImagePlay3TestApplication::loadCallback()
{
    //DJV_DEBUG("djvImagePlay3TestApplication::loadCallback");
    
    QFutureWatcher<Image> * watcher =
        dynamic_cast<QFutureWatcher<Image> *>(sender());
    
    const QFuture<Image> & future = watcher->future();
    
    Image image = future.result();
    
    //DJV_DEBUG_PRINT("image = " << *image.image);
    
    _cache.insert(image.frame, image.image);
        
    watcher->deleteLater();
}

void djvImagePlay3TestApplication::cacheUpdate()
{
    for (qint64 i = _frame; i < _frame + 16; ++i)
    {
        qint64 frame = djvMath::wrap<qint64>(
            i,
            0,
            _fileInfo.sequence().frames.count() - 1);
        
        if (! _cache.contains(frame))
        {
            _cache.insert(frame, 0);
        
            QFuture<Image> future = QtConcurrent::run(load, _fileInfo, frame);

            QFutureWatcher<Image> * watcher = new QFutureWatcher<Image>;
            watcher->setFuture(future);
            
            connect(
                watcher,
                SIGNAL(finished()),
                SLOT(loadCallback()));
        }
    }
    
    _cacheWidget->update();
}

int main(int argc, char ** argv)
{
    return djvImagePlay3TestApplication(argc, argv).run();
}

