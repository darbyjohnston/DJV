//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
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

#include <djvAV/PixelData.h>

#include <QMetaType>

namespace djv
{
    namespace AV
    {
        //! This class provides a color profile.
        class ColorProfile
        {
            Q_GADGET

        public:
            ColorProfile();

            //! This struct provides exposure values.
            struct Exposure
            {
                Exposure();
                Exposure(
                    float value,
                    float defog,
                    float kneeLow,
                    float kneeHigh);

                float value = 0.f;
                float defog = 0.f;
                float kneeLow = 0.f;
                float kneeHigh = 5.f;
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
            Q_ENUM(PROFILE);

            //! Get the color profile labels.
            static const QStringList & profileLabels();

            PROFILE   type = RAW;
            float     gamma = 2.2f;
            PixelData lut;
            Exposure  exposure;
        };

    } // namespace AV

    DJV_COMPARISON_OPERATOR(AV::ColorProfile::Exposure);
    DJV_COMPARISON_OPERATOR(AV::ColorProfile);

    DJV_STRING_OPERATOR(AV::ColorProfile::Exposure);
    DJV_STRING_OPERATOR(AV::ColorProfile::PROFILE);

    DJV_DEBUG_OPERATOR(AV::ColorProfile::Exposure);
    DJV_DEBUG_OPERATOR(AV::ColorProfile::PROFILE);
    DJV_DEBUG_OPERATOR(AV::ColorProfile);

} // namespace djv

Q_DECLARE_METATYPE(djv::AV::ColorProfile)
