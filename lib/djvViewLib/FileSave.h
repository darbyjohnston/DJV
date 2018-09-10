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

#pragma once

#include <djvViewLib/Core.h>

#include <djvGraphics/ImageIO.h>
#include <djvGraphics/OpenGLImage.h>
#include <djvGraphics/Pixel.h>

#include <djvCore/FileInfo.h>

#include <QObject>

#include <memory>

class djvViewContext;

//! \addtogroup djvViewFile
//@{

//------------------------------------------------------------------------------
//! \struct djvViewFileSaveInfo
//!
//! This struct provides file saving information.
//------------------------------------------------------------------------------

struct djvViewFileSaveInfo
{
    djvViewFileSaveInfo(
        const djvFileInfo &           inputFile    = djvFileInfo(),
        const djvFileInfo &           outputFile   = djvFileInfo(),
        const djvPixelDataInfo &      info         = djvPixelDataInfo(),
        const djvSequence &           sequence     = djvSequence(),
        int                           layer        = 0,
        const djvPixelDataInfo::PROXY proxy        = djvPixelDataInfo::PROXY_NONE,
        bool                          u8Conversion = false,
        bool                          colorProfile = true,
        const djvOpenGLImageOptions & options      = djvOpenGLImageOptions());

    djvFileInfo             inputFile;
    djvFileInfo             outputFile;
    djvPixelDataInfo        info;
    djvSequence             sequence;
    int                     layer;
    djvPixelDataInfo::PROXY proxy;
    bool                    u8Conversion;
    bool                    colorProfile;
    djvImageIOFrameInfo     frameInfo;
    djvOpenGLImageOptions   options;
};

//------------------------------------------------------------------------------
//! \class djvViewFileSave
//!
//! This class provides file saving.
//------------------------------------------------------------------------------

class djvViewFileSave : public QObject
{
    Q_OBJECT
    
public:
    explicit djvViewFileSave(djvViewContext *, QObject * parent = nullptr);

    virtual ~djvViewFileSave();

public Q_SLOTS:
    //! Save a file.
    void save(const djvViewFileSaveInfo &);

    //! Cancel an in progress save.
    void cancel();

Q_SIGNALS:
    //! This signal is emitted when the save is finished.
    void finished();

private Q_SLOTS:
    void callback(int);
    void finishedCallback();

private:    
    DJV_PRIVATE_COPY(djvViewFileSave);
    
    struct Private;
    std::unique_ptr<Private> _p;
};

//@} // djvViewFile

