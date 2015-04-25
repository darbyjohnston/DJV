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

//! \file djvFileBrowserModelPrivate.cpp

#include <djvFileBrowserModelPrivate.h>

#include <djvFileBrowserCache.h>

#include <djvImage.h>
#include <djvOpenGlContext.h>
#include <djvOpenGlImage.h>
#include <djvPixelDataUtil.h>
#include <djvTime.h>

#include <QDateTime>
#include <QFutureWatcher>
#if QT_VERSION < 0x050000
#include <QtConcurrentRun>
#else
#include <QtConcurrent/QtConcurrentRun>
#endif

//------------------------------------------------------------------------------
// djvFileBrowserItem
//------------------------------------------------------------------------------

djvFileBrowserItem::djvFileBrowserItem(
    const djvFileInfo &                  fileInfo,
    djvFileBrowserModel::THUMBNAILS      thumbnails,
    djvFileBrowserModel::THUMBNAILS_SIZE thumbnailsSize,
    QObject *                            parent) :
    QObject          (parent),
    _fileInfo        (fileInfo),
    _thumbnails      (thumbnails),
    _thumbnailsSize  (thumbnailsSize),
    _thumbnailProxy  (static_cast<djvPixelDataInfo::PROXY>(0)),
    _imageInfoRequest(false),
    _thumbnailRequest(false)
{
    // Initialize the display role data.

    _displayRole[djvFileBrowserModel::NAME] = fileInfo.name();
    _displayRole[djvFileBrowserModel::SIZE] =
        djvMemory::sizeLabel(fileInfo.size());
#if ! defined(DJV_WINDOWS)
    _displayRole[djvFileBrowserModel::USER] =
        djvUser::uidToString(fileInfo.user());
#endif // DJV_WINDOWS
    _displayRole[djvFileBrowserModel::PERMISSIONS] =
        djvFileInfo::permissionsLabel(fileInfo.permissions());
    _displayRole[djvFileBrowserModel::TIME] =
        djvTime::timeToString(fileInfo.time());

    // Initialize the edit role data.

    _editRole[djvFileBrowserModel::NAME].setValue<djvFileInfo>(fileInfo);
    _editRole[djvFileBrowserModel::SIZE] = fileInfo.size();
#if ! defined(DJV_WINDOWS)
    _editRole[djvFileBrowserModel::USER] = fileInfo.user();
#endif // DJV_WINDOWS
    _editRole[djvFileBrowserModel::PERMISSIONS] = fileInfo.permissions();
    _editRole[djvFileBrowserModel::TIME] =
        QDateTime::fromTime_t(fileInfo.time());

   // Check the cache to see if this item already exists.
   
   if (djvFileBrowserCacheItem * item =
       djvFileBrowserCache::global()->object(_fileInfo))
   {
        _imageInfoRequest = true;
        _thumbnailSize    = item->thumbnailSize;
        _thumbnailProxy   = item->thumbnailProxy;

        updateImageInfo();
       
        _thumbnailRequest = true;
        _thumbnail        = item->thumbnail;
   }
}

const djvFileInfo & djvFileBrowserItem::fileInfo() const
{
    return _fileInfo;
}

const djvImageIoInfo & djvFileBrowserItem::imageInfo() const
{
    return _imageInfo;
}

const QPixmap & djvFileBrowserItem::thumbnail() const
{
    return _thumbnail;
}

const QVariant & djvFileBrowserItem::displayRole(int column) const
{
    return _displayRole[column];
}

const QVariant & djvFileBrowserItem::editRole(int column) const
{
    return _editRole[column];
}

namespace
{

struct ImageInfoThreadResult
{
    ImageInfoThreadResult() :
        valid(false)
    {}
    
    djvFileInfo    fileInfo;
    bool           valid;
    djvImageIoInfo info;
};

ImageInfoThreadResult imageInfoThreadFunction(const djvFileInfo & fileInfo)
{
    //DJV_DEBUG("imageInfoThreadFunction");
    //DJV_DEBUG_PRINT("fileInfo = " << fileInfo);
    
    ImageInfoThreadResult out;
    out.fileInfo = fileInfo;
    
    try
    {
        QScopedPointer<djvImageLoad> load(
            djvImageIoFactory::global()->load(fileInfo, out.info));
        
        //DJV_DEBUG_PRINT("info = " << out.info);
        
        out.valid = true;
    }
    catch (const djvError & error)
    {}

    return out;
}

struct ThumbnailThreadResult
{
    ThumbnailThreadResult() :
        valid(false)
    {}
    
    djvFileInfo          fileInfo;
    bool                 valid;
    QPixmap              thumbnail;
};

ThumbnailThreadResult thumbnailThreadFunction(
    const djvFileInfo &             fileInfo,
    djvFileBrowserModel::THUMBNAILS thumbnails,
    const djvVector2i &             thumbnailSize,
    djvPixelDataInfo::PROXY         proxy)
{
    //DJV_DEBUG("thumbnailThreadFunction");
    //DJV_DEBUG_PRINT("fileInfo = " << fileInfo);
    //DJV_DEBUG_PRINT("thumbnails = " << thumbnails);
    //DJV_DEBUG_PRINT("thumbnailSize = " << thumbnailSize);
    //DJV_DEBUG_PRINT("proxy = " << proxy);

    ThumbnailThreadResult out;
    out.fileInfo = fileInfo;
    
    try
    {
        // Load the image.
        
        djvImage image;
        
        djvImageIoInfo imageIoInfo;
        
        QScopedPointer<djvImageLoad> load(
            djvImageIoFactory::global()->load(fileInfo, imageIoInfo));
        
        load->read(image, djvImageIoFrameInfo(-1, 0, proxy));

        //DJV_DEBUG_PRINT("image = " << image);
        
        // Create an OpenGL context.
        
        QScopedPointer<djvOpenGlContext> context;
        context.reset(djvOpenGlContextFactory::create());
        
        // Scale the image.
        
        djvImage tmp(djvPixelDataInfo(
            thumbnailSize,
            image.pixel()));

        djvOpenGlImageOptions options;

        options.xform.scale =
            djvVector2f(tmp.size()) /
            (djvVector2f(image.size() *
                djvPixelDataUtil::proxyScale(image.info().proxy)));

        options.colorProfile = image.colorProfile;
        
        if (djvFileBrowserModel::THUMBNAILS_HIGH == thumbnails)
        {
            options.filter = djvOpenGlImageFilter::filterHighQuality();
        }

        djvOpenGlImage::copy(image, tmp, options);

        out.thumbnail = djvPixelDataUtil::toQt(tmp);

        out.valid = true;
    }
    catch (const djvError & error)
    {
        //for (int i = 0; i < error.messages().count(); ++i)
        //    DJV_DEBUG_PRINT("error = " << error.messages()[i].string);
        
        if (! out.thumbnail.isNull())
        {
            out.valid = true;
        }
    }

    return out;
}

} // namespace

void djvFileBrowserItem::requestImage()
{
    if (! _imageInfoRequest)
    {
        _imageInfoRequest = true;
                        
        QFuture<ImageInfoThreadResult> future = QtConcurrent::run(
            imageInfoThreadFunction,
            _fileInfo);
        
        QFutureWatcher<ImageInfoThreadResult> * watcher =
            new QFutureWatcher<ImageInfoThreadResult>;
        
        connect(
            watcher,
            SIGNAL(finished()),
            SLOT(imageInfoCallback()));

        watcher->setFuture(future);
    }
    
    if (! _thumbnailRequest && _thumbnailSize > djvVector2i(0, 0))
    {
        _thumbnailRequest = true;

        QFuture<ThumbnailThreadResult> future = QtConcurrent::run(
            thumbnailThreadFunction,
            _fileInfo,
            _thumbnails,
            _thumbnailSize,
            _thumbnailProxy);
        
        QFutureWatcher<ThumbnailThreadResult> * watcher =
            new QFutureWatcher<ThumbnailThreadResult>;
        
        connect(
            watcher,
            SIGNAL(finished()),
            SLOT(thumbnailCallback()));

        watcher->setFuture(future);
    }
}

namespace
{

djvVector2i thumbnailSize(
    djvFileBrowserModel::THUMBNAILS thumbnails,
    const djvVector2i &             in,
    int                             size,
    djvPixelDataInfo::PROXY *       proxy = 0)
{
    const int imageSize = djvMath::max(in.x, in.y);

    if (imageSize <= 0)
        return djvVector2i();

    int _proxy = 0;
    
    double proxyScale = static_cast<double>(
        djvPixelDataUtil::proxyScale(djvPixelDataInfo::PROXY(_proxy)));

    if (djvFileBrowserModel::THUMBNAILS_LOW == thumbnails)
    {
        while (
            (imageSize / proxyScale) > size * 2 &&
            _proxy < djvPixelDataInfo::PROXY_COUNT)
        {
            proxyScale = static_cast<double>(
                djvPixelDataUtil::proxyScale(djvPixelDataInfo::PROXY(++_proxy)));
        }
    }
    
    if (proxy)
    {
        *proxy = djvPixelDataInfo::PROXY(_proxy);
    }

    const double scale = size / static_cast<double>(imageSize / proxyScale);
    
    return djvVectorUtil::ceil<double, int>(djvVector2f(in) / proxyScale * scale);
}

} // namespace

void djvFileBrowserItem::imageInfoCallback()
{
    //DJV_DEBUG("djvFileBrowserItem::imageInfoCallback");
    //DJV_DEBUG_PRINT("fileInfo = " << _fileInfo);
    
    QFutureWatcher<ImageInfoThreadResult> * watcher =
        dynamic_cast<QFutureWatcher<ImageInfoThreadResult> *>(sender());
    
    const QFuture<ImageInfoThreadResult> & future = watcher->future();
    
    const ImageInfoThreadResult & result = future.result();
    
    //DJV_DEBUG_PRINT("info = " << result.info);
    //DJV_DEBUG_PRINT("valid = " << result.valid);
    
    if (result.valid)
    {
        djvPixelDataInfo::PROXY thumbnailProxy =
            static_cast<djvPixelDataInfo::PROXY>(0);
        
        _thumbnailSize = thumbnailSize(
            _thumbnails,
            result.info.size,
            djvFileBrowserModel::thumbnailsSizeValue(_thumbnailsSize),
            &thumbnailProxy);
        
        //DJV_DEBUG_PRINT("thumbnailSize = " << _thumbnailSize);
            
        _imageInfo = result.info;
        
        updateImageInfo();
        
        Q_EMIT imageInfoAvailable();
    }
    
    watcher->deleteLater();
}

void djvFileBrowserItem::thumbnailCallback()
{
    //DJV_DEBUG("djvFileBrowserItem::thumbnailCallback");
    
    QFutureWatcher<ThumbnailThreadResult> * watcher =
        dynamic_cast<QFutureWatcher<ThumbnailThreadResult> *>(sender());
    
    const QFuture<ThumbnailThreadResult> & future = watcher->future();
    
    const ThumbnailThreadResult & result = future.result();
    
    //DJV_DEBUG_PRINT("file = " << result.fileInfo);
    //DJV_DEBUG_PRINT("valid = " << result.valid);
    
    if (result.valid)
    {
        _thumbnail = result.thumbnail;

        djvFileBrowserCache::global()->insert(
            _fileInfo,
            new djvFileBrowserCacheItem(
                _imageInfo,
                _thumbnailSize,
                _thumbnailProxy,
                _thumbnail),
            _thumbnail.width() * _thumbnail.height() * 4);
    }
    
    Q_EMIT thumbnailAvailable();
    
    watcher->deleteLater();
}

void djvFileBrowserItem::updateImageInfo()
{
    _displayRole[djvFileBrowserModel::NAME] =
        QString("%1\n%2x%3:%4 %5\n%6@%7").
            arg(_fileInfo.name()).
            arg(_imageInfo.size.x).
            arg(_imageInfo.size.y).
            arg(djvVectorUtil::aspect(_imageInfo.size), 0, 'f', 2).
            arg(djvStringUtil::label(_imageInfo.pixel).join(", ")).
            arg(djvTime::frameToString(
                _imageInfo.sequence.frames.count(),
                _imageInfo.sequence.speed)).
            arg(djvSpeed::speedToFloat(_imageInfo.sequence.speed));

    _thumbnail = QPixmap(_thumbnailSize.x, _thumbnailSize.y);
    _thumbnail.fill(Qt::transparent);
}
