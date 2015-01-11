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

//! \file djvLutPlugin.h

#ifndef DJV_LUT_PLUGIN_H
#define DJV_LUT_PLUGIN_H

#include <djvFileIo.h>
#include <djvImageIo.h>

//! \addtogroup plugins
//@{

//! \defgroup djvLutPlugin djvLutPlugin
//!
//! This plugin provides support for two-dimensional lookup table file formats.
//!
//! File extensions: .lut, .1dl
//!
//! Supported features:
//!
//! - Inferno and Kodak formats
//! - 8-bit, 16-bit, Luminance, Luminance Alpha, RGB, RGBA; 10-bit RGB
//!
//! \todo Add support for detecting whether the input LUT file is horizontal
//! or vertical.

//@} // plugins

//! \addtogroup djvLutPlugin
//@{

//------------------------------------------------------------------------------
//! \class djvLutPlugin
//!
//! This class provides a LUT plugin.
//------------------------------------------------------------------------------

class djvLutPlugin : public djvImageIo
{
public:

    //! Plugin name.

    static const QString staticName;
    
    //! Plugin extensions.
    
    static const QStringList staticExtensions;

    //! This enumeration provides the file formats.

    enum FORMAT
    {
        FORMAT_INFERNO,
        FORMAT_KODAK,

        FORMAT_COUNT
    };

    //! Get the file format labels.

    static const QStringList & formatLabels();

    //! This enumeration provides the file types.

    enum TYPE
    {
        TYPE_AUTO,
        TYPE_U8,
        TYPE_U10,
        TYPE_U16,

        TYPE_COUNT
    };

    //! Get the file type labels.

    static const QStringList & typeLabels();

    //! Open an Inferno LUT.

    static void infernoOpen(djvFileIo &, djvPixelDataInfo &, TYPE)
        throw (djvError);

    //! Load an Inferno LUT.

    static void infernoLoad(djvFileIo &, djvImage &) throw (djvError);

    //! Open a Kodak LUT.

    static void kodakOpen(djvFileIo &, djvPixelDataInfo &, TYPE)
        throw (djvError);

    //! Load a Kodak LUT.

    static void kodakLoad(djvFileIo &, djvImage &) throw (djvError);

    //! Open an Inferno LUT.

    static void infernoOpen(djvFileIo &, const djvPixelDataInfo &)
        throw (djvError);

    //! Save an Inferno LUT.

    static void infernoSave(djvFileIo & io, const djvPixelData *)
        throw (djvError);

    //! Open a Kodak LUT.

    static void kodakOpen(djvFileIo &, const djvPixelDataInfo &)
        throw (djvError);

    //! Save a Kodak LUT.

    static void kodakSave(djvFileIo &, const djvPixelData *)
        throw (djvError);

    //! This enumeration provides the options.

    enum OPTIONS
    {
        TYPE_OPTION,

        OPTIONS_COUNT
    };

    //! Get the option labels.

    static const QStringList & optionsLabels();

    //! This struct provides options.

    struct Options
    {
        //! Constructor.

        Options();

        TYPE   type;
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

DJV_STRING_OPERATOR(, djvLutPlugin::FORMAT);
DJV_STRING_OPERATOR(, djvLutPlugin::TYPE);

//@} // djvLutPlugin

#endif // DJV_LUT_H

