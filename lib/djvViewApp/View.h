// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/Enum.h>

#include <djvAV/Color.h>
#include <djvAV/ImageData.h>

#include <djvCore/FrameNumber.h>
#include <djvCore/RapidJSON.h>
#include <djvCore/Speed.h>

namespace djv
{
    namespace ViewApp
    {
        //! This struct provides grid options.
        struct GridOptions
        {
            bool                enabled     = false;
            float               size        = 100.F;
            AV::Image::Color    color       = AV::Image::Color(0.F, 0.F, 0.F, .5F);
            GridLabels          labels      = GridLabels::X_Y;
            AV::Image::Color    labelsColor = AV::Image::Color(1.F, 1.F, 1.F, .65F);

            bool operator == (const GridOptions&) const;
        };

        //! This struct provides HUD data.
        struct HUDData
        {
            std::string          fileName;
            std::string          layer;
            AV::Image::Size      size;
            AV::Image::Type      type            = AV::Image::Type::None;
            bool                 isSequence      = false;
            std::string          currentFrame;
            Core::Math::Rational speed;
            float                realSpeed       = 0.F;

            bool operator == (const HUDData&) const;
        };

        //! This struct provides HUD options.
        struct HUDOptions
        {
            bool                enabled     = false;
            AV::Image::Color    color       = AV::Image::Color(1.F, 1.F, 1.F);
            HUDBackground       background  = HUDBackground::Overlay;

            bool operator == (const HUDOptions&) const;
        };

        //! This struct provides background options.
        struct ViewBackgroundOptions
        {
            ViewBackground      background          = ViewBackground::First;
            AV::Image::Color    color               = AV::Image::Color(0.F, 0.F, 0.F);
            float               checkersSize        = 20.F;
            AV::Image::Color    checkersColors[2]   = { AV::Image::Color(.6F), AV::Image::Color(.4F) };
            bool                border              = false;
            float               borderWidth         = 2.F;
            AV::Image::Color    borderColor         = AV::Image::Color(.8F);

            bool operator == (const ViewBackgroundOptions&) const;
        };

    } // namespace ViewApp

    rapidjson::Value toJSON(const ViewApp::GridOptions&, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(const ViewApp::HUDOptions&, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(const ViewApp::ViewBackgroundOptions&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, ViewApp::GridOptions&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, ViewApp::HUDOptions&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, ViewApp::ViewBackgroundOptions&);

} // namespace djv

