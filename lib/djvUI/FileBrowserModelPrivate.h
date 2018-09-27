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

#pragma once

#include <djvUI/FileBrowserModel.h>

#include <djvGraphics/ImageIO.h>

#include <QPixmap>
#include <QVariant>

#include <future>

namespace djv
{
    namespace UI
    {
        class FileBrowserThumbnailSystem;

        class FileBrowserItem : public QObject
        {
            Q_OBJECT

        public:
            explicit FileBrowserItem(
                const Core::FileInfo &,
                FileBrowserModel::THUMBNAIL_MODE,
                FileBrowserModel::THUMBNAIL_SIZE,
                const QPointer<UIContext> &,
                QObject * parent);

            //! Get the file information.
            const Core::FileInfo & fileInfo() const;

            //! Get the image information.
            const Graphics::ImageIOInfo & imageInfo() const;

            //! Get the thumbnail.
            const QPixmap & thumbnail() const;

            //! Get the display role data.
            const QVariant & displayRole(int column) const;

            //! Get the edit role data.
            const QVariant & editRole(int column) const;

        public Q_SLOTS:
            //! Request the image.
            void requestImage();

        Q_SIGNALS:
            //! This signal is emitted when the image information is available.
            void imageInfoAvailable();

            //! This signal is emitted when the thumbnail is available.
            void thumbnailAvailable();

        protected:
            void timerEvent(QTimerEvent *) override;

        private:
            void updateImageInfo();

            QPointer<UIContext> _context;
            Core::FileInfo _fileInfo;
            FileBrowserModel::THUMBNAIL_MODE _thumbnailMode = static_cast<FileBrowserModel::THUMBNAIL_MODE>(0);
            FileBrowserModel::THUMBNAIL_SIZE _thumbnailSize = static_cast<FileBrowserModel::THUMBNAIL_SIZE>(0);
            glm::ivec2 _thumbnailResolution = glm::ivec2(0, 0);
            Graphics::PixelDataInfo::PROXY _thumbnailProxy = static_cast<Graphics::PixelDataInfo::PROXY>(0);
            bool _imageInfoInit = false;
            std::future<Graphics::ImageIOInfo> _imageInfoRequest;
            int _imageInfoRequestTimer = 0;
            Graphics::ImageIOInfo _imageInfo;
            bool _thumbnailInit = false;
            std::future<QPixmap> _thumbnailRequest;
            int _thumbnailRequestTimer = 0;
            QPixmap _thumbnail;
            QVariant _displayRole[FileBrowserModel::COLUMNS_COUNT];
            QVariant _editRole[FileBrowserModel::COLUMNS_COUNT];
        };

    } // namespace UI
} // namespace djv

