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

#include <djvConfig.h>

class QColor;

namespace djv
{
    namespace AV
    {
        class Color;

        //! This class provides color utilities.
        class ColorUtil
        {
        public:
            virtual ~ColorUtil() = 0;

            //! Scale a color.
            static void scale(float, const Color &, Color &);

            //! Scale a color.
            static Color scale(float, const Color &);

            //! Interpolate a color.
            static void lerp(float, const Color &, const Color &, Color &);

            //! Interpolate a color.
            static void lerp(float, const QColor &, const QColor &, QColor &);

            //! Interpolate a color.
            static Color lerp(float, const Color &, const Color &);

            //! Interpolate a color.
            static QColor lerp(float, const QColor &, const QColor &);

            //! Convert a color.
            static void convert(const Color &, Color &);

            //! Convert RGB to HSV.
            static inline void rgbToHsv(const float[3], float[3]);

            //! Convert HSV to RGB.
            static inline void hsvToRgb(const float[3], float[3]);

            //! Convert to Qt.
            static QColor toQt(const Color &);

            //! Convert from Qt.
            static Color fromQt(const QColor &);
        };

    } // namespace AV
} // namespace djv

#include <djvAV/ColorUtilInline.h>

