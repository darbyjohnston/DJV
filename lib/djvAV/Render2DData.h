// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/Enum.h>
#include <djvAV/Image.h>
#include <djvAV/OCIO.h>

namespace djv
{
    namespace AV
    {
        //! This namespace provides rendering functionality.
        namespace Render2D
        {
            //! This enumeration provides which image channels are displayed.
            enum class ImageChannelDisplay
            {
                Color,
                Red,
                Green,
                Blue,
                Alpha,

                Count,
                First = Color
            };

            //! This eumeration provides the image cache options.
            enum class ImageCache
            {
                Atlas,
                Dynamic,

                Count,
                First = Atlas
            };
            
            //! This class provides color values.
            class ImageColor
            {
            public:
                ImageColor();
                
                float brightness = 1.F;
                float contrast   = 1.F;
                float saturation = 1.F;
                bool  invert     = false;

                bool operator == (const ImageColor&) const;
                bool operator != (const ImageColor&) const;
            };
            
            //! This class provides levels values.
            class ImageLevels
            {
            public:
                ImageLevels();
                
                float inLow   = 0.F;
                float inHigh  = 1.F;
                float gamma   = 1.F;
                float outLow  = 0.F;
                float outHigh = 1.F;

                bool operator == (const ImageLevels&) const;
                bool operator != (const ImageLevels&) const;
            };
            
            //! This class provides exposure values.
            class ImageExposure
            {
            public:
                ImageExposure();
                
                float exposure = 0.F;
                float defog    = 0.F;
                float kneeLow  = 0.F;
                float kneeHigh = 5.F;

                bool operator == (const ImageExposure&) const;
                bool operator != (const ImageExposure&) const;
            };

            //! This class provides image options.
            class ImageOptions
            {
            public:
                ImageOptions();
                
                ImageChannelDisplay channelDisplay    = ImageChannelDisplay::Color;
                AlphaBlend          alphaBlend        = AlphaBlend::Straight;
                AV::Image::Mirror   mirror;
                AV::OCIO::Convert   colorSpace;
                ImageColor          color;
                bool                colorEnabled      = false;
                ImageLevels         levels;
                bool                levelsEnabled     = false;
                ImageExposure       exposure;
                bool                exposureEnabled   = false;
                float               softClip          = 0.F;
                bool                softClipEnabled   = false;
                ImageCache          cache             = ImageCache::Atlas;

                bool operator == (const ImageOptions&) const;
                bool operator != (const ImageOptions&) const;
            };

            //! This eumeration provides the image filtering options.
            enum class ImageFilter
            {
                Nearest,
                Linear,

                Count,
                First = Nearest
            };

            class ImageFilterOptions
            {
            public:
                ImageFilterOptions();
                explicit ImageFilterOptions(ImageFilter minMag);
                ImageFilterOptions(ImageFilter min, ImageFilter mag);

                ImageFilter min = ImageFilter::Linear;
                ImageFilter mag = ImageFilter::Nearest;

                bool operator == (const ImageFilterOptions&) const;
                bool operator != (const ImageFilterOptions&) const;
            };

        } // namespace Render2D
    } // namespace AV
} // namespace djv

#include <djvAV/Render2DDataInline.h>
