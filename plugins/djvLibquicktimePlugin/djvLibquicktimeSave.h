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

//! \file djvLibquicktimeSave.h

#ifndef DJV_LIBQUICKTIME_SAVE_H
#define DJV_LIBQUICKTIME_SAVE_H

#include <djvLibquicktimePlugin.h>

#include <djvImage.h>

//! \addtogroup djvLibquicktimePlugin
//@{

//------------------------------------------------------------------------------
//! \class djvLibquicktimeSave
//!
//! This class provides a libquicktime saver.
//------------------------------------------------------------------------------

class djvLibquicktimeSave : public djvImageSave
{
public:

    //! Constructor.

    djvLibquicktimeSave(const djvLibquicktimePlugin::Options & options);
    
    //! Destructor.
    
    virtual ~djvLibquicktimeSave();
    
    //! This enumeration provides error codes.
    
    enum ERROR
    {
        ERROR_SET_CODEC,
        ERROR_SET_COLOR_MODEL,
        
        ERROR_COUNT
    };
    
    //! Get the error code labels.
    
    static const QStringList & errorLabels();

    virtual void open(const djvFileInfo &, const djvImageIoInfo &)
        throw (djvError);

    virtual void write(const djvImage &, const djvImageIoFrameInfo &)
        throw (djvError);

    virtual void close() throw (djvError);

private:

    djvLibquicktimePlugin::Options _options;
    quicktime_t *                  _f;
    int                            _frame;
    int                            _frameDuration;
    djvPixelDataInfo               _info;
    djvImage                       _image;
};

//@} // djvLibquicktimePlugin

#endif // DJV_LIBQUICKTIME_SAVE_H

