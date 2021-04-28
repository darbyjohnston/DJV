// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Enum.h>

#include <djvRender2D/Data.h>

#include <djvCore/RapidJSON.h>

namespace djv
{
    namespace ViewApp
    {
        struct ImageData
        {
            Render2D::ImageChannelsDisplay channelsDisplay = Render2D::ImageChannelsDisplay::Color;
            Render2D::AlphaBlend           alphaBlend      = Render2D::AlphaBlend::Straight;

            Image::Mirror                  mirror;
            UI::ImageRotate                rotate          = UI::ImageRotate::_0;
            UI::ImageAspectRatio           aspectRatio     = UI::ImageAspectRatio::FromSource;

            bool                           colorEnabled    = true;
            Render2D::ImageColor           color;

            bool                           levelsEnabled   = true;
            Render2D::ImageLevels          levels;

            bool                           exposureEnabled = false;
            Render2D::ImageExposure        exposure;

            bool                           softClipEnabled = false;
            float                          softClip        = 0.F;

            bool operator == (const ImageData&) const;
            bool operator != (const ImageData&) const;
        };

    } // namespace ViewApp

    rapidjson::Value toJSON(const ViewApp::ImageData&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, ViewApp::ImageData&);

} // namespace djv

