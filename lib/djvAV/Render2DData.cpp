// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvAV/Render2DData.h>

#if defined(GetObject)
#undef GetObject
#endif // GetObject

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace Render2D
        {
            GLenum toGL(ImageFilter value)
            {
                GLenum out = GL_NONE;
                switch (value)
                {
                case ImageFilter::Nearest: out = GL_NEAREST; break;
                case ImageFilter::Linear:  out = GL_LINEAR;  break;
                default: break;
                }
                return out;
            }

        } // namespace Render2D
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        AV::Render2D,
        ImageChannelDisplay,
        DJV_TEXT("render_image_channel_display_color"),
        DJV_TEXT("render_image_channel_display_red"),
        DJV_TEXT("render_image_channel_display_green"),
        DJV_TEXT("render_image_channel_display_blue"),
        DJV_TEXT("render_image_channel_display_alpha"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        AV::Render2D,
        ImageCache,
        DJV_TEXT("render_image_cache_atlas"),
        DJV_TEXT("render_image_cache_dynamic"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        AV::Render2D,
        ImageFilter,
        DJV_TEXT("render_filter_nearest"),
        DJV_TEXT("render_filter_linear"));

    rapidjson::Value toJSON(AV::Render2D::ImageFilter value, rapidjson::Document::AllocatorType& allocator)
    {
        std::stringstream ss;
        ss << value;
        const std::string& s = ss.str();
        return rapidjson::Value(s.c_str(), s.size(), allocator);
    }

    rapidjson::Value toJSON(const AV::Render2D::ImageFilterOptions& value, rapidjson::Document::AllocatorType& allocator)
    {
        rapidjson::Value out(rapidjson::kObjectType);
        {
            out.AddMember("Min", toJSON(value.min, allocator), allocator);
            out.AddMember("Mag", toJSON(value.mag, allocator), allocator);
        }
        return out;
    }

    void fromJSON(const rapidjson::Value& value, AV::Render2D::ImageFilter& out)
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

    void fromJSON(const rapidjson::Value& value, AV::Render2D::ImageFilterOptions& out)
    {
        if (value.IsObject())
        {
            for (const auto& i : value.GetObject())
            {
                if (0 == strcmp("Min", i.name.GetString()) && i.value.IsString())
                {
                    std::stringstream ss(i.value.GetString());
                    ss >> out.min;
                }
                else if (0 == strcmp("Mag", i.name.GetString()) && i.value.IsString())
                {
                    std::stringstream ss(i.value.GetString());
                    ss >> out.mag;
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
