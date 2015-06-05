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

//! \file djvFileBrowserModelPrivate.h

#include <djvFileBrowserModel.h>

#include <djvImageIo.h>

#include <QPixmap>
#include <QVariant>

class djvFileBrowserItemHelper;

//------------------------------------------------------------------------------
// djvFileBrowserItem
//------------------------------------------------------------------------------

class djvFileBrowserItem : public QObject
{
    Q_OBJECT
    
public:

    //! Constructor.
    
    explicit djvFileBrowserItem(
        const djvFileInfo &                  fileInfo,
        djvFileBrowserModel::THUMBNAILS      thumbnails,
        djvFileBrowserModel::THUMBNAILS_SIZE thumbnailsSize,
        djvGuiContext *                      context,
        QObject *                            parent);

    //! Get the file information.

    const djvFileInfo & fileInfo() const;
    
    //! Get the image information.
    
    const djvImageIoInfo & imageInfo() const;
    
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

private Q_SLOTS:

    void imageInfoCallback();
    void thumbnailCallback();
    
private:

    void updateImageInfo();

    djvFileInfo                          _fileInfo;
    djvFileBrowserModel::THUMBNAILS      _thumbnails;
    djvFileBrowserModel::THUMBNAILS_SIZE _thumbnailsSize;
    djvVector2i                          _thumbnailSize;
    djvPixelDataInfo::PROXY              _thumbnailProxy;
    bool                                 _imageRequest;
    bool                                 _imageInfoRequest;
    djvImageIoInfo                       _imageInfo;
    bool                                 _thumbnailRequest;
    QPixmap                              _thumbnail;

    QVariant _displayRole[djvFileBrowserModel::COLUMNS_COUNT];
    QVariant _editRole   [djvFileBrowserModel::COLUMNS_COUNT];

    djvGuiContext *                      _context;
};
