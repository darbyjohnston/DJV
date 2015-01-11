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

//! \file djvCineon.h

#ifndef DJV_CINEON_H
#define DJV_CINEON_H

#include <djvPixelData.h>

//! \addtogroup djvCineonPlugin
//@{

//------------------------------------------------------------------------------
//! \class djvCineon
//!
//! This class provides Cineon functionality.
//------------------------------------------------------------------------------

class djvCineon
{
public:

    //! Plugin name.
    
    static const QString staticName;

    //! This enumeration provides the color profiles.

    enum COLOR_PROFILE
    {
        COLOR_PROFILE_AUTO,
        COLOR_PROFILE_RAW,
        COLOR_PROFILE_FILM_PRINT,

        COLOR_PROFILE_COUNT
    };

    //! Get the color profile labels.

    static const QStringList & colorProfileLabels();

    //! This struct provides options to convert from a linear color space to
    //! the Cineon film print color space.
    //!
    //! - Black point range = 0 - 1023, default value = 95
    //! - White point range = 0 - 1023, default value = 685
    //! - Gamma range = 0.01 - 4.0, default value = 1.7

    struct LinearToFilmPrint
    {
        //! Constructor.

        LinearToFilmPrint() :
            black(95),
            white(685),
            gamma(1.7)
        {}

        int    black;
        int    white;
        double gamma;
    };

    //! Create a linear color space to Cineon film print color space LUT.

    static djvPixelData linearToFilmPrintLut(const LinearToFilmPrint &);

    //! This struct provides options to convert from the Cineon film print
    //! color space to a linear color space.
    //!
    //! - Black point range = 0 - 1023, default value = 95
    //! - White point range = 0 - 1023, default value = 685
    //! - Gamma range = 0.01 - 4.0, default value = 1.7
    //! - Soft clip range = 0 - 50, default value = 0

    struct FilmPrintToLinear
    {
        //! Constructor.

        FilmPrintToLinear() :
            black   (95),
            white   (685),
            gamma   (1.7),
            softClip(0)
        {}

        int    black;
        int    white;
        double gamma;
        int    softClip;
    };

    //! Create a Cineon film print color space to linear space LUT.

    static djvPixelData filmPrintToLinearLut(const FilmPrintToLinear &);

    //! This enumeration provides additional image tags for Cineon files.

    enum TAG
    {
        TAG_SOURCE_OFFSET,
        TAG_SOURCE_FILE,
        TAG_SOURCE_TIME,
        TAG_SOURCE_INPUT_DEVICE,
        TAG_SOURCE_INPUT_MODEL,
        TAG_SOURCE_INPUT_SERIAL,
        TAG_SOURCE_INPUT_PITCH,
        TAG_SOURCE_GAMMA,
        TAG_FILM_FORMAT,
        TAG_FILM_FRAME,
        TAG_FILM_FRAME_RATE,
        TAG_FILM_FRAME_ID,
        TAG_FILM_SLATE,

        TAG_COUNT
    };

    //! Get the image tag labels.

    static const QStringList & tagLabels();
};

//------------------------------------------------------------------------------

bool operator == (const djvCineon::LinearToFilmPrint &,
    const djvCineon::LinearToFilmPrint &);
bool operator == (const djvCineon::FilmPrintToLinear &,
    const djvCineon::FilmPrintToLinear &);

bool operator != (const djvCineon::LinearToFilmPrint &,
    const djvCineon::LinearToFilmPrint &);
bool operator != (const djvCineon::FilmPrintToLinear &,
    const djvCineon::FilmPrintToLinear &);

DJV_STRING_OPERATOR(, djvCineon::LinearToFilmPrint);
DJV_STRING_OPERATOR(, djvCineon::FilmPrintToLinear);
DJV_STRING_OPERATOR(, djvCineon::COLOR_PROFILE);

//@} // djvCineonPlugin

#endif // DJV_CINEON_H

