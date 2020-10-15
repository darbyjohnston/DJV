// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/Enum.h>

#include <djvImage/Color.h>
#include <djvImage/Info.h>

#include <djvMath/FrameNumber.h>
#include <djvMath/Rational.h>

namespace djv
{
    namespace ViewApp
    {
        //! This struct provides grid options.
        struct GridOptions
        {
            bool            enabled     = false;
            float           size        = 100.F;
            Image::Color    color       = Image::Color(0.F, 0.F, 0.F, .5F);
            GridLabels      labels      = GridLabels::X_Y;
            Image::Color    labelsColor = Image::Color(1.F, 1.F, 1.F, .65F);

            bool operator == (const GridOptions&) const;
        };

        //! This struct provides HUD data.
        struct HUDData
        {
            std::string    fileName;
            std::string    layer;
            Image::Size    size;
            Image::Type    type            = Image::Type::None;
            bool           isSequence      = false;
            std::string    currentFrame;
            Math::Rational speed;
            float          realSpeed       = 0.F;

            bool operator == (const HUDData&) const;
        };

        //! This struct provides HUD options.
        struct HUDOptions
        {
            bool          enabled    = false;
            Image::Color  color      = Image::Color(1.F, 1.F, 1.F);
            HUDBackground background = HUDBackground::Overlay;

            bool operator == (const HUDOptions&) const;
        };

        //! This struct provides background options.
        struct ViewBackgroundOptions
        {
            ViewBackground background        = ViewBackground::First;
            Image::Color   color             = Image::Color(0.F, 0.F, 0.F);
            float          checkersSize      = 20.F;
            Image::Color   checkersColors[2] = { Image::Color(.6F), Image::Color(.4F) };
            bool           border            = false;
            float          borderWidth       = 2.F;
            Image::Color   borderColor       = Image::Color(.8F);

            bool operator == (const ViewBackgroundOptions&) const;
        };

    } // namespace ViewApp
} // namespace djv

