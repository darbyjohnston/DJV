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

#include <djvUI/FileBrowserModelPrivate.h>

#include <djvUI/FileBrowserCache.h>
#include <djvUI/FileBrowserThumbnailSystem.h>
#include <djvUI/UIContext.h>

#include <djvGraphics/Image.h>
#include <djvGraphics/OpenGLImage.h>
#include <djvGraphics/PixelDataUtil.h>

#include <djvCore/Time.h>
#include <djvCore/VectorUtil.h>

#include <QDateTime>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QTimerEvent>

namespace djv
{
    namespace UI
    {
        FileBrowserItem::FileBrowserItem(
            const Core::FileInfo & fileInfo,
            FileBrowserModel::THUMBNAIL_MODE thumbnailMode,
            FileBrowserModel::THUMBNAIL_SIZE thumbnailSize,
            const QPointer<UIContext> & context,
            QObject * parent) :
            QObject(parent),
            _fileInfo(fileInfo),
            _thumbnailMode(thumbnailMode),
            _thumbnailSize(thumbnailSize),
            _context(context)
        {
            // Initialize the display role data.
            _displayRole[FileBrowserModel::NAME] = fileInfo.name();
            _displayRole[FileBrowserModel::SIZE] = Core::Memory::sizeLabel(fileInfo.size());
#if ! defined(DJV_WINDOWS)
            _displayRole[FileBrowserModel::USER] =
                Core::User::uidToString(fileInfo.user());
#endif // DJV_WINDOWS
            _displayRole[FileBrowserModel::PERMISSIONS] = Core::FileInfo::permissionsLabel(fileInfo.permissions());
            _displayRole[FileBrowserModel::TIME] = Core::Time::timeToString(fileInfo.time());

            // Initialize the edit role data.
            _editRole[FileBrowserModel::NAME].setValue<Core::FileInfo>(fileInfo);
            _editRole[FileBrowserModel::SIZE] = fileInfo.size();
#if ! defined(DJV_WINDOWS)
            _editRole[FileBrowserModel::USER] = fileInfo.user();
#endif // DJV_WINDOWS
            _editRole[FileBrowserModel::PERMISSIONS] = fileInfo.permissions();
            _editRole[FileBrowserModel::TIME] = QDateTime::fromTime_t(fileInfo.time());

            // Check the cache to see if this item already exists.
            if (FileBrowserCacheItem * item = context->fileBrowserCache()->object(_fileInfo))
            {
                _thumbnailResolution = item->thumbnailResolution;
                _thumbnailProxy = item->thumbnailProxy;
                _imageInfoInit = true;
                _imageInfo = item->imageInfo;
                updateImageInfo();
                _thumbnailInit = true;
                _thumbnail = item->thumbnail;
            }
        }

        const Core::FileInfo & FileBrowserItem::fileInfo() const
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

        void FileBrowserItem::requestImage()
        {
            if (!_imageInfoInit)
            {
                _imageInfoInit = true;
                _imageInfoRequest = _context->fileBrowserThumbnailSystem()->getInfo(_fileInfo);
                _imageInfoRequestTimer = startTimer(0);
            }

            if (!_thumbnailInit && Core::VectorUtil::isSizeValid(_thumbnailResolution))
            {
                _thumbnailInit = true;
                _thumbnailRequest = _context->fileBrowserThumbnailSystem()->getPixmap(
                    _fileInfo,
                    _thumbnailMode,
                    _thumbnailResolution,
                    _thumbnailProxy);
                _thumbnailRequestTimer = startTimer(0);
            }
        }

        namespace
        {
            glm::ivec2 thumbnailSize(
                FileBrowserModel::THUMBNAIL_MODE thumbnailMode,
                const glm::ivec2 & in,
                int size,
                Graphics::PixelDataInfo::PROXY * proxy = 0)
            {
                const int imageSize = Core::Math::max(in.x, in.y);
                if (imageSize <= 0)
                    return glm::ivec2(0, 0);
                int _proxy = 0;
                float proxyScale = static_cast<float>(
                    Graphics::PixelDataUtil::proxyScale(Graphics::PixelDataInfo::PROXY(_proxy)));
                if (FileBrowserModel::THUMBNAIL_MODE_LOW == thumbnailMode)
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
                return Core::VectorUtil::ceil(glm::vec2(in) / proxyScale * scale);
            }

        } // namespace

        void FileBrowserItem::timerEvent(QTimerEvent * event)
        {
            if (_imageInfoRequestTimer == event->timerId())
            {
                if (_imageInfoRequest.valid())
                {
                    _imageInfo = _imageInfoRequest.get();
                    Graphics::PixelDataInfo::PROXY thumbnailProxy = static_cast<Graphics::PixelDataInfo::PROXY>(0);
                    _thumbnailResolution = thumbnailSize(
                        _thumbnailMode,
                        _imageInfo.size,
                        FileBrowserModel::thumbnailSizeValue(_thumbnailSize),
                        &thumbnailProxy);
                    _thumbnail = QPixmap(_thumbnailResolution.x, _thumbnailResolution.y);
                    _thumbnail.fill(Qt::transparent);
                    updateImageInfo();
                    killTimer(_imageInfoRequestTimer);
                    _imageInfoRequestTimer = 0;
                    Q_EMIT imageInfoAvailable();
                }
            }
            else if (_thumbnailRequestTimer == event->timerId())
            {
                if (_thumbnailRequest.valid())
                {
                    _thumbnail = _thumbnailRequest.get();
                    _context->fileBrowserCache()->insert(
                        _fileInfo,
                        new FileBrowserCacheItem(
                            _imageInfo,
                            _thumbnailResolution,
                            _thumbnailProxy,
                            _thumbnail),
                        _thumbnail.width() * _thumbnail.height() * 4);
                    killTimer(_thumbnailRequestTimer);
                    _thumbnailRequestTimer = 0;
                    Q_EMIT thumbnailAvailable();
                }
            }
        }

        void FileBrowserItem::updateImageInfo()
        {
            if (Core::VectorUtil::isSizeValid(_imageInfo.size))
            {
                QStringList pixelLabel;
                pixelLabel << _imageInfo.pixel;
                _displayRole[FileBrowserModel::NAME] =
                    QString("%1\n%2x%3:%4 %5\n%6@%7").
                    arg(_fileInfo.name()).
                    arg(_imageInfo.size.x).
                    arg(_imageInfo.size.y).
                    arg(Core::VectorUtil::aspect(_imageInfo.size), 0, 'f', 2).
                    arg(pixelLabel.join(", ")).
                    arg(Core::Time::frameToString(
                        _imageInfo.sequence.frames.count(),
                        _imageInfo.sequence.speed)).
                    arg(Core::Speed::speedToFloat(_imageInfo.sequence.speed));
            }
        }

    } // namespace UI
} // namespace djv
