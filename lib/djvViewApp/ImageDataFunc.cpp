// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ImageDataFunc.h>

#include <djvViewApp/ImageData.h>

#include <djvUI/EnumFunc.h>

#include <djvRender2D/DataFunc.h>
#include <djvRender2D/EnumFunc.h>

#include <djvImage/InfoFunc.h>

// These need to be included last on macOS.
#include <djvCore/RapidJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

namespace djv
{
    rapidjson::Value toJSON(const ViewApp::ImageData& value, rapidjson::Document::AllocatorType& allocator)
    {
        rapidjson::Value out(rapidjson::kObjectType);
        {
            out.AddMember("ChannelsDisplay", toJSON(value.channelsDisplay, allocator), allocator);
            out.AddMember("AlphaBlend", toJSON(value.alphaBlend, allocator), allocator);

            out.AddMember("Mirror", toJSON(value.mirror, allocator), allocator);
            out.AddMember("Rotate", toJSON(value.rotate, allocator), allocator);
            out.AddMember("AspectRatio", toJSON(value.aspectRatio, allocator), allocator);

            out.AddMember("ColorEnabled", toJSON(value.colorEnabled, allocator), allocator);
            out.AddMember("Color", toJSON(value.color, allocator), allocator);

            out.AddMember("LevelsEnabled", toJSON(value.levelsEnabled, allocator), allocator);
            out.AddMember("Levels", toJSON(value.levels, allocator), allocator);

            out.AddMember("ExposureEnabled", toJSON(value.exposureEnabled, allocator), allocator);
            out.AddMember("Exposure", toJSON(value.exposure, allocator), allocator);

            out.AddMember("SoftClipEnabled", toJSON(value.softClipEnabled, allocator), allocator);
            out.AddMember("SoftClip", toJSON(value.softClip, allocator), allocator);
        }
        return out;
    }

    void fromJSON(const rapidjson::Value& value, ViewApp::ImageData& out)
    {
        if (value.IsObject())
        {
            for (const auto& i : value.GetObject())
            {
                if (0 == strcmp("ChannelsDisplay", i.name.GetString()))
                {
                    fromJSON(i.value, out.channelsDisplay);
                }
                else if (0 == strcmp("AlphaBlend", i.name.GetString()))
                {
                    fromJSON(i.value, out.alphaBlend);
                }
                else if (0 == strcmp("Mirror", i.name.GetString()))
                {
                    fromJSON(i.value, out.mirror);
                }
                else if (0 == strcmp("Rotate", i.name.GetString()))
                {
                    fromJSON(i.value, out.rotate);
                }
                else if (0 == strcmp("AspectRatio", i.name.GetString()))
                {
                    fromJSON(i.value, out.aspectRatio);
                }
                else if (0 == strcmp("ColorEnabled", i.name.GetString()))
                {
                    fromJSON(i.value, out.colorEnabled);
                }
                else if (0 == strcmp("Color", i.name.GetString()))
                {
                    fromJSON(i.value, out.color);
                }
                else if (0 == strcmp("LevelsEnabled", i.name.GetString()))
                {
                    fromJSON(i.value, out.levelsEnabled);
                }
                else if (0 == strcmp("Levels", i.name.GetString()))
                {
                    fromJSON(i.value, out.levels);
                }
                else if (0 == strcmp("ExposureEnabled", i.name.GetString()))
                {
                    fromJSON(i.value, out.exposureEnabled);
                }
                else if (0 == strcmp("Exposure", i.name.GetString()))
                {
                    fromJSON(i.value, out.exposure);
                }
                else if (0 == strcmp("Enabled", i.name.GetString()))
                {
                    fromJSON(i.value, out.softClipEnabled);
                }
                else if (0 == strcmp("SoftClip", i.name.GetString()))
                {
                    fromJSON(i.value, out.softClip);
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

