// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/AnnotateFunc.h>

#include <array>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        float getAnnotateLineSize(AnnotateLineSize value)
        {
            const std::vector<float> data =
            {
                1.F,
                5.F,
                10.F
            };
            return data[static_cast<size_t>(value)];
        }

        DJV_ENUM_HELPERS_IMPLEMENTATION(AnnotateTool);
        DJV_ENUM_HELPERS_IMPLEMENTATION(AnnotateLineSize);

    } // namespace ViewApp

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        ViewApp,
        AnnotateTool,
        DJV_TEXT("annotate_tool_polyline"),
        DJV_TEXT("annotate_tool_line"),
        DJV_TEXT("annotate_tool_rectangle"),
        DJV_TEXT("annotate_tool_ellipse"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        ViewApp,
        AnnotateLineSize,
        DJV_TEXT("annotate_line_size_small"),
        DJV_TEXT("annotate_line_size_medium"),
        DJV_TEXT("annotate_line_size_large"));

    rapidjson::Value toJSON(ViewApp::AnnotateTool value, rapidjson::Document::AllocatorType& allocator)
    {
        std::stringstream ss;
        ss << value;
        const std::string& s = ss.str();
        return rapidjson::Value(s.c_str(), s.size(), allocator);
    }

    rapidjson::Value toJSON(ViewApp::AnnotateLineSize value, rapidjson::Document::AllocatorType& allocator)
    {
        std::stringstream ss;
        ss << value;
        const std::string& s = ss.str();
        return rapidjson::Value(s.c_str(), s.size(), allocator);
    }

    void fromJSON(const rapidjson::Value& value, ViewApp::AnnotateTool& out)
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

    void fromJSON(const rapidjson::Value& value, ViewApp::AnnotateLineSize& out)
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
