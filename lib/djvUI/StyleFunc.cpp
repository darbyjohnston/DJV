// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/StyleFunc.h>

#include <djvUI/EnumFunc.h>

#include <djvImage/ColorFunc.h>

#include <sstream>

#if defined(GetObject)
#undef GetObject
#endif // GetObject

using namespace djv::Core;

namespace djv
{
    rapidjson::Value toJSON(const UI::Style::Palette& value, rapidjson::Document::AllocatorType& allocator)
    {
        rapidjson::Value out(rapidjson::kObjectType);
        {
            rapidjson::Value object(rapidjson::kObjectType);
            for (auto role : UI::getColorRoleEnums())
            {
                std::stringstream ss;
                ss << role;
                std::stringstream ss2;
                ss2 << value.getColor(role);
                object.AddMember(rapidjson::Value(ss.str().c_str(), allocator), rapidjson::Value(ss2.str().c_str(), allocator), allocator);
            }
            out.AddMember("Roles", object, allocator);
            out.AddMember("DisabledMult", toJSON(value.getDisabledMult(), allocator), allocator);
        }
        return out;
    }

    rapidjson::Value toJSON(const UI::Style::Metrics& value, rapidjson::Document::AllocatorType& allocator)
    {
        rapidjson::Value out(rapidjson::kObjectType);
        {
            rapidjson::Value object(rapidjson::kObjectType);
            for (auto role : UI::getMetricsRoleEnums())
            {
                std::stringstream ss;
                ss << role;
                object.AddMember(rapidjson::Value(ss.str().c_str(), allocator), toJSON(value.getMetric(role), allocator), allocator);
            }
            out.AddMember("Roles", object, allocator);
        }
        return out;
    }

    void fromJSON(const rapidjson::Value& value, UI::Style::Palette& out)
    {
        if (value.IsObject())
        {
            for (const auto& i : value.GetObject())
            {
                if (0 == strcmp("Roles", i.name.GetString()))
                {
                    if (i.value.IsObject())
                    {
                        for (const auto& j : i.value.GetObject())
                        {
                            UI::ColorRole role = UI::ColorRole::First;
                            std::stringstream ss(j.name.GetString());
                            ss >> role;
                            Image::Color color;
                            fromJSON(j.value, color);
                            out.setColor(role, color);
                        }
                    }
                    else
                    {
                        //! \todo How can we translate this?
                        throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
                    }
                }
                else if (0 == strcmp("DisabledMult", i.name.GetString()))
                {
                    float v = 0.F;
                    fromJSON(i.value, v);
                    out.setDisabledMult(v);
                }
            }
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    void fromJSON(const rapidjson::Value& value, UI::Style::Metrics& out)
    {
        if (value.IsObject())
        {
            for (const auto& i : value.GetObject())
            {
                if (0 == strcmp("Roles", i.name.GetString()))
                {
                    if (i.value.IsObject())
                    {
                        for (const auto& j : i.value.GetObject())
                        {
                            UI::MetricsRole role = UI::MetricsRole::First;
                            std::stringstream ss(j.name.GetString());
                            ss >> role;
                            float v = 0.F;
                            fromJSON(j.value, v);
                            out.setMetric(role, v);
                        }
                    }
                    else
                    {
                        //! \todo How can we translate this?
                        throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
                    }
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
