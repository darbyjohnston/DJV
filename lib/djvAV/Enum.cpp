// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/Enum.h>

#include <algorithm>

using namespace djv::Core;

namespace djv
{
    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        AV,
        Side,
        DJV_TEXT("av_side_none"),
        DJV_TEXT("av_side_left"),
        DJV_TEXT("av_side_top"),
        DJV_TEXT("av_side_right"),
        DJV_TEXT("av_side_bottom"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        AV,
        SwapInterval,
        DJV_TEXT("av_swap_interval_default"),
        DJV_TEXT("av_swap_interval_0"),
        DJV_TEXT("av_swap_interval_1"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        AV,
        AlphaBlend,
        DJV_TEXT("av_alpha_none"),
        DJV_TEXT("av_alpha_straight"),
        DJV_TEXT("av_alpha_premultiplied"));

    rapidjson::Value toJSON(AV::SwapInterval value, rapidjson::Document::AllocatorType& allocator)
    {
        std::stringstream ss;
        ss << value;
        const std::string& s = ss.str();
        return rapidjson::Value(s.c_str(), s.size(), allocator);
    }

    rapidjson::Value toJSON(AV::AlphaBlend value, rapidjson::Document::AllocatorType& allocator)
    {
        std::stringstream ss;
        ss << value;
        const std::string& s = ss.str();
        return rapidjson::Value(s.c_str(), s.size(), allocator);
    }

    void fromJSON(const rapidjson::Value& value, AV::SwapInterval& out)
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

    void fromJSON(const rapidjson::Value& value, AV::AlphaBlend& out)
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

