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

#include <djvFileInfo.h>
#include <djvImage.h>
#include <djvImageContext.h>
#include <djvOpenGlImage.h>

//! \addtogroup djv_convert
//@{

//------------------------------------------------------------------------------
//! \struct djvConvertOptions
//!
//! This struct provides conversion options.
//------------------------------------------------------------------------------

struct djvConvertOptions
{
    djvConvertOptions();

    djvPixelDataInfo::Mirror       mirror;
    glm::vec2                      scale = glm::vec2(1.f, 1.f);
    djvOpenGlImageOptions::CHANNEL channel;
    glm::ivec2                     size = glm::ivec2(0, 0);
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
    djvConvertOutput();

    djvFileInfo                     file;
    QScopedPointer<djvPixel::PIXEL> pixel;
    djvImageTags                    tags;
    bool                            tagsAuto;
    QScopedPointer<djvSpeed::FPS>   speed;
};

//------------------------------------------------------------------------------
//! \class djvConvertContext
//!
//! This class provides global functionality for the application.
//------------------------------------------------------------------------------

class djvConvertContext : public djvImageContext
{
    Q_OBJECT
    
public:
    explicit djvConvertContext(QObject * parent = nullptr);

    virtual ~djvConvertContext();
    
    //! Get the options.
    const djvConvertOptions & options() const;
    
    //! Get the input options.
    const djvConvertInput & input() const;
    
    //! Get the output options.
    const djvConvertOutput & output() const;

protected:
    virtual bool commandLineParse(QStringList &) throw (QString);

    virtual QString commandLineHelp() const;

private:
    djvConvertOptions                        _options;
    djvConvertInput                          _input;
    djvConvertOutput                         _output;
};

//@} // djv_convert

