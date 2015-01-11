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

//! \file djvCineonPlugin.h

#ifndef DJV_CINEON_PLUGIN_H
#define DJV_CINEON_PLUGIN_H

#include <djvCineon.h>

#include <djvFileIo.h>
#include <djvImageIo.h>

//! \addtogroup plugins
//@{

//! \defgroup djvCineonPlugin djvCineonPlugin
//!
//! This plugin provides support for the Kodak Cineon image file format. Cineon
//! is a specialized image file format for working with motion picture film.
//!
//! File extensions: .cin
//!
//! Supported features:
//!
//! - 10-bit RGB (the most common variety)
//! - Interleaved channels only
//!
//! References:
//!
//! - Kodak, "4.5 DRAFT - Image File Format Proposal for Digital Pictures"

//@} // plugins

//! \addtogroup djvCineonPlugin
//@{

class djvCineonHeader;

//------------------------------------------------------------------------------
//! \class djvCineonPlugin
//!
//! This class provides a Cineon plugin.
//------------------------------------------------------------------------------

class djvCineonPlugin : public djvImageIo
{
public:

    //! This enumeration provides the options.

    enum OPTIONS
    {
        INPUT_COLOR_PROFILE_OPTION,
        INPUT_FILM_PRINT_OPTION,
        OUTPUT_COLOR_PROFILE_OPTION,
        OUTPUT_FILM_PRINT_OPTION,

        OPTIONS_COUNT
    };

    //! Get the option labels.

    static const QStringList & optionsLabels();

    //! This struct provides options.

    struct Options
    {
        //! Constructor.

        Options();

        djvCineon::COLOR_PROFILE     inputColorProfile;
        djvCineon::FilmPrintToLinear inputFilmPrint;
        djvCineon::COLOR_PROFILE     outputColorProfile;
        djvCineon::LinearToFilmPrint outputFilmPrint;
    };
    
    //! Destructor.

    virtual ~djvCineonPlugin();

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

//@} // djvCineonPlugin

#endif // DJV_CINEON_PLUGIN_H

