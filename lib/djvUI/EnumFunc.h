// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Enum.h>

#include <djvCore/Enum.h>
#include <djvCore/RapidJSONFunc.h>

#include <sstream>
#include <vector>

namespace djv
{
    namespace UI
    {
        Orientation getOpposite(Orientation);

        float getImageRotate(ImageRotate);

        float getImageAspectRatio(ImageAspectRatio);
        float getPixelAspectRatio(ImageAspectRatio, float pixelAspectRatio);
        float getAspectRatioScale(ImageAspectRatio, float aspectRatio);

        DJV_ENUM_HELPERS(Orientation);
        DJV_ENUM_HELPERS(Side);
        DJV_ENUM_HELPERS(Corner);
        DJV_ENUM_HELPERS(Expand);
        DJV_ENUM_HELPERS(HAlign);
        DJV_ENUM_HELPERS(VAlign);
        DJV_ENUM_HELPERS(TextHAlign);
        DJV_ENUM_HELPERS(TextVAlign);
        DJV_ENUM_HELPERS(SelectionType);
        DJV_ENUM_HELPERS(SortOrder);
        DJV_ENUM_HELPERS(ButtonType);
        DJV_ENUM_HELPERS(ColorRole);
        DJV_ENUM_HELPERS(MetricsRole);
        DJV_ENUM_HELPERS(ImageRotate);
        DJV_ENUM_HELPERS(ImageAspectRatio);
        DJV_ENUM_HELPERS(Popup);

    } // namespace UI

    DJV_ENUM_SERIALIZE_HELPERS(UI::Orientation);
    DJV_ENUM_SERIALIZE_HELPERS(UI::Side);
    DJV_ENUM_SERIALIZE_HELPERS(UI::Corner);
    DJV_ENUM_SERIALIZE_HELPERS(UI::Expand);
    DJV_ENUM_SERIALIZE_HELPERS(UI::HAlign);
    DJV_ENUM_SERIALIZE_HELPERS(UI::VAlign);
    DJV_ENUM_SERIALIZE_HELPERS(UI::TextHAlign);
    DJV_ENUM_SERIALIZE_HELPERS(UI::TextVAlign);
    DJV_ENUM_SERIALIZE_HELPERS(UI::SelectionType);
    DJV_ENUM_SERIALIZE_HELPERS(UI::SortOrder);
    DJV_ENUM_SERIALIZE_HELPERS(UI::ButtonType);
    DJV_ENUM_SERIALIZE_HELPERS(UI::ViewType);
    DJV_ENUM_SERIALIZE_HELPERS(UI::ColorRole);
    DJV_ENUM_SERIALIZE_HELPERS(UI::MetricsRole);
    DJV_ENUM_SERIALIZE_HELPERS(UI::ImageRotate);
    DJV_ENUM_SERIALIZE_HELPERS(UI::ImageAspectRatio);
    DJV_ENUM_SERIALIZE_HELPERS(UI::Popup);

    rapidjson::Value toJSON(UI::ViewType, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(UI::ImageRotate, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(UI::ImageAspectRatio, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, UI::ViewType&);
    void fromJSON(const rapidjson::Value&, UI::ImageRotate&);
    void fromJSON(const rapidjson::Value&, UI::ImageAspectRatio&);

} // namespace djv

