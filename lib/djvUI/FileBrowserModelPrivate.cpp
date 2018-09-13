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

#include <djvUI/FileBrowserModelPrivate.h>

#include <djvUI/FileBrowserCache.h>
#include <djvUI/UIContext.h>

#include <djvGraphics/Image.h>
#include <djvGraphics/OpenGLImage.h>
#include <djvGraphics/PixelDataUtil.h>

#include <djvCore/Time.h>
#include <djvCore/VectorUtil.h>

#include <QDateTime>
#include <QFutureWatcher>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QtConcurrent/QtConcurrentRun>

namespace djv
{
    namespace UI
    {
        FileBrowserItem::FileBrowserItem(
            const djvFileInfo & fileInfo,
            FileBrowserModel::THUMBNAILS thumbnails,
            FileBrowserModel::THUMBNAILS_SIZE thumbnailsSize,
            UIContext * context,
            QObject * parent) :
            QObject(parent),
            _fileInfo(fileInfo),
            _thumbnails(thumbnails),
            _thumbnailsSize(thumbnailsSize),
            _context(context)
        {
            // Initialize the display role data.
            _displayRole[FileBrowserModel::NAME] = fileInfo.name();
            _displayRole[FileBrowserModel::SIZE] =
                djvMemory::sizeLabel(fileInfo.size());
#if ! defined(DJV_WINDOWS)
            _displayRole[FileBrowserModel::USER] =
                djvUser::uidToString(fileInfo.user());
#endif // DJV_WINDOWS
            _displayRole[FileBrowserModel::PERMISSIONS] =
                djvFileInfo::permissionsLabel(fileInfo.permissions());
            _displayRole[FileBrowserModel::TIME] =
                djvTime::timeToString(fileInfo.time());

            // Initialize the edit role data.
            _editRole[FileBrowserModel::NAME].setValue<djvFileInfo>(fileInfo);
            _editRole[FileBrowserModel::SIZE] = fileInfo.size();
#if ! defined(DJV_WINDOWS)
            _editRole[FileBrowserModel::USER] = fileInfo.user();
#endif // DJV_WINDOWS
            _editRole[FileBrowserModel::PERMISSIONS] = fileInfo.permissions();
            _editRole[FileBrowserModel::TIME] =
                QDateTime::fromTime_t(fileInfo.time());

            // Check the cache to see if this item already exists.
            if (FileBrowserCacheItem * item =
                context->fileBrowserCache()->object(_fileInfo))
            {
                _thumbnailSize = item->thumbnailSize;
                _thumbnailProxy = item->thumbnailProxy;

                _imageInfoRequest = true;
                _imageInfo = item->imageInfo;

                updateImageInfo();

                _thumbnailRequest = true;
                _thumbnail = item->thumbnail;
            }
        }

        const djvFileInfo & FileBrowserItem::fileInfo() const
        {
            return _fileInfo;
        }

        const Graphics::ImageIOInfo & FileBrowserItem::imageInfo() const
        {
            return _imageInfo;
        }

        const QPixmap & FileBrowserItem::thumbnail() const
        {
            return _thumbnail;
        }

        const QVariant & FileBrowserItem::displayRole(int column) const
        {
            return _displayRole[column];
        }

        const QVariant & FileBrowserItem::editRole(int column) const
        {
            return _editRole[column];
        }

        namespace
        {

            struct ImageInfoThreadResult
            {
                djvFileInfo           fileInfo;
                bool                  valid = false;
                Graphics::ImageIOInfo info;
            };

            ImageInfoThreadResult imageInfoThreadFunction(
                const djvFileInfo & fileInfo,
                UIContext * context)
            {
                //DJV_DEBUG("imageInfoThreadFunction");
                //DJV_DEBUG_PRINT("fileInfo = " << fileInfo);
                ImageInfoThreadResult out;
                out.fileInfo = fileInfo;
                try
                {
                    QScopedPointer<Graphics::ImageLoad> load(context->imageIOFactory()->load(fileInfo, out.info));
                    //DJV_DEBUG_PRINT("info = " << out.info);
                    out.valid = true;
                }
                catch (const djvError &)
                {
                }
                return out;
            }

            struct ThumbnailThreadResult
            {
                djvFileInfo fileInfo;
                bool        valid = false;
                QPixmap     thumbnail;
            };

            ThumbnailThreadResult thumbnailThreadFunction(
                const djvFileInfo & fileInfo,
                FileBrowserModel::THUMBNAILS thumbnails,
                const glm::ivec2 & thumbnailSize,
                Graphics::PixelDataInfo::PROXY proxy,
                UIContext * context)
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
                    // Create an OpenGL context.        
                    QScopedPointer<QOffscreenSurface> offscreenSurface;
                    offscreenSurface.reset(new QOffscreenSurface);
                    QSurfaceFormat surfaceFormat = QSurfaceFormat::defaultFormat();
                    surfaceFormat.setSwapBehavior(QSurfaceFormat::SingleBuffer);
                    surfaceFormat.setSamples(1);
                    offscreenSurface->setFormat(surfaceFormat);
                    offscreenSurface->create();
                    QScopedPointer<QOpenGLContext> openGlContext;
                    openGlContext.reset(new QOpenGLContext);
                    openGlContext->setFormat(surfaceFormat);
                    openGlContext->create();
                    openGlContext->makeCurrent(offscreenSurface.data());

                    // Load the image.
                    Graphics::Image image;
                    Graphics::ImageIOInfo imageIOInfo;
                    QScopedPointer<Graphics::ImageLoad> load(
                        context->imageIOFactory()->load(fileInfo, imageIOInfo));
                    load->read(image, Graphics::ImageIOFrameInfo(-1, 0, proxy));
                    //DJV_DEBUG_PRINT("image = " << image);

                    // Scale the image.
                    Graphics::Image tmp(Graphics::PixelDataInfo(
                        thumbnailSize,
                        image.pixel()));
                    Graphics::OpenGLImageOptions options;
                    options.xform.scale =
                        glm::vec2(tmp.size()) /
                        (glm::vec2(image.size() * Graphics::PixelDataUtil::proxyScale(image.info().proxy)));
                    options.colorProfile = image.colorProfile;
                    if (FileBrowserModel::THUMBNAILS_HIGH == thumbnails)
                    {
                        options.filter = Graphics::OpenGLImageFilter::filterHighQuality();
                    }
                    std::unique_ptr<Graphics::OpenGLImage> openGLImage(new Graphics::OpenGLImage);
                    openGLImage->copy(image, tmp, options);
                    out.thumbnail = openGLImage->toQt(tmp);
                    out.valid = true;
                }
                catch (const djvError &)
                {
                    //for (int i = 0; i < error.messages().count(); ++i)
                    //    DJV_DEBUG_PRINT("error = " << error.messages()[i].string);
                    if (!out.thumbnail.isNull())
                    {
                        out.valid = true;
                    }
                }

                return out;
            }

        } // namespace

        void FileBrowserItem::requestImage()
        {
            if (!_imageInfoRequest)
            {
                _imageInfoRequest = true;
                QFuture<ImageInfoThreadResult> future = QtConcurrent::run(
                    imageInfoThreadFunction,
                    _fileInfo,
                    _context);
                QFutureWatcher<ImageInfoThreadResult> * watcher =
                    new QFutureWatcher<ImageInfoThreadResult>;
                connect(
                    watcher,
                    SIGNAL(finished()),
                    SLOT(imageInfoCallback()));
                watcher->setFuture(future);
            }

            if (!_thumbnailRequest && djvVectorUtil::isSizeValid(_thumbnailSize))
            {
                _thumbnailRequest = true;
                QFuture<ThumbnailThreadResult> future = QtConcurrent::run(
                    thumbnailThreadFunction,
                    _fileInfo,
                    _thumbnails,
                    _thumbnailSize,
                    _thumbnailProxy,
                    _context);
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

            glm::ivec2 thumbnailSize(
                FileBrowserModel::THUMBNAILS thumbnails,
                const glm::ivec2 & in,
                int size,
                Graphics::PixelDataInfo::PROXY * proxy = 0)
            {
                const int imageSize = djvMath::max(in.x, in.y);
                if (imageSize <= 0)
                    return glm::ivec2(0, 0);
                int _proxy = 0;
                float proxyScale = static_cast<float>(
                    Graphics::PixelDataUtil::proxyScale(Graphics::PixelDataInfo::PROXY(_proxy)));
                if (FileBrowserModel::THUMBNAILS_LOW == thumbnails)
                {
                    while (
                        (imageSize / proxyScale) > size * 2 &&
                        _proxy < Graphics::PixelDataInfo::PROXY_COUNT)
                    {
                        proxyScale = static_cast<float>(
                            Graphics::PixelDataUtil::proxyScale(Graphics::PixelDataInfo::PROXY(++_proxy)));
                    }
                }
                if (proxy)
                {
                    *proxy = Graphics::PixelDataInfo::PROXY(_proxy);
                }
                const float scale = size / static_cast<float>(imageSize / proxyScale);
                return djvVectorUtil::ceil(glm::vec2(in) / proxyScale * scale);
            }

        } // namespace

        void FileBrowserItem::imageInfoCallback()
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
                Graphics::PixelDataInfo::PROXY thumbnailProxy =
                    static_cast<Graphics::PixelDataInfo::PROXY>(0);
                _thumbnailSize = thumbnailSize(
                    _thumbnails,
                    result.info.size,
                    FileBrowserModel::thumbnailsSizeValue(_thumbnailsSize),
                    &thumbnailProxy);
                //DJV_DEBUG_PRINT("thumbnailSize = " << _thumbnailSize);
                _imageInfo = result.info;
                updateImageInfo();
                Q_EMIT imageInfoAvailable();
            }

            watcher->deleteLater();
        }

        void FileBrowserItem::thumbnailCallback()
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
                _context->fileBrowserCache()->insert(
                    _fileInfo,
                    new FileBrowserCacheItem(
                        _imageInfo,
                        _thumbnailSize,
                        _thumbnailProxy,
                        _thumbnail),
                    _thumbnail.width() * _thumbnail.height() * 4);
            }
            Q_EMIT thumbnailAvailable();
            watcher->deleteLater();
        }

        void FileBrowserItem::updateImageInfo()
        {
            _displayRole[FileBrowserModel::NAME] =
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

    } // namespace UI
} // namespace djv
