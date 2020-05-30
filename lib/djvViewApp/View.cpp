// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/View.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        bool GridOptions::operator == (const GridOptions& other) const
        {
            return enabled == other.enabled &&
                size == other.size &&
                color == other.color &&
                labels == other.labels &&
                labelsColor == other.labelsColor;
        }

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

        bool ViewBackgroundOptions::operator == (const ViewBackgroundOptions& other) const
        {
            return
                background == other.background &&
                color == other.color &&
                checkersSize == other.checkersSize &&
                checkersColors[0] == other.checkersColors[0] &&
                checkersColors[1] == other.checkersColors[1] &&
                border == other.border &&
                borderWidth == other.borderWidth &&
                borderColor == other.borderColor;
        }

    } // namespace ViewApp

    picojson::value toJSON(const ViewApp::GridOptions& value)
    {
        picojson::value out(picojson::object_type, true);
        out.get<picojson::object>()["Enabled"] = toJSON(value.enabled);
        out.get<picojson::object>()["Size"] = toJSON(value.size);
        out.get<picojson::object>()["Color"] = toJSON(value.color);
        out.get<picojson::object>()["Labels"] = toJSON(value.labels);
        out.get<picojson::object>()["LabelsColor"] = toJSON(value.labelsColor);
        return out;
    }

    picojson::value toJSON(const ViewApp::HUDOptions& value)
    {
        picojson::value out(picojson::object_type, true);
        out.get<picojson::object>()["Enabled"] = toJSON(value.enabled);
        out.get<picojson::object>()["Color"] = toJSON(value.color);
        out.get<picojson::object>()["Background"] = toJSON(value.background);
        return out;
    }

    picojson::value toJSON(const ViewApp::ViewBackgroundOptions& value)
    {
        picojson::value out(picojson::object_type, true);
        out.get<picojson::object>()["Background"] = toJSON(value.background);
        out.get<picojson::object>()["Color"] = toJSON(value.color);
        out.get<picojson::object>()["CheckersSize"] = toJSON(value.checkersSize);
        out.get<picojson::object>()["CheckersColors0"] = toJSON(value.checkersColors[0]);
        out.get<picojson::object>()["CheckersColors1"] = toJSON(value.checkersColors[1]);
        out.get<picojson::object>()["Border"] = toJSON(value.border);
        out.get<picojson::object>()["BorderWidth"] = toJSON(value.borderWidth);
        out.get<picojson::object>()["BorderColor"] = toJSON(value.borderColor);
        return out;
    }

    void fromJSON(const picojson::value& value, ViewApp::GridOptions& out)
    {
        if (value.is<picojson::object>())
        {
            for (const auto& i : value.get<picojson::object>())
            {
                if ("Enabled" == i.first)
                {
                    fromJSON(i.second, out.enabled);
                }
                else if ("Size" == i.first)
                {
                    fromJSON(i.second, out.size);
                }
                else if ("Color" == i.first)
                {
                    fromJSON(i.second, out.color);
                }
                else if ("Labels" == i.first)
                {
                    fromJSON(i.second, out.labels);
                }
                else if ("LabelsColor" == i.first)
                {
                    fromJSON(i.second, out.labelsColor);
                }
            }
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

    void fromJSON(const picojson::value& value, ViewApp::ViewBackgroundOptions& out)
    {
        if (value.is<picojson::object>())
        {
            for (const auto& i : value.get<picojson::object>())
            {
                if ("Background" == i.first)
                {
                    fromJSON(i.second, out.background);
                }
                else if ("Color" == i.first)
                {
                    fromJSON(i.second, out.color);
                }
                else if ("CheckersSize" == i.first)
                {
                    fromJSON(i.second, out.checkersSize);
                }
                else if ("CheckersColors0" == i.first)
                {
                    fromJSON(i.second, out.checkersColors[0]);
                }
                else if ("CheckersColors1" == i.first)
                {
                    fromJSON(i.second, out.checkersColors[1]);
                }
                else if ("Border" == i.first)
                {
                    fromJSON(i.second, out.border);
                }
                else if ("BorderWidth" == i.first)
                {
                    fromJSON(i.second, out.borderWidth);
                }
                else if ("BorderColor" == i.first)
                {
                    fromJSON(i.second, out.borderColor);
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

