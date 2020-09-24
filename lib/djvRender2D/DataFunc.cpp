// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvRender2D/DataFunc.h>

#include <array>
#include <sstream>

using namespace djv::Core;

namespace djv
{
    namespace Render2D
    {
        DJV_ENUM_HELPERS_IMPLEMENTATION(ImageChannelDisplay);
        DJV_ENUM_HELPERS_IMPLEMENTATION(ImageCache);
        DJV_ENUM_HELPERS_IMPLEMENTATION(ImageFilter);

    } // namespace Render2D

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Render2D,
        ImageChannelDisplay,
        DJV_TEXT("render2d_image_channel_display_color"),
        DJV_TEXT("render2d_image_channel_display_red"),
        DJV_TEXT("render2d_image_channel_display_green"),
        DJV_TEXT("render2d_image_channel_display_blue"),
        DJV_TEXT("render2d_image_channel_display_alpha"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Render2D,
        ImageCache,
        DJV_TEXT("render2d_image_cache_atlas"),
        DJV_TEXT("render2d_image_cache_dynamic"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Render2D,
        ImageFilter,
        DJV_TEXT("render2d_filter_nearest"),
        DJV_TEXT("render2d_filter_linear"));

    rapidjson::Value toJSON(Render2D::ImageChannelDisplay value, rapidjson::Document::AllocatorType& allocator)
    {
        std::stringstream ss;
        ss << value;
        const std::string& s = ss.str();
        return rapidjson::Value(s.c_str(), s.size(), allocator);
    }

    rapidjson::Value toJSON(const Render2D::ImageColor& value, rapidjson::Document::AllocatorType& allocator)
    {
        rapidjson::Value out(rapidjson::kObjectType);
        {
            out.AddMember("Brightness", toJSON(value.brightness, allocator), allocator);
            out.AddMember("Contrast", toJSON(value.contrast, allocator), allocator);
            out.AddMember("Saturation", toJSON(value.saturation, allocator), allocator);
            out.AddMember("Invert", toJSON(value.invert, allocator), allocator);
        }
        return out;
    }

    rapidjson::Value toJSON(const Render2D::ImageLevels& value, rapidjson::Document::AllocatorType& allocator)
    {
        rapidjson::Value out(rapidjson::kObjectType);
        {
            out.AddMember("InLow", toJSON(value.inLow, allocator), allocator);
            out.AddMember("InHigh", toJSON(value.inHigh, allocator), allocator);
            out.AddMember("Gamma", toJSON(value.gamma, allocator), allocator);
            out.AddMember("OutLow", toJSON(value.outLow, allocator), allocator);
            out.AddMember("OutHigh", toJSON(value.outHigh, allocator), allocator);
        }
        return out;
    }

    rapidjson::Value toJSON(const Render2D::ImageExposure& value, rapidjson::Document::AllocatorType& allocator)
    {
        rapidjson::Value out(rapidjson::kObjectType);
        {
            out.AddMember("Exposure", toJSON(value.exposure, allocator), allocator);
            out.AddMember("Defog", toJSON(value.defog, allocator), allocator);
            out.AddMember("KneeLow", toJSON(value.kneeLow, allocator), allocator);
            out.AddMember("KneeHigh", toJSON(value.kneeHigh, allocator), allocator);
        }
        return out;
    }

    rapidjson::Value toJSON(Render2D::ImageFilter value, rapidjson::Document::AllocatorType& allocator)
    {
        std::stringstream ss;
        ss << value;
        const std::string& s = ss.str();
        return rapidjson::Value(s.c_str(), s.size(), allocator);
    }

    rapidjson::Value toJSON(const Render2D::ImageFilterOptions& value, rapidjson::Document::AllocatorType& allocator)
    {
        rapidjson::Value out(rapidjson::kObjectType);
        {
            out.AddMember("Min", toJSON(value.min, allocator), allocator);
            out.AddMember("Mag", toJSON(value.mag, allocator), allocator);
        }
        return out;
    }

    void fromJSON(const rapidjson::Value& value, Render2D::ImageChannelDisplay& out)
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

    void fromJSON(const rapidjson::Value& value, Render2D::ImageColor& out)
    {
        if (value.IsObject())
        {
            for (const auto& i : value.GetObject())
            {
                if (0 == strcmp("Brightness", i.name.GetString()))
                {
                    fromJSON(i.value, out.brightness);
                }
                else if (0 == strcmp("Contrast", i.name.GetString()))
                {
                    fromJSON(i.value, out.contrast);
                }
                else if (0 == strcmp("Saturation", i.name.GetString()))
                {
                    fromJSON(i.value, out.saturation);
                }
                else if (0 == strcmp("Invert", i.name.GetString()))
                {
                    fromJSON(i.value, out.invert);
                }
            }
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    void fromJSON(const rapidjson::Value& value, Render2D::ImageLevels& out)
    {
        if (value.IsObject())
        {
            for (const auto& i : value.GetObject())
            {
                if (0 == strcmp("InLow", i.name.GetString()))
                {
                    fromJSON(i.value, out.inLow);
                }
                else if (0 == strcmp("InHigh", i.name.GetString()))
                {
                    fromJSON(i.value, out.inHigh);
                }
                else if (0 == strcmp("Gamma", i.name.GetString()))
                {
                    fromJSON(i.value, out.gamma);
                }
                else if (0 == strcmp("OutLow", i.name.GetString()))
                {
                    fromJSON(i.value, out.outLow);
                }
                else if (0 == strcmp("OutHigh", i.name.GetString()))
                {
                    fromJSON(i.value, out.outHigh);
                }
            }
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    void fromJSON(const rapidjson::Value& value, Render2D::ImageExposure& out)
    {
        if (value.IsObject())
        {
            for (const auto& i : value.GetObject())
            {
                if (0 == strcmp("Exposure", i.name.GetString()))
                {
                    fromJSON(i.value, out.exposure);
                }
                else if (0 == strcmp("Defog", i.name.GetString()))
                {
                    fromJSON(i.value, out.defog);
                }
                else if (0 == strcmp("KneeLow", i.name.GetString()))
                {
                    fromJSON(i.value, out.kneeLow);
                }
                else if (0 == strcmp("KneeHigh", i.name.GetString()))
                {
                    fromJSON(i.value, out.kneeHigh);
                }
            }
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    void fromJSON(const rapidjson::Value& value, Render2D::ImageFilter& out)
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

    void fromJSON(const rapidjson::Value& value, Render2D::ImageFilterOptions& out)
    {
        if (value.IsObject())
        {
            for (const auto& i : value.GetObject())
            {
                if (0 == strcmp("Min", i.name.GetString()))
                {
                    fromJSON(i.value, out.min);
                }
                else if (0 == strcmp("Mag", i.name.GetString()))
                {
                    fromJSON(i.value, out.mag);
                }
            }
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

} // namespace djv
