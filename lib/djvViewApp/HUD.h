// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/Color.h>
#include <djvAV/ImageData.h>

#include <djvCore/Frame.h>
#include <djvCore/PicoJSON.h>
#include <djvCore/Speed.h>

namespace djv
{
    namespace ViewApp
    {
        //! This struct provides HUD data.
        struct HUDData
        {
            std::string         fileName;
            std::string         layer;
            AV::Image::Size     size;
            AV::Image::Type     type            = AV::Image::Type::None;
            bool                isSequence      = false;
            std::string         currentFrame;
            Core::Time::Speed   speed;
            float               realSpeed       = 0.F;

            bool operator == (const HUDData&) const;
        };

        //! This enumeration provides HUD background options.
        enum class HUDBackground
        {
            None,
            Overlay,

            Count,
            First = None
        };
        DJV_ENUM_HELPERS(HUDBackground);

        //! This struct provides HUD options.
        struct HUDOptions
        {
            bool                enabled     = false;
            AV::Image::Color    color       = AV::Image::Color(1.F, 1.F, 1.F);
            HUDBackground       background  = HUDBackground::Overlay;

            bool operator == (const HUDOptions&) const;
        };

    } // namespace ViewApp

    DJV_ENUM_SERIALIZE_HELPERS(ViewApp::HUDBackground);

    picojson::value toJSON(ViewApp::HUDBackground);
    picojson::value toJSON(const ViewApp::HUDOptions&);

    //! Throws:
    //! - std::exception
    void fromJSON(const picojson::value&, ViewApp::HUDBackground&);

    //! Throws:
    //! - std::exception
    void fromJSON(const picojson::value&, ViewApp::HUDOptions&);

} // namespace djv

