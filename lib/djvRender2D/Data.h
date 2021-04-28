// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvRender2D/Enum.h>

#include <djvOCIO/OCIO.h>

#include <djvImage/Info.h>

#include <djvCore/Enum.h>
#include <djvCore/RapidJSON.h>

namespace djv
{
    namespace Render2D
    {
        //! Default DPI.
        const float dpiDefault = 96.F;
        
        //! Image channels display.
        enum class ImageChannelsDisplay
        {
            Color,
            Red,
            Green,
            Blue,
            Alpha,

            Count,
            First = Color
        };
        DJV_ENUM_HELPERS(ImageChannelsDisplay);

        //! Image caching options.
        enum class ImageCache
        {
            Atlas,
            Dynamic,

            Count,
            First = Atlas
        };
        DJV_ENUM_HELPERS(ImageCache);
        
        //! Image color values.
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
        
        //! Image levels values.
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
        
        //! Image exposure values.
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

        //! Image options.
        class ImageOptions
        {
        public:
            ImageOptions();
            
            ImageChannelsDisplay channelsDisplay = ImageChannelsDisplay::Color;
            AlphaBlend           alphaBlend      = AlphaBlend::Straight;
            Image::Mirror        mirror;
            OCIO::Convert        colorSpace;
            bool                 colorEnabled    = false;
            ImageColor           color;
            bool                 levelsEnabled   = false;
            ImageLevels          levels;
            bool                 exposureEnabled = false;
            ImageExposure        exposure;
            bool                 softClipEnabled = false;
            float                softClip        = 0.F;
            ImageCache           cache           = ImageCache::Atlas;

            bool operator == (const ImageOptions&) const;
            bool operator != (const ImageOptions&) const;
        };

        //! Image filtering options.
        enum class ImageFilter
        {
            Nearest,
            Linear,

            Count,
            First = Nearest
        };
        DJV_ENUM_HELPERS(ImageFilter);

        //! Image filter options.
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
        
        //! \name Conversion
        ///@{

        GLenum toGL(ImageFilter);

        ///@}

    } // namespace Render2D

    DJV_ENUM_SERIALIZE_HELPERS(Render2D::ImageChannelsDisplay);
    DJV_ENUM_SERIALIZE_HELPERS(Render2D::ImageCache);
    DJV_ENUM_SERIALIZE_HELPERS(Render2D::ImageFilter);

    rapidjson::Value toJSON(Render2D::ImageChannelsDisplay, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(const Render2D::ImageColor&, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(const Render2D::ImageLevels&, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(const Render2D::ImageExposure&, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(Render2D::ImageFilter, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(const Render2D::ImageFilterOptions&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Render2D::ImageChannelsDisplay&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Render2D::ImageColor&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Render2D::ImageLevels&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Render2D::ImageExposure&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Render2D::ImageFilter&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Render2D::ImageFilterOptions&);

} // namespace djv

#include <djvRender2D/DataInline.h>
