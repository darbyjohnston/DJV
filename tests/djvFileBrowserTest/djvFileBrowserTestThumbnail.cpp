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

#include <djvFileBrowserTestThumbnail.h>

#include <djvPixmapUtil.h>

#include <djvImage.h>
#include <djvGraphicsContext.h>
#include <djvPixelDataUtil.h>

#include <djvDebug.h>

#include <QMutex>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QScopedPointer>
#include <QThread>

//------------------------------------------------------------------------------
// djvFileBrowserTestThumbnailRequest
//------------------------------------------------------------------------------

djvFileBrowserTestThumbnailRequest::djvFileBrowserTestThumbnailRequest() :
    thumbnails   (static_cast<djvFileBrowserTestUtil::THUMBNAILS>(0)),
    thumbnailSize(0),
    row          (0),
    id           (0)
{}

//------------------------------------------------------------------------------
// djvFileBrowserTestThumbnailRequester
//------------------------------------------------------------------------------

djvFileBrowserTestThumbnailRequester::djvFileBrowserTestThumbnailRequester(
    QObject * parent) :
    QObject(parent)
{}

//------------------------------------------------------------------------------
// djvFileBrowserTestThumbnailResult
//------------------------------------------------------------------------------

djvFileBrowserTestThumbnailResult::djvFileBrowserTestThumbnailResult() :
    pixmapEmpty(false),
    row        (0),
    id         (0)
{}

//------------------------------------------------------------------------------
// djvFileBrowserTestThumbnailWorker::Private
//------------------------------------------------------------------------------

struct djvFileBrowserTestThumbnailWorker::Private
{
    Private(djvGraphicsContext * context) :
        context(context)
    {}
    
    djvGraphicsContext *              context          = nullptr;
    QScopedPointer<QOffscreenSurface> offscreenSurface;
    QScopedPointer<QOpenGLContext>    openGlContext;
    djvImage                          image;
    djvImage                          imageScaled;
};

//------------------------------------------------------------------------------
// djvFileBrowserTestThumbnailWorker
//------------------------------------------------------------------------------

djvFileBrowserTestThumbnailWorker::djvFileBrowserTestThumbnailWorker(
    djvGraphicsContext * context,
    QObject *            parent) :
    djvFileBrowserTestAbstractWorker(parent),
    _p(new Private(context))
{}
    
djvFileBrowserTestThumbnailWorker::~djvFileBrowserTestThumbnailWorker()
{}

void djvFileBrowserTestThumbnailWorker::request(
    const djvFileBrowserTestThumbnailRequest & request)
{
    {
        QMutexLocker locker(mutex());
        if (request.id != id())
            return;
    }

    //DJV_DEBUG("djvFileBrowserTestThumbnailWorker::request");
    //DJV_DEBUG_PRINT("fileInfo = " << request.fileInfo);
    //DJV_DEBUG_PRINT("thumbnails = " << request.thumbnails);
    //DJV_DEBUG_PRINT("thumbnailSize = " << request.thumbnailSize);
    //DJV_DEBUG_PRINT("row = " << request.row);
    //DJV_DEBUG_PRINT("id = " << request.id);
    
    djvFileBrowserTestThumbnailResult result;
    result.row = request.row;
    result.id  = request.id;
    try
    {
        // Load the image.
        QScopedPointer<djvImageLoad> load;
        load.reset(_p->context->imageIOFactory()->load(request.fileInfo, result.info));
        djvPixelDataInfo::PROXY proxy = djvPixelDataInfo::PROXY_NONE;
        glm::ivec2 size = djvFileBrowserTestUtil::thumbnailSize(
            request.thumbnails,
            request.thumbnailSize,
            result.info.size,
            &proxy);
        //DJV_DEBUG_PRINT("size = " << size);
        load->read(_p->image, djvImageIOFrameInfo(-1, 0, proxy));
        //DJV_DEBUG_PRINT("image = " << _p->image);
        
        // Scale the image.
        _p->imageScaled.set(djvPixelDataInfo(size, _p->image.pixel()));
        djvOpenGLImageOptions options;
        options.xform.scale =
            glm::vec2(_p->imageScaled.size()) /
            (glm::vec2(_p->image.size() * djvPixelDataUtil::proxyScale(_p->image.info().proxy)));
        //DJV_DEBUG_PRINT("scale = " << options.xform.scale);
        options.colorProfile = _p->image.colorProfile;
        if (djvFileBrowserTestUtil::THUMBNAILS_HIGH == request.thumbnails)
        {
            options.filter = djvOpenGLImageFilter::filterHighQuality();
        }
        djvOpenGLImage::copy(_p->image, _p->imageScaled, options);
        result.pixmap = djvPixmapUtil::toQt(_p->imageScaled);
    }
    catch (djvError error)
    {}

    Q_EMIT this->result(result);
}

void djvFileBrowserTestThumbnailWorker::start()
{
    //DJV_DEBUG("djvFileBrowserTestThumbnailWorker::start");
    _p->offscreenSurface.reset(new QOffscreenSurface);
    QSurfaceFormat surfaceFormat;
    surfaceFormat.setSamples(1);
    surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
    surfaceFormat.setSwapBehavior(QSurfaceFormat::SingleBuffer);
    _p->offscreenSurface->setFormat(surfaceFormat);
    _p->offscreenSurface->create();
    _p->openGlContext.reset(new QOpenGLContext);
    _p->openGlContext->setFormat(surfaceFormat);
    _p->openGlContext->create();
    _p->openGlContext->makeCurrent(_p->offscreenSurface.data());
}

void djvFileBrowserTestThumbnailWorker::finish()
{
    //DJV_DEBUG("djvFileBrowserTestThumbnailWorker::finish");
    _p->openGlContext.reset();
    _p->offscreenSurface.reset();
}

//------------------------------------------------------------------------------
// djvFileBrowserTestThumbnail
//------------------------------------------------------------------------------

djvFileBrowserTestThumbnail::djvFileBrowserTestThumbnail(
    djvGraphicsContext * context,
    QObject *            parent) :
    QObject(parent),
    _context    (context),
    _threadIndex(0)
{
    static const int threads = 12;
    for (int i = 0; i < threads; ++i)
    {
        _requesters.append(new djvFileBrowserTestThumbnailRequester);
        _workers.append(new djvFileBrowserTestThumbnailWorker(context));
        _threads.append(new QThread);
    }
    for (int i = 0; i < threads; ++i)
    {
        connect(
            _workers[i],
            SIGNAL(result(const djvFileBrowserTestThumbnailResult &)),
            SIGNAL(result(const djvFileBrowserTestThumbnailResult &)));
        _workers[i]->connect(
            _requesters[i],
            SIGNAL(request(const djvFileBrowserTestThumbnailRequest &)),
            SLOT(request(const djvFileBrowserTestThumbnailRequest &)));
        _workers[i]->connect(
            _threads[i],
            SIGNAL(started()),
            SLOT(start()));
        _workers[i]->connect(
            _threads[i],
            SIGNAL(finished()),
            SLOT(finish()));
    }
    for (int i = 0; i < threads; ++i)
    {
        _workers[i]->moveToThread(_threads[i]);
        _threads[i]->start();
        //DJV_DEBUG_PRINT("thread = " << qint64(_workers[i]->thread()));
    }
}

djvFileBrowserTestThumbnail::~djvFileBrowserTestThumbnail()
{
    for (int i = 0; i < _threads.count(); ++i)
    {
        _threads[i]->quit();
    }
    for (int i = 0; i < _threads.count(); ++i)
    {
        _threads[i]->wait();
    }
    for (int i = 0; i < _threads.count(); ++i)
    {
        delete _threads[i];
        delete _workers[i];
        delete _requesters[i];
    }
}

void djvFileBrowserTestThumbnail::request(
    const djvFileBrowserTestThumbnailRequest & request)
{
    Q_EMIT nextRequester()->request(request);
}

void djvFileBrowserTestThumbnail::setId(quint64 id)
{
    for (int i = 0; i < _workers.count(); ++i)
    {
        QMutexLocker locker(_workers[i]->mutex());
        _workers[i]->setId(id);
    }
}

djvFileBrowserTestThumbnailRequester *
djvFileBrowserTestThumbnail::nextRequester()
{
    djvFileBrowserTestThumbnailRequester * requester = _requesters[_threadIndex];
    ++_threadIndex;
    if (_threadIndex >= _requesters.count())
    {
        _threadIndex = 0;
    }
    return requester;
}

