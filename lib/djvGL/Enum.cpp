// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvGL/Enum.h>

#include <algorithm>
#include <array>

using namespace djv::Core;

namespace djv
{
    namespace GL
    {
        DJV_ENUM_HELPERS_IMPLEMENTATION(SwapInterval);
    }

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        GL,
        SwapInterval,
        DJV_TEXT("glfw_swap_interval_default"),
        DJV_TEXT("glfw_swap_interval_0"),
        DJV_TEXT("glfw_swap_interval_1"));

    rapidjson::Value toJSON(GL::SwapInterval value, rapidjson::Document::AllocatorType& allocator)
    {
        std::stringstream ss;
        ss << value;
        const std::string& s = ss.str();
        return rapidjson::Value(s.c_str(), s.size(), allocator);
    }

    void fromJSON(const rapidjson::Value& value, GL::SwapInterval& out)
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

