// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ViewFunc.h>

#include <djvViewApp/EnumFunc.h>

#include <djvImage/ColorFunc.h>

using namespace djv::Core;

namespace djv
{
    rapidjson::Value toJSON(const ViewApp::GridOptions& value, rapidjson::Document::AllocatorType& allocator)
    {
        rapidjson::Value out(rapidjson::kObjectType);
        out.AddMember("Enabled", toJSON(value.enabled, allocator), allocator);
        out.AddMember("Size", toJSON(value.size, allocator), allocator);
        out.AddMember("Color", toJSON(value.color, allocator), allocator);
        out.AddMember("Labels", toJSON(value.labels, allocator), allocator);
        out.AddMember("LabelsColor", toJSON(value.labelsColor, allocator), allocator);
        return out;
    }

    rapidjson::Value toJSON(const ViewApp::HUDOptions& value, rapidjson::Document::AllocatorType& allocator)
    {
        rapidjson::Value out(rapidjson::kObjectType);
        out.AddMember("Enabled", toJSON(value.enabled, allocator), allocator);
        out.AddMember("Color", toJSON(value.color, allocator), allocator);
        out.AddMember("Background", toJSON(value.background, allocator), allocator);
        return out;
    }

    rapidjson::Value toJSON(const ViewApp::ViewBackgroundOptions& value, rapidjson::Document::AllocatorType& allocator)
    {
        rapidjson::Value out(rapidjson::kObjectType);
        out.AddMember("Background", toJSON(value.background, allocator), allocator);
        out.AddMember("Color", toJSON(value.color, allocator), allocator);
        out.AddMember("CheckersSize", toJSON(value.checkersSize, allocator), allocator);
        out.AddMember("CheckersColors0", toJSON(value.checkersColors[0], allocator), allocator);
        out.AddMember("CheckersColors1", toJSON(value.checkersColors[1], allocator), allocator);
        out.AddMember("Border", toJSON(value.border, allocator), allocator);
        out.AddMember("BorderWidth", toJSON(value.borderWidth, allocator), allocator);
        out.AddMember("BorderColor", toJSON(value.borderColor, allocator), allocator);
        return out;
    }

    void fromJSON(const rapidjson::Value& value, ViewApp::GridOptions& out)
    {
        if (value.IsObject())
        {
            for (const auto& i : value.GetObject())
            {
                if (0 == strcmp("Enabled", i.name.GetString()))
                {
                    fromJSON(i.value, out.enabled);
                }
                else if (0 == strcmp("Size", i.name.GetString()))
                {
                    fromJSON(i.value, out.size);
                }
                else if (0 == strcmp("Color", i.name.GetString()))
                {
                    fromJSON(i.value, out.color);
                }
                else if (0 == strcmp("Labels", i.name.GetString()))
                {
                    fromJSON(i.value, out.labels);
                }
                else if (0 == strcmp("LabelsColor", i.name.GetString()))
                {
                    fromJSON(i.value, out.labelsColor);
                }
            }
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    void fromJSON(const rapidjson::Value& value, ViewApp::HUDOptions& out)
    {
        if (value.IsObject())
        {
            for (const auto& i : value.GetObject())
            {
                if (0 == strcmp("Enabled", i.name.GetString()))
                {
                    fromJSON(i.value, out.enabled);
                }
                else if (0 == strcmp("Color", i.name.GetString()))
                {
                    fromJSON(i.value, out.color);
                }
                else if (0 == strcmp("Background", i.name.GetString()))
                {
                    fromJSON(i.value, out.background);
                }
            }
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    void fromJSON(const rapidjson::Value& value, ViewApp::ViewBackgroundOptions& out)
    {
        if (value.IsObject())
        {
            for (const auto& i : value.GetObject())
            {
                if (0 == strcmp("Background", i.name.GetString()))
                {
                    fromJSON(i.value, out.background);
                }
                else if (0 == strcmp("Color", i.name.GetString()))
                {
                    fromJSON(i.value, out.color);
                }
                else if (0 == strcmp("CheckersSize", i.name.GetString()))
                {
                    fromJSON(i.value, out.checkersSize);
                }
                else if (0 == strcmp("CheckersColors0", i.name.GetString()))
                {
                    fromJSON(i.value, out.checkersColors[0]);
                }
                else if (0 == strcmp("CheckersColors1", i.name.GetString()))
                {
                    fromJSON(i.value, out.checkersColors[1]);
                }
                else if (0 == strcmp("Border", i.name.GetString()))
                {
                    fromJSON(i.value, out.border);
                }
                else if (0 == strcmp("BorderWidth", i.name.GetString()))
                {
                    fromJSON(i.value, out.borderWidth);
                }
                else if (0 == strcmp("BorderColor", i.name.GetString()))
                {
                    fromJSON(i.value, out.borderColor);
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

