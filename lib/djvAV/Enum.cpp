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
        AlphaBlend,
        DJV_TEXT("av_alpha_none"),
        DJV_TEXT("av_alpha_straight"),
        DJV_TEXT("av_alpha_premultiplied"));

    picojson::value toJSON(AV::AlphaBlend value)
    {
        std::stringstream ss;
        ss << value;
        return picojson::value(ss.str());
    }

    void fromJSON(const picojson::value& value, AV::AlphaBlend& out)
    {
        if (value.is<std::string>())
        {
            std::stringstream ss(value.get<std::string>());
            ss >> out;
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

} // namespace djv

