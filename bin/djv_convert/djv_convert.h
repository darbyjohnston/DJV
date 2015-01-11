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

//! \file djv_convert.h

#ifndef DJV_CONVERT_H
#define DJV_CONVERT_H

#include <djvFileInfo.h>
#include <djvImage.h>
#include <djvImageApplication.h>
#include <djvOpenGlImage.h>
#include <djvOpenGlOffscreenBuffer.h>

//! \addtogroup bin
//@{

//! \defgroup djv_convert djv_convert
//!
//! This program provides a command line tool for image and movie conversion.

//@} // bin

//! \addtogroup djv_convert
//@{

//------------------------------------------------------------------------------
//! \struct djvConvertOptions
//!
//! This struct provides conversion options.
//------------------------------------------------------------------------------

struct djvConvertOptions
{
    //! Constructor.

    djvConvertOptions();

    djvPixelDataInfo::Mirror       mirror;
    djvVector2f                    scale;
    djvOpenGlImageOptions::CHANNEL channel;
    djvVector2i                    size;
    djvBox2i                       crop;
    djvBox2f                       cropPercent;
    djvSequence::COMPRESS          sequence;
};

//------------------------------------------------------------------------------
//! \struct djvConvertInput
//!
//! This struct provides input options.
//------------------------------------------------------------------------------

struct djvConvertInput
{
    //! Constructor.

    djvConvertInput();

    djvFileInfo             file;
    int                     layer;
    djvPixelDataInfo::PROXY proxy;
    QString                 start;
    QString                 end;
    djvFileInfo             slate;
    int                     slateFrames;
    int                     timeout;
};

//------------------------------------------------------------------------------
//! \struct djvConvertOutput
//!
//! This struct provides output options.
//------------------------------------------------------------------------------

struct djvConvertOutput
{
    //! Constructor.

    djvConvertOutput();

    djvFileInfo                     file;
    QScopedPointer<djvPixel::PIXEL> pixel;
    djvImageTags                    tags;
    bool                            tagsAuto;
    QScopedPointer<djvSpeed::FPS>   speed;
};

//------------------------------------------------------------------------------
//! \class djvConvertApplication
//!
//! The class provides the application.
//------------------------------------------------------------------------------

class djvConvertApplication : public djvImageApplication
{
    Q_OBJECT
    
public:

    //! Constructor.

    djvConvertApplication(int, char **) throw (djvError);

    //! Parse the command line.

    void commandLine(QStringList &) throw (QString);
    
    //! This enumeration provides error codes.
    
    enum ERROR
    {
        ERROR_OPEN,
        ERROR_OPEN_SLATE,
        ERROR_READ,
        ERROR_WRITE,
        
        ERROR_COUNT
    };
    
    //! Get the error code labels.
    
    static const QStringList & errorLabels();

    virtual QString commandLineHelp() const;

private Q_SLOTS:

    void debugLogCallback(const QString &);

private:

    bool work();
    
    QString labelImage(const djvPixelDataInfo &, const djvSequence &) const;

    djvConvertOptions                        _options;
    djvConvertInput                          _input;
    djvConvertOutput                         _output;
    QScopedPointer<djvOpenGlOffscreenBuffer> _offscreenBuffer;
    djvOpenGlImageState                      _state;
};

//@} // djv_convert

#endif // DJV_CONVERT_H

