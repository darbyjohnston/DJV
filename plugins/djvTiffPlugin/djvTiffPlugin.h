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

//! \file djvTiffPlugin.h

#ifndef DJV_TIFF_PLUGIN_H
#define DJV_TIFF_PLUGIN_H

#include <djvImageIo.h>

#include <tiffio.h>

//! \addtogroup plugins
//@{

//! \defgroup djvTiffPlugin djvTiffPlugin
//!
//! This plugin provides support for the Tagged Image File Format (TIFF).
//!
//! Requires:
//!
//! - libtiff - http://www.libtiff.org
//!
//! File extensions: .tiff, .tif
//!
//! Supported features:
//!
//! - 8-bit, 16-bit, 32-bit float, Luminance, Luminance Alpha, RGB, RGBA
//! - Interleaved channels only
//! - File compression

//@} // plugins

//! \addtogroup djvTiffPlugin
//@{

//------------------------------------------------------------------------------
//! \class djvTiffPlugin
//!
//! This class provides a TIFF plugin.
//------------------------------------------------------------------------------

class djvTiffPlugin : public djvImageIo
{
public:

    //! The plugin name.
    
    static const QString staticName;

    //! This enumeration provides the file compression. Note that libtiff
    //! defines similar values so we prefix ours with "_".

    enum COMPRESSION
    {
        _COMPRESSION_NONE,
        _COMPRESSION_RLE,
        _COMPRESSION_LZW,

        COMPRESSION_COUNT
    };

    //! Get compression labels.

    static const QStringList & compressionLabels();

    //! Load a palette.

   static void paletteLoad(
        quint8 *  out,
        int       size,
        int       bytes,
        quint16 * red,
        quint16 * green,
        quint16 * blue);

    //! This enumeration provides the options.

    enum OPTIONS
    {
        COMPRESSION_OPTION,

        OPTIONS_COUNT
    };

    //! Get option labels.

    static const QStringList & optionsLabels();

    //! This struct provides options.

    struct Options
    {
        Options();

        COMPRESSION compression;
    };
    
    virtual void initPlugin() throw (djvError);

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

DJV_STRING_OPERATOR(, djvTiffPlugin::COMPRESSION);

//@} // djvTiffPlugin

#endif // DJV_TIFF_PLUGIN_H

