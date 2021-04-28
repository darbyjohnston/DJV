// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/Enum.h>

#include <djvImage/Color.h>
#include <djvImage/Info.h>

#include <djvMath/FrameNumber.h>
#include <djvMath/Rational.h>

#include <djvCore/RapidJSON.h>

namespace djv
{
    namespace ViewApp
    {
        //! Grid options.
        struct GridOptions
        {
            bool            enabled     = false;
            float           size        = 100.F;
            Image::Color    color       = Image::Color(0.F, 0.F, 0.F, .5F);
            GridLabels      labels      = GridLabels::X_Y;
            Image::Color    labelsColor = Image::Color(1.F, 1.F, 1.F, .65F);

            bool operator == (const GridOptions&) const;
        };

        //! HUD data.
        struct HUDData
        {
            std::string       fileName;
            std::string       layer;
            Image::Size       size;
            Image::Type       type            = Image::Type::None;
            bool              isSequence      = false;
            std::string       currentFrame;
            Math::IntRational speed;
            float             realSpeed       = 0.F;

            bool operator == (const HUDData&) const;
        };

        //! HUD options.
        struct HUDOptions
        {
            bool          enabled    = false;
            Image::Color  color      = Image::Color(1.F, 1.F, 1.F);
            HUDBackground background = HUDBackground::Overlay;

            bool operator == (const HUDOptions&) const;
        };

        //! Background options.
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

