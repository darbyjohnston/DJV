// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvOCIO/OCIOSystemFunc.h>

#include <array>
#include <sstream>

// These need to be included last on macOS.
#include <djvCore/RapidJSONTemplates.h>

namespace djv
{
    namespace OCIO
    {
        DJV_ENUM_HELPERS_IMPLEMENTATION(ConfigMode);

    } // namespace OCIO

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        OCIO,
        ConfigMode,
        DJV_TEXT("ocio_config_mode_none"),
        DJV_TEXT("ocio_config_mode_cmd_line"),
        DJV_TEXT("ocio_config_mode_env"),
        DJV_TEXT("ocio_config_mode_user"));

    rapidjson::Value toJSON(OCIO::ConfigMode value, rapidjson::Document::AllocatorType& allocator)
    {
        std::stringstream ss;
        ss << value;
        const std::string& s = ss.str();
        return rapidjson::Value(s.c_str(), s.size(), allocator);
    }

    rapidjson::Value toJSON(const OCIO::Config& value, rapidjson::Document::AllocatorType& allocator)
    {
        rapidjson::Value out(rapidjson::kObjectType);
        out.AddMember("FileName", toJSON(value.fileName, allocator), allocator);
        out.AddMember("Name", toJSON(value.name, allocator), allocator);
        out.AddMember("Display", toJSON(value.display, allocator), allocator);
        out.AddMember("View", toJSON(value.view, allocator), allocator);
        out.AddMember("ImageColorSpaces", toJSON(value.imageColorSpaces, allocator), allocator);
        return out;
    }

    void fromJSON(const rapidjson::Value& value, OCIO::ConfigMode& out)
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

    void fromJSON(const rapidjson::Value& value, OCIO::Config& out)
    {
        if (value.IsObject())
        {
            for (const auto& i : value.GetObject())
            {
                if (0 == strcmp("FileName", i.name.GetString()))
                {
                    fromJSON(i.value, out.fileName);
                }
                else if (0 == strcmp("Name", i.name.GetString()))
                {
                    fromJSON(i.value, out.name);
                }
                else if (0 == strcmp("Display", i.name.GetString()))
                {
                    fromJSON(i.value, out.display);
                }
                else if (0 == strcmp("View", i.name.GetString()))
                {
                    fromJSON(i.value, out.view);
                }
                else if (0 == strcmp("ImageColorSpaces", i.name.GetString()))
                {
                    fromJSON(i.value, out.imageColorSpaces);
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

