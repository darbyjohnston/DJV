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

//! \file djvPpmPlugin.h

#ifndef DJV_PPM_PLUGIN_H
#define DJV_PPM_PLUGIN_H

#include <djvFileIo.h>
#include <djvImageIo.h>

//! \addtogroup plugins
//@{

//! \defgroup djvPpmPlugin djvPpmPlugin
//!
//! This plugin provides support for the NetPBM image file format.
//!
//! File extensions: .ppm, pnm, .pgm, .pbm
//!
//! Supported features:
//!
//! - 1-bit, 8-bit, 16-bit, Luminance, RGB
//! - Binary and ASCII data
//!
//! References:
//!
//! - Netpbm, "PPM Format Specification"
//!   http://netpbm.sourceforge.net/doc/ppm.html

//@} // plugins

//! \addtogroup djvPpmPlugin
//@{

//------------------------------------------------------------------------------
//! \class djvPpmPlugin
//!
//! This class provides a PPM plugin.
//------------------------------------------------------------------------------

class djvPpmPlugin : public djvImageIo
{
public:

    //! Plugin name.
    
    static const QString staticName;

    //! This enumeration provides the file types.

    enum TYPE
    {
        TYPE_AUTO,
        TYPE_U1,

        TYPE_COUNT
    };

    //! Get the type labels.

    static const QStringList & typeLabels();

    //! This enumeration provides the file data types.

    enum DATA
    {
        DATA_ASCII,
        DATA_BINARY,

        DATA_COUNT
    };

    //! Get the data labels.

    static const QStringList & dataLabels();

    //! Get the number of bytes in a scanline.

    static quint64 scanlineByteCount(
        int  width,
        int  channels,
        int  bitDepth,
        DATA data);

    //! Load ASCII data.

    static void asciiLoad(
        djvFileIo & io,
        void *      out,
        int         size,
        int         bitDepth) throw (djvError);

    //! Save ASCII data.

    static quint64 asciiSave(
        const void * in,
        void *       out,
        int          size,
        int          bitDepth);

    //! This enumeration provides the options.

    enum OPTIONS
    {
        TYPE_OPTION,
        DATA_OPTION,

        OPTIONS_COUNT
    };

    //! Get the option labels.

    static const QStringList & optionsLabels();

    //! This struct provides options.

    struct Options
    {
        Options();

        TYPE type;
        DATA data;
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

DJV_STRING_OPERATOR(, djvPpmPlugin::TYPE);
DJV_STRING_OPERATOR(, djvPpmPlugin::DATA);

//@} // djvPpmPlugin

#endif // DJV_PPM_PLUGIN_H

