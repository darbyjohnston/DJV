// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/HUD.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        bool HUDData::operator == (const HUDData& other) const
        {
            return
                fileName == other.fileName &&
                layer == other.layer &&
                size == other.size &&
                type == other.type &&
                isSequence == other.isSequence &&
                currentFrame == other.currentFrame &&
                speed == other.speed &&
                realSpeed == other.realSpeed;
        }

        bool HUDOptions::operator == (const HUDOptions& other) const
        {
            return
                enabled == other.enabled &&
                color == other.color &&
                background == other.background;
        }

    } // namespace ViewApp

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        ViewApp,
        HUDBackground,
        DJV_TEXT("hud_background_none"),
        DJV_TEXT("hud_background_overlay"));

    picojson::value toJSON(ViewApp::HUDBackground value)
    {
        std::stringstream ss;
        ss << value;
        return picojson::value(ss.str());
    }

    picojson::value toJSON(const ViewApp::HUDOptions& value)
    {
        picojson::value out(picojson::object_type, true);
        out.get<picojson::object>()["Enabled"] = toJSON(value.enabled);
        out.get<picojson::object>()["Color"] = toJSON(value.color);
        out.get<picojson::object>()["Background"] = toJSON(value.background);
        return out;
    }

    void fromJSON(const picojson::value& value, ViewApp::HUDBackground& out)
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

    void fromJSON(const picojson::value& value, ViewApp::HUDOptions& out)
    {
        if (value.is<picojson::object>())
        {
            for (const auto& i : value.get<picojson::object>())
            {
                if ("Enabled" == i.first)
                {
                    fromJSON(i.second, out.enabled);
                }
                else if ("Color" == i.first)
                {
                    fromJSON(i.second, out.color);
                }
                else if ("Background" == i.first)
                {
                    fromJSON(i.second, out.background);
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

