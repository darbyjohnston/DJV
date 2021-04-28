// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvRender2D/Enum.h>

#include <algorithm>
#include <array>

using namespace djv::Core;

namespace djv
{
    namespace Render2D
    {
        DJV_ENUM_HELPERS_IMPLEMENTATION(Side);
        DJV_ENUM_HELPERS_IMPLEMENTATION(AlphaBlend);
    }

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Render2D,
        Side,
        DJV_TEXT("render2d_side_none"),
        DJV_TEXT("render2d_side_left"),
        DJV_TEXT("render2d_side_top"),
        DJV_TEXT("render2d_side_right"),
        DJV_TEXT("render2d_side_bottom"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Render2D,
        AlphaBlend,
        DJV_TEXT("render2d_alpha_none"),
        DJV_TEXT("render2d_alpha_straight"),
        DJV_TEXT("render2d_alpha_premultiplied"));

    rapidjson::Value toJSON(Render2D::AlphaBlend value, rapidjson::Document::AllocatorType& allocator)
    {
        std::stringstream ss;
        ss << value;
        const std::string& s = ss.str();
        return rapidjson::Value(s.c_str(), s.size(), allocator);
    }

    void fromJSON(const rapidjson::Value& value, Render2D::AlphaBlend& out)
    {
        if (value.IsString())
        {
            std::stringstream ss(value.GetString());
            ss >> out;
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

} // namespace djv

