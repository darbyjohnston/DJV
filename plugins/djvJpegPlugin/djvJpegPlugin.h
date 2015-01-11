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

//! \file djvJpegPlugin.h

#ifndef DJV_JPEG_PLUGIN_H
#define DJV_JPEG_PLUGIN_H

#include <djvImageIo.h>

#include <stdio.h>

//! \todo This namespace is meant to resolve conflicts on Windows, is it still
//! necessary?

namespace libjpeg
{
extern "C"
{
#include <jpeglib.h>
}
}

#include <setjmp.h>

#undef TRUE

//! \addtogroup plugins
//@{

//! \defgroup djvJpegPlugin djvJpegPlugin
//!
//! This plugin provides support for the Joint Photographic Experts Group
//! (JPEG) image file format.
//!
//! Requires:
//!
//! - libjpeg - http://www.ijg.org
//!
//! File extensions: .jpeg, .jpg, .jfif
//!
//! Supported features:
//!
//! - 8-bit, Luminance, RGB
//! - File compression

//@} // plugins

//! \addtogroup djvJpegPlugin
//@{

//------------------------------------------------------------------------------
//! \class djvJpegPlugin
//!
//! This class provides a JPEG plugin.
//------------------------------------------------------------------------------

class djvJpegPlugin : public djvImageIo
{
public:

    //! The plugin name.
    
    static const QString staticName;

    //! This enumeration provides the options.

    enum OPTIONS
    {
        QUALITY_OPTION,

        OPTIONS_COUNT
    };

    //! Get option labels.

    static const QStringList & optionsLabels();

    //! This struct provides options.

    struct Options
    {
        Options();

        int quality;
    };
    
    virtual djvPlugin * copyPlugin() const;

    virtual QString pluginName() const;

    virtual QStringList extensions() const;

    virtual QStringList option(const QString &) const;

    virtual bool setOption(const QString &, QStringList &);

    virtual QStringList options() const;

    virtual void commandLine(QStringList &) throw (QString);

    virtual QString commandLineHelp() const;
    
    virtual djvImageLoad * createLoad() const;
    
    virtual djvImageSave * createSave() const;

    virtual djvAbstractPrefsWidget * createWidget();

private:

    Options _options;
};

//------------------------------------------------------------------------------

//! This struct provides libjpeg error handling.

struct djvJpegErrorStruct
{
    struct libjpeg::jpeg_error_mgr pub;

    char msg [JMSG_LENGTH_MAX];

    jmp_buf jump;
};

extern "C" {

void djvJpegError(libjpeg::j_common_ptr);
void djvJpegWarning(libjpeg::j_common_ptr, int);

}

//@} // djvJpegPlugin

#endif // DJV_JPEG_PLUGIN_H

