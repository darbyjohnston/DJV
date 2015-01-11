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

//! \file djvColorProfile.h

#ifndef DJV_COLOR_PROFILE_H
#define DJV_COLOR_PROFILE_H

#include <djvPixelData.h>

#include <QMetaType>

//! \addtogroup djvCoreImage
//@{

//------------------------------------------------------------------------------
//! \class djvColorProfile
//!
//! This class provides a color profile.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvColorProfile
{
    Q_GADGET
    Q_ENUMS(PROFILE)

public:

    //! This struct provides exposure values.

    struct DJV_CORE_EXPORT Exposure
    {
        //! Constructor.
        
        Exposure(
            double value    = 0.0,
            double defog    = 0.0,
            double kneeLow  = 0.0,
            double kneeHigh = 5.0);

        double value;
        double defog;
        double kneeLow;
        double kneeHigh;
    };

    //! This enumeration provides the color profiles.

    enum PROFILE
    {
        RAW,
        GAMMA,
        LUT,
        EXPOSURE,

        PROFILE_COUNT
    };

    //! Get the color profile labels.

    static const QStringList & profileLabels();

    //! Constructor.

    djvColorProfile();

    PROFILE      type;
    double       gamma;
    djvPixelData lut;
    Exposure     exposure;
};

//------------------------------------------------------------------------------

Q_DECLARE_METATYPE(djvColorProfile)

DJV_COMPARISON_OPERATOR(DJV_CORE_EXPORT, djvColorProfile::Exposure);
DJV_COMPARISON_OPERATOR(DJV_CORE_EXPORT, djvColorProfile);

DJV_STRING_OPERATOR(DJV_CORE_EXPORT, djvColorProfile::Exposure);
DJV_STRING_OPERATOR(DJV_CORE_EXPORT, djvColorProfile::PROFILE);

DJV_DEBUG_OPERATOR(DJV_CORE_EXPORT, djvColorProfile::Exposure);
DJV_DEBUG_OPERATOR(DJV_CORE_EXPORT, djvColorProfile::PROFILE);
DJV_DEBUG_OPERATOR(DJV_CORE_EXPORT, djvColorProfile);

//@} // djvCoreImage

#endif // DJV_COLOR_PROFILE_H

